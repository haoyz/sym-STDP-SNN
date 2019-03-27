#ifndef _EVALUATION_H_
#define _EVALUATION_H_

#include <iostream>
#include <queue>
#include <fstream>
#include <math.h>
#include "../global.h"
#include "../model_CODE/definitions.h"
#include "train.h"
#include "model.h"
#include <cstring>
using namespace std;

void normalize_weights(float *tmp_gP2E);
void cla_normalize_weights(float *tmp_gE2C);

int get_recognized_number(int spike_rates[_NExc], int *assignments);                                                     //每个样本，每个神经元的发放次数
void get_new_assignments(int *assignments, int offset, int result_monitor[_LABELS_CONST][_NExc], vector<float> &labels); //result_monitor每个神经元对每个样本的激发次数

bool whether_evaluation_or_not(int *assignments);

void get_assignments_distribution(int *assignments, int assignments_dist[10]);
void write_assignments_distribution_to_file(int assignments_dist[10]);

void get_visual_PEw(float tmp_visual[_NPoi_Sqrt * _NExc_Sqrt][_NPoi_Sqrt * _NExc_Sqrt]);
void write_visual_PEw_to_file(float tmp_visual[_NPoi_Sqrt * _NExc_Sqrt][_NPoi_Sqrt * _NExc_Sqrt]);

void get_visual_ECw(float tmp_visual[_ECw_X][_ECw_Y]);
void write_visual_ECw_to_file(float tmp_visual[_ECw_X][_ECw_Y]);

void get_visual_ECw_inferred(float tmp_visual[_ECw_X][_ECw_Y], int *assignments);
void write_visual_ECw_inferred_to_file(float tmp_visual[_ECw_X][_ECw_Y]);

void get_performance(float *tmp_performance, int offset, int current_evaluation, int result_monitor[][_NExc], int *assignments, vector<float> &labels);
void write_performance_to_file(float *tmp_performance1, float *tmp_performance2, int current_evaluation, int fileName);

void cla_get_performance(float *tmp_performance, int offset, int current_evaluation, int cla_result_monitor[][_NCla], vector<float> &labels);

void get_spike_rate(float spike_rate[3], int result_monitor_last1000[][_NExc], int *assignments, vector<float> &labels);
void write_spike_rate_to_file(float spike_rate[3]);

void get_confusion_m(float confusion_m[11][11], int offset, int result_monitor[][_NExc], int *assignments, vector<float> &labels);
void get_confusion_m_supervised(float confusion_m[11][11], int offset, int cla_result_monitor[][_NCla], vector<float> &labels); //revised at 2018-5-13
void write_confusion_m_to_file(float confusion_m[11][11], int flag);

void get_variance(float variance[_NExc]);
void write_variance_to_file(float variance[_NExc], int current_evaluation);

void get_variance_gEC(float variance[_NExc]);
void write_variance_gEC_to_file(float variance[_NExc], int current_evaluation);

bool write_spike_to_file(int (*tmp_spike)[_NExc]);
bool write_cla_spike_to_file(int (*tmp_spike)[_NCla]);

void write_response_rate_to_file(int *tmp_response);

void write_theta_to_file(int *tmp_theta);

void write_result_monitor_to_file(int tmp_result_monitor[_NExc], int tmp_label);
void write_result_monitor_to_file_0518(int tmp_result_monitor[_NExc], int tmp_label);
void write_result_monitor_to_file_0518_cla(int tmp_result_monitor[_NCla], int tmp_label);

void save_tmp_gP2E();
void save_tmp_theta();
void save_tmp_gE2C();

void save_gP2E();
void save_theta();
void save_gE2C();

#endif