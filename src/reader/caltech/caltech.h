#ifndef __CALTECH__
#define __CALTECH__

// #include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
// using namespace cv;
using namespace std;

void read_caltech(string datapath, vector<vector<float>> &images, vector<float> &labels, vector<vector<float>> &images_test, vector<float> &labels_test);

#endif