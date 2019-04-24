#ifndef DRAFTING_H
#define DRAFTING_H
#include "gnuplot.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

class Drafting
{
public: //怎么指定本类的名字？是否有隐藏指针.似乎真没有
  vector<std::string> init;
  vector<std::string> plot;
  GNUplot gnuplot;
  bool replot;
  int replot_count = 0; //仍然是在构造函数时才初始化，有时发现数值已经是0是因为分配内存的那地方原本就是0
  explicit Drafting(    //数组怎么接受引用传参？这里不修改数组，因此指针就够用了
      vector<std::string> &init,
      vector<std::string> &plot,
      bool replot = false)
      : init{init}, plot{plot}, replot{replot}
  { //构造函数里做init plot的功能合适吗？
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