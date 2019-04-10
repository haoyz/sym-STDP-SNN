#include "caltech.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

void read_caltech(string datapath, vector<vector<float>> &images, vector<float> &labels, vector<vector<float>> &images_test, vector<float> &labels_test)
{
    Mat img;
    Mat img1;
    vector<float> image;
    vector<String> files;
    vector<vector<float>> tmp_images;
    vector<float> tmp_labels;

    string filepath = datapath + "Faces/*.jpg";
    glob(filepath, files, false);
    size_t count = files.size();
    int num_train_samples = 400;
    for (int i = 0; i < count; i++)
    {
        img = imread(files[i]);
        img.convertTo(img1, CV_32F);
        // imshow("caltech", img1);
        // waitKey(10);
        image.assign(img1.datastart, img1.dataend);
        tmp_images.push_back(image);
    }
    tmp_labels.insert(tmp_labels.begin(), count, 0); //在pos位置插入n个elem数据。无返回值。

    filepath = datapath + "Motorbikes/*.jpg";
    glob(filepath, files, false);
    count = files.size();
    for (int i = 0; i < count; i++)
    {
        img = imread(files[i]);
        img.convertTo(img1, CV_32F);
        image.assign((float *)img1.datastart, (float *)img1.dataend);
        tmp_images.push_back(image);
    }
    tmp_labels.insert(tmp_labels.end(), count, 1); //在pos位置插入n个elem数据。无返回值。

    vector<int> id;
    int i = 0;
    while (id.size() < tmp_images.size())
        id.push_back(i++);
    random_shuffle(id.begin(), id.end());

    for (int j = 0; j < num_train_samples; j++)
    {
        images.push_back(tmp_images[id[j]]);
        labels.push_back(tmp_labels[id[j]]);
    }
    for (int j = num_train_samples; j < id.size(); j++)
    {
        images_test.push_back(tmp_images[id[j]]);
        labels_test.push_back(tmp_labels[id[j]]);
    }
}
