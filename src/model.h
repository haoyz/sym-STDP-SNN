#ifndef _MODEL_H_
#define _MODEL_H_

#include "modelSpec.h"
#include "../global.h"
#include "sparseUtils.h"
#include "utils.h"
#include <cstdlib> //不用srand和rand了
#include <ctime>
#include <queue>
#include <iostream>

#define DT 0.500000f
#define _FTYPE GENN_FLOAT
#define scalar float
#define testDataEvaluateModeInit false

//------------------------------------------------------------------------------
// possion
//------------------------------------------------------------------------------
#define Poi_V -60.0
#define Poi_seed 1
#define Poi_trace 1

#define Poi_Vspike 20.0
#define Poi_Vrest -60.0
#define Poi_Ttrace 20

//------------------------------------------------------------------------------
// LIF Exc
//------------------------------------------------------------------------------
#define Exc_V (Exc_Vrest - 40.0)
#define Exc_theta 20 // 1 - theta 初始值？(20!!!和offset_exc刚开始时抵消，但还是要用20由于theta_e自身的微分方程)
#define Exc_timer 10 // 2 - timer 初始值？目前兴奋神经元加了50,而抑制还是10.只有兴奋有不应期，设置为10为了刚开始可以响应输入而一但spike的话timer归零就正常轨道了
#define Exc_trace1 1
#define Exc_trace2 1
#define Exc_trace 1
#define Exc_testDataEvaluateMode testDataEvaluateModeInit

#define Exc_TV 100
#define Exc_Vrest -65.0
#define Exc_Vreset -65.0
#define Exc_Vthresh -52
#define Exc_Refrac 2
#define Exc_test_mode TEST_MODE_NEURON
#define Exc_Ttheta 1e7 * 1.5
#define Exc_theta_plus 0.05 * 1.8
#define Exc_offset 20.0
#define Exc_Ttrace1 20
#define Exc_Ttrace2 40
#define Exc_Ttrace 20

//------------------------------------------------------------------------------
// LIF Inh
//------------------------------------------------------------------------------
#define Inh_V (Inh_Vrest - 40.0)
#define Inh_timer 10

#define Inh_TV 10
#define Inh_Vrest -60.0
#define Inh_Vreset -45.0
#define Inh_Vthresh -40
#define Inh_Refrac 2

//------------------------------------------------------------------------------
// LIF Cla
//------------------------------------------------------------------------------
#define Cla_V (Cla_Vrest - 40.0)
#define Cla_seed 1 //rewrite in the train.cc
#define Cla_trace1 1
#define Cla_trace2 1
#define Cla_testDataEvaluateMode testDataEvaluateModeInit

#define Cla_TV 10
#define Cla_Vrest -60.0
#define Cla_Vreset -45.0
#define Cla_Vthresh -40.0
#define Cla_Vspike 20
#define Cla_Ttrace1 20
#define Cla_Ttrace2 40
#define Cla_test_mode TEST_MODE_NEURON

//------------------------------------------------------------------------------
// postSynV
//------------------------------------------------------------------------------
#define P2E_tau_S 1.0 //1.0,//1.0的话电导衰减太快，电导增加比不上衰减。 //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
#define P2E_Erev 0.0  // 1 - Erev: Reversal potential //python程序中-85不用

#define E2I_tau_S 1.0
#define E2I_Erev 0.0

#define I2E_tau_S 2.0
#define I2E_Erev -100.0

//------------------------------------------------------------------------------
// synapse
//------------------------------------------------------------------------------
#define gPE_INIT_MAX 0.3
#define gPE_INIT_MAX_1000 gPE_INIT_MAX * 1000
#define gPE_MAX 1.0
#define gEC_INIT_MAX 0.3
#define gEC_INIT_MAX_1000 gEC_INIT_MAX * 1000
#define gEC_MAX 8.0
#define g_EI 10.4
#define g_IE 17
#define NORMAL 78.4
#define Cla_NORMAL (0.1 * NExc * gEC_MAX) //即电导既不太大也不太小，下一次有充分的空间学习
//------------------------------------------------------------------------------
// STDP
//------------------------------------------------------------------------------
#define stdp_PE_g 0.1
#define stdp_PE_testDataEvaluateMode testDataEvaluateModeInit

#define stdp_PE_nu_ee_pre 0.0001
#define stdp_PE_nu_ee_post 0.01
#define stdp_PE_g_min 0
#define stdp_PE_g_max gPE_MAX

#define stdp_EC_g 0.1
#define stdp_EC_testDataEvaluateMode testDataEvaluateModeInit

#define stdp_EC_nu_ee_pre 0.0001
#define stdp_EC_nu_ee_post 0.01
#define stdp_EC_g_min 0
#define stdp_EC_g_max gEC_MAX

//------------------------------------------------------------------------------
// DA_STDP
//------------------------------------------------------------------------------
#define da_stdp_PE_g 0.1
#define da_stdp_PE_testDataEvaluateMode testDataEvaluateModeInit

#define da_stdp_PE_nu_ee_pre 0.01 * 2
#define da_stdp_PE_nu_ee_post 0.01 * 2
#define da_stdp_PE_g_min 0
#define da_stdp_PE_g_max gPE_MAX
#define da_stdp_PE_a_plus 0.1
#define da_stdp_PE_a_minus 0.1

#define da_stdp_EC_g 0.1
#define da_stdp_EC_testDataEvaluateMode testDataEvaluateModeInit

#define da_stdp_EC_nu_ee_pre 0.01 * 8
#define da_stdp_EC_nu_ee_post 0.01 * 8
#define da_stdp_EC_g_min 0
#define da_stdp_EC_g_max gEC_MAX
#define da_stdp_EC_a_plus 0.1
#define da_stdp_EC_a_minus 0.1

//------------------------------------------------------------------------------
// Sym_STDP
//------------------------------------------------------------------------------
#define sym_stdp_PE_g 0.1
#define sym_stdp_PE_testDataEvaluateMode testDataEvaluateModeInit

#define sym_stdp_PE_nu_ee_pre 0.01 * 2
#define sym_stdp_PE_nu_ee_post 0.01 * 2
#define sym_stdp_PE_g_min 0
#define sym_stdp_PE_g_max gPE_MAX
#define sym_stdp_PE_a_plus 0.1
#define sym_stdp_PE_a_minus 0.1

#define sym_stdp_EC_g 0.1
#define sym_stdp_EC_testDataEvaluateMode testDataEvaluateModeInit

#define sym_stdp_EC_nu_ee_pre 0.01 * 8
#define sym_stdp_EC_nu_ee_post 0.01 * 8
#define sym_stdp_EC_g_min 0
#define sym_stdp_EC_g_max gEC_MAX
#define sym_stdp_EC_a_plus 0.1
#define sym_stdp_EC_a_minus 0.1

//------------------------------------------------------------------------------
// soft_bounds_STDP
//------------------------------------------------------------------------------
#define soft_stdp_g 0.1
#define soft_stdp_testDataEvaluateMode testDataEvaluateModeInit

#define soft_stdp_nu_ee_pre 0.01
#define soft_stdp_nu_ee_post 0.01
#define soft_stdp_g_min 0
#define soft_stdp_g_max_PE 1.0
#define soft_stdp_a_plus 0.1
#define soft_stdp_a_minus 0.1

//------------------------------------------------------------------------------
// noSTDP
//------------------------------------------------------------------------------
#define no_stdp_g 0.1

//------------------------------------------------------------------------------
// POISON model P
//------------------------------------------------------------------------------
extern double ini_Poi[];
extern double p_Poi[];
//------------------------------------------------------------------------------
// LIF model E&I
//------------------------------------------------------------------------------
extern double ini_Exc[];
extern double p_Exc[];
extern double ini_Inh[];
extern double p_Inh[];
//------------------------------------------------------------------------------
// LIF model Cal
//------------------------------------------------------------------------------
extern double ini_Cal[];
extern double p_Cal[];
//------------------------------------------------------------------------------
// postSynV
//------------------------------------------------------------------------------
extern double postExpP2E[2];
extern double postExpE2I[2];
extern double postExpI2E[2];
extern double *postSynV;
//------------------------------------------------------------------------------
// STDP
//------------------------------------------------------------------------------
extern double ini_STDP_PE[];
extern double p_STDP_PE[];
extern double ini_STDP_EC[];
extern double p_STDP_EC[];
//------------------------------------------------------------------------------
// DA_STDP
//------------------------------------------------------------------------------
extern double ini_DA_STDP_PE[];
extern double p_DA_STDP_PE[];
extern double ini_DA_STDP_EC[];
extern double p_DA_STDP_EC[];
//------------------------------------------------------------------------------
// noSTDP
//------------------------------------------------------------------------------
extern double ini_noSTDP[];
extern double *p_noSTDP_PE;
extern double *p_noSTDP_EC;
//------------------------------------------------------------------------------
// E——>I & I——>E
//------------------------------------------------------------------------------
extern double ini_NSYNAPSE_EI[];
extern double *p_NSYNAPSE_EI;
extern double ini_NSYNAPSE_IE[];
extern double *p_NSYNAPSE_IE;

extern size_t size_gP2E;
extern size_t size_gE2C;

void modelDefinition(NNmodel &model);

#endif
