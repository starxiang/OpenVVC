#ifndef OVTHREADS_H
#define OVTHREADS_H

#include <stdint.h>
// #include <stdio.h>

#include "slicedec.h"

struct SliceThread;
struct OVVCDec;
struct OVFrame;

// struct OutputThread
// {
//     FILE *fout;

//     /* Thread displaying the frames after 
//      * the decoding is finished.
//      */
//     pthread_t thread;
//     pthread_mutex_t gnrl_mtx;
//     pthread_cond_t gnrl_cnd;

//     // uint8_t state;
//     uint8_t kill;
// };

int ovthread_decode_entries(struct SliceThread *th_info, DecodeFunc decode_entry, int nb_entries);

int init_entry_threads(struct SliceThread *th_info, int nb_threads);

void uninit_entry_threads(struct SliceThread *th_info);


int ovthread_slice_thread_init(struct SliceThread *th_slice, int nb_threads);

void ovthread_slice_thread_uninit(struct SliceThread *th_slice);


uint32_t write_decoded_frame_to_file(OVFrame *const frame, FILE *fp);

int ovthread_output_init(OVVCDec *dec, FILE* fout);

#endif
