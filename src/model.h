#ifndef _MODEL_H_
#define _MODEL_H_

#include "modelSpec.h"
#include "../global.h"
#include "sparseUtils.h"
#include "utils.h"
#include <cstdlib> 
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
#define Exc_theta 20 
#define Exc_timer 10 
#define Exc_trace1 1
#define Exc_trace2 1
#define Exc_trace 1
#define Exc_testDataEvaluateMode testDataEvaluateModeInit

#define Exc_TV 100
#define Exc_Vrest -65.0
#define Exc_Vreset -65.0

/***setting for mnist***/
#define Exc_Vthresh -52
/***setting for fashion-mnist***/
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define Exc_Vthresh -60

#define Exc_Refrac 2
#define Exc_test_mode TEST_MODE_NEURON

//------------------------------------------------------------------------------
// The Exc_theta_plus in the code corresponds to the α * dt / τ_θ in the paper Eq(5). 
// For network sizes N_hidden = 100, 400, 1600, 6400, and 10 000, 
// α * dt / τ_θ = 0.07, 0.07, 0.07, 0.05 and 0.05.
//------------------------------------------------------------------------------

/***parameters of 10000 excitatory neurons for mnist***/
/***parameters of 6400 excitatory neurons for mnist***/
// #define Exc_Ttheta 1e7 * 2
// #define Exc_theta_plus 0.05
/***parameters of 1600 excitatory neurons for mnist***/
// #define Exc_Ttheta 1e7 * 0.8
// #define Exc_theta_plus 0.05 * 1.4
/***parameters of 400 excitatory neurons for mnist***/
/***parameters of 100 excitatory neurons for mnist***/
#define Exc_Ttheta 1e7 * 0.6
#define Exc_theta_plus 0.07
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define Exc_Ttheta 1e7 * 5
// #define Exc_theta_plus 0.05
/***parameters of 6400 excitatory neurons for fashion-mnist***/
// #define Exc_Ttheta 1e7 * 2
// #define Exc_theta_plus 0.05


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
#define P2E_tau_S 1.0 
#define P2E_Erev 0.0  

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

/***setting for mnist***/
#define gEC_MAX 8.0
/***setting for fashion-mnist***/
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define gEC_MAX 40.0

#define g_EI 10.4
#define g_IE 17

/***setting for mnist***/
#define NORMAL 78.4
#define Cla_NORMAL (0.1 * NExc * gEC_MAX) 
/***setting for fashion-mnist***/
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define NORMAL 784*0.025
// #define Cla_NORMAL (20 * gEC_MAX)
/***parameters of 6400 excitatory neurons for fashion-mnist***/
// #define NORMAL 784*0.025
// #define Cla_NORMAL (0.1 * NExc * gEC_MAX) 

//------------------------------------------------------------------------------
// DA_STDP
//------------------------------------------------------------------------------
#define da_stdp_PE_g 0.1
#define da_stdp_PE_testDataEvaluateMode testDataEvaluateModeInit

/***parameters of 10000 excitatory neurons for mnist***/
// #define da_stdp_PE_nu_ee_pre 0.01 * 0.75
// #define da_stdp_PE_nu_ee_post 0.01 * 0.75
/***parameters of 6400 excitatory neurons for mnist***/
// #define da_stdp_PE_nu_ee_pre 0.01
// #define da_stdp_PE_nu_ee_post 0.01
/***parameters of 1600 excitatory neurons for mnist***/
// #define da_stdp_PE_nu_ee_pre 0.01 * 2
// #define da_stdp_PE_nu_ee_post 0.01 * 2
/***parameters of 400 excitatory neurons for mnist***/
/***parameters of 100 excitatory neurons for mnist***/
#define da_stdp_PE_nu_ee_pre 0.01 * 0.5
#define da_stdp_PE_nu_ee_post 0.01 * 0.5
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define da_stdp_PE_nu_ee_pre 0.01 * 2
// #define da_stdp_PE_nu_ee_post 0.01 * 2
/***parameters of 6400 excitatory neurons for fashion-mnist***/
// #define da_stdp_PE_nu_ee_pre 0.01 * 0.05
// #define da_stdp_PE_nu_ee_post 0.01 * 0.05

#define da_stdp_PE_g_min 0
#define da_stdp_PE_g_max gPE_MAX
#define da_stdp_PE_a_plus 0.1
#define da_stdp_PE_a_minus 0.1

#define da_stdp_EC_g 0.1
#define da_stdp_EC_testDataEvaluateMode testDataEvaluateModeInit

/***parameters of 10000 excitatory neurons for mnist***/
/***parameters of 6400 excitatory neurons for mnist***/
/***parameters of 400 excitatory neurons for mnist***/
/***parameters of 100 excitatory neurons for mnist***/
#define da_stdp_EC_nu_ee_pre 0.01 * 4
#define da_stdp_EC_nu_ee_post 0.01 * 4
/***parameters of 1600 excitatory neurons for mnist***/
// #define da_stdp_EC_nu_ee_pre 0.01 * 8
// #define da_stdp_EC_nu_ee_post 0.01 * 8
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define da_stdp_EC_nu_ee_pre 0.01 * 8 *4
// #define da_stdp_EC_nu_ee_post 0.01 * 8 *4
/***parameters of 6400 excitatory neurons for fashion-mnist***/
// #define da_stdp_EC_nu_ee_pre 0.01 * 8
// #define da_stdp_EC_nu_ee_post 0.01 * 8


#define da_stdp_EC_g_min 0
#define da_stdp_EC_g_max gEC_MAX
#define da_stdp_EC_a_plus 0.1
#define da_stdp_EC_a_minus 0.1

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
