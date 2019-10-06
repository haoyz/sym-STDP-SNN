#ifndef _MNIST_H_
#define _MNIST_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int ReverseInt(int i); 
void read_mnist_label(string filename, vector<float> &labels);
void read_mnist_images(string filename, vector<vector<float>> &images);

#endif