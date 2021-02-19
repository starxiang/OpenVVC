#ifndef VCL_H
#define VCL_H
#include <stdint.h>

#include "dec_structures.h"
#include "ovdefs.h"
#include "vcl_cabac.h"

int
transform_unit_wrap(OVCTUDec *const ctu_dec,
                    const OVPartInfo *const part_ctx,
                    uint8_t x0, uint8_t y0,
                    uint8_t log2_cb_w, uint8_t log2_cb_h,
                    VVCCU cu);

int transform_unit_st(OVCTUDec *const ctu_dec,
                      unsigned int x0, unsigned int y0,
                      unsigned int log2_tb_w, unsigned int log2_tb_h,
                      uint8_t rqt_root_cbf, uint8_t cu_flags);

int transform_unit_l(OVCTUDec *const ctu_dec,
                     unsigned int x0, unsigned int y0,
                     unsigned int log2_tb_w, unsigned int log2_tb_h,
                     uint8_t rqt_root_cbf, uint8_t cu_flags);

int transform_unit_c(OVCTUDec *const ctu_dec,
                     unsigned int x0, unsigned int y0,
                     unsigned int log2_tb_w, unsigned int log2_tb_h,
                     uint8_t rqt_root_cbf, uint8_t cu_flags);

VVCCU coding_unit_intra_st(OVCTUDec *const ctu_dec,
                           const OVPartInfo *const part_ctx,
                           uint8_t x0, uint8_t y0,
                           uint8_t log2_cu_w, uint8_t log2_cu_h);

VVCCU coding_unit_intra(OVCTUDec *const ctu_dec,
                        const OVPartInfo *const part_ctx,
                        uint8_t x0, uint8_t y0,
                        uint8_t log2_cb_w, uint8_t log2_cb_h);

VVCCU coding_unit_intra_c(OVCTUDec *const ctu_dec,
                          const OVPartInfo *const part_ctx,
                          uint8_t x0, uint8_t y0,
                          uint8_t log2_cb_w, uint8_t log2_cb_h);

VVCCU
coding_unit_inter_st(OVCTUDec *const ctu_dec,
                     const OVPartInfo *const part_ctx,
                     uint8_t x0, uint8_t y0,
                     uint8_t log2_cu_w, uint8_t log2_cu_h);

int
prediction_unit_inter_b(OVCTUDec *const ctu_dec,
                        const OVPartInfo *const part_ctx,
                        uint8_t x0, uint8_t y0,
                        uint8_t log2_pb_w, uint8_t log2_pb_h,
                        uint8_t merge_flag);
int
prediction_unit_inter_p(OVCTUDec *const ctu_dec,
                        const OVPartInfo *const part_ctx,
                        uint8_t x0, uint8_t y0,
                        uint8_t log2_pb_w, uint8_t log2_pb_h,
                        uint8_t merge_flag);

int coding_quadtree(OVCTUDec *const ctu_dec,
                    const OVPartInfo *const part_ctx,
                    unsigned int x0, unsigned int y0,
                    unsigned int log2_cb_s, unsigned int qt_depth);

int coding_quadtree_implicit(OVCTUDec *const ctu_dec,
                             const OVPartInfo *const part_ctx,
                             unsigned int x0, unsigned int y0,
                             unsigned int log2_cb_s, unsigned int qt_depth,
                             unsigned int rem_w, unsigned int rem_h);

int dual_tree(OVCTUDec *const ctu_dec,
              const OVPartInfo *const part_ctx,
              unsigned int x0, unsigned int y0,
              unsigned int log2_cb_s, unsigned int qt_depth);

int dual_tree_implicit(OVCTUDec *const ctu_dec,
                       const OVPartInfo *const part_ctx,
                       unsigned int x0, unsigned int y0,
                       unsigned int log2_cb_s, unsigned int qt_depth,
                       unsigned int rem_w,
                       unsigned int rem_h);

int residual_coding_isp_h_sdh(OVCTUDec *const ctu_dec, int16_t *const dst,
                              unsigned int log2_tb_w, unsigned int log2_tb_h,
                              uint16_t last_pos);

int residual_coding_isp_v_sdh(OVCTUDec *const ctu_dec, int16_t *const dst,
                              unsigned int log2_tb_w, unsigned int log2_tb_h,
                              uint16_t last_pos);

int residual_coding_isp_h_dpq(OVCTUDec *const ctu_dec, int16_t *const dst,
                              unsigned int log2_tb_w, unsigned int log2_tb_h,
                              uint16_t last_pos);

int residual_coding_isp_v_dpq(OVCTUDec *const ctu_dec, int16_t *const dst,
                              unsigned int log2_tb_w, unsigned int log2_tb_h,
                              uint16_t last_pos);

uint64_t residual_coding_sdh(OVCTUDec *const ctu_dec, int16_t *const dst,
                             unsigned int log2_tb_w, unsigned int log2_tb_h,
                             uint16_t last_pos);

int residual_coding_chroma_sdh(OVCTUDec *const ctu_dec, int16_t *const dst,
                                    unsigned int log2_tb_w, unsigned int log2_tb_h,
                                    uint16_t last_pos);

uint64_t residual_coding_dpq(OVCTUDec *const ctu_dec, int16_t *const dst,
                             unsigned int log2_tb_w, unsigned int log2_tb_h,
                             uint16_t last_pos);

int residual_coding_chroma_dpq(OVCTUDec *const ctu_dec, int16_t *const dst,
                                    unsigned int log2_tb_w, unsigned int log2_tb_h,
                                    uint16_t last_pos);

int residual_coding_ts(OVCTUDec *const ctu_dec, unsigned int log2_tb_w, unsigned int log2_tb_h);

int coding_unit(OVCTUDec *const ctu_dec,
                const OVPartInfo *const part_ctx,
                uint8_t x0, uint8_t y0,
                uint8_t log2_cb_w, uint8_t log2_cb_h);


uint8_t ovcabac_read_ae_sao_merge_type(OVCABACCtx *const cabac_ctx, uint64_t *const cabac_state, uint8_t neighbour_flags);

void ovcabac_read_ae_sao_type_idx(OVCABACCtx *const cabac_ctx, uint64_t *const cabac_state, SAOParams *sao_ctu,
                        uint8_t sao_luma_flag, uint8_t sao_chroma_flag, uint8_t num_bits_sao, uint8_t num_bits_sao_c);
#endif
