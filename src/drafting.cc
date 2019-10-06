#include "drafting.h"
#include "global.h"
#include "model.h"
#include <iostream>
using namespace std;
Drafting::~Drafting()
{
    gnuplot.~GNUplot();
}
void Drafting::update()
{
    if (replot == true)
    {
        plot[0] = output_file;
        replace(plot[0], ".png", to_string(replot_count) + ".png");
        replot_count++;
    }
    gnuplot(plot);
}
vector<string> init_assign_dist{
    "unset key",
    "set terminal png size 640*1,480*1",
    "set title \"Assignments distribution\"",
    "set xlabel \"assignments\"",
    "set ylabel \"count\"",
    "set style data histogram",
    "set style histogram clustered gap 1",
    "set style fill solid 0.4 border",
};
vector<string> plot_assign_dist{
    "set output \"./output/assign_dist.png\"", 
    "plot \"./output/assign_dist.dat\" u 2:xticlabels(1) ",
};
vector<string> init_PEw{
    "unset key",
    "set terminal png size 640,480",
    "unset border",
    "unset xtics",
    "unset ytics",
    "set size square",
    "set cbrange [0:1.0]",
    "set palette defined (-2 \"white\", -1 \"yellow\", 1 \"red\", 2 \"black\")",
};
vector<string> plot_PEw{
    "set output \"./output/PEweight.png\"",
    "plot \"./output/plot_visual_PEw_bin.dat\" binary array=(" +
        to_string(NExc_Sqrt *WIDTH) + "," + to_string(NExc_Sqrt *HEIGHT) +
        ") endian=little format=\"%float\" with image",
};
vector<string> init_ECw{
    "unset key",
    "set terminal png size 640,480",
    "unset border",
    "unset xtics",
    "unset ytics",
    "set size square",
    "set cbrange [0:" + to_string(gEC_MAX) + "]",
};
vector<string> plot_ECw{
    "set output \"./output/ECw.png\"",
    "plot \"./output/plot_visual_ECw_bin.dat\" binary array=(" + to_string(ECw_X) + "," +
        to_string(ECw_Y) + ") endian=little format=\"%float\" with image",
};
vector<string> init_varianceP2E{
    "unset key",
    "set title \"Weight Variance\"",
    "set terminal png",
    "set xlabel \"number of samples\"",
    "set ylabel \"variance\"",
    "set xrange [0:" + to_string(UPDATE_TOTAL) + "]", 
    "set grid",
};
vector<string> plot_varianceP2E{
    "set output \"./output/variance.png\"",
    "plot \"./output/variance.dat\" with lines lc 3 lw 2",
};
vector<string> init_varianceP2E_distribution{
    "unset key",
    "set terminal png size 640,480",
    "set title \"variance distribution\"",
    "set xlabel \"variance\"",
    "set ylabel \"count\"",
    "set style data histogram",
    "set style histogram clustered gap 1",
    "set style fill solid 0.4 border",
    "nth(countCol,labelCol,n) = ((int(column(countCol)) % n == 0) ? stringcolumn(labelCol) : \"\")",
};
vector<string> plot_varianceP2E_distribution{
    "set output \"./output/var_dist.png\"",
    "plot \"./output/var_dist.dat\" u 2:xticlabels(nth(1,1,10)) ",
};
vector<string> init_varianceE2C{
    "unset key",
    "set title \"Weight Variance E2C\"",
    "set terminal png",
    "set xlabel \"number of samples\"",
    "set ylabel \"variance E2C\"",
    "set xrange [0:" + to_string(UPDATE_TOTAL) + "]",
    "set grid",
};
vector<string> plot_varianceE2C{
    "set output \"./output/variance_E2C.png\"",
    "plot \"./output/variance_E2C.dat\" with lines lc 3 lw 2",
};
vector<string> init_varianceE2C_distribution{
    "unset key",
    "set terminal png size 640,480",
    "set title \"Variance E2C Distribution\"",
    "set xlabel \"variance E2C\"",
    "set ylabel \"count\"",
    "set style data histogram",
    "set style histogram clustered gap 1",
    "set style fill solid 0.4 border",
    "nth(countCol,labelCol,n) = ((int(column(countCol)) % n == 0) ? stringcolumn(labelCol) : \"\")",
};
vector<string> plot_varianceE2C_distribution{
    "set output \"./output/var_E2C_dist.png\"",
    "plot \"./output/var_E2C_dist.dat\" u 2:xticlabels(nth(1,1,10)) ",
};
vector<string> init_performance{
    "set size ratio 0.5",
    "set title \"Classification Performance\"",
    "set xlabel \"number of samples\"",
    "set ylabel \"accuracy(%)\"",
    "set terminal png size 640,480",
    "set xrange [0:" + to_string(UPDATE_TOTAL) + "]", 
    "set yrange [0:100]",
    "set xtics 0,6," + to_string(UPDATE_TOTAL),
    "set ytics 0,5,100",
    "set grid",
};
vector<string> plot_performance{
    "set output \"./output/performance_xyNowUseTestData.png\"",
    "plot \"./output/performance_xyNowUseTestData.dat\" using 1:2 with linespoints lc 3 lw 2 pt 7 ps 1 title \"exc\"",
};

vector<string> init_confusion{
    "unset key",
    "unset border",
    "set size square",
    "set cbrange [0:1.0]",
    "set palette defined (-2 \"white\", -1 \"yellow\", 1 \"red\", 2 \"black\")",
    "set title \"Confusion Matrix\"",
    "set xlabel \"Desired output digit\"",
    "set ylabel \"Inferred output digit\"",
    "set xrange [-2:10]",
    "set yrange [-2:10]",
    "set xtics 0,1,9 nomirror",
    "set ytics 0,1,9 nomirror",
    "set terminal png",
};
vector<string> plot_usl_confusion{
    "set output \"./output/confusion_m_unsupervised.png\"",
    "set pm3d map",
    "splot \"./output/plot_confusion_m_unsupervised.dat\" with image",
};
vector<string> plot_sl_confusion{
    "set output \"./output/confusion_m_supervised.png\"",
    "set pm3d map",
    "splot \"./output/plot_confusion_m_supervised.dat\" with image",
};
vector<string> init_spikes{
    "unset key",
    "set title \"Spikes\"",
    "set style line 1 lc rgb \'blue\' pt 3 ps 1",
    "set xlabel \"time(ms)\"",
    "set ylabel \"neuron ID\"",
    "set xrange [-1:" + to_string(RUN_TIME) + "]",
    "set yrange [-10:" + to_string(NExc + 10) + "]",
    "set xtics 0,50," + to_string(RUN_TIME),
};
vector<string> plot_spikes{
    "plot \"./output/spike.dat\" w p ls 1",
};
vector<string> init_Claspikes{
    "unset key",
    "set title \"Cla Spikes\"",
    "set style line 1 lc rgb \'blue\' pt 3 ps 1",
    "set xlabel \"time(ms)\"",
    "set ylabel \"neuron ID\"",
    "set xrange [-1:" + to_string(RUN_TIME) + "]",
    "set yrange [-1:" + to_string(NCla) + "]",
    "set xtics 0,50," + to_string(RUN_TIME),
    "set ytics -1,1,10",
};
vector<string> plot_Claspikes{
    "plot \"./output/cla_spike.dat\" w p ls 1",
};
vector<string> init_response_rates{
    "unset key",
    "set xtics nomirror",
    "set ytics nomirror",
    "set terminal png size 640,480",
    "set title \"Response Rate\"",
    "set xlabel \"response rates\"",
    "set ylabel \"count\"",
    "set style data histogram",
    "set style histogram clustered gap 1",
    "set style fill solid 0.4 border",
    "nth(countCol,labelCol,n) = ((int(column(countCol)) % n == 0) ? stringcolumn(labelCol) : \"\")",
};
vector<string> plot_response_rates{
    "set output \"./output/response_rate.png\"",
    "plot \"./output/response_rate.dat\" u 2:xticlabels(nth(1,1,10))",
};
vector<string> init_theta{
    "unset key",
    "set xtics nomirror",
    "set ytics nomirror",
    "set terminal png size 640,480",
    "set title \"theta\"",
    "set xlabel \"theta\"",
    "set ylabel \"count\"",
    "set style data histogram",
    "set style histogram clustered gap 1",
    "set style fill solid 0.4 border",
    "nth(countCol,labelCol,n) = ((int(column(countCol)) % n == 0) ? stringcolumn(labelCol) : \"\")",
};
vector<string> plot_theta{
    "set output \"./output/theta.png\"",
    "plot \"./output/theta.dat\" u 2:xticlabels(nth(1,1,10))",
};
bool replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}