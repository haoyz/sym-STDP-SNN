#ifndef _CIFAR10_H_
#define _CIFAR10_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void read_cifar10_test(string datapath, bool grayscale, vector<vector<float>> &images, vector<float> &labels);
void read_cifar10_train(string datapath, bool grayscale, vector<vector<float>> &images, vector<float> &labels);
void read_cifar10_batch(ifstream &file, bool grayscale, vector<vector<float>> &images, vector<float> &labels);
#endif