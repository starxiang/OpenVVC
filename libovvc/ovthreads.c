#include <pthread.h>
/* FIXME tmp*/
#include <stdatomic.h>

#include "slicedec.h"
#include "overror.h"
#include "ovutils.h"
#include "ovmem.h"
#include "ovthreads.h"
#include "ovdpb.h"


static int
ovthread_decode_entry(struct EntryJob *entry_job, struct EntryThread *entry_th)
{   
    struct SliceSynchro* slice_sync = entry_job->slice_sync;
    uint8_t entry_idx              = entry_job->entry_idx;

    uint16_t nb_entries  = slice_sync->nb_entries;
    ov_log(NULL, OVLOG_DEBUG, "Decoder with POC %d, start entry %d\n", slice_sync->owner->pic->poc, entry_idx);
    
    OVCTUDec *const ctudec  = entry_th->ctudec;
    OVSliceDec *const sldec = slice_sync->owner;
    const OVPS *const prms  = sldec->active_params;

    slice_sync->decode_entry(sldec, ctudec, prms, entry_idx);

    uint16_t nb_entries_decoded = atomic_fetch_add_explicit(&slice_sync->nb_entries_decoded, 1, memory_order_acq_rel);

    /* Last thread to exit loop will have entry_idx set to nb_entry - 1*/
    return nb_entries_decoded == nb_entries - 1 ;
}


struct EntryJob *
entry_thread_select_job(struct EntryThread *entry_th)
{
    /* Get the first available job in the job fifo. 
     */
    struct MainThread* main_thread = entry_th->main_thread;
    uint16_t size_fifo = main_thread->size_fifo; 
    struct EntryJob *entry_jobs_fifo = main_thread->entry_jobs_fifo;
    struct EntryJob *entry_job = NULL;

    pthread_mutex_lock(&main_thread->main_mtx); 
    int64_t first_idx = main_thread->first_idx_fifo;
    int64_t last_idx  = main_thread->last_idx_fifo;
    if (first_idx <= last_idx) {
        int idx = first_idx % size_fifo;
        entry_job = &entry_jobs_fifo[idx];
        main_thread->first_idx_fifo ++;
    }
    pthread_mutex_unlock(&main_thread->main_mtx);

    return entry_job;
}


static void *
entry_thread_main_function(void *opaque)
{
    struct EntryThread *entry_th = (struct EntryThread *)opaque;
    struct MainThread* main_thread = entry_th->main_thread;

    pthread_mutex_lock(&entry_th->entry_mtx);
    entry_th->state = IDLE;
    pthread_mutex_unlock(&entry_th->entry_mtx);

    while (!entry_th->kill){

        struct EntryJob *entry_job = entry_thread_select_job(entry_th);

        if (entry_job){
            slicedec_update_entry_decoder(entry_job->slice_sync->owner, entry_th->ctudec);
            pthread_mutex_lock(&entry_th->entry_mtx);
            entry_th->state = ACTIVE;
            pthread_mutex_unlock(&entry_th->entry_mtx);

            uint8_t is_last = ovthread_decode_entry(entry_job, entry_th);

            /* Check if the entry was the last of the slice
             */
            if (is_last) {
                slicedec_finish_decoding(entry_job->slice_sync->owner);
            }
        }
        else{
            pthread_mutex_lock(&main_thread->entry_threads_mtx);
            pthread_mutex_lock(&entry_th->entry_mtx);
            entry_th->state = IDLE;

            // ov_log(NULL, OVLOG_DEBUG,"entry sign main\n");
            pthread_cond_signal(&main_thread->entry_threads_cnd);
            pthread_mutex_unlock(&main_thread->entry_threads_mtx);

            // ov_log(NULL, OVLOG_DEBUG,"entry wait main\n");
            pthread_cond_wait(&entry_th->entry_cnd, &entry_th->entry_mtx);
            pthread_mutex_unlock(&entry_th->entry_mtx);
            
        }
    }
    return NULL;
}


int
ovthread_init_entry_thread(struct EntryThread *entry_th)
{
    entry_th->state = IDLE;
    entry_th->kill  = 0;

    int ret = ctudec_init(&entry_th->ctudec);
    if (ret < 0) {
        ov_log(NULL, OVLOG_ERROR, "Failed line decoder initialisation\n");
        ctudec_uninit(entry_th->ctudec);
        return OVVC_ENOMEM;
    }

    pthread_mutex_init(&entry_th->entry_mtx, NULL);
    pthread_cond_init(&entry_th->entry_cnd, NULL);
    pthread_mutex_lock(&entry_th->entry_mtx);

#if USE_THREADS
    if (pthread_create(&entry_th->thread, NULL, entry_thread_main_function, entry_th)) {
        pthread_mutex_unlock(&entry_th->entry_mtx);
        ov_log(NULL, OVLOG_ERROR, "Thread creation failed at decoder init\n");
        return OVVC_ENOMEM;
    }
#endif
    pthread_mutex_unlock(&entry_th->entry_mtx);
    return 1;
}


void
ovthread_uninit_entry_thread(struct EntryThread *entry_th)
{       
        pthread_mutex_destroy(&entry_th->entry_mtx);
        pthread_cond_destroy(&entry_th->entry_cnd);

        ctudec_uninit(entry_th->ctudec);
}


/*
Functions needed for the synchro of threads decoding the slice
*/
int
ovthread_slice_add_entry_jobs(struct SliceSynchro *slice_sync, DecodeFunc decode_entry, int nb_entries)
{
    slice_sync->nb_entries = nb_entries;
    slice_sync->decode_entry = decode_entry;
    atomic_store_explicit(&slice_sync->nb_entries_decoded, 0, memory_order_relaxed);

    struct MainThread* main_thread = slice_sync->main_thread;
    int size_fifo = main_thread->size_fifo; 
    struct EntryJob *entry_jobs_fifo = main_thread->entry_jobs_fifo;

    /* Add entry jobs to the job FIFO of the main thread. 
     */
    pthread_mutex_lock(&main_thread->main_mtx);
    for (int i = 1; i <= nb_entries; ++i) {
        main_thread->last_idx_fifo++;
        int idx = main_thread->last_idx_fifo % size_fifo;
        struct EntryJob *entry_job = &entry_jobs_fifo[idx];
        entry_job->entry_idx = i-1;
        entry_job->slice_sync = slice_sync;
        ov_log(NULL, OVLOG_DEBUG, "Main adds POC %d entry %d\n", slice_sync->owner->pic->poc, i-1);
    }
    pthread_mutex_unlock(&main_thread->main_mtx);

    /*Signal all entry threads that new jobs are available
    */
    struct EntryThread *entry_threads_list = main_thread->entry_threads_list;
    for (int i = 0; i < main_thread->nb_entry_th; ++i){
        struct EntryThread *th_entry = &entry_threads_list[i];
        pthread_mutex_lock(&th_entry->entry_mtx);
        // ov_log(NULL, OVLOG_DEBUG,"main sign entry\n");
        pthread_cond_signal(&th_entry->entry_cnd);
        pthread_mutex_unlock(&th_entry->entry_mtx);
    }

    return 0;
}


int
ovthread_slice_sync_init(struct SliceSynchro *slice_sync)
{   
    atomic_init(&slice_sync->nb_entries_decoded, 0);

    pthread_mutex_init(&slice_sync->gnrl_mtx, NULL);
    pthread_cond_init(&slice_sync->gnrl_cnd,  NULL);

    return 0;
}


void
ovthread_slice_sync_uninit(struct SliceSynchro *slice_sync)
{   
    OVSliceDec * slicedec = slice_sync->owner;

    pthread_mutex_lock(&slice_sync->gnrl_mtx);
    if (slice_sync->active_state == DECODING_FINISHED) {
        pthread_mutex_unlock(&slice_sync->gnrl_mtx);

        OVPicture *slice_pic = slicedec->pic;

        if (slice_pic && (slice_pic->flags & OV_IN_DECODING_PIC_FLAG)) {

            ov_log(NULL, OVLOG_TRACE, "Remove DECODING_PIC_FLAG POC: %d\n", slice_pic->poc);

            ovdpb_unref_pic(slice_pic, OV_IN_DECODING_PIC_FLAG);
            ovdpb_unmark_ref_pic_lists(slicedec->slice_type, slice_pic);

            pthread_mutex_lock(&slice_sync->gnrl_mtx);
            slice_sync->active_state = IDLE;
            pthread_mutex_unlock(&slice_sync->gnrl_mtx);
        }
    } else {
        pthread_mutex_unlock(&slice_sync->gnrl_mtx);
    }

    pthread_mutex_destroy(&slice_sync->gnrl_mtx);
    pthread_cond_destroy(&slice_sync->gnrl_cnd);

}

