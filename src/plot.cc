#include "plot.h"

void plot_assignments_distribution_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set terminal png size 640*1,480*1");
  tmp_plot("set title \"assignments distribution\"");
  tmp_plot("set xlabel \"assignments\"");
  tmp_plot("set ylabel \"assignments count\"");
  tmp_plot("set style data histogram");
  tmp_plot("set style histogram clustered gap 1");
  tmp_plot("set style fill solid 0.4 border");
}
void plot_assignments_distribution(GNUplot &tmp_plot)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  cout << "plot_assignments_distribution" << endl;
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/assign_dist%d.png\"", tmp_id);
  tmp_plot(tmp_commond);
  tmp_plot("plot \"./output/assign_dist.dat\" u 2:xticlabels(1) ");
  tmp_id++;
}
void plot_visual_PEw_init(GNUplot &tmp_plotter)
{
  tmp_plotter("unset key"); //去掉图例
  tmp_plotter("unset border");
  tmp_plotter("unset xtics");
  tmp_plotter("unset ytics");
  // tmp_plotter("set title \"2D receptive fields\"");
  tmp_plotter("set size square");     //高宽比为1
  tmp_plotter("set cbrange [0:1.0]"); //色版对应取值范围
  tmp_plotter("set palette defined (-2 \"white\", -1 \"yellow\", 1 \"red\", 2 \"black\")");
  // tmp_plotter("pause mouse"); //用户点击后退出
}
void plot_visual_PEw(GNUplot &tmp_plotter)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  char plot_image[150];  //for no longer "line 0: undefined variable: NExc_Sqrt"
  sprintf(plot_image, "plot \"./output/plot_visual_PEw_bin.dat\" binary array=(%d,%d) endian=little format=\"%%float\" with image",
          NExc_Sqrt * WIDTH, NExc_Sqrt * HEIGHT); //否则会出现图片文件全部是0字节的错误
  cout << "plot_visual_PEw" << endl;
  if (NExc <= 400)
    tmp_plotter("set terminal png"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 1600)
    tmp_plotter("set terminal png size 640*2,480*2"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 6400)
    tmp_plotter("set terminal png size 640*4,480*4"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 10000)
    tmp_plotter("set terminal png size 640*5,480*5"); //wxt指屏幕，term是terminal缩写
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/PEweight%d.png\"", tmp_id); //文本式3d图
  tmp_plotter(tmp_commond);
  // tmp_plot("replot"); //replot跑起来不能用
  tmp_plotter(plot_image); //for no longer "line 0: undefined variable: NExc_Sqrt"
  tmp_id++;
}
void plot_visual_ECw_init(GNUplot &tmp_plotter)
{
  tmp_plotter("unset key"); //去掉图例
  tmp_plotter("unset border");
  tmp_plotter("unset xtics");
  tmp_plotter("unset ytics");
  tmp_plotter("set size square"); //高宽比为1
  char tmp_cbrange[30];
  sprintf(tmp_cbrange, "set cbrange [0:%f]", (double)gEC_MAX);
  // sprintf(tmp_cbrange, "set cbrange [0:%d]", (int)gEC_MAX);
  tmp_plotter(tmp_cbrange); //色版对应取值范围
  // tmp_plotter("set title \"visual ECw\"");
  // tmp_plotter("set xlabel \"neuron ID\"");
  // tmp_plotter("set ylabel \"weights\"");
}
void plot_visual_ECw(GNUplot &tmp_plotter)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  char plot_image[150];  //for no longer "line 0: undefined variable: NExc_Sqrt"
  sprintf(plot_image, "plot \"./output/plot_visual_ECw_bin.dat\" binary array=(%d,%d) endian=little format=\"%%float\" with image",
          _ECw_X, _ECw_Y); //否则会出现图片文件全部是0字节的错误
  cout << "plot_visual_ECw" << endl;
  if (NExc <= 400)
    tmp_plotter("set terminal png"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 1600)
    tmp_plotter("set terminal png size 640*2,480*2"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 6400)
    tmp_plotter("set terminal png size 640*4,480*4"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 10000)
    tmp_plotter("set terminal png size 640*5,480*5"); //wxt指屏幕，term是terminal缩写
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/ECw%d.png\"", tmp_id); //文本式3d图
  tmp_plotter(tmp_commond);
  // tmp_plot("replot"); //replot跑起来不能用
  tmp_plotter(plot_image); //for no longer "line 0: undefined variable: NExc_Sqrt"
  tmp_id++;
}
void plot_visual_ECw_inferred_init(GNUplot &tmp_plotter)
{
  tmp_plotter("unset key"); //去掉图例
  tmp_plotter("unset border");
  tmp_plotter("unset xtics");
  tmp_plotter("unset ytics");
  tmp_plotter("set size square");     //高宽比为1
  tmp_plotter("set cbrange [0:1.0]"); //色版对应取值范围
  // tmp_plotter("set title \"visual ECw inferred\"");
  // tmp_plotter("set xlabel \"neuron ID\"");
  // tmp_plotter("set ylabel \"weights\"");
}
void plot_visual_ECw_inferred(GNUplot &tmp_plotter)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  char plot_image[150];  //for no longer "line 0: undefined variable: NExc_Sqrt"
  sprintf(plot_image, "plot \"./output/plot_visual_ECw_inferred_bin.dat\" binary array=(%d,%d) endian=little format=\"%%float\" with image",
          _ECw_X, _ECw_Y); //否则会出现图片文件全部是0字节的错误
  cout << "plot_visual_ECw_inferred" << endl;
  if (NExc <= 400)
    tmp_plotter("set terminal png"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 1600)
    tmp_plotter("set terminal png size 640*2,480*2"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 6400)
    tmp_plotter("set terminal png size 640*4,480*4"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 10000)
    tmp_plotter("set terminal png size 640*5,480*5"); //wxt指屏幕，term是terminal缩写
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/ECw_inferred%d.png\"", tmp_id); //文本式3d图
  tmp_plotter(tmp_commond);
  // tmp_plot("replot"); //replot跑起来不能用
  tmp_plotter(plot_image); //for no longer "line 0: undefined variable: NExc_Sqrt"
  tmp_id++;
}

void plot_variance_init(GNUplot &tmp_plot, int current_evaluation)
{
  tmp_plot("unset key");
  tmp_plot("set title \"Weight Variance\"");
  tmp_plot("set xlabel \"number of samples()\"");
  tmp_plot("set ylabel \"variance\"");
  tmp_plot("set terminal png");
  char tmp_commond[30];
  sprintf(tmp_commond, "set xrange [0:%d]", current_evaluation);
  tmp_plot(tmp_commond);
  if (NExc <= 1600)
    tmp_plot("set yrange [0:100]");
  else if (NExc <= 6400)
    tmp_plot("set yrange [0:250]");
  else if (NExc <= 10000)
    tmp_plot("set yrange [0:300]");
  tmp_plot("set grid");
}
void plot_variance(GNUplot &tmp_plot)
{
  cout << "plot_variance" << endl;
  tmp_plot("set output \"./output/variance.png\""); //否则不work，set output时会把原来的图片消除。
  tmp_plot("plot \"./output/variance.dat\" with lines lc 3 lw 2");
  // tmp_plot("replot");//replot跑起来不能用
}
void plot_variance_distribution_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set terminal png size 640*2,480*2");
  tmp_plot("set title \"variance distribution\"");
  tmp_plot("set xlabel \"variance\"");
  tmp_plot("set ylabel \"variance count\"");
  tmp_plot("set style data histogram");
  tmp_plot("set style histogram clustered gap 1");
  tmp_plot("set style fill solid 0.4 border");
}
void plot_variance_distribution(GNUplot &tmp_plot)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  cout << "plot_variance_distribution" << endl;
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/var_dist%d.png\"", tmp_id);
  tmp_plot(tmp_commond);
  tmp_plot("plot \"./output/var_dist.dat\" u 2:xticlabels(1) ");
  tmp_id++;
}
void plot_variance_E2C_init(GNUplot &tmp_plot, int current_evaluation)
{
  tmp_plot("unset key");
  tmp_plot("set title \"Weight Variance E2C\"");
  tmp_plot("set xlabel \"number of samples()\"");
  tmp_plot("set ylabel \"variance E2C\"");
  tmp_plot("set terminal png");
  char tmp_commond[30];
  sprintf(tmp_commond, "set xrange [0:%d]", current_evaluation);
  tmp_plot(tmp_commond);
  if (NExc <= 1600)
    tmp_plot("set yrange [0:1000]");
  else if (NExc <= 6400)
    tmp_plot("set yrange [0:6000]"); // 600 -> 6000
  else if (NExc <= 10000)
    tmp_plot("set yrange [0:10000]"); // added -
  tmp_plot("set grid");
}
void plot_variance_E2C(GNUplot &tmp_plot)
{
  cout << "plot_variance_E2C" << endl;
  tmp_plot("set output \"./output/variance_E2C.png\""); //否则不work，set output时会把原来的图片消除。
  tmp_plot("plot \"./output/variance_E2C.dat\" with lines lc 3 lw 2");
  // tmp_plot("replot");//replot跑起来不能用
}
void plot_variance_E2C_distribution_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set terminal png size 640*2,480*2");
  tmp_plot("set title \"variance E2C distribution\"");
  tmp_plot("set xlabel \"variance E2C\"");
  tmp_plot("set ylabel \"variance count\"");
  tmp_plot("set style data histogram");
  tmp_plot("set style histogram clustered gap 1");
  tmp_plot("set style fill solid 0.4 border");
}
void plot_variance_E2C_distribution(GNUplot &tmp_plot)
{
  static int tmp_id = 0; //   1 -> 0 due to new Estimation in train.cc:
  cout << "plot_variance_E2C_distribution" << endl;
  char tmp_commond[100];
  sprintf(tmp_commond, "set output \"./output/var_E2C_dist%d.png\"", tmp_id);
  tmp_plot(tmp_commond);
  tmp_plot("plot \"./output/var_E2C_dist.dat\" u 2:xticlabels(1) ");
  tmp_id++;
}
// ------------------------------------------------------------------------------
//  准确率计算与画图（todo！！！）
// ------------------------------------------------------------------------------
void plot_performance_init(GNUplot &tmp_pt, int num_samples) //做好画图的准备。画图的框框，准备performance存储实时的准确率。
{
  // tmp_pt("unset key"); //去掉图例
  tmp_pt("set size ratio 0.5"); //高宽比为0.5
  tmp_pt("set title \"Classification Performance\"");
  tmp_pt("set xlabel \"number of samples()\"");
  tmp_pt("set ylabel \"accuracy(%)\"");
  char tmp_commond[30];
  sprintf(tmp_commond, "set xrange [0:%d]", num_samples / 10000);
  tmp_pt(tmp_commond);
  tmp_pt("set yrange [0:100]");
  char tmp_commond_[30];
  sprintf(tmp_commond_, "set xtics 0,1,%d", num_samples / 10000);
  tmp_pt(tmp_commond_);
  tmp_pt("set ytics 0,5,100");
  tmp_pt("set grid");
}
void plot_performance(GNUplot &tmp_pt, int tmp) //代码重用
{                                               //用上一个update_interval来计算此时准确率
  char tmp_commond[150];
  char file_name[50];
  cout << "Plot performance type:  " << tmp << endl;
  switch (tmp)
  {
  case 0:
    std::strcpy(file_name, "./output/performance_xy");
    break;
  case 1:
    std::strcpy(file_name, "./output/performance_xyNowUseTrainData");
    break;
  case 2:
    std::strcpy(file_name, "./output/performance_xyNowUseTestData");
    break;
  default:
    printf("error!\n");
    break;
  }
  cout << "plot " << file_name << endl;
  if (NExc <= 400)
    tmp_pt("set terminal png"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 1600)
    tmp_pt("set terminal png size 640*2,480*2"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 6400)
    tmp_pt("set terminal png size 640*4,480*4"); //wxt指屏幕，term是terminal缩写
  else if (NExc <= 10000)
    tmp_pt("set terminal png size 640*5,480*5"); //wxt指屏幕，term是terminal缩写
  sprintf(tmp_commond, "set output \"%s.png\"", file_name);
  tmp_pt(tmp_commond);
  // #ifdef _classification_layer
  //   tmp_pt("plot \"./output/performance_xy.dat\" using 1:2 with linespoints lc 3 lw 2 pt 7 ps 1 title \"exc\", \"./output/performance_xy.dat\" using 1:3 with linespoints lc 1 lw 2 pt 5 ps 1 title \"cla\"");
  // #else
  sprintf(tmp_commond, "plot \"%s.dat\" using 1:2 with linespoints lc 3 lw 2 pt 7 ps 1 title \"exc\"", file_name);
  tmp_pt(tmp_commond);
  // #endif
}
void plot_confusion_m_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("unset border");
  tmp_plot("set size square");     //高宽比为1
  tmp_plot("set cbrange [0:1.0]"); //色版对应取值范围
  tmp_plot("set palette defined (-2 \"white\", -1 \"yellow\", 1 \"red\", 2 \"black\")");
  tmp_plot("set title \"Confusion Matrix\"");
  tmp_plot("set xlabel \"Desired output digit\"");
  tmp_plot("set ylabel \"Inferred output digit\"");
  tmp_plot("set xrange [-2:10]");
  tmp_plot("set yrange [-2:10]");
  tmp_plot("set xtics 0,1,9 nomirror");
  // tmp_plot("set xtics add ("uk" -1)");
  tmp_plot("set ytics 0,1,9 nomirror");
  // tmp_plot("set ytics add ("uk" -1)");
  tmp_plot("set terminal png");
}
void plot_confusion_m(GNUplot &tmp_plot, int flag)
{
  static int tmp_id_unsupervised = 0; //   1 -> 0 due to new Estimation in train.cc:
  static int tmp_id_supervised = 0;
  char tmp_commond[100];
  cout << "plot_confusion_m" << endl;
  if (flag == SUPERVISED)
    sprintf(tmp_commond, "set output \"./output/confusion_m_supervised_%d.png\"", tmp_id_supervised);
  else if (flag == UNSUPERVISED)
    sprintf(tmp_commond, "set output \"./output/confusion_m_unsupervised_%d.png\"", tmp_id_unsupervised);
  else
    exit(2);
  tmp_plot(tmp_commond);
  tmp_plot("set pm3d map"); //视角
  if (flag == SUPERVISED)
  {
    tmp_plot("splot \"./output/plot_confusion_m_supervised.dat\" with image"); //文本式3d图
    tmp_id_supervised++;
  }
  else if (flag == UNSUPERVISED)
  {
    tmp_plot("splot \"./output/plot_confusion_m_unsupervised.dat\" with image"); //文本式3d图
    tmp_id_unsupervised++;
  }
  else
    exit(2);
  // tmp_plot("replot"); //replot跑起来不能用
}
void plot_spikes_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set title \"Spikes\"");
  tmp_plot("set style line 1 lc rgb \'blue\' pt 3 ps 1");
  tmp_plot("set xlabel \"time(ms)\"");
  tmp_plot("set ylabel \"neuron ID\"");
  char tmp_commond[30];
  sprintf(tmp_commond, "set xrange [-1:%d]", RUN_TIME);
  tmp_plot(tmp_commond);
  char tmp_commond_[30];
  sprintf(tmp_commond_, "set yrange [-10:%d]", NExc + 10);
  tmp_plot(tmp_commond_);
  char tmp_commond__[40];
  sprintf(tmp_commond__, "set xtics 0,%d,%d", 50, RUN_TIME);
  tmp_plot(tmp_commond__);
  // tmp_plot("set ytics 0,1,9");
}
void plot_spikes(GNUplot &tmp_plot)
{
  tmp_plot("plot \"./output/spike.dat\" w p ls 1");
}
void plot_cla_spikes_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set title \"Cla Spikes\"");
  tmp_plot("set style line 1 lc rgb \'blue\' pt 3 ps 1");
  tmp_plot("set xlabel \"time(ms)\"");
  tmp_plot("set ylabel \"neuron ID\"");
  char tmp_commond[30];
  sprintf(tmp_commond, "set xrange [-1:%d]", RUN_TIME);
  tmp_plot(tmp_commond);
  tmp_plot("set yrange [-1:10]");
  char tmp_commond__[40];
  sprintf(tmp_commond__, "set xtics 0,%d,%d", 50, RUN_TIME);
  tmp_plot(tmp_commond__);
  tmp_plot("set ytics -1,1,10");
}
void plot_cla_spikes(GNUplot &tmp_plot)
{
  tmp_plot("plot \"./output/cla_spike.dat\" w p ls 1");
}
void plot_response_rates_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set xtics nomirror");
  tmp_plot("set ytics nomirror");
  tmp_plot("set terminal png size 640*2,480*2");
  tmp_plot("set title \"Response Rate\"");
  tmp_plot("set xlabel \"response rates\"");
  tmp_plot("set ylabel \"response rates count\"");
  tmp_plot("set style data histogram");
  tmp_plot("set style histogram clustered gap 1");
  tmp_plot("set style fill solid 0.4 border");
}
void plot_response_rates(GNUplot &tmp_plot)
{
  cout << "plot_response_rates" << endl;
  // tmp_plot("plot \"./output/response_rate.dat\" u 2:xticlabels(1) title columnheader(2)");
  tmp_plot("set output \"./output/response_rate.png\"");
  tmp_plot("plot \"./output/response_rate.dat\" u 2:xticlabels(1)");
}
void plot_theta_init(GNUplot &tmp_plot)
{
  tmp_plot("unset key");
  tmp_plot("set xtics nomirror");
  tmp_plot("set ytics nomirror");
  tmp_plot("set terminal png size 640*2,480*2");
  tmp_plot("set title \"theta\"");
  tmp_plot("set xlabel \"theta\"");
  tmp_plot("set ylabel \"theta count\"");
  tmp_plot("set style data histogram");
  tmp_plot("set style histogram clustered gap 1");
  tmp_plot("set style fill solid 0.4 border");
}
void plot_thetas(GNUplot &tmp_plot)
{
  cout << "plot_thetas" << endl;
  tmp_plot("set output \"./output/theta.png\"");
  tmp_plot("plot \"./output/theta.dat\" u 2:xticlabels(1)");
}
