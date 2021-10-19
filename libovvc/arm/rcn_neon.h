#ifndef RCN_NEON_H
#define RCN_NEON_H
#include "rcn_structures.h"

#define SIMDE_ENABLE_NATIVE_ALIASES

void rcn_init_mc_functions_neon(struct RCNFunctions *const rcn_funcs);
void rcn_init_mc_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_tr_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_dc_planar_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_ict_functions_sse(struct RCNFunctions *rcn_func, uint8_t type);
void rcn_init_alf_functions_sse(struct RCNFunctions *rcn_func);
void rcn_init_cclm_functions_sse(struct RCNFunctions *rcn_func);
void rcn_init_lfnst_functions_sse(struct RCNFunctions *rcn_func);
void rcn_init_mip_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_sao_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_dmvr_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_prof_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_bdof_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_ciip_functions_sse(struct RCNFunctions *const rcn_funcs);
void rcn_init_df_functions_sse(struct RCNFunctions *const rcn_funcs);

#endif//RCN_NEON_H
