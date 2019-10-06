#ifndef _GNUPLOT_H_
#define _GNUPLOT_H_

#include <string>
#include <stdio.h> 
#include <vector>
using namespace std;

class GNUplot
{
public:
  GNUplot() throw(string);
  ~GNUplot();
  void operator()(const string &command);
  void operator()(const vector<string> &command);

protected:
  FILE *gnuplotpipe;
};
#endif