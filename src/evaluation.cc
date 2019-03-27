#include "evaluation.h"
#include <iostream>
using namespace std;

void normalize_weights(float *tmp_gP2E)
{
  float tmp_sum, tmp_factors;
  for (int i = 0; i < _NExc; i++)
  {
    tmp_sum = 0; //为了优化不单独归零
    for (int j = 0; j < _NPoi; j++)
      tmp_sum += tmp_gP2E[i + j * _NExc]; //PE之间归一化权重
    tmp_factors = _NORMAL / tmp_sum;
    for (int j = 0; j < _NPoi; j++)
    {
      tmp_gP2E[i + j * _NExc] *= tmp_factors; //c+=支持向量乘法么……
    }
  }
}

void cla_normalize_weights(float *tmp_gE2C) // revised at 2017-10-12 16:00
{
  float tmp_sum, tmp_factors;
  for (int i = 0; i < _NCla; i++)
  {
    tmp_sum = 0; //为了优化不单独归零
    for (int j = 0; j < _NExc; j++)
      tmp_sum += tmp_gE2C[i + j * _NCla]; //EC之间归一化权重
    tmp_factors = _cla_NORMAL / tmp_sum;
    for (int j = 0; j < _NExc; j++)
    {
      tmp_gE2C[i + j * _NCla] *= tmp_factors; //c+=支持向量乘法么……
    }
  }
}
// void cla_normalize_weights(float *tmp_gE2C)
// {
//   float tmp_sum, tmp_factors;
//   for (int i = 0; i < _NExc; i++)
//   {
//     tmp_sum = 0; //为了优化不单独归零
//     for (int j = 0; j < _NCla; j++)
//       tmp_sum += tmp_gE2C[i * _NCla + j]; //EC之间归一化权重
//     tmp_factors = _cla_NORMAL / tmp_sum;
//     for (int j = 0; j < _NCla; j++)
//     {
//       tmp_gE2C[i * _NCla + j] *= tmp_factors; //c+=支持向量乘法么……
//     }
//   }
// }
template <class T>
int getArrayLen(T &array)
{ //使用模板定义一 个函数getArrayLen,该函数将返回数组array的长度
  return (sizeof(array) / sizeof(array[0]));
}

// ------------------------------------------------------------------------------
//  识别样本
// ------------------------------------------------------------------------------
int get_recognized_number(int spike_rates[_NExc], int *assignments) //每个样本，每个神经元的发放次数
{
  int inferred_num = -1;
  int num_assignments[10] = {0};
  float rate_sum[10] = {0};
  float rate[10] = {0};
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < _NExc; j++) //assignments有几个i
    {
      if (assignments[j] == i)
      {
        num_assignments[i]++;          //assignments有几个i.绝对不会是0的。
        rate_sum[i] += spike_rates[j]; //对应的spike_rates求和.rate_sum[i]存的是每个数字共发放多少次
      }
    }
    rate[i] = rate_sum[i] / num_assignments[i]; //存起来？！
  }
  float rate_max = 0;
  for (int i = 0; i < 10; i++)
  {
    if (rate[i] > rate_max)
    {
      rate_max = rate[i];
      inferred_num = i;
    }
  }
  return inferred_num;
}
// int get_recognized_number(int spike_rates[_NExc], int *assignments)
// {
// #define threshold 0.7
//   int sum = 0;
//   int order_ID[4] = {0};
//   int max = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     sum += spike_rates[i];
//     if (spike_rates[i] > max)
//     {
//       max = spike_rates[i];
//       order_ID[0] = i;
//     }
//   }
//   max = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     if (spike_rates[i] > max && i != order_ID[0])
//     {
//       max = spike_rates[i];
//       order_ID[1] = i;
//     }
//   }
//   max = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     if (spike_rates[i] > max && i != order_ID[0] && i != order_ID[1])
//     {
//       max = spike_rates[i];
//       order_ID[2] = i;
//     }
//   }
//   max = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     if (spike_rates[i] > max && i != order_ID[0] && i != order_ID[1] && i != order_ID[2])
//     {
//       max = spike_rates[i];
//       order_ID[3] = i;
//     }
//   }
//   if (spike_rates[order_ID[0]] > threshold * sum)
//     return assignments[order_ID[0]];
//   else if ((spike_rates[order_ID[0]] + spike_rates[order_ID[1]]) > threshold * sum && assignments[order_ID[0]] == assignments[order_ID[1]])
//     return assignments[order_ID[0]];
//   else if ((spike_rates[order_ID[0]] + spike_rates[order_ID[1]] + spike_rates[order_ID[2]]) > threshold * sum && assignments[order_ID[0]] == assignments[order_ID[1]] && assignments[order_ID[0]] == assignments[order_ID[2]])
//     return assignments[order_ID[0]];
//   else if ((spike_rates[order_ID[0]] + spike_rates[order_ID[1]] + spike_rates[order_ID[2]] + spike_rates[order_ID[3]]) > threshold * sum && assignments[order_ID[0]] == assignments[order_ID[1]] && assignments[order_ID[0]] == assignments[order_ID[2]] && assignments[order_ID[0]] == assignments[order_ID[3]])
//     return assignments[order_ID[0]];
//   else if (assignments[order_ID[0]] == assignments[order_ID[1]] && assignments[order_ID[0]] == assignments[order_ID[2]] && assignments[order_ID[0]] == assignments[order_ID[3]])
//   {
//     cout << "not enough" << endl;
//     return -1;
//   }
//   else
//   {
//     cout << assignments[order_ID[0]] << "&" << assignments[order_ID[1]] << "&" << assignments[order_ID[2]] << "&" << assignments[order_ID[3]] << "  " << spike_rates[order_ID[0]] << "&" << spike_rates[order_ID[1]] << "&" << spike_rates[order_ID[2]] << "&" << spike_rates[order_ID[3]] << endl;
//     return -1;
//   }
// }
int cla_get_recognized_number(int spike_rates[_NCla]) //每个样本，每个神经元的发放次数
{
  int inferred_num = -1;
  int rate_max = 0;
  for (int i = 0; i < 10; i++)
  {
    if (spike_rates[i] > rate_max)
    {
      rate_max = spike_rates[i];
      inferred_num = i;
    }
  }
  return inferred_num;
}
// ------------------------------------------------------------------------------
//  准确率计数// 注意形参
// ------------------------------------------------------------------------------
void get_new_assignments(int *assignments, int offset, int result_monitor[_LABELS_CONST][_NExc], vector<float> &labels) //result_monitor每个神经元对每个样本的激发次数
{
  int num_assignments[10] = {0};
  float rate_sum[_NExc][10] = {0};
  float maximum_rate = 0;
  for (int i = 0; i < _LABELS_CONST; i++)
  {
    for (int j = 0; j < 10; j++) //测试集中每个数字各有多少个。十维
    {
      if (labels[i + offset] == j)
      {
        num_assignments[j]++;
        j = 9;
      }
    }
  }
  // cout << "测试集每个数字各有多少个:" << endl;
  // for (int i = 0; i < 10; i++)
  //   cout << num_assignments[i] << "  ";
  // cout << endl;
  for (int i = 0; i < _LABELS_CONST; i++)
  {
    for (int k = 0; k < 10; k++)
    {
      if (labels[i + offset] == k)
      {
        for (int j = 0; j < _NExc; j++)
        {
          rate_sum[j][k] += result_monitor[i][j]; //测试集中每个样本，每个数字发放次数分别求和
        }
        k = 9; // 进入一次就可以跳出k的循环了
      }
    }
  }
  // cout << "测试集" << _LABELS_CONST << "*" << _NExc << "矩阵:" << endl;
  // for (int i = 0; i < _LABELS_CONST; i++)
  // {
  //   for (int j = 0; j < _NExc; j++)
  //   {
  //     cout << result_monitor[i][j] << " ";
  //     if (j % 40 == 0 && j > 0)
  //       cout << endl;
  //   }
  // }
  // cout << "测试集" << _NExc << "*10矩阵:" << endl;
  // for (int i = 0; i < _NExc; i++)
  // {
  //   for (int j = 0; j < 10; j++)
  //     cout << rate_sum[i][j] << " ";
  //   if (i % 4 == 0 && i > 0)
  //     cout << endl;
  // }

  for (int j = 0; j < 10; j++)
  {
    if (num_assignments[j] > 0)
    {
      for (int k = 0; k < _NExc; k++)
      {
        rate_sum[k][j] = rate_sum[k][j] / (float)num_assignments[j];
      }
    }
  }
  // cout << "测试集_NExc*10矩阵平均值:" << endl;
  // for (int i = 0; i < _NExc; i++)
  // {
  //   for (int j = 0; j < 10; j++)
  //     cout << rate_sum[i][j] << " ";
  //   cout << endl;
  // }
  for (int i = 0; i < _NExc; i++)
  {
    maximum_rate = 0;
    for (int j = 0; j < 10; j++)
    {
      if (rate_sum[i][j] > maximum_rate)
      {
        maximum_rate = rate_sum[i][j];
        assignments[i] = j;
      }
    }
  }
  cout << "assignments[" << _NExc << "]:" << endl;
  int neuron_not_learn_num = 0;
  for (int i = 0; i < _NExc; i++)
  {
    if (i % 40 == 0 && i > 0)
      cout << endl;
    cout << assignments[i] << " ";
    if (assignments[i] == -1)
      neuron_not_learn_num++;
  }
  cout << endl;
  cout << "no_learning Neuron:" << neuron_not_learn_num << " in " << _NExc << endl;
}
// void get_new_assignments(int *assignments, int offset, int result_monitor[_LABELS_CONST][_NExc], vector<float> &labels) //result_monitor每个神经元对每个样本的激发次数
// {
// #define thres 0.7
//   float rate_sum[_NExc][4] = {0};
//   int max;
//   int sum;
//   int tmp, tmp1, tmp2, tmp3;
//   for (int j = 0; j < _NExc; j++)
//   {
//     max = 0;
//     sum = 0;
//     for (int i = 0; i < _LABELS_CONST; i++)
//     {
//       sum += result_monitor[i][j];
//     }
//     for (int i = 0; i < _LABELS_CONST; i++)
//     {
//       if (result_monitor[i][j] > max)
//       {
//         max = result_monitor[i][j];
//         rate_sum[j][0] = i;
//       }
//     }
//     max = 0;
//     for (int i = 0; i < _LABELS_CONST; i++)
//     {
//       if (result_monitor[i][j] > max && rate_sum[j][0] != i)
//       {
//         max = result_monitor[i][j];
//         rate_sum[j][1] = i;
//       }
//     }
//     max = 0;
//     for (int i = 0; i < _LABELS_CONST; i++)
//     {
//       if (result_monitor[i][j] > max && rate_sum[j][0] != i && rate_sum[j][1] != i)
//       {
//         max = result_monitor[i][j];
//         rate_sum[j][2] = i;
//       }
//     }
//     max = 0;
//     for (int i = 0; i < _LABELS_CONST; i++)
//     {
//       if (result_monitor[i][j] > max && rate_sum[j][0] != i && rate_sum[j][1] != i && rate_sum[j][2] != i)
//       {
//         max = result_monitor[i][j];
//         rate_sum[j][3] = i;
//       }
//     }
//     tmp = rate_sum[j][0];
//     tmp1 = rate_sum[j][1];
//     tmp2 = rate_sum[j][2];
//     tmp3 = rate_sum[j][3];
//     if (result_monitor[tmp][j] > thres * sum)
//       assignments[j] = labels[tmp];
//     else if ((result_monitor[tmp][j] + result_monitor[tmp1][j]) > thres * sum && labels[tmp] == labels[tmp1])
//       assignments[j] = labels[tmp];
//     else if ((result_monitor[tmp][j] + result_monitor[tmp1][j] + result_monitor[tmp2][j]) > thres * sum && labels[tmp] == labels[tmp1] && labels[tmp] == labels[tmp2])
//       assignments[j] = labels[tmp];
//     else if ((result_monitor[tmp][j] + result_monitor[tmp1][j] + result_monitor[tmp2][j] + result_monitor[tmp3][j]) > thres * sum && labels[tmp] == labels[tmp1] && labels[tmp] == labels[tmp2] && labels[tmp] == labels[tmp3])
//       assignments[j] = labels[tmp];
//     else if (labels[tmp] == labels[tmp1] && labels[tmp] == labels[tmp2] && labels[tmp] == labels[tmp3])
//     {
//       cout << "not enough" << endl;
//       assignments[j] = -1;
//     }
//     else
//     {
//       cout << labels[tmp] << "&" << labels[tmp1] << "&" << labels[tmp2] << "&" << labels[tmp3] << "  " << result_monitor[tmp][j] << "&" << result_monitor[tmp1][j] << "&" << result_monitor[tmp2][j] << "&" << result_monitor[tmp3][j] << endl;
//       assignments[j] = -1;
//     }
//   }
//   cout << "assignments[" << _NExc << "]:" << endl;
//   int neuron_not_learn_num = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     cout << assignments[i] << " ";
//     if (assignments[i] == -1)
//       neuron_not_learn_num++;
//     if (i % 40 == 0 && i > 0)
//       cout << endl;
//   }
//   cout << endl;
//   cout << "the Num of no_learning Neuron:" << neuron_not_learn_num << " of total " << _NExc << endl;
// }
// void get_new_assignments(int *assignments, int offset, int result_monitor[_LABELS_CONST][_NExc], vector<float> &labels) //result_monitor每个神经元对每个样本的激发次数
// {
//   int num_assignments[10] = {0};
//   float rate_sum[_NExc][10] = {0};
//   float maximum_rate = 0;
//   int assignments2[_NExc] = {0};
//   int assignments3[_NExc] = {0};
//   float sum = 0;
//   int tmp, tmp2, tmp3;
//   for (int i = 0; i < _LABELS_CONST; i++)
//   {
//     for (int j = 0; j < 10; j++) //测试集中每个数字各有多少个。十维
//     {
//       if (labels[i + offset] == j)
//       {
//         num_assignments[j]++;
//         j = 9;
//       }
//     }
//   }
//   for (int i = 0; i < _LABELS_CONST; i++)
//   {
//     for (int k = 0; k < 10; k++)
//     {
//       if (labels[i + offset] == k)
//       {
//         for (int j = 0; j < _NExc; j++)
//         {
//           rate_sum[j][k] += result_monitor[i][j]; //测试集中每个样本，每个数字发放次数分别求和
//         }
//         k = 9; // 进入一次就可以跳出k的循环了
//       }
//     }
//   }
//   for (int j = 0; j < 10; j++)
//   {
//     if (num_assignments[j] > 0)
//     {
//       for (int k = 0; k < _NExc; k++)
//       {
//         rate_sum[k][j] = rate_sum[k][j] / (float)num_assignments[j];
//       }
//     }
//   }
//   for (int i = 0; i < _NExc; i++)
//   {
//     sum = 0;
//     for (int j = 0; j < 10; j++)
//     {
//       sum += rate_sum[i][j];
//     }
//     maximum_rate = 0;
//     for (int j = 0; j < 10; j++)
//     {
//       if (rate_sum[i][j] > maximum_rate)
//       {
//         maximum_rate = rate_sum[i][j];
//         assignments[i] = j;
//       }
//     }
//     maximum_rate = 0;
//     for (int j = 0; j < 10; j++)
//     {
//       if (rate_sum[i][j] > maximum_rate && assignments[i] != j)
//       {
//         maximum_rate = rate_sum[i][j];
//         assignments2[i] = j;
//       }
//     }
//     maximum_rate = 0;
//     for (int j = 0; j < 10; j++)
//     {
//       if (rate_sum[i][j] > maximum_rate && assignments[i] != j && assignments2[i] != j)
//       {
//         maximum_rate = rate_sum[i][j];
//         assignments3[i] = j;
//       }
//     }
//     tmp = assignments[i];
//     tmp2 = assignments2[i];
//     tmp3 = assignments3[i];
// #define thresh 0.75
//     if (rate_sum[i][tmp] > thresh * sum)
//       assignments[i] = tmp;
//     else if ((rate_sum[i][tmp] + rate_sum[i][tmp2]) > thresh * sum && tmp2 == tmp)
//       assignments[i] = tmp;
//     else if ((rate_sum[i][tmp] + rate_sum[i][tmp2] + rate_sum[i][tmp3]) > thresh * sum && tmp2 == tmp && tmp3 == tmp)
//       assignments[i] = tmp;
//     else if (tmp2 == tmp && tmp3 == tmp)
//     {
//       cout << "assignment not enough" << endl;
//       assignments[i] = -1;
//     }
//     else
//     {
//       cout << tmp << "&" << tmp2 << "&" << tmp3 << " " << rate_sum[i][tmp] << "&" << rate_sum[i][tmp2] << "&" << rate_sum[i][tmp3] << endl;
//       assignments[i] = -1;
//     }
//   }
//   cout << "assignments[" << _NExc << "]:" << endl;
//   int neuron_not_learn_num = 0;
//   for (int i = 0; i < _NExc; i++)
//   {
//     cout << assignments[i] << " ";
//     if (assignments[i] == -1)
//       neuron_not_learn_num++;
//     if (i % 40 == 0 && i > 0)
//       cout << endl;
//   }
//   cout << endl;
//   cout << "the Num of no_learning Neuron:" << neuron_not_learn_num << " of total " << _NExc << endl;
// }

void get_assignments_distribution(int *assignments, int assignments_dist[10])
{
  fill_n(assignments_dist, 10, 0);
  for (int i = 0; i < _NExc; i++)
  {
    if (assignments[i] != -1)
      assignments_dist[assignments[i]]++;
  }
}
void write_assignments_distribution_to_file(int assignments_dist[10])
{
  int no_learnning_neuron_num = _NExc;
  char const *file_name = "./output/assign_dist.dat";
  ofstream tmp_assign_dist_file(file_name, ios::trunc);
  if (tmp_assign_dist_file)
  {
    tmp_assign_dist_file << "assignments_distribution" << endl
                         << "assignments_distribution_count" << endl;
    for (int i = 0; i < 10; i++)
    {
      tmp_assign_dist_file << i << " " << assignments_dist[i] << endl;
      no_learnning_neuron_num -= assignments_dist[i];
    }
    tmp_assign_dist_file << -1 << " " << no_learnning_neuron_num << endl;
    tmp_assign_dist_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/assign_dist.dat!" << endl;
  }
}
void get_visual_PEw(float tmp_visual[_NPoi_Sqrt * _NExc_Sqrt][_NPoi_Sqrt * _NExc_Sqrt])
{
  int i_v = 0;
  int j_v = 0;
  // for (int i = 0; i < _NExc; i++)
  // {
  //   i_v = (i / _NExc_Sqrt) * _NPoi_Sqrt;
  //   j_v = (i % _NExc_Sqrt) * _NPoi_Sqrt;
  //   for (int j = 0; j < _NPoi; j++)
  //   {
  //     tmp_visual[i_v + j / _NPoi_Sqrt][j_v + j % _NPoi_Sqrt] = gP2E[i + j * _NExc];
  //   }
  // }
  for (int i = 0; i < _NPoi; i++)
  {
    for (int j = 0; j < _NExc; j++)
    {
      tmp_visual[i / _NPoi_Sqrt + j / _NExc_Sqrt * _NPoi_Sqrt][i % _NPoi_Sqrt + j % _NExc_Sqrt * _NPoi_Sqrt] = gP2E[i * _NExc + j];
    }
  }
}
void write_visual_PEw_to_file(float tmp_visual[_NPoi_Sqrt * _NExc_Sqrt][_NPoi_Sqrt * _NExc_Sqrt])
{
  // ------------------------------------------------------------------------------
  //  文本读取
  // ------------------------------------------------------------------------------
  // ofstream tmp_plot_file("./output/plot_2d_txt.dat",ios:trunc);
  // for (int i = 0; i < _NPoi_Sqrt * _NExc_Sqrt; i++)
  // {
  //   for (int j = 0; j < _NPoi_Sqrt * _NExc_Sqrt; j++)
  //   {
  //     tmp_plot_file << i << " " << j << " " << tmp_visual[i][j] << endl;
  //   }
  //   tmp_plot_file << endl;
  // }
  // tmp_plot_file << endl;
  // tmp_plot_file.close();
  // tmp_plotter("splot \"./output/plot_2d_txt.dat\"");
  // ------------------------------------------------------------------------------
  //  二进制读取
  // ------------------------------------------------------------------------------
  char const *file_name = "./output/plot_visual_PEw_bin.dat"; //file_name make valgrind happy but fuck not!!!
  ofstream tmp_plot_file(file_name, ios::binary | ios::trunc);
  for (int i = 0; i < _NPoi_Sqrt * _NExc_Sqrt; i++)
  {
    for (int j = 0; j < _NPoi_Sqrt * _NExc_Sqrt; j++)
    {
      tmp_plot_file.write((char *)&tmp_visual[_NPoi_Sqrt * _NExc_Sqrt - i][j], sizeof(float)); //float make valgrind happy but tmp_visual can't but fuck not!!!
    }
  }
  tmp_plot_file.close();
}
void get_visual_ECw(float tmp_visual[_ECw_X][_ECw_Y])
{
  for (int i = 0; i < _ECw_X; i++)
  {
    for (int j = 0; j < _ECw_Y; j++)
    {
      tmp_visual[i][j] = gE2C[i * _NCla + j];
    }
  }
}
void write_visual_ECw_to_file(float tmp_visual[_ECw_X][_ECw_Y])
{
  // ------------------------------------------------------------------------------
  //  二进制读取
  // ------------------------------------------------------------------------------
  char const *file_name = "./output/plot_visual_ECw_bin.dat"; //file_name make valgrind happy but fuck not!!!
  ofstream tmp_plot_file(file_name, ios::binary | ios::trunc);
  for (int i = 0; i < _ECw_X; i++)
  {
    for (int j = 0; j < _ECw_Y; j++)
    {
      tmp_plot_file.write((char *)&tmp_visual[i][j], sizeof(float)); //float make valgrind happy but tmp_visual can't but fuck not!!!
    }
  }
  tmp_plot_file.close();
}
void get_visual_ECw_inferred(float tmp_visual[_ECw_X][_ECw_Y], int *assignments)
{
  for (int i = 0; i < _ECw_X; i++)
  {
    for (int j = 0; j < _ECw_Y; j++)
    {
      if (assignments[i] == j)
        tmp_visual[i][j] = 1;
      else
        tmp_visual[i][j] = 0;
    }
  }
}
void write_visual_ECw_inferred_to_file(float tmp_visual[_ECw_X][_ECw_Y])
{
  // ------------------------------------------------------------------------------
  //  二进制读取
  // ------------------------------------------------------------------------------
  char const *file_name = "./output/plot_visual_ECw_inferred_bin.dat"; //file_name make valgrind happy but fuck not!!!
  ofstream tmp_plot_file(file_name, ios::binary | ios::trunc);
  for (int i = 0; i < _ECw_X; i++)
  {
    for (int j = 0; j < _ECw_Y; j++)
    {
      tmp_plot_file.write((char *)&tmp_visual[i][j], sizeof(float)); //float make valgrind happy but tmp_visual can't but fuck not!!!
    }
  }
  tmp_plot_file.close();
}
void get_performance(float *tmp_performance, int offset, int current_evaluation, int result_monitor[][_NExc], int *assignments, vector<float> &labels)
{
  int test_results[_LABELS_CONST] = {-1};
  int difference = 0;
  int correct = 0;
  char const *file_name = "./output/error_ID.dat";
  ofstream tmp_errorID_file(file_name, ios::trunc);
  for (int i = 0; i < 10000; i++) //计算assignments
  {
    test_results[i] = get_recognized_number(result_monitor[i], assignments); //result_monitor[i]单个样本，_NExc神经元各自发放数                                                                             //todo rate->test_results.降序排列，rate[9]最大
  }
  for (int i = 0; i < _performance_update_interval; i++)
  {
    difference = test_results[i] - labels[i + offset];
    if (difference < 1 && difference > -1)
      correct++;
    else
    {
      if (tmp_errorID_file)
        tmp_errorID_file << i << " " << labels[i + offset] << " " << test_results[i] << endl;
      else
        cout << "error! Can't open ./output/error_ID.dat!" << endl;
    }
  }
  tmp_errorID_file.close();
  float sum_accurracy = correct / 10000.0 * 100.0;
  fprintf(stdout, "usl accuracy: %f%%  correct:%d\n", sum_accurracy, correct);
  tmp_performance[current_evaluation] = sum_accurracy;
}
void write_performance_to_file(float *tmp_performance1, float *tmp_performance2, int current_evaluation, int fileName) //代码重用//revised at 2018-4-6
{
  char file_name[50];
  switch (fileName)
  {
  case 0:
    std::strcpy(file_name, "./output/performance_xy.dat");
    break;
  case 1:
    std::strcpy(file_name, "./output/performance_xyNowUseTrainData.dat");
    break;
  case 2:
    std::strcpy(file_name, "./output/performance_xyNowUseTestData.dat");
    break;
  default:
    printf("error!\n");
    break;
  }
  ofstream tmp_performance_file(file_name, ios::trunc);
  if (tmp_performance_file)
  {
    for (int i = 0; i < current_evaluation; i++)
    {
      tmp_performance_file << i << " " << tmp_performance1[i] << " " << tmp_performance2[i] << endl;
    }
    tmp_performance_file << endl;
    tmp_performance_file.close();
  }
  else
  {
    cout << "error! Can't open " << file_name << "!" << endl;
  }
}

void cla_get_performance(float *tmp_performance, int offset, int current_evaluation, int cla_result_monitor[][_NCla], vector<float> &labels)
{
  int test_results[_LABELS_CONST] = {-1};
  int difference = 0;
  int correct = 0;
  for (int i = 0; i < 10000; i++) //计算assignments
  {
    test_results[i] = cla_get_recognized_number(cla_result_monitor[i]); //todo rate->test_results.降序排列，rate[9]最大
  }
  for (int i = 0; i < 10000; i++)
  {
    difference = test_results[i] - labels[i + offset];
    if (difference < 1 && difference > -1)
      correct++;
  }
  float sum_accurracy = correct / 10000.0 * 100.0;
  fprintf(stdout, "sl accuracy: %f%%  correct:%d\n", sum_accurracy, correct);
  tmp_performance[current_evaluation] = sum_accurracy;
}
/*todu*/
void get_spike_rate(float spike_rate[3], int offset, int result_monitor_last1000[][_NExc], int *assignments, vector<float> &labels)
{
  // fill_n(spike_rate, 3, 0);
  //   for (int i = 0; i < 1000; i++)
  //     for (int j = 0; j < _NExc; j++)
  //     {
  //       spike_rate[0] += result_monitor_last1000[i][j];
  //       if (assignments[i] == labels[i + offset])
  //         spike_rate[1] += result_monitor_last1000[i][];
  //       else
  //         spike_rate[1] += ;
  //     }
  //   spike_rate[0] /= 1000.0;
}
void write_spike_rate_to_file(float spike_rate[3])
{
}
void get_confusion_m(float confusion_m[11][11], int offset, int result_monitor[][_NExc], int *assignments, vector<float> &labels)
{
  int test_results[_LABELS_CONST] = {-1};
  int x_i, y_j;
  fill_n(confusion_m[0], 11 * 11, 0);
  for (int i = 0; i < 10000; i++) //计算assignments
  {
    test_results[i] = get_recognized_number(result_monitor[i], assignments); //result_monitor[i]单个样本，_NExc神经元各自发放数                                                                             //todo rate->test_results.降序排列，rate[9]最大
  }
  for (int i = 0; i < _performance_update_interval; i++)
  {
    x_i = labels[i + offset] + 1;
    y_j = test_results[i] + 1;
    confusion_m[x_i][y_j]++;
  }
}
void get_confusion_m_supervised(float confusion_m[11][11], int offset, int cla_result_monitor[][_NCla], vector<float> &labels) //revised at 2018-5-13
{
  int test_results[_LABELS_CONST] = {-1};
  int x_i, y_j;
  fill_n(confusion_m[0], 11 * 11, 0);
  for (int i = 0; i < 10000; i++)
  {
    test_results[i] = cla_get_recognized_number(cla_result_monitor[i]); //cla_result_monitor[i]单个样本，_NExc神经元各自发放数                                                                             //todo rate->test_results.降序排列，rate[9]最大
  }
  for (int i = 0; i < _performance_update_interval; i++)
  {
    x_i = labels[i + offset] + 1;
    y_j = test_results[i] + 1;
    confusion_m[x_i][y_j]++;
  }
}
void write_confusion_m_to_file(float confusion_m[11][11], int flag)
{
  float confusion_m_max[11] = {0};
  ofstream tmp_plot_confusion_m_file;
  if (flag == SUPERVISED)
    tmp_plot_confusion_m_file.open("./output/plot_confusion_m_supervised.dat", ios::trunc);
  else if (flag == UNSUPERVISED)
    tmp_plot_confusion_m_file.open("./output/plot_confusion_m_unsupervised.dat", ios::trunc);
  else
    exit(2);
  for (int i = 0; i < 11; i++)
  {
    for (int j = 0; j < 11; j++)
    {
      confusion_m_max[i] += confusion_m[i][j];
    }
  }
  for (int i = 0; i < 11; i++)
  {
    for (int j = 0; j < 11; j++)
    {
      tmp_plot_confusion_m_file << i - 1 << " " << j - 1 << " " << confusion_m[i][j] / (1e-6 + confusion_m_max[i]) << endl; //1e6为了防止0/0
    }
    tmp_plot_confusion_m_file << endl;
  }
  tmp_plot_confusion_m_file << endl;
  tmp_plot_confusion_m_file.close();
}
void get_variance(float variance[_NExc])
{
  for (int i = 0; i < _NExc; i++)
  {
    variance[i] = 0;
    for (int j = 0; j < _NPoi; j++)
    {
      variance[i] += pow(abs(gP2E[i + j * _NExc] - _NORMAL / (float)_NPoi), 2); //离差平方和
    }
    variance[i] = variance[i] / _NPoi; //离差平方和平均
  }
}
void write_variance_to_file(float variance[_NExc], int current_evaluation)
{
  float variance_sum_P2E = 0;
  int variance_distribution[60] = {0};
  ofstream os_variance("./output/variance.dat", ios::app); //ios::app是追加方式写文件
  os_variance << current_evaluation << " ";
  for (int i = 0; i < _NExc; i++)
  {
    // os_variance << variance[i] << " ";
    variance_sum_P2E += variance[i];
  }
  os_variance << variance_sum_P2E << " " << endl;
  os_variance.close();
  //write_variance_distribution_to_file
  for (int i = 0; i < 60; i++)
  {
    for (int j = 0; j < _NExc; j++)
    {
      if (variance[j] * _NPoi >= i && variance[j] * _NPoi < i + 1)
        variance_distribution[i]++;
    }
  }
  ofstream os_var_dist("./output/var_dist.dat", ios::trunc);
  if (os_var_dist)
  {
    os_var_dist << "variance_distribution" << endl
                << "variance_distribution_count" << endl;
    for (int i = 0; i < 60; i++)
    {
      os_var_dist << i << " " << variance_distribution[i] << endl;
    }
    os_var_dist.close();
  }
  else
  {
    cout << "error! Can't open ./output/var_dist.dat!" << endl;
  }
}
void get_variance_gEC(float variance[_NExc])
{
  for (int i = 0; i < _NExc; i++)
  {
    variance[i] = 0;
    for (int j = 0; j < _NCla; j++)
    {
      variance[i] += pow(abs(gE2C[i + j * _NExc] - _cla_NORMAL / (float)_NCla), 2); //离差平方和
    }
    variance[i] = variance[i] / _NCla; //离差平方和平均
  }
}
void write_variance_gEC_to_file(float variance[_NExc], int current_evaluation)
{
  float variance_sum_E2C = 0;
  int variance_distribution[60] = {0};
  ofstream os_variance("./output/variance_E2C.dat", ios::app); //ios::app是追加方式写文件
  os_variance << current_evaluation << " ";
  for (int i = 0; i < _NExc; i++)
  {
    // os_variance << variance[i] << " ";
    variance_sum_E2C += variance[i];
  }
  os_variance << variance_sum_E2C << " " << endl;
  os_variance.close();
  //write_variance_distribution_to_file
  for (int i = 0; i < 60; i++)
  {
    for (int j = 0; j < _NExc; j++)
    {
      if (variance[j] * _NPoi >= i && variance[j] * _NPoi < i + 1)
        variance_distribution[i]++;
    }
  }
  ofstream os_var_dist("./output/var_E2C_dist.dat", ios::trunc);
  if (os_var_dist)
  {
    os_var_dist << "variance_gEC_distribution" << endl
                << "variance_gEC_distribution_count" << endl;
    for (int i = 0; i < 60; i++)
    {
      os_var_dist << i << " " << variance_distribution[i] << endl;
    }
    os_var_dist.close();
  }
  else
  {
    cout << "error! Can't open ./output/var_E2C_dist.dat!" << endl;
  }
}
bool write_spike_to_file(int (*tmp_spike)[_NExc])
{
  bool if_wrote = false;
  char const *file_name = "./output/spike.dat";
  ofstream tmp_spike_file(file_name, ios::trunc);
  if (tmp_spike_file)
  {
    for (int i = 0; i < _RunTime_singleImg / DT; i++)
    {
      for (int j = 0; j < _NExc; j++)
      {
        if (tmp_spike[i][j] == 1)
        {
          tmp_spike_file << i * DT << " ";
          tmp_spike_file << j << endl;
          if_wrote = true;
        }
      }
    }
    tmp_spike_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/spike.dat!" << endl;
  }
  return if_wrote;
}
bool write_cla_spike_to_file(int (*tmp_spike)[_NCla])
{
  bool if_wrote = false;
  char const *file_name = "./output/cla_spike.dat";
  ofstream tmp_spike_file(file_name, ios::trunc);
  if (tmp_spike_file)
  {
    for (int i = 0; i < _RunTime_singleImg / DT; i++)
    {
      for (int j = 0; j < _NCla; j++)
        if (tmp_spike[i][j] == 1)
        {
          tmp_spike_file << i * DT << " ";
          tmp_spike_file << j << endl;
          if_wrote = true;
        }
    }
    tmp_spike_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/cla_spike.dat!" << endl;
  }
  return if_wrote;
}
void write_response_rate_to_file(int *tmp_response)
{
  char const *file_name = "./output/response_rate.dat";
  ofstream tmp_response_file(file_name, ios::trunc);
  if (tmp_response_file)
  {
    tmp_response_file << "response_rate" << endl
                      << "response_rate_count" << endl;
    for (int i = 0; i < 60; i++)
    {
      tmp_response_file << i << " " << tmp_response[i] << endl;
    }
    tmp_response_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/response_rate.dat!" << endl;
  }
}
void write_theta_to_file(int *tmp_theta)
{
  char const *file_name = "./output/theta.dat";
  ofstream tmp_theta_file(file_name, ios::trunc);
  if (tmp_theta_file)
  {
    tmp_theta_file << "theta" << endl
                   << "theta_count" << endl;
    for (int i = 0; i < 75; i++)
    {
      tmp_theta_file << i << " " << tmp_theta[i] << endl;
    }
    tmp_theta_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/theta.dat!" << endl;
  }
}
void write_result_monitor_to_file(int tmp_result_monitor[_NExc], int tmp_label)
{
#ifdef svm_train
  char const *file_name = "./output/svm_train.dat";
#else
  char const *file_name = "./output/svm_test.dat";
#endif
  ofstream tmp_result_monitor_file(file_name, ios::app);
  if (tmp_result_monitor_file)
  {
    tmp_result_monitor_file << tmp_label << " ";
    for (int i = 0; i < _NExc; i++)
    {
      if (tmp_result_monitor[i] != 0)
        tmp_result_monitor_file << i << ":" << tmp_result_monitor[i] << " ";
    }
    tmp_result_monitor_file << endl;
    tmp_result_monitor_file.close();
  }
  else
  {
#ifdef svm_train
    cout << "error! Can't open ./output/svm_train.dat!" << endl;
#else
    cout << "error! Can't open ./output/svm_test.dat!" << endl;
#endif
  }
}
/**************************2018.05.18***************************/
void write_result_monitor_to_file_0518(int tmp_result_monitor[_NExc], int tmp_label)
{
  char const *file_name = "./output/svm_test_0518.dat";
  ofstream tmp_result_monitor_file(file_name, ios::app);
  if (tmp_result_monitor_file)
  {
    tmp_result_monitor_file << tmp_label << " ";
    for (int i = 0; i < _NExc; i++)
    {
      tmp_result_monitor_file << tmp_result_monitor[i] << " ";
    }
    tmp_result_monitor_file << endl;
    tmp_result_monitor_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/svm_test_0518.dat!" << endl;
  }
}
void write_result_monitor_to_file_0518_cla(int tmp_result_monitor[_NCla], int tmp_label)
{
  char const *file_name = "./output/svm_test_0518_cla.dat";
  ofstream tmp_result_monitor_file(file_name, ios::app);
  if (tmp_result_monitor_file)
  {
    tmp_result_monitor_file << tmp_label << " ";
    for (int i = 0; i < _NCla; i++)
    {
      tmp_result_monitor_file << tmp_result_monitor[i] << " ";
    }
    tmp_result_monitor_file << endl;
    tmp_result_monitor_file.close();
  }
  else
  {
    cout << "error! Can't open ./output/svm_test_0518_cla.dat!" << endl;
  }
}
// ------------------------------------------------------------------------------
//  temporatory XeAe 权重保存
// ------------------------------------------------------------------------------
void save_tmp_gP2E() //　revised at 2017-10-13 22:15
{
  static int tmp_id = 1;
  char tmp_commond[100];
  sprintf(tmp_commond, "./weights/gPE_%d", tmp_id);
  ofstream os_gP2E(tmp_commond);
  cout << "save gP2E" << endl;
  for (int i = 0; i < _NPoi; i++)
  {
    for (int j = 0; j < _NExc; j++)
    {
      os_gP2E << gP2E[i * _NExc + j] << " ";
    }
    os_gP2E << endl;
  }
  os_gP2E << endl;
  os_gP2E.close();
  tmp_id++;
}
// ------------------------------------------------------------------------------
//  temporatory Ae-theta 保存
// ------------------------------------------------------------------------------
void save_tmp_theta() //　revised at 2017-10-13 22:15
{
  static int tmp_id = 1;
  char tmp_commond[100];
  sprintf(tmp_commond, "./weights/theta_%d", tmp_id);
  ofstream os_theta(tmp_commond);
  cout << "save theta" << endl;
  for (int i = 0; i < _NExc; i++)
  {
    os_theta << thetaPExc[i] << " ";
  }
  os_theta << endl;
  os_theta.close();
  tmp_id++;
}
// ------------------------------------------------------------------------------
//  temporatory E2C 权重保存
// ------------------------------------------------------------------------------
void save_tmp_gE2C() //　revised at 2017-10-13 22:15
{
  static int tmp_id = 1;
  char tmp_commond[100];
  sprintf(tmp_commond, "./weights/gEC_%d", tmp_id);
  ofstream os_gE2C(tmp_commond);
  cout << "save gE2C" << endl;
  for (int i = 0; i < _NExc; i++)
  {
    for (int j = 0; j < _NCla; j++)
    {
      os_gE2C << gE2C[i * _NCla + j] << " ";
    }
    os_gE2C << endl;
  }
  os_gE2C << endl;
  os_gE2C.close();
  tmp_id++;
}
// ------------------------------------------------------------------------------
//  final XeAe权重保存
// ------------------------------------------------------------------------------
void save_gP2E()
{
  ofstream os_gP2E("./weights/gPE");
  cout << "save gP2E" << endl;
  for (int i = 0; i < _NPoi; i++)
  {
    for (int j = 0; j < _NExc; j++)
    {
      os_gP2E << gP2E[i * _NExc + j] << " ";
    }
    os_gP2E << endl;
  }
  os_gP2E << endl;
  os_gP2E.close();
}
// ------------------------------------------------------------------------------
//  final Ae-theta保存
// ------------------------------------------------------------------------------
void save_theta()
{
  ofstream os_theta("./weights/theta");
  cout << "save theta" << endl;
  for (int i = 0; i < _NExc; i++)
  {
    os_theta << thetaPExc[i] << " ";
  }
  os_theta << endl;
  os_theta.close();
}
// ------------------------------------------------------------------------------
//  final E2C权重保存
// ------------------------------------------------------------------------------
void save_gE2C()
{
  ofstream os_gE2C("./weights/gEC");
  cout << "save gE2C" << endl;
  for (int i = 0; i < _NExc; i++)
  {
    for (int j = 0; j < _NCla; j++)
    {
      os_gE2C << gE2C[i * _NCla + j] << " ";
    }
    os_gE2C << endl;
  }
  os_gE2C << endl;
  os_gE2C.close();
}
