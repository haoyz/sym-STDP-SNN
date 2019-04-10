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

//#define DT 0.100000f //
#define _FTYPE GENN_FLOAT
#define scalar float

#define poi_firing_rate 1 // 0 - firing rate 根本没用到……
#define poi_ref 0         //2.5 // 1 - refratory period//泊松神经元不应期时间.Brian似乎没用泊松的不应期
#define poi_spike 20.0    // 2 - Vspike
#define poi_rest -60.0    // 3 - Vrest
#define poi_tc_trace 20

#define poi_V -60.0         // 0 - V
#define poi_seed 1          //1100000, // 1 - seed
#define poi_theRand 0       // 2 - theRand
#define poi_SpikeTime -10.0 // 3 - SpikeTime 克服不应期，保证t=0时就可以spike
#define poi_trace 1

#define gPE_INIT_MAX 0.3
#define gPE_INIT_MAX_1000 gPE_INIT_MAX * 1000
#define gEC_INIT_MAX 0.3
#define gEC_INIT_MAX_1000 gEC_INIT_MAX * 1000
#define gPE_MAX 1.0
#define _g_EI 10.4                         //*1e-3 //g
#define _g_IE 17                           //* 0.25  //*1e-3 //g
#define gEC_MAX 8.0                        // 8.0 * 1.25
#define _NORMAL 78.4                       // * 1.5
#define _cla_NORMAL (0.1 * NExc * gEC_MAX) // #define _cla_NORMAL (0.1 * NCla * gEC_MAX) //即电导既不太大也不太小，下一次有充分的空间学习

#define tau_e 100                    //  The time constant of membrane potential of Exc. Neurons
#define v_rest_e -65.0               // 0 - v_rest_e: Na conductance in muS？？？
#define v_reset_e -65.0              // 1 - v_reset_e: K conductance in muS
#define v_thresh_e -52               // 2 - v_thresh_e: leak conductance in muS
#define refrac_e 2                   // 3 - refrac_e: membr. capacity density in nF
#define test_mode_e test_mode_neuron // 4 - test_mode: Na equi potential in mV
#define tc_theta 1e7 * 0.8             //* 2                 // 5 - tc_theta: K conductance in muS
#define theta_plus_e 0.05 * 5      //* 1.5            // 6 - theta_plus_e: K equi potential in mV
#define offset_exc 20.0              // 7 - offset_exc: leak conductance in muS
#define directInput_e 0.0            // 8 - directInput: 电流单位？？？0.1时vexc只变化了0.0001  10时从69.975变化到69.965认为有效。
#define tc_trace1 20
#define tc_trace2 40
#define exc_tc_trace 20

#define v_e (v_rest_e - 40.0) // 0 - membrane potential V
#define theta_e 20            // 1 - theta_e 初始值？(20!!!和offset_exc刚开始时抵消，但还是要用20由于theta_e自身的微分方程)
#define timer 10              // 2 - timer 初始值？目前兴奋神经元加了50,而抑制还是10.只有兴奋有不应期，设置为10为了刚开始可以响应输入而一但spike的话timer归零就正常轨道了
#define trace1 1
#define trace2 1
#define exc_trace 1
#define testDataEvaluateModeInit false

#define v_rest_i -60.0               // 0 - v_rest_i: Na equi potential in mV
#define v_reset_i -45.0              // 1 - v_reset_i: K equi potential in mV
#define v_thresh_i -40               // 2 - v_thresh_i: leak equi potential in mV
#define refrac_i 2                   // 3 - refrac_i: Na conductance in muS？？？//抑制神经元并不需要不应期
#define test_mode_i test_mode_neuron // 4 - test_mode: Na equi potential in mV
#define directInput_i 0.0            // 5 - directInput: 电流单位？？？0.1时vexc只变化了0.0001  10时从69.975变化到69.965认为有效。

#define v_i (v_rest_i - 40.0) // 0 - membrane potential V
#define timer_i 10            // 1

#define tau_i 10                       //  The time constant of membrane potential of Inh. Neurons
#define cla_v_rest -60.0               // 0 - v_rest_i: Na equi potential in mV
#define cla_v_reset -45.0              // 1 - v_reset_i: K equi potential in mV
#define cla_v_thresh -40.0             // 2 - v_thresh_i: leak equi potential in mV
#define cla_refrac 2                   // 3 - refrac_i: Na conductance in muS？？？
#define cla_test_mode test_mode_neuron // 4 - test_mode: Na equi potential in mV
#define cla_firing_rate 1              // 5 - firing rate 根本没用到……
#define cla_ref 0                      // 6 - refratory period
#define cla_spike 20                   // 7 - Vspike
#define cla_rest -60                   // 8 - Vrest

#define cla_v (v_rest_i - 40.0) // 0 - membrane potential V
#define cla_seed 1              //1100000, // 1 - seed
#define cla_theRand 0           // 2 - theRand
#define cla_SpikeTime -10       // 3 - SpikeTime 克服不应期，保证t=0时就可以spike

#define P2E_tau_S 1.0 //1.0,//1.0的话电导衰减太快，电导增加比不上衰减。 //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
#define P2E_Erev 0.0  // 1 - Erev: Reversal potential

#define E2I_tau_S 1.0 //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
#define E2I_Erev 0.0  // 1 - Erev: Reversal potential

#define I2E_tau_S 2.0   // 0 - tau_S: decay time constant for S [ms]
#define I2E_Erev -100.0 // 1 - Erev: Reversal potential//python程序中-85不用

#define stdp_g 0.1 //0.01, // ("g");//0

#define stdp_nu_ee_pre 0.0001 //("nu_ee_pre");  //0
#define stdp_nu_ee_post 0.01  //("nu_ee_post"); //1
#define stdp_g_min 0          //g_min //5
#define stdp_g_max_PE gPE_MAX //g_max
#define stdp_g_max_EC gEC_MAX //g_max

#define stdp_sym_g 0.1 //0.01, // ("g");//0

#define stdp_sym_nu_ee_pre 0.01
#define stdp_sym_nu_ee_post 0.01
#define stdp_sym_g_min 0          //g_min //5
#define stdp_sym_g_max_PE gPE_MAX //g_max  //6
#define stdp_sym_g_max_EC gEC_MAX //g_max  //6
#define stdp_sym_a_plus 0.1       //* 2
#define stdp_sym_a_minus 0.1      //* 2

#define stdp_DA_nu_ee_pre 0.01 * 8
#define stdp_DA_nu_ee_post 0.01 * 8
#define stdp_DA_a_plus 0.1
#define stdp_DA_a_minus 0.1

#define stdp_soft_g 0.1 //0.01, // ("g");//0

#define stdp_soft_nu_ee_pre 0.01  //("nu_ee_pre");  //0
#define stdp_soft_nu_ee_post 0.01 //("nu_ee_post"); //1
#define stdp_soft_g_min 0         //g_min //5
#define stdp_soft_g_max_PE 1.0    //g_max  //6
#define stdp_soft_a_plus 0.1      //("a_plus");
#define stdp_soft_a_minus 0.1     //("a_minus");

#define no_stdp_g 0.1 // ("g");

//------------------------------------------------------------------------------
// POISON model P
//------------------------------------------------------------------------------
extern double myPOI_p[];
extern double myPOI_ini[];
//------------------------------------------------------------------------------
// LIF model E&I
//------------------------------------------------------------------------------
extern double p_exc[];
extern double ini_exc[];
extern double p_inh[];
extern double ini_inh[];
//------------------------------------------------------------------------------
// LIF model Cal
//------------------------------------------------------------------------------
extern double p_cal[];
extern double ini_cal[];
//------------------------------------------------------------------------------
// postSynV
//------------------------------------------------------------------------------
extern double postExpP2E[2];
extern double postExpE2I[2];
extern double postExpI2E[2];
extern double *postSynV;
//------------------------------------------------------------------------------
// P——>E mySTDP
//------------------------------------------------------------------------------
extern double mySTDP_ini[];
extern double mySTDP_p_PE[];
//------------------------------------------------------------------------------
// P——>E mySTDP_symmetric
//------------------------------------------------------------------------------
extern double mySTDP_symmetric_ini[];
extern double mySTDP_symmetric_p_PE[];
//------------------------------------------------------------------------------
// P——>E noSTDP
//------------------------------------------------------------------------------
extern double noSTDP_ini[1];
extern double *noSTDP_p_PE;
//------------------------------------------------------------------------------
// E——>I & I——>E
//------------------------------------------------------------------------------
extern double NSYNAPSE_ini_EI[1];
extern double *NSYNAPSE_p_EI;
extern double NSYNAPSE_ini_IE[1];
extern double *NSYNAPSE_p_IE;

extern size_t size_gP2E;
extern size_t size_gE2C;

void get_rand_g(float *p, long int n, int g_max);
void get_rand(uint64_t *p, long int n, int max);
void modelDefinition(NNmodel &model);

#endif
