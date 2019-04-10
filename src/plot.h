#ifndef _PLOT_H_
#define _PLOT_H_

#include "gnuplot.h"
#include <fstream>
#include <unistd.h> //sleep()需要
#include <stdio.h>  //FILE*需要
using namespace std;
#include <iostream>
#include <queue>
#include "evaluation.h"
#include "model.h"

#define UNSUPERVISED 0
#define SUPERVISED 1

void plot_assignments_distribution_init(GNUplot &tmp_plot);
void plot_assignments_distribution(GNUplot &tmp_plot);

void plot_visual_PEw_init(GNUplot &tmp_plotter);
void plot_visual_PEw(GNUplot &tmp_plotter);

void plot_visual_ECw_init(GNUplot &tmp_plotter);
void plot_visual_ECw(GNUplot &tmp_plotter);

void plot_visual_ECw_inferred_init(GNUplot &tmp_plotter);
void plot_visual_ECw_inferred(GNUplot &tmp_plotter);

void plot_variance_init(GNUplot &tmp_plot, int current_evaluation);
void plot_variance(GNUplot &tmp_plot);

void plot_variance_distribution_init(GNUplot &tmp_plot);
void plot_variance_distribution(GNUplot &tmp_plot);

void plot_variance_E2C_init(GNUplot &tmp_plot, int current_evaluation);
void plot_variance_E2C(GNUplot &tmp_plot);
void plot_variance_E2C_distribution_init(GNUplot &tmp_plot);
void plot_variance_E2C_distribution(GNUplot &tmp_plot);

void plot_performance_init(GNUplot &tmp_pt, int num_samples); //做好画图的准备。画图的框框，准备performance存储实时的准确率。
void plot_performance(GNUplot &tmp_pt, int fileName);         //代码重用

void plot_confusion_m_init(GNUplot &tmp_plot);
void plot_confusion_m(GNUplot &tmp_plot, int flag);

void plot_spikes_init(GNUplot &tmp_plot);
void plot_spikes(GNUplot &tmp_plot);

void plot_cla_spikes_init(GNUplot &tmp_plot);
void plot_cla_spikes(GNUplot &tmp_plot);

void plot_response_rates_init(GNUplot &tmp_plot);
void plot_response_rates(GNUplot &tmp_plot);

void plot_theta_init(GNUplot &tmp_plot);
void plot_thetas(GNUplot &tmp_plot);
#endif