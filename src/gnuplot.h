#ifndef _GNUPLOT_H_
#define _GNUPLOT_H_

#include <string>
#include <stdio.h> //FILE*需要
using namespace std;

class GNUplot
{
public:
  GNUplot() throw(string);
  ~GNUplot();
  void operator()(const string &command);
  // send any command to gnuplot
protected:
  FILE *gnuplotpipe;
};
#endif