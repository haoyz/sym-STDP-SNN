#include "add_model.h"

int addMyInputNeuronModel(vector<neuronModel> &nModels)
{
    neuronModel n;
    unsigned int myPOISSONNEURON; //!< variable attaching the name "myPOISSONNEURON"
    // Poisson neurons
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("seed");
    n.varTypes.push_back("uint64_t");
    // n.varNames.push_back("theRnd"); //自己加的，仅仅为了方便观察。之后为了计算快可以删去
    // n.varTypes.push_back("uint64_t");
    n.varNames.push_back("spikeTime");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    n.pNames.push_back("therate");
    n.pNames.push_back("trefract"); //不应期，之后可删
    n.pNames.push_back("Vspike");
    n.pNames.push_back("Vrest");
    n.pNames.push_back("tc_trace");
    n.dpNames.clear();
    n.extraGlobalNeuronKernelParameters.push_back("rates");
    n.extraGlobalNeuronKernelParameterTypes.push_back("uint64_t *");
    n.extraGlobalNeuronKernelParameters.push_back("offset");
    n.extraGlobalNeuronKernelParameterTypes.push_back("unsigned int");
    n.simCode = R"(    
    uint64_t theRnd;
    if($(V)>$(Vrest))
    {
        $(V)= $(Vrest);
    }
    else
    {
        // if ($(t) - $(spikeTime) > ($(trefract))) {    // revised at 2018-03-27
            MYRAND($(seed),theRnd);
            if (theRnd < *($(rates)+$(offset)+$(id))){
                $(V)= $(Vspike);
                $(spikeTime)= $(t);
            }
        // }
        $(trace)  +=  (-$(trace)/$(tc_trace)) * DT;
    }
    )";
    n.thresholdConditionCode = "$(V) >= $(Vspike)";
    n.dps = NULL;
    nModels.push_back(n);
    myPOISSONNEURON = nModels.size() - 1;
    return myPOISSONNEURON;
}
int addMyNeuronModel_LIF_exc(vector<neuronModel> &nModels)
{
    neuronModel n;
    unsigned int LIF_exc; //!< variable attaching the name "LIF_exc"
    // LIF exc_neurons  确认下变量的单位？
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("theta"); //加一个theta变量？
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer"); //加一个timer变量？
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    n.varTypes.push_back("bool");
    n.pNames.clear();
    //n.pNames.push_back("Vspike");
    n.pNames.push_back("tau_e");
    n.pNames.push_back("v_rest_e");     // 兴奋神经元静息电位.mV
    n.pNames.push_back("v_reset_e");    // （不是直接使用还有其余运算）兴奋神经元复位电位.mV
    n.pNames.push_back("v_thresh_e");   // （不是直接使用还有其余运算）兴奋神经元复位电位.mV
    n.pNames.push_back("refrac_e");     // 兴奋神经元不响应时期.ms
    n.pNames.push_back("test_mode");    // 是否测试模式
    n.pNames.push_back("tc_theta");     // theta和兴奋神经元活动阈值有关，这个是theta指数下降的时间常数 tc_theta = 1e7;
    n.pNames.push_back("theta_plus_e"); //  theta_plus_e= 0.05;
    n.pNames.push_back("offset_e");     // offset_e和兴奋神经元活动阈值有关offset_e = 20.0mV;
    // n.pNames.push_back("directInput");  // directInput
    n.pNames.push_back("tc_trace1");
    n.pNames.push_back("tc_trace2");
    n.pNames.push_back("tc_trace");
    n.dpNames.clear();
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    //下面式子电压mv电导nS时间ms
    //test_mode条件指定阈值是否活动。是实时变量，可以写在这里
    //DT是多少毫秒？python程序里用timer控制不应期内不能有发放。导数100,这里呢？？？会不会有25的影响？？？
    n.simCode = R"(
    if($(timer)>$(refrac_e)) //不应期内电压不变化，电导呢？! %revised at 2017-10-12
    {
        $(V)+=( $(v_rest_e) - $(V) + $(Isyn) ) / $(tau_e) *DT; //at two times for numerical stability
    }
    if($(V)<-100)//为了防止抑制低于-100时突然反向发放
    $(V)=-100;
    if($(test_mode)||$(testDataEvaluateMode)) //revised at 2018-4-6
    {
       $(theta) += 0*DT;
    }
    else {
        $(theta) -= $(theta) / $(tc_theta)*DT; 
        $(trace1)+=  (-$(trace1)/$(tc_trace1)) *DT;
        $(trace2)+=  (-$(trace2)/$(tc_trace2)) *DT;
        $(trace) +=  (-$(trace)/$(tc_trace)) * DT;
    }
    $(timer)+= 1*DT;
    // $(timer)+= 1*DT;
    )"; //这里不乘任何数因为应该与refrac_e正常比较！
    n.thresholdConditionCode = "($(V)>($(theta) - $(offset_e) + $(v_thresh_e))) && ($(timer)>$(refrac_e))"; //不应期可以把电位强制到静息电位！！！查找下不应期brian怎么处理的？
    // n.thresholdConditionCode = "($(V)>($(theta) - $(offset_e) + $(v_thresh_e))) && (100*$(timer)>$(refrac_e))"; //不应期可以把电位强制到静息电位！！！查找下不应期brian怎么处理的？
    n.resetCode = R"(
    //reset code is here
    if($(test_mode)||$(testDataEvaluateMode)) //revised at 2018-4-6
    {
       $(V) = $(v_reset_e); $(timer) = 0;
     }
    else {
       $(V) = $(v_reset_e); $(theta) += $(theta_plus_e) * ($(offset_e)-10)/abs($(theta)-10); $(timer) = 0;  //果然体会到t是很厉害的全局变量了
    }
    )";
    nModels.push_back(n);
    LIF_exc = nModels.size() - 1;
    return LIF_exc;
}
int addMyNeuronModel_LIF_inh(vector<neuronModel> &nModels)
{
    neuronModel n;
    unsigned int LIF_inh; //!< variable attaching the name "LIF_inh"
    // LIF inh_neurons  确认下变量的单位？
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    //n.pNames.push_back("Vspike");
      n.pNames.push_back("tau_i");
    n.pNames.push_back("v_rest_i");   // 抑制神经元静息电位.mV
    n.pNames.push_back("v_reset_i");  // （不是直接使用还有其余运算）抑制神经元复位电位.mV
    n.pNames.push_back("v_thresh_i"); // 抑制神经元复位电位.mV
    n.pNames.push_back("refrac_i");   // 抑制神经元不响应时期.ms
    n.pNames.push_back("test_mode");  // 是否测试模式
    // n.pNames.push_back("directInput"); // directInput
    n.dpNames.clear();
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    //下面式子电压mv电导nS时间ms
    //无test_mode条件。相比兴奋神经元少了两个test_mode！
    //DT是多少毫秒？python程序里用timer控制不应期内不能有发放。导数100,这里呢？？？会不会有25的影响？？？
    //if($(timer)>$(refrac_i))//根本不需要不应期，兴奋一个spike后有5ms，抑制在未来5ms内都没有输入了
    n.simCode = R"(
    if($(timer)>$(refrac_i)) //不应期内电压不变化，电导呢？!
    {
        $(V)+=( $(v_rest_i) - $(V) + $(Isyn) ) / $(tau_i) *DT;
    }
    $(timer)+= DT;
    )";
    //没有theta和timer的微分方程：没有活动阈值和计时器控制不应期
    n.thresholdConditionCode = "$(V) > $(v_thresh_i) && $(timer) > $(refrac_i)"; //没有不应期的限制？（自然没有）
    //抑制神经元resetCode没有test_mode的限制，没有活动阈值，也没有不应期限制？？？
    n.resetCode = R"(//reset code is here
       $(V) = $(v_reset_i);
       $(timer) = 0;
     )";
    nModels.push_back(n);
    LIF_inh = nModels.size() - 1;
    return LIF_inh;
}
int addMyNeuronModel_LIF_cla(vector<neuronModel> &nModels)
{
    neuronModel n;
    unsigned int LIF_cla; //!< variable attaching the name "LIF_cla"
    // LIF cla_neurons  确认下变量的单位？
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("seed");
    n.varTypes.push_back("uint64_t");
    n.varNames.push_back("theRnd"); //自己加的，仅仅为了方便观察。之后为了计算快可以删去
    n.varTypes.push_back("uint64_t");
    n.varNames.push_back("spikeTime");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    n.varTypes.push_back("bool");
    n.pNames.clear();
    n.pNames.push_back("tau_i");
    n.pNames.push_back("v_rest_i");   // 抑制神经元静息电位.mV
    n.pNames.push_back("v_reset_i");  // （不是直接使用还有其余运算）抑制神经元复位电位.mV
    n.pNames.push_back("v_thresh_i"); // 抑制神经元复位电位.mV
    n.pNames.push_back("refrac_i");   // 抑制神经元不响应时期.ms
    n.pNames.push_back("test_mode");  // 是否测试模式
    n.pNames.push_back("tc_trace1");
    n.pNames.push_back("tc_trace2");
    n.pNames.push_back("therate");
    n.pNames.push_back("trefract");
    n.pNames.push_back("Vspike");
    n.pNames.push_back("Vrest");
    n.dpNames.clear();
    n.extraGlobalNeuronKernelParameters.push_back("rates");
    n.extraGlobalNeuronKernelParameterTypes.push_back("uint64_t *");
    //TODO: replace the resetting in the following with BRIAN-like threshold and resetting
    n.simCode = R"(
    if($(test_mode)||$(testDataEvaluateMode)) //revised at 2018-4-6
    {
        $(V)+=( $(v_rest_i) - $(V) + $(Isyn) ) / $(tau_i) *DT;
    }
    else
    {
        if($(V)>$(Vrest))
        {
            $(V)= $(Vrest);
        }
        else
        {
            //uint64_t theRnd;//原poisson模型不能连续发放，350ms，只相当于175ms？？？
            // if ($(t) - $(spikeTime) > ($(trefract)))  //revised at 2018-04-26
            // {
                MYRAND($(seed),$(theRnd));
                if ($(theRnd) < *($(rates)+$(id)))
                {
                    $(V)= $(Vspike);
                    $(spikeTime)= $(t);
                }
            // }
        }
        $(trace1)+=  (-$(trace1)/$(tc_trace1)) *DT;
        $(trace2)+=  (-$(trace2)/$(tc_trace2)) *DT;
    }
    )";
    n.thresholdConditionCode = R"(    
    ($(test_mode)||$(testDataEvaluateMode))&&$(V)> $(v_thresh_i)||(!($(test_mode)||$(testDataEvaluateMode)))&&$(V) >= $(Vspike)//revised at 2018-4-6
    )";
    n.resetCode = R"(//reset code is here
    if($(test_mode)||$(testDataEvaluateMode))//revised at 2018-4-6
        $(V) = $(v_reset_i);
     )";
    nModels.push_back(n);
    LIF_cla = nModels.size() - 1;
    return LIF_cla;
}
int addMySynapseModel(vector<weightUpdateModel> &weightUpdateModels)
{
    unsigned int mySTDP; //!< Variable attaching  the name LEARN1SYNAPSE to the the primitive STDP model for learning
    // mySTDP weightupdate model: "normal" synapse with a type of STDP
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g"); //0
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    wuSTDP.varTypes.push_back("bool");
    // wuSTDP.varNames.push_back("post2before"); //4//提升性能降低理论性
    // wuSTDP.varTypes.push_back("scalar");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");  //0
    wuSTDP.pNames.push_back("nu_ee_post"); //1
    wuSTDP.pNames.push_back("g_min");      //5
    wuSTDP.pNames.push_back("g_max");      //6
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
    }
    else
    {
        if($(timer_post)>$(refrac_e_post)) //不应期内电压不变化，电导呢？!
        {
            $(addtoinSyn) = $(g);
            $(updatelinsyn);
        }
        $(trace_pre) = 1;
        $(g) -= $(nu_ee_pre) * $(trace1_post);
        if ($(g)<$(g_min))
        $(g)=$(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？？？？？？？？？？？？？？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6   
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
    mySTDP = weightUpdateModels.size() - 1;
    return mySTDP;
}
// ------------------------------------------------------------------------------
//  DA STDP models
// ------------------------------------------------------------------------------
int addMySynapseModel_mySTDP_DA(vector<weightUpdateModel> &weightUpdateModels)
{
    unsigned int mySTDP_DA; //!< Variable attaching  the name LEARN1SYNAPSE to the the primitive STDP model for learning
    // mySTDP_DA weightupdate model: "normal" synapse with a type of STDP
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g"); //0
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");  //0
    wuSTDP.pNames.push_back("nu_ee_post"); //1
    wuSTDP.pNames.push_back("g_min");      //5
    wuSTDP.pNames.push_back("g_max");      //6
    wuSTDP.pNames.push_back("a_plus");     //5
    wuSTDP.pNames.push_back("a_minus");    //6
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
    }
    else
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
        $(trace_pre) += $(a_plus);
        $(g) += $(nu_ee_pre) * $(trace1_post);
        if ($(g)>$(g_max))
        $(g)=$(g_max);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？？？？？？？？？？？？？？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6   
    {
        
    }
    else
    {
        $(trace1_post) += $(a_minus);
        $(g) += $(nu_ee_post) * $(trace_pre);
        if ($(g)>$(g_max))
        $(g)=$(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    mySTDP_DA = weightUpdateModels.size() - 1;
    return mySTDP_DA;
}
// ------------------------------------------------------------------------------
//  Online implementation of STDP models
// ------------------------------------------------------------------------------
int addMySynapseModel_mySTDP_symmetric(vector<weightUpdateModel> &weightUpdateModels)
{
    unsigned int mySTDP_symmetric; //!< Variable attaching  the name LEARN1SYNAPSE to the the primitive STDP model for learning
    // mySTDP_symmetric weightupdate model: "normal" synapse with a type of STDP
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g"); //0
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");  //0
    wuSTDP.pNames.push_back("nu_ee_post"); //1
    wuSTDP.pNames.push_back("g_min");      //5
    wuSTDP.pNames.push_back("g_max");      //6
    wuSTDP.pNames.push_back("a_plus");     //5
    wuSTDP.pNames.push_back("a_minus");    //6
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
    }
    else
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
        $(trace_pre) += $(a_plus);
        $(g) -= $(nu_ee_pre) * $(trace1_post);
        if ($(g)<$(g_min))
        $(g)=$(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？？？？？？？？？？？？？？？？
    // code for post-synaptic spike
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6   
    {
        
    }
    else
    {
        $(trace1_post) += $(a_minus);
        $(g) += $(nu_ee_post) * $(trace_pre);
        if ($(g)>$(g_max))
        $(g)=$(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    mySTDP_symmetric = weightUpdateModels.size() - 1;
    return mySTDP_symmetric;
}
// ------------------------------------------------------------------------------
//  weight dependence:hard bounds and soft bounds
// ------------------------------------------------------------------------------
int addMySynapseModel_soft_bounds(vector<weightUpdateModel> &weightUpdateModels)
{
    unsigned int mySTDP_soft_bounds; //!< Variable attaching  the name LEARN1SYNAPSE to the the primitive STDP model for learning
    // mySTDP_soft_bounds weightupdate model: "normal" synapse with a type of STDP
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g"); //0
    wuSTDP.varTypes.push_back("scalar");
    wuSTDP.varNames.push_back("testDataEvaluateMode");  // 是否学习？不学习时使用测试集评估//revised at 2018-4-6
    wuSTDP.varTypes.push_back("bool");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");  //0
    wuSTDP.pNames.push_back("nu_ee_post"); //1
    wuSTDP.pNames.push_back("g_min");      //5
    wuSTDP.pNames.push_back("g_max");      //6
    wuSTDP.pNames.push_back("a_plus");     //5
    wuSTDP.pNames.push_back("a_minus");    //6
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
    }
    else
    {
        $(addtoinSyn) = $(g);
        $(updatelinsyn);
        $(trace_pre) += $(a_plus);
        $(g) -= $(nu_ee_pre) * ($(g)-$(g_min)) * $(trace1_post);
        if ($(g)<$(g_min))
        $(g)=$(g_min);
    }
    )";
    wuSTDP.dps = new pwSTDP; //？？？？？？？？？？？？？？？？？？
    // code for post-synaptic spike
    //$(post2before) = $(post2); //提升性能降低理论性
    //$(g) += $(nu_ee_post) * $(pre) * $(post2before);
    wuSTDP.simLearnPost = R"(
    if($(testDataEvaluateMode))//revised at 2018-4-6   
    {
        
    }
    else
    {
        $(trace1_post) += $(a_minus);
        $(g) += $(nu_ee_post) * ($(g_max)-$(g)) * $(trace_pre);
        if ($(g)>$(g_max))
        $(g)=$(g_max);
    }
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = true;
    weightUpdateModels.push_back(wuSTDP);
    mySTDP_soft_bounds = weightUpdateModels.size() - 1;
    return mySTDP_soft_bounds;
}
int addMySynapse_mySTDP_nolearnning_Model(vector<weightUpdateModel> &weightUpdateModels)
{
    unsigned int mySTDP_nolearnning; //!< Variable attaching  the name LEARN1SYNAPSE to the the primitive STDP model for learning
    // mySTDP_nolearnning weightupdate model: "normal" synapse with a type of STDP
    weightUpdateModel wuSTDP;
    wuSTDP.varNames.clear();
    wuSTDP.varTypes.clear();
    wuSTDP.varNames.push_back("g"); //0
    wuSTDP.varTypes.push_back("scalar");
    // wuSTDP.varNames.push_back("post2before"); //4//提升性能降低理论性
    // wuSTDP.varTypes.push_back("scalar");
    wuSTDP.pNames.clear();
    wuSTDP.pNames.push_back("nu_ee_pre");  //0
    wuSTDP.pNames.push_back("nu_ee_post"); //1
    wuSTDP.pNames.push_back("g_min");      //5
    wuSTDP.pNames.push_back("g_max");      //6
    // wuSTDP.synapseDynamics = R"()";
    // code for presynaptic spike
    wuSTDP.simCode = R"(
    $(addtoinSyn) = $(g);
    $(updatelinsyn);
    // $(trace_pre) = 1;
    // $(g) -= $(nu_ee_pre) * $(trace1_post);
    // if ($(g)<$(g_min))
    // $(g)=$(g_min);
    )";
    wuSTDP.dps = new pwSTDP; //？？？？？？？？？？？？？？？？？？
    // code for post-synaptic spike
    //$(post2before) = $(post2); //提升性能降低理论性
    //$(g) += $(nu_ee_post) * $(pre) * $(post2before);
    wuSTDP.simLearnPost = R"(
    // $(g) += $(nu_ee_post) * $(trace_pre) * $(trace2_post);
    // $(trace1_post) = 1;
    // $(trace2_post) = 1;
    // if ($(g)>$(g_max))
    // $(g)=$(g_max);
    )";
    wuSTDP.needPreSt = true;
    wuSTDP.needPostSt = false;
    weightUpdateModels.push_back(wuSTDP);
    mySTDP_nolearnning = weightUpdateModels.size() - 1;
    return mySTDP_nolearnning;
}
int addMyNeuronModel_LIF_exc_no_learnning(vector<neuronModel> &nModels)
{
    neuronModel n;
    unsigned int LIF_exc_no_learnning; //!< variable attaching the name "LIF_exc_no_learnning"
    // LIF exc_neurons  确认下变量的单位？
    n.varNames.clear();
    n.varTypes.clear();
    n.varNames.push_back("V");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("theta"); //加一个theta变量？
    n.varTypes.push_back("scalar");
    n.varNames.push_back("timer"); //加一个timer变量？
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace1");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace2");
    n.varTypes.push_back("scalar");
    n.varNames.push_back("trace");
    n.varTypes.push_back("scalar");
    n.pNames.clear();
    n.pNames.push_back("tau_e");
    n.pNames.push_back("v_rest_e");     // 兴奋神经元静息电位.mV
    n.pNames.push_back("v_reset_e");    // （不是直接使用还有其余运算）兴奋神经元复位电位.mV
    n.pNames.push_back("v_thresh_e");   // （不是直接使用还有其余运算）兴奋神经元复位电位.mV
    n.pNames.push_back("refrac_e");     // 兴奋神经元不响应时期.ms
    n.pNames.push_back("test_mode");    // 是否测试模式
    n.pNames.push_back("tc_theta");     // theta和兴奋神经元活动阈值有关，这个是theta指数下降的时间常数 tc_theta = 1e7;
    n.pNames.push_back("theta_plus_e"); //  theta_plus_e= 0.05;
    n.pNames.push_back("offset_e");     // offset_e和兴奋神经元活动阈值有关offset_e = 20.0mV;
    n.pNames.push_back("tc_trace1");
    n.pNames.push_back("tc_trace2");
    n.pNames.push_back("tc_trace");
    n.dpNames.clear();
    n.simCode = R"(
    if($(timer)>$(refrac_e)) //不应期内电压不变化，电导呢？!
    {
        $(V)+=( $(v_rest_e) - $(V) + $(Isyn) ) / $(tau_e) *DT; //at two times for numerical stability
    }
    if($(V)<-100)//为了防止抑制低于-100时突然反向发放
    $(V)=-100;
    // if($(test_mode)){
    //    $(theta) += 0*DT;
    //  }
    // else {
    //    $(theta) -= $(theta) / $(tc_theta)*DT; 
    // }
    $(timer)+= 1*DT;
    $(trace1)+=  (-$(trace1)/$(tc_trace1)) *DT;
    $(trace2)+=  (-$(trace2)/$(tc_trace2)) *DT;
    $(trace) +=  (-$(trace)/$(tc_trace)) * DT;
    )";                                                                                                     //这里不乘任何数因为应该与refrac_e正常比较！
    n.thresholdConditionCode = "($(V)>($(theta) - $(offset_e) + $(v_thresh_e))) && ($(timer)>$(refrac_e))"; //不应期可以把电位强制到静息电位！！！查找下不应期brian怎么处理的？
    n.resetCode = R"(
    //reset code is here
    // if($(test_mode)){
       $(V) = $(v_reset_e); $(timer) = 0;
    //  }
    // else {
    //    $(V) = $(v_reset_e); $(theta) += $(theta_plus_e); $(timer) = 0;  //果然体会到t是很厉害的全局变量了
    // }
    )";
    nModels.push_back(n);
    LIF_exc_no_learnning = nModels.size() - 1;
    return LIF_exc_no_learnning;
}
