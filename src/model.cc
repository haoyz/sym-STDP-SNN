#include "model.h"
// #include "add_model.h"
#include "add_model.cc"
//------------------------------------------------------------------------------
// POISON model P
//------------------------------------------------------------------------------
double myPOI_p[] = {
    //POISSON neuron parameters
    poi_firing_rate, // 0 - firing rate 根本没用到……
    poi_ref,         // 1 - refratory period
    poi_spike,       // 2 - Vspike
    poi_rest,        // 3 - Vrest
    poi_tc_trace,
};
double myPOI_ini[] = {
    poi_V,    // 0 - V
    poi_seed, //1100000, // 1 - seed
    // poi_theRand,   // 2 - theRand
    poi_SpikeTime, // 3 - SpikeTime 克服不应期，保证t=0时就可以spike
    poi_trace,
};
//------------------------------------------------------------------------------
// LIF model E&I
//------------------------------------------------------------------------------
double p_exc[] = {
    tau_e,
    v_rest_e,     // 0 - v_rest_e: Na conductance in muS？？？
    v_reset_e,    // 1 - v_reset_e: K conductance in muS
    v_thresh_e,   // 2 - v_thresh_e: leak conductance in muS
    refrac_e,     // 3 - refrac_e: membr. capacity density in nF
    test_mode_e,  // 4 - test_mode: Na equi potential in mV
    tc_theta,     // 5 - tc_theta: K conductance in muS
    theta_plus_e, // 6 - theta_plus_e: K equi potential in mV
    offset_exc,   // 7 - offset_exc: leak conductance in muS
    tc_trace1,
    tc_trace2,
    exc_tc_trace,
};
double ini_exc[] = {
    //初始值python程序里没有。貌似没直接写的话就是0？？？
    v_e,     // 0 - membrane potential V
    theta_e, // 1 - theta_e 初始值？
    timer,   // 2 - timer 初始值？
    trace1,
    trace2,
    exc_trace,
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double p_inh[] = {
    tau_i,
    v_rest_i,    // 0 - v_rest_i: Na equi potential in mV
    v_reset_i,   // 1 - v_reset_i: K equi potential in mV
    v_thresh_i,  // 2 - v_thresh_i: leak equi potential in mV
    refrac_i,    // 3 - refrac_i: Na conductance in muS？？？
    test_mode_i, // 4 - test_mode: Na equi potential in mV
};
double ini_inh[] = {
    //初始值python程序里没有。貌似没直接写的话就是0？？？
    v_i, // 0 - membrane potential V
    timer_i,
};
double p_cla[] = {
    tau_i,
    cla_v_rest,    // 0 - v_rest_i: Na equi potential in mV
    cla_v_reset,   // 1 - v_reset_i: K equi potential in mV
    cla_v_thresh,  // 2 - v_thresh_i: leak equi potential in mV
    cla_refrac,    // 3 - refrac_i: Na conductance in muS？？？
    cla_test_mode, // 4 - test_mode: Na equi potential in mV
    tc_trace1,
    tc_trace2,
    cla_firing_rate, // 5 - firing rate 根本没用到……
    cla_ref,         // 6 - refratory period
    cla_spike,       // 7 - Vspike
    cla_rest,        // 8 - Vrest
};
double ini_cla[] = {
    //初始值python程序里没有。貌似没直接写的话就是0？？？
    cla_v, // 0 - membrane potential V
    trace1,
    trace2,
    cla_seed,                 //1100000, // 1 - seed
    cla_theRand,              // 2 - theRand
    cla_SpikeTime,            // 3 - SpikeTime 克服不应期，保证t=0时就可以spike
    testDataEvaluateModeInit, //revised at 2018-4-6
};
//------------------------------------------------------------------------------
// postSynV
//------------------------------------------------------------------------------
double postExpP2E[2] = {
    //MB例子两个poison到其余神经元都是这参数
    //难怪电导的衰减远远比不上电导的增加……
    P2E_tau_S, //1.0,//1.0的话电导衰减太快，电导增加比不上衰减。 //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
    P2E_Erev   // 1 - Erev: Reversal potential
};
double postExpE2I[2] = {
    //MB例子两个poison到其余神经元都是这参数
    //难怪电导的衰减远远比不上电导的增加……
    //E2I的总电导衰减过快，因此独立设置
    E2I_tau_S, //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
    E2I_Erev   // 1 - Erev: Reversal potential
};
double postExpI2E[2] = {
    //MB例子两个poison到其余神经元都是这参数。对比：ge：1ms，gi：2ms。认为Erev应写o，就像e是0，i是-100或者-85一样。
    I2E_tau_S, // 0 - tau_S: decay time constant for S [ms]
    I2E_Erev   // 1 - Erev: Reversal potential//python程序中-85不用
};
double *postSynV = NULL;
//------------------------------------------------------------------------------
// mySTDP P——>E  E——>E  E——>C
//------------------------------------------------------------------------------
double mySTDP_ini_PE[] = {
    stdp_g,                   //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_p_PE[] = {
    stdp_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_g_min,      //g_min //5
    stdp_g_max_PE,   //g_max  //6
};
double mySTDP_ini_EC[] = {
    stdp_g,                   //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_p_EC[] = {
    stdp_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_g_min,      //g_min //5
    stdp_g_max_EC,   //g_max  //6
};
//------------------------------------------------------------------------------
// mySTDP_DA P——>E  E——>E  E——>C
//------------------------------------------------------------------------------
double mySTDP_DA_ini_PE[] = {
    stdp_sym_g,               //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_DA_p_PE[] = {
    stdp_sym_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_sym_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_sym_g_min,      //g_min //5
    stdp_sym_g_max_PE,   //g_max  //6
    stdp_sym_a_plus,     //("a_plus");
    stdp_sym_a_minus,    //("a_minus");
};
double mySTDP_DA_ini_EC[] = {
    stdp_sym_g,               //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_DA_p_EC[] = {
    stdp_sym_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_sym_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_sym_g_min,      //g_min //5
    stdp_sym_g_max_EC,   //g_max  //6
    stdp_sym_a_plus,     //("a_plus");
    stdp_sym_a_minus,    //("a_minus");
};
//------------------------------------------------------------------------------
// mySTDP_symmetric P——>E  E——>E  E——>C
//------------------------------------------------------------------------------
double mySTDP_symmetric_ini_PE[] = {
    stdp_sym_g,               //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_symmetric_p_PE[] = {
    stdp_sym_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_sym_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_sym_g_min,      //g_min //5
    stdp_sym_g_max_PE,   //g_max  //6
    stdp_sym_a_plus,     //("a_plus");
    stdp_sym_a_minus,    //("a_minus");
};
double mySTDP_symmetric_ini_EC[] = {
    stdp_sym_g,               //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_symmetric_p_EC[] = {
    stdp_DA_nu_ee_pre,  //revised at 2018-05-12
    stdp_DA_nu_ee_post, //revised at 2018-05-12
    stdp_sym_g_min,     //g_min //5
    stdp_sym_g_max_EC,  //g_max  //6
    stdp_DA_a_plus,     //revised at 2018-05-12
    stdp_DA_a_minus,    //revised at 2018-05-12
};
//------------------------------------------------------------------------------
// P——>E mySTDP_soft_bounds
//------------------------------------------------------------------------------
double mySTDP_soft_bounds_ini[] = {
    stdp_soft_g,              //0.01, // ("g");//0
    testDataEvaluateModeInit, //revised at 2018-4-6
};
double mySTDP_soft_bounds_p_PE[] = {
    stdp_soft_nu_ee_pre,  //0.01   //0.0001, //("nu_ee_pre");  //0
    stdp_soft_nu_ee_post, //0.1    //0.01,  //("nu_ee_post"); //1
    stdp_soft_g_min,      //g_min //5
    stdp_soft_g_max_PE,   //g_max  //6
    stdp_soft_a_plus,     //("a_plus");
    stdp_soft_a_minus,    //("a_minus");
};
//------------------------------------------------------------------------------
// P——>E noSTDP
//------------------------------------------------------------------------------
double noSTDP_ini[1] = {
    no_stdp_g // ("g");
};
double *noSTDP_p_PE = NULL;
//------------------------------------------------------------------------------
// E——>I & I——>E
//------------------------------------------------------------------------------
double NSYNAPSE_ini_EI[1] = {
    _g_EI //g
};
double *NSYNAPSE_p_EI = NULL;
double NSYNAPSE_ini_IE[1] = {
    _g_IE //g
};
double *NSYNAPSE_p_IE = NULL;

size_t size_gP2E = _NPoi * _NExc;
size_t size_gE2C = _NExc * _NCla;

void get_rand_g(float *p, long int n, int g_max) //函数功能为产生n个N_rand内的随机数，存储于数组*ratesPop1中。
{
    long int i;                  //大小更本不够大
    srand((unsigned)time(NULL)); //设置随机数种子，使每次获取的随机序列不同。
    for (i = 0; i < n; i++)
        p[i] = rand() % (g_max) / 1000.0; //生成N_rand间的随机数。注意float否则只是0……转换为khz
                                          //p[i] = 0.03;//生成N_rand间的随机数。注意float否则只是0……  0.030khz
                                          //int i;//大小更本不够大
                                          //for(i = 0; i < n; i ++)
                                          //MYRAND(myrand,p[i]);
}
void get_rand(float *p, long int n, int max)
{
    long int i;
    srand((unsigned)time(NULL));
    for (i = 0; i < n; i++)
        p[i] = rand() % (max); //返回的是秒数，不能在短时间内多次调用该函数来获得随机数列，应该一次性获得所有随机数列
}
void modelDefinition(NNmodel &model)
{
    // definition of Model
    initGeNN();

#ifdef DEBUG
    GENN_PREFERENCES::debugCode = true;
#else
    GENN_PREFERENCES::optimizeCode = true;
#endif
    GENN_PREFERENCES::autoRefractory = 0;

    int myPOISSONNEURON = addMyInputNeuronModel(nModels);
    int LIF_exc = addMyNeuronModel_LIF_exc(nModels);
    int LIF_exc_no_learnning = addMyNeuronModel_LIF_exc_no_learnning(nModels);
    int LIF_inh = addMyNeuronModel_LIF_inh(nModels);
    int LIF_cla = addMyNeuronModel_LIF_cla(nModels);
    int mySTDP = addMySynapseModel(weightUpdateModels);
    int mySTDP_DA = addMySynapseModel_mySTDP_DA(weightUpdateModels);
    int mySTDP_symmetric = addMySynapseModel_mySTDP_symmetric(weightUpdateModels);
    int mySTDP_soft_bounds = addMySynapseModel_soft_bounds(weightUpdateModels);
    int mySTDP_nolearnning = addMySynapse_mySTDP_nolearnning_Model(weightUpdateModels);

    model.setGPUDevice(ID_device); //选择GPU。在不进行GPU优化的前提下
    model.setDT(0.5);
    model.setPrecision(_FTYPE);
    model.setName("model");
    //model.setSeed(1234);//extraGlobalNeuronKernelParameters有两个参数rates和offset但是不知道怎么搞？（get）
    model.addNeuronPopulation("PPoi", _NPoi, myPOISSONNEURON, myPOI_p, myPOI_ini);

#ifndef _gPE_read_from_file
    model.addNeuronPopulation("PExc", _NExc, LIF_exc, p_exc, ini_exc);
#else
    model.addNeuronPopulation("PExc", _NExc, LIF_exc_no_learnning, p_exc, ini_exc);
#endif
    model.addNeuronPopulation("PInh", _NInh, LIF_inh, p_inh, ini_inh); //GLOBALG
    model.addNeuronPopulation("PCla", _NCla, LIF_cla, p_cla, ini_cla); //inh与poi 的结合体
#ifndef test_mode
#ifndef _gPE_read_from_file
        // model.addSynapsePopulation("P2E", mySTDP, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", mySTDP_ini_PE, mySTDP_p_PE, postSynV, postExpP2E); //是否要加delay？
    model.addSynapsePopulation("P2E", mySTDP_DA, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", mySTDP_symmetric_ini_PE, mySTDP_symmetric_p_PE, postSynV, postExpP2E);
// model.addSynapsePopulation("P2E", mySTDP_symmetric, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", mySTDP_symmetric_ini_PE, mySTDP_symmetric_p_PE, postSynV, postExpP2E);
// model.addSynapsePopulation("P2E", mySTDP_soft_bounds, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", mySTDP_soft_bounds_ini_PE, mySTDP_soft_bounds_p_PE, postSynV, postExpP2E);
#else
    model.addSynapsePopulation("P2E", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", noSTDP_ini, noSTDP_p_PE, postSynV, postExpP2E); //是否要加delay？
#endif
    // model.addSynapsePopulation("E2C", mySTDP, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PExc", "PCla", mySTDP_ini_EC, mySTDP_p_EC, postSynV, postExpP2E);
    model.addSynapsePopulation("E2C", mySTDP_DA, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PExc", "PCla", mySTDP_symmetric_ini_EC, mySTDP_symmetric_p_EC, postSynV, postExpP2E);
// model.addSynapsePopulation("E2C", mySTDP_symmetric, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PExc", "PCla", mySTDP_symmetric_ini_EC, mySTDP_symmetric_p_EC, postSynV, postExpP2E);
#else
    model.addSynapsePopulation("P2E", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PPoi", "PExc", noSTDP_ini, noSTDP_p_PE, postSynV, postExpP2E); //是否要加delay？
    model.addSynapsePopulation("E2C", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY /*5*/, EXPDECAY, "PExc", "PCla", noSTDP_ini, noSTDP_p_PE, postSynV, postExpP2E);
#endif
    model.addSynapsePopulation("E2I", NSYNAPSE, SPARSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PExc", "PInh", NSYNAPSE_ini_EI, NSYNAPSE_p_EI, postSynV, postExpE2I);
    model.addSynapsePopulation("I2E", NSYNAPSE, SPARSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PInh", "PExc", NSYNAPSE_ini_IE, NSYNAPSE_p_IE, postSynV, postExpI2E);
    model.finalize();
    fprintf(stdout, "#model created.\n"); //在genn-buildmodel.sh Model.cc即模型生成时会执行一遍！
}
