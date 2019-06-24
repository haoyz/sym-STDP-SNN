#ifndef _EVALUATION_H_
#define _EVALUATION_H_

#include <iostream>
#include <queue>
#include <fstream>
#include <math.h>
#include "../global.h"
#include "../model_CODE/definitions.h"
#include "model.h"
#include <cstring>
using namespace std;

#define UNSUPERVISED 0
#define SUPERVISED 1

void normalize_weights(float *tmp_gP2E);
void cla_normalize_weights(float *tmp_gE2C);

int get_recognized_number(vector<int> spike_record, vector<int> assignments);                                 //每个样本，每个神经元的发放次数
void get_new_assignments(vector<int> &assignments, vector<vector<int>> result_monitor, vector<float> labels); //result_monitor每个神经元对每个样本的激发次数

bool whether_evaluation_or_not(vector<int> assignments);

void get_assignments_distribution(vector<int> assignments, vector<int> &assignments_dist);
void write_assignments_distribution_to_file(vector<int> assignments_dist);

void get_visual_PEw(vector<vector<float>> &tmp_visual);
void write_visual_PEw_to_file(vector<vector<float>> tmp_visual);

void get_visual_ECw(vector<vector<float>> &tmp_visual);
void write_visual_ECw_to_file(vector<vector<float>> tmp_visual);

void get_visual_ECw_inferred(vector<vector<float>> &tmp_visual, vector<int> assignments);
void write_visual_ECw_inferred_to_file(vector<vector<float>> tmp_visual);

void write_result_monitor_to_file(vector<vector<int>> result_monitor);

void get_performance(vector<float> &tmp_performance, int current_evaluation, vector<vector<int>> result_monitor, vector<int> assignments, vector<float> labels);
void write_performance_to_file(vector<float> tmp_performance1, vector<float> tmp_performance2, int current_evaluation, int fileName);

void cla_get_performance(vector<float> &tmp_performance, int current_evaluation, vector<vector<int>> cla_result_monitor, vector<float> labels);

void get_spike_rate(float spike_rate[3], vector<vector<int>> result_monitor_last1000, vector<int> assignments, vector<float> &labels);
void write_spike_rate_to_file(float spike_rate[3]);

void get_confusion_m(vector<vector<float>> &confusion_m, vector<vector<int>> result_monitor, vector<int> assignments, vector<float> labels);
void get_confusion_m_supervised(vector<vector<float>> &confusion_m, vector<vector<int>> cla_result_monitor, vector<float> labels);
void write_confusion_m_to_file(vector<vector<float>> confusion_m, int flag);

void get_variance(vector<float> &variance, float *g, int dimen, int dimen1, float normal);
void write_variance_to_file(vector<float> variance, int current_evaluation);
void write_variance_gEC_to_file(vector<float> variance, int current_evaluation);

bool write_spike_to_file(vector<vector<int>> data, int num, string filename);

void write_vector_to_file(vector<int> data, int num, string filename);

void write_result_monitor_to_file(vector<int> tmp_result_monitor, int tmp_label);
void write_result_monitor_to_file_0518(vector<int> tmp_result_monitor, int tmp_label);
void write_result_monitor_to_file_0518_cla(vector<int> tmp_result_monitor, int tmp_label);

void save_gP2E(string id);
void save_theta(string id);
void save_gE2C(string id);

#endif