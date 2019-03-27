#include "gnuplot.h"

GNUplot::GNUplot() throw(string)
{
  //想在程序运行结束后还看到gnuplot的画图窗口用
  // gnuplotpipe = popen("gnuplot -persist", "w");
  //不想看到
  gnuplotpipe = popen("gnuplot ", "w");
  if (!gnuplotpipe)
  {
    throw("Gnuplot not found !");
  }
}

GNUplot::~GNUplot()
{
  fprintf(gnuplotpipe, "exit\n");
  pclose(gnuplotpipe);
}

void GNUplot::operator()(const string &command)
{
  fprintf(gnuplotpipe, "%s\n", command.c_str());
  fflush(gnuplotpipe);
  // flush is necessary, nothing gets plotted else
};

// // fprintf(fp, str_plot_set);//奇怪，只能接受一句命令不能多命令输入
// fprintf(fp, str_plot);
// fprintf(fp, "pause mouse\n"); //用户点击后退出
// pclose(fp);