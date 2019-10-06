#ifndef DRAFTING_H
#define DRAFTING_H
#include "gnuplot.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

class Drafting
{
public: 
  vector<std::string> init;
  vector<std::string> plot;
  GNUplot gnuplot;
  bool replot;
  int replot_count = 0; 
  explicit Drafting(    
      vector<std::string> &init,
      vector<std::string> &plot,
      bool replot = false)
      : init{init}, plot{plot}, replot{replot}
  { 
    gnuplot(this->init);
  }
  virtual ~Drafting();
  virtual void update();

private:
  std::string output_file = plot[0];
};

extern vector<string> init_assign_dist;
extern vector<string> plot_assign_dist;
extern vector<string> init_PEw;
extern vector<string> plot_PEw;
extern vector<string> init_ECw;
extern vector<string> plot_ECw;
extern vector<string> init_varianceP2E;
extern vector<string> plot_varianceP2E;
extern vector<string> init_varianceP2E_distribution;
extern vector<string> plot_varianceP2E_distribution;
extern vector<string> init_varianceE2C;
extern vector<string> plot_varianceE2C;
extern vector<string> init_varianceE2C_distribution;
extern vector<string> plot_varianceE2C_distribution;
extern vector<string> init_performance;
extern vector<string> plot_performance;
extern vector<string> init_confusion;
extern vector<string> plot_usl_confusion;
extern vector<string> plot_sl_confusion;
extern vector<string> init_spikes;
extern vector<string> plot_spikes;
extern vector<string> init_Claspikes;
extern vector<string> plot_Claspikes;
extern vector<string> init_response_rates;
extern vector<string> plot_response_rates;
extern vector<string> init_theta;
extern vector<string> plot_theta;

bool replace(std::string &str, const std::string &from, const std::string &to);
#endif