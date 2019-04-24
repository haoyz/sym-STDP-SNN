#include "gnuplot.h"
#include <vector>

GNUplot::GNUplot() throw(string)
{
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

void GNUplot::operator()(const vector<string> &command)
{
  for (auto val : command)
  {
    fprintf(gnuplotpipe, "%s\n", val.c_str());
    fflush(gnuplotpipe);
  }
};