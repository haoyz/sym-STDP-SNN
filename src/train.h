#ifndef _TRAIN_H_
#define _TRAIN_H_

#include "gnuplot.h"
#include "../global.h"
#include <stddef.h> //vscode下必须有
#include <vector>
using namespace std;

#define N_rand 256 //精度

#define preN_EI NExc
#define postN_EI NInh
#define preN_IE NInh
#define postN_IE NExc
#define connN_E2I NExc             //一对一
#define connN_I2E NInh *(NExc - 1) //一对多

#define INPUT_INTENSITY_INIT 2
#define Cla_INPUT_INTENSITY_INIT 0.1 //khz

#define LastTrain 0
#define NowTrain 1
#define NowTest 2
#define FinalTest 0

extern long int imageNum;
// ------------------------------------------------------------------------------
//  可视化突触权重
// ------------------------------------------------------------------------------
vector<vector<float>> PEVisual(WIDTH *NExc_Sqrt, vector<float>(HEIGHT *NExc_Sqrt));
vector<vector<float>> ECVisual(ECw_X, vector<float>(ECw_Y));
vector<vector<float>> ECVisual_inferred(ECw_X, vector<float>(ECw_Y));
vector<vector<float>> CEVisual(ECw_Y, vector<float>(ECw_X));
vector<vector<float>> CEVisual_inferred(ECw_Y, vector<float>(ECw_X));
#ifdef PLOT_ON
GNUplot PEplotter;
GNUplot ECplotter, ECplotter_inferred;
GNUplot CEplotter, CEplotter_inferred;
GNUplot plot_per;
GNUplot plot_perNowUseTrainData;
GNUplot plot_perNowUseTestData;
GNUplot plot_v, plot_vE2C;
GNUplot plot_confusion;
GNUplot plot_confusion_supervised;
GNUplot plot_v_dist, plot_vE2C_dist;
GNUplot plot_assign_dist;
GNUplot plot_theta;
GNUplot plot_response_rate;
#endif
#ifdef SPIKES_MONITOR //plot_spike如果声明和使用不在同一个#ifdef下会报错
GNUplot plot_spike;
GNUplot plot_cla_spike;
#endif

int str2int(string s);
void convertRateToRandomNumberThreshold(vector<float> rateKHz_pattern, uint64_t *pattern, int Num);
void get_inputdata(string datapath, vector<vector<float>> &images, vector<float> &labels, vector<vector<float>> &images_test, vector<float> &labels_test);
void rewrite_gEI_gIE();
void reset_Cla_para();
void feed_to_networks(vector<float> image, vector<float> &FR_khz, float input_intensity);
void Cla_feed_to_networks(int label, vector<float> &cla_FR_khz, float cla_input_intensity);
void reset_ratesPPoi(vector<float> &FR_khz);
void reset_ratesPCla(vector<float> &cla_FR_khz);
#endif