#include "add_model.h"

int addNeuronModel_Poi(vector<neuronModel> &nModels)
{
    neuronModel n;
    int Poi;
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("seed");
    n.varTypes.push_back("uint64_t");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    n.pNames.push_back("Vspike");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("Ttrace");
    n.dpNames.clear();
    n.extraGlobalNeuronKernelParameters.push_back("rates");
    n.extraGlobalNeuronKernelParameterTypes.push_back("uint64_t *");
    n.simCode = R"(    
    uint64_t theRnd;
    if($(V) > $(Vrest))
    {
        $(V) = $(Vrest);
    }
    else
    {  
        MYRAND($(seed), theRnd);
        if (theRnd < *($(rates) + $(id)))
        {
            $(V) = $(Vspike);
        }
        $(trace) += -$(trace) / $(Ttrace) * DT;
    }
    )";
    n.thresholdConditionCode = "$(V) >= $(Vspike)";
    n.dps = NULL;
    nModels.push_back(n);
    Poi = nModels.size() - 1;
    return Poi;
}
int addNeuronModel_LIF_Exc(vector<neuronModel> &nModels)
{
    neuronModel n;
    int LIF_Exc;
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("theta");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("testDataEvaluateMode");
    n.varTypes.push_back("bool");
    n.pNames.clear();
    n.pNames.push_back("TV");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("Vreset");
    n.pNames.push_back("Vthresh");
    n.pNames.push_back("Refrac");
    n.pNames.push_back("test_mode");
    n.pNames.push_back("Ttheta");
    n.pNames.push_back("theta_plus");
    n.pNames.push_back("offset");
    n.pNames.push_back("Ttrace1");
    n.pNames.push_back("Ttrace2");
    n.pNames.push_back("Ttrace");
    n.dpNames.clear();
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    //电压mv电导nS时间ms
    //会不会有25的影响？？？
    n.simCode = R"(
    if($(timer) > $(Refrac)) //不应期内电压不变化，电导呢？! 
    {
        $(V) += ($(Vrest) - $(V) + $(Isyn)) / $(TV) * DT; //at two times for numerical stability
    }
    if($(V) < -100)//为了防止抑制电流输入在V低于-100时效果转变为兴奋电流
        $(V) = -100;
    if($(test_mode) || $(testDataEvaluateMode)) 
    {

    }
    else {
        $(theta) -= $(theta) / $(Ttheta) * DT; 
        $(trace1) += -$(trace1) / $(Ttrace1) * DT;
        $(trace2) += -$(trace2) / $(Ttrace2) * DT;
        $(trace) += -$(trace) / $(Ttrace) * DT;
    }
    $(timer) += 1 * DT;
    )";
    n.thresholdConditionCode = "$(V) > ($(theta) - $(offset) + $(Vthresh)) && $(timer) > $(Refrac)"; //不应期可以把电位强制到静息电位！！！查找下不应期brian怎么处理的？
    // n.thresholdConditionCode = "$(V) > ($(theta) - $(offset) + $(Vthresh)) && 100*$(timer) > $(Refrac)"; //peter模型这里theta要乘100
    n.resetCode = R"(
    //reset code is here
    if($(test_mode) || $(testDataEvaluateMode)) 
    {
                
    }
    else
    {
        $(theta) += $(theta_plus) * ($(offset) - 10) / abs($(theta) - 10); 
    }
    $(V) = $(Vreset); 
    $(timer) = 0;
    )";
    nModels.push_back(n);
    LIF_Exc = nModels.size() - 1;
    return LIF_Exc;
}
int addNeuronModel_LIF_Inh(vector<neuronModel> &nModels)
{
    neuronModel n;
    int LIF_Inh;
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    n.pNames.push_back("TV");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("Vreset");
    n.pNames.push_back("Vthresh");
    n.pNames.push_back("Refrac");
    n.dpNames.clear();
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    //会不会有25的影响？？？
    n.simCode = R"(
    if($(timer) > $(Refrac)) //不应期内电压不变化，电导呢？!
    {
        $(V) += ($(Vrest) - $(V) + $(Isyn)) / $(TV) * DT;
    }
    $(timer) += DT;
    )";
    n.thresholdConditionCode = "$(V) > $(Vthresh) && $(timer) > $(Refrac)";
    n.resetCode = R"(//reset code is here
       $(V) = $(Vreset);
       $(timer) = 0;
     )";
    nModels.push_back(n);
    LIF_Inh = nModels.size() - 1;
    return LIF_Inh;
}
int addNeuronModel_LIF_Cla(vector<neuronModel> &nModels)
{
    neuronModel n;
    int LIF_Cla;
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("seed");
    n.varTypes.push_back("uint64_t");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("testDataEvaluateMode");
    n.varTypes.push_back("bool");
    n.pNames.clear();
    n.pNames.push_back("TV");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("Vreset");
    n.pNames.push_back("Vthresh");
    n.pNames.push_back("Vspike");
    n.pNames.push_back("Ttrace1");
    n.pNames.push_back("Ttrace2");
    n.pNames.push_back("test_mode");
    n.dpNames.clear();
    n.extraGlobalNeuronKernelParameters.push_back("rates");
    n.extraGlobalNeuronKernelParameterTypes.push_back("uint64_t *");
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    n.simCode = R"(
    if($(test_mode) || $(testDataEvaluateMode)) 
    {
        $(V) += ($(Vrest) - $(V) + $(Isyn)) / $(TV) * DT;
    }
    else
    {
        if($(V) > $(Vrest))
        {
            $(V) = $(Vrest);
        }
        else
        {
            uint64_t theRnd;//原poisson模型不能连续发放，350ms，只相当于175ms？？？  
            MYRAND($(seed), theRnd);
            if (theRnd < *($(rates) + $(id)))
            {
                $(V) = $(Vspike);
            } 
        }
        $(trace1) +=  -$(trace1) / $(Ttrace1) * DT;
        $(trace2) +=  -$(trace2) / $(Ttrace2) * DT;
    }
    )";
    n.thresholdConditionCode = R"(    
    ($(test_mode) || $(testDataEvaluateMode)) && $(V) > $(Vthresh) || (!($(test_mode) || $(testDataEvaluateMode))) && $(V) >= $(Vspike)
    )";
    n.resetCode = R"(//reset code is here
    if($(test_mode) || $(testDataEvaluateMode))
        $(V) = $(Vreset);
     )";
    nModels.push_back(n);
    LIF_Cla = nModels.size() - 1;
    return LIF_Cla;
}
int addSynapseModel(vector<weightUpdateModel> &weightUpdateModels)
{
    int STDP;
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g");
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");
    wuSTDP.pNames.push_back("nu_ee_post");
    wuSTDP.pNames.push_back("g_min");
    wuSTDP.pNames.push_back("g_max");
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    if($(testDataEvaluateMode))
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
    }
    else
    {
        if($(timer_post) > $(Refrac_post)) //不应期内电压不变化，电导呢？!
        {
            $(addtoinSyn) = $(g);
            $(updatelinsyn);
        }
        $(g) -= $(nu_ee_pre) * $(trace1_post);
        $(trace_pre) = 1;
        if ($(g) < $(g_min))
            $(g) = $(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))   
    {
        
    }
    else
    {
        $(g) += $(nu_ee_post) * $(trace_pre) * $(trace2_post);
        $(trace1_post) = 1;
        $(trace2_post) = 1;
        if ($(g)>$(g_max))
            $(g)=$(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    STDP = weightUpdateModels.size() - 1;
    return STDP;
}
// ------------------------------------------------------------------------------
//  DA STDP
// ------------------------------------------------------------------------------
int addSynapseModel_DA_STDP(vector<weightUpdateModel> &weightUpdateModels)
{
    int DA_STDP;
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g");
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");
    wuSTDP.pNames.push_back("nu_ee_post");
    wuSTDP.pNames.push_back("g_min");
    wuSTDP.pNames.push_back("g_max");
    wuSTDP.pNames.push_back("a_plus");
    wuSTDP.pNames.push_back("a_minus");
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    $(addtoinSyn) = $(g);//stdp需要加不应期吗……
    $(updatelinsyn);
    if($(testDataEvaluateMode))
    {
        
    }
    else
    {
        $(g) += $(nu_ee_pre) * $(trace1_post);
        $(trace_pre) += $(a_plus);
        if ($(g) > $(g_max))
            $(g) = $(g_max);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))   
    {
        
    }
    else
    {
        $(g) += $(nu_ee_post) * $(trace_pre);
        $(trace1_post) += $(a_minus);
        if ($(g) > $(g_max))
            $(g) = $(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    DA_STDP = weightUpdateModels.size() - 1;
    return DA_STDP;
}
// ------------------------------------------------------------------------------
//  Online implementation of STDP
// ------------------------------------------------------------------------------
int addSynapseModel_Sym_STDP(vector<weightUpdateModel> &weightUpdateModels)
{
    int STDP_symmetric;
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g");
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");
    wuSTDP.pNames.push_back("nu_ee_post");
    wuSTDP.pNames.push_back("g_min");
    wuSTDP.pNames.push_back("g_max");
    wuSTDP.pNames.push_back("a_plus");
    wuSTDP.pNames.push_back("a_minus");
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    $(addtoinSyn) = $(g);
    $(updatelinsyn);
    if($(testDataEvaluateMode))
    {

    }
    else
    {
        $(g) -= $(nu_ee_pre) * $(trace1_post);
        $(trace_pre) += $(a_plus);
        if ($(g) < $(g_min))
            $(g) = $(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))   
    {
        
    }
    else
    {
        $(g) += $(nu_ee_post) * $(trace_pre);
        $(trace1_post) += $(a_minus);
        if ($(g) > $(g_max))
            $(g) = $(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    STDP_symmetric = weightUpdateModels.size() - 1;
    return STDP_symmetric;
}
// ------------------------------------------------------------------------------
//  weight dependence:hard bounds and soft bounds
// ------------------------------------------------------------------------------
int addSynapseModel_soft_bounds(vector<weightUpdateModel> &weightUpdateModels)
{
    int STDP_soft_bounds;
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g");
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");
    wuSTDP.pNames.push_back("nu_ee_post");
    wuSTDP.pNames.push_back("g_min");
    wuSTDP.pNames.push_back("g_max");
    wuSTDP.pNames.push_back("a_plus");
    wuSTDP.pNames.push_back("a_minus");
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    $(addtoinSyn) = $(g);
    $(updatelinsyn);
    if($(testDataEvaluateMode))
    {

    }
    else
    {
        $(g) -= $(nu_ee_pre) * ($(g) - $(g_min)) * $(trace1_post);
        $(trace_pre) += $(a_plus);
        if ($(g) < $(g_min))
            $(g) = $(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？
    // code for post-synaptic spike
    //$(post2before) = $(post2); //提升性能降低理论性
    //$(g) += $(nu_ee_post) * $(pre) * $(post2before);
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))   
    {
        
    }
    else
    {
        $(trace1_post) += $(a_minus);
        $(g) += $(nu_ee_post) * ($(g_max) - $(g)) * $(trace_pre);
        if ($(g) > $(g_max))
            $(g) = $(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    STDP_soft_bounds = weightUpdateModels.size() - 1;
    return STDP_soft_bounds;
}
int addSynapse_fixed_Model(vector<weightUpdateModel> &weightUpdateModels)
{
    int fixed;
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g");
    wuSTDP.varTypes.push_back("scalar");
    // wuSTDP.varNames.push_back("post2before"); //4//提升性能降低理论性
    // wuSTDP.varTypes.push_back("scalar");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");
    wuSTDP.pNames.push_back("nu_ee_post");
    wuSTDP.pNames.push_back("g_min");
    wuSTDP.pNames.push_back("g_max");
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    $(addtoinSyn) = $(g);
    $(updatelinsyn);
    )";
    wuSTDP.dps = new pwSTDP; //？？？
    // code for post-synaptic spike
    //$(post2before) = $(post2); //提升性能降低理论性
    //$(g) += $(nu_ee_post) * $(pre) * $(post2before);
    wuSTDP.simLearnPost = R"(
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = false;
    weightUpdateModels.push_back(wuSTDP);
    fixed = weightUpdateModels.size() - 1;
    return fixed;
}
int addNeuronModel_LIF_Exc_fixed(vector<neuronModel> &nModels)
{
    neuronModel n;
    int LIF_Exc_fixed;
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("theta");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    n.pNames.push_back("TV");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("Vreset");
    n.pNames.push_back("Vthresh");
    n.pNames.push_back("Refrac");
    n.pNames.push_back("test_mode");
    n.pNames.push_back("Ttheta");
    n.pNames.push_back("theta_plus");
    n.pNames.push_back("offset");
    n.pNames.push_back("Ttrace1");
    n.pNames.push_back("Ttrace2");
    n.pNames.push_back("Ttrace");
    n.dpNames.clear();
    n.simCode = R"(
    if($(timer) > $(Refrac)) //不应期内电压不变化，电导呢？!
    {
        $(V) += ($(Vrest) - $(V) + $(Isyn)) / $(TV) * DT; //at two times for numerical stability
    }
    if($(V) < -100)//为了防止抑制低于-100时突然反向发放
        $(V) = -100;
    $(timer) += DT;
    )";
    n.thresholdConditionCode = "$(V) > ($(theta) - $(offset) + $(Vthresh)) && $(timer) > $(Refrac)"; //不应期可以把电位强制到静息电位！！！查找下不应期brian怎么处理的？
    n.resetCode = R"(
    //reset code is here
    $(V) = $(Vreset); 
    $(timer) = 0;
    )";
    nModels.push_back(n);
    LIF_Exc_fixed = nModels.size() - 1;
    return LIF_Exc_fixed;
}
