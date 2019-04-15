#include "model.h"
#include "add_model.cc"
//------------------------------------------------------------------------------
// POISON model P
//------------------------------------------------------------------------------
double ini_Poi[] = {
    Poi_V,
    Poi_seed,
    Poi_trace,
};
double p_Poi[] = {
    Poi_Vspike,
    Poi_Vrest,
    Poi_Ttrace,
};
//------------------------------------------------------------------------------
// LIF model E&I
//------------------------------------------------------------------------------
double ini_Exc[] = {
    //初始值python程序里没有。貌似没直接写的话就是0？？？毕竟初值影响不大
    Exc_V,
    Exc_theta,
    Exc_timer,
    Exc_trace1,
    Exc_trace2,
    Exc_trace,
    Exc_testDataEvaluateMode,
};
double p_Exc[] = {
    Exc_TV,
    Exc_Vrest,
    Exc_Vreset,
    Exc_Vthresh,
    Exc_Refrac,
    Exc_test_mode,
    Exc_Ttheta,
    Exc_theta_plus,
    Exc_offset,
    Exc_Ttrace1,
    Exc_Ttrace2,
    Exc_Ttrace,
};
double ini_Inh[] = {
    Inh_V,
    Inh_timer,
};
double p_Inh[] = {
    Inh_TV,
    Inh_Vrest,
    Inh_Vreset,
    Inh_Vthresh,
    Inh_Refrac,
};
double ini_Cla[] = {
    Cla_V,
    Cla_seed,
    Cla_trace1,
    Cla_trace2,
    Cla_testDataEvaluateMode,
};
double p_Cla[] = {
    Cla_TV,
    Cla_Vrest,
    Cla_Vreset,
    Cla_Vthresh,
    Cla_Vspike,
    Cla_Ttrace1,
    Cla_Ttrace2,
    Cla_test_mode,
};
//------------------------------------------------------------------------------
// postSynV
//------------------------------------------------------------------------------
double postExpP2E[2] = {
    //MB例子两个poison到其余神经元都是这参数。对比：ge：1ms，gi：2ms。认为Erev应写o，就像e是0，i是-100或者-85一样。
    //难怪电导的衰减远远比不上电导的增加……
    P2E_tau_S, //1.0,//1.0的话电导衰减太快，电导增加比不上衰减。 //100.0, //1.0, // 0 - tau_S: decay time constant for S [ms]
    P2E_Erev,  // 1 - Erev: Reversal potential//python程序中-85不用
};
double postExpE2I[2] = {
    //E2I的总电导衰减过快，因此独立设置
    E2I_tau_S,
    E2I_Erev,
};
double postExpI2E[2] = {
    I2E_tau_S,
    I2E_Erev,
};
double *postSynV = NULL;
//------------------------------------------------------------------------------
// STDP
//------------------------------------------------------------------------------
double ini_STDP_PE[] = {
    stdp_PE_g,
    stdp_PE_testDataEvaluateMode,
};
double p_STDP_PE[] = {
    stdp_PE_nu_ee_pre,
    stdp_PE_nu_ee_post,
    stdp_PE_g_min,
    stdp_PE_g_max,
};
double ini_STDP_EC[] = {
    stdp_EC_g,
    stdp_EC_testDataEvaluateMode,
};
double p_STDP_EC[] = {
    stdp_EC_nu_ee_pre,
    stdp_EC_nu_ee_post,
    stdp_EC_g_min,
    stdp_EC_g_max,
};
//------------------------------------------------------------------------------
// DA_STDP
//------------------------------------------------------------------------------
double ini_DA_STDP_PE[] = {
    da_stdp_PE_g,
    da_stdp_PE_testDataEvaluateMode,
};
double p_DA_STDP_PE[] = {
    da_stdp_PE_nu_ee_pre,
    da_stdp_PE_nu_ee_post,
    da_stdp_PE_g_min,
    da_stdp_PE_g_max,
    da_stdp_PE_a_plus,
    da_stdp_PE_a_minus,
};
double ini_DA_STDP_EC[] = {
    da_stdp_EC_g,
    da_stdp_EC_testDataEvaluateMode,
};
double p_DA_STDP_EC[] = {
    da_stdp_EC_nu_ee_pre,
    da_stdp_EC_nu_ee_post,
    da_stdp_EC_g_min,
    da_stdp_EC_g_max,
    da_stdp_EC_a_plus,
    da_stdp_EC_a_minus,
};
//------------------------------------------------------------------------------
// Sym_STDP
//------------------------------------------------------------------------------
double ini_Sym_STDP_PE[] = {
    sym_stdp_PE_g,
    sym_stdp_PE_testDataEvaluateMode,
};
double p_Sym_STDP_PE[] = {
    sym_stdp_PE_nu_ee_pre,
    sym_stdp_PE_nu_ee_post,
    sym_stdp_PE_g_min,
    sym_stdp_PE_g_max,
    sym_stdp_PE_a_plus,
    sym_stdp_PE_a_minus,
};
double ini_Sym_STDP_EC[] = {
    sym_stdp_EC_g,
    sym_stdp_EC_testDataEvaluateMode,
};
double p_Sym_STDP_EC[] = {
    sym_stdp_EC_nu_ee_pre,
    sym_stdp_EC_nu_ee_post,
    sym_stdp_EC_g_min,
    sym_stdp_EC_g_max,
    sym_stdp_EC_a_plus,
    sym_stdp_EC_a_minus,
};
//------------------------------------------------------------------------------
// soft_bounds_STDP
//------------------------------------------------------------------------------
double ini_soft_bounds_STDP[] = {
    soft_stdp_g,
    soft_stdp_testDataEvaluateMode,
};
double p_soft_bounds_STDP_PE[] = {
    soft_stdp_nu_ee_pre,
    soft_stdp_nu_ee_post,
    soft_stdp_g_min,
    soft_stdp_g_max_PE,
    soft_stdp_a_plus,
    soft_stdp_a_minus,
};
//------------------------------------------------------------------------------
// noSTDP
//------------------------------------------------------------------------------
double ini_noSTDP[] = {
    no_stdp_g,
};
double *p_noSTDP_PE = NULL;
double *p_noSTDP_EC = NULL;
//------------------------------------------------------------------------------
// E——>I & I——>E
//------------------------------------------------------------------------------
double ini_NSYNAPSE_EI[] = {
    g_EI,
};
double *p_NSYNAPSE_EI = NULL;
double ini_NSYNAPSE_IE[] = {
    g_IE,
};
double *p_NSYNAPSE_IE = NULL;

size_t size_gP2E = NPoi * NExc;
size_t size_gE2C = NExc * NCla;

void modelDefinition(NNmodel &model)
{
    initGeNN();

#ifdef DEBUG
    GENN_PREFERENCES::debugCode = true;
#else
    GENN_PREFERENCES::optimizeCode = true;
#endif
    GENN_PREFERENCES::autoRefractory = 0;

    int Poi = addNeuronModel_Poi(nModels);
    int LIF_Exc = addNeuronModel_LIF_Exc(nModels);
    int LIF_Exc_fixed = addNeuronModel_LIF_Exc_fixed(nModels);
    int LIF_Inh = addNeuronModel_LIF_Inh(nModels);
    int LIF_Cla = addNeuronModel_LIF_Cla(nModels);
    int STDP = addSynapseModel(weightUpdateModels);
    int DA_STDP = addSynapseModel_DA_STDP(weightUpdateModels);
    int Sym_STDP = addSynapseModel_Sym_STDP(weightUpdateModels);
    int soft_STDP_bounds = addSynapseModel_soft_bounds(weightUpdateModels);
    int fixed = addSynapse_fixed_Model(weightUpdateModels);

    model.setGPUDevice(ID_DEVICE); //选择GPU。在不进行GPU优化的前提下
    model.setDT(DT);
    model.setPrecision(_FTYPE);
    model.setName("model");
    //model.setSeed(1234);//extraGlobalNeuronKernelParameters有两个参数rates和offset但是不知道怎么搞？（get）

    model.addNeuronPopulation("PPoi", NPoi, Poi, p_Poi, ini_Poi);
#ifndef READ_gPE_FROM_FILE
    model.addNeuronPopulation("PExc", NExc, LIF_Exc, p_Exc, ini_Exc);
#else
    model.addNeuronPopulation("PExc", NExc, LIF_Exc_fixed, p_Exc, ini_Exc);
#endif
    model.addNeuronPopulation("PInh", NInh, LIF_Inh, p_Inh, ini_Inh); //GLOBALG
    model.addNeuronPopulation("PCla", NCla, LIF_Cla, p_Cla, ini_Cla);

#ifndef test_mode
#ifndef READ_gPE_FROM_FILE
    // model.addSynapsePopulation("P2E", STDP, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PPoi", "PExc", ini_STDP_PE, p_STDP_PE, postSynV, postExpP2E); //是否要加delay？尝试过5ms
    model.addSynapsePopulation("P2E", DA_STDP, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PPoi", "PExc", ini_DA_STDP_PE, p_DA_STDP_PE, postSynV, postExpP2E);
#else
    model.addSynapsePopulation("P2E", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PPoi", "PExc", ini_noSTDP, p_noSTDP_PE, postSynV, postExpP2E); //是否要加delay？
#endif
    // model.addSynapsePopulation("E2C", STDP, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PExc", "PCla", ini_STDP_EC, p_STDP_EC, postSynV, postExpP2E);
    model.addSynapsePopulation("E2C", DA_STDP, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PExc", "PCla", ini_DA_STDP_EC, p_DA_STDP_EC, postSynV, postExpP2E);
#else
    model.addSynapsePopulation("P2E", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PPoi", "PExc", ini_noSTDP, p_noSTDP_PE, postSynV, postExpP2E); //是否要加delay？
    model.addSynapsePopulation("E2C", NSYNAPSE, DENSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PExc", "PCla", ini_noSTDP, p_noSTDP_EC, postSynV, postExpP2E);
#endif
    model.addSynapsePopulation("E2I", NSYNAPSE, SPARSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PExc", "PInh", ini_NSYNAPSE_EI, p_NSYNAPSE_EI, postSynV, postExpE2I);
    model.addSynapsePopulation("I2E", NSYNAPSE, SPARSE, INDIVIDUALG, NO_DELAY, EXPDECAY, "PInh", "PExc", ini_NSYNAPSE_IE, p_NSYNAPSE_IE, postSynV, postExpI2E);

    model.finalize();
    fprintf(stdout, "#model created.\n"); //在genn-buildmodel.sh Model.cc即模型生成时会执行一遍！
}
