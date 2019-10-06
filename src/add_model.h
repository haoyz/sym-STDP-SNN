#ifndef _ADD_MODEL_H_
#define _ADD_MODEL_H_

#include "modelSpec.h"

#include <vector>

using std::vector;

int addNeuronModel_Poi(vector<neuronModel> &nModels);
int addNeuronModel_LIF_Exc(vector<neuronModel> &nModels);
int addNeuronModel_LIF_Inh(vector<neuronModel> &nModels);
int addNeuronModel_LIF_Cla(vector<neuronModel> &nModels);
int addSynapseModel_DA_STDP(vector<weightUpdateModel> &weightUpdateModels);
#endif