#include "cifar10.h"

void read_cifar10_test(string datapath, bool grayscale, vector<vector<float>> &images, vector<float> &labels)
{
  ifstream file;
  file.open((datapath + "cifar-10-batches-bin/test_batch.bin").c_str(), ios::binary);
  if (file.is_open())
  {
    read_cifar10_batch(file, grayscale, images, labels);
    cout << "**********test_batch.bin**********" << endl;
    file.close();
  }
  else
  {
    cout << "error open file!" << endl;
    exit(1);
  }
}

void read_cifar10_train(string datapath, bool grayscale, vector<vector<float>> &images, vector<float> &labels)
{
  ifstream file;
  char Strs[][38] = {"cifar-10-batches-bin/data_batch_1.bin", "cifar-10-batches-bin/data_batch_2.bin",
                     "cifar-10-batches-bin/data_batch_3.bin", "cifar-10-batches-bin/data_batch_4.bin",
                     "cifar-10-batches-bin/data_batch_5.bin"};

  for (int i = 0; i < 5; i++)
  {
    file.open((datapath + Strs[i]).c_str(), ios::binary);
    if (file.is_open())
    {
      read_cifar10_batch(file, grayscale, images, labels);
      cout << "**********data_batch_" << i + 1 << ".bin**********" << endl;
      file.close();
    }
    else
    {
      cout << "error open file!" << endl;
      exit(1);
    }
  }
}

void read_cifar10_batch(ifstream &file, bool grayscale, vector<vector<float>> &images, vector<float> &labels)
{
  int number_of_images = 10000;
  int n_channel = 3;
  int n_rows = 32;
  int n_cols = 32;
  unsigned char label;
  unsigned char image = 0;
  vector<float> tp_gray;
  vector<float> tp;
  float R, G, B;

  for (int i = 0; i < number_of_images; i++)
  {
    file.read((char *)&label, sizeof(label));
    labels.push_back((float)label);
    for (int ch = 0; ch < n_channel; ch++)
    {
      for (int r = 0; r < n_rows; r++)
      {
        for (int c = 0; c < n_cols; c++)
        {
          file.read((char *)&image, sizeof(image));
          tp.push_back(image);
        }
      }
    }
    if (grayscale == true)
    {
      for (int r = 0; r < n_rows; r++)
      {
        for (int c = 0; c < n_cols; c++)
        {
          tp_gray.push_back(0.2989 * R + 0.5870 * G + 0.1140 * B);
        }
      }
      images.push_back(tp_gray);
    }
    else
      images.push_back(tp);
  }

  cout << "number of images = " << images.size() << endl;
  cout << "images[0].size() = " << images[0].size() << endl;
  cout << "channel = " << n_channel << endl;
  cout << "rows = " << n_rows << endl;
  cout << "cols = " << n_cols << endl;
}