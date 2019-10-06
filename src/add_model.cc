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
    n.simCode = R"(
    if($(timer) > $(Refrac)) 
    {
        $(V) += ($(Vrest) - $(V) + $(Isyn)) / $(TV) * DT; 
    }
    if($(V) < -100)
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
    n.thresholdConditionCode = "$(V) > ($(theta) - $(offset) + $(Vthresh)) && $(timer) > $(Refrac)"; 
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
    n.simCode = R"(
    if($(timer) > $(Refrac)) 
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
            uint64_t theRnd;
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
    $(addtoinSyn) = $(g);
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
    wuSTDP.dps = NULL;
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
