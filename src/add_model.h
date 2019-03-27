#ifndef _ADD_MODEL_H_
#define _ADD_MODEL_H_

#include "modelSpec.h"

#include <vector>

using std::vector;

int addMyInputNeuronModel(vector<neuronModel> &nModels);
int addMyNeuronModel_LIF_exc(vector<neuronModel> &nModels);
int addMyNeuronModel_LIF_exc_no_learnning(vector<neuronModel> &nModels);
int addMyNeuronModel_LIF_inh(vector<neuronModel> &nModels);
int addMyNeuronModel_LIF_cla(vector<neuronModel> &nModels);
int addMySynapseModel(vector<weightUpdateModel> &weightUpdateModels);
int addMySynapseModel_mySTDP_DA(vector<weightUpdateModel> &weightUpdateModels);
int addMySynapseModel_mySTDP_symmetric(vector<weightUpdateModel> &weightUpdateModels);
int addMySynapseModel_soft_bounds(vector<weightUpdateModel> &weightUpdateModels);
int addMySynapse_mySTDP_nolearnning_Model(vector<weightUpdateModel> &weightUpdateModels);
#endif