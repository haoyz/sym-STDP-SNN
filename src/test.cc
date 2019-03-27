#include "train.h"
#include "model.h"
#include "reader/mnist/mnist.h"
#include "reader/cifar10/cifar10.h"
#include "gnuplot.h"
#include "plot.h"
#include "file_recorder.h"
#include "evaluation.h"
#include "file_read.h"

#include <cuda_runtime.h>
#include <fstream>
// #include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

#include "../global.h"
#include "../model_CODE/definitions.h"

long int imageNum;
long int imageNum_train;
string data = DATA;
string path = "../../Downloads/dataset/" + data;
int str2int(string s)
{
  int num;
  stringstream ss(s);
  ss >> num;
  return num;
}

int main()
{
  //  timer
  time_t t_start = time(NULL);
  // required by GeNN
  allocateMem();
  allocateE2I(_connN_E2I);
  allocateI2E(_connN_I2E);
  initialize();
  //void initModel();//Called after initializing the weight, otherwise it is not copied to the GPU.
  //initializeAllSparseArrays();//This function itself tells the user to initialize and then copy. Did not find the implementation of the initializeSparseArray function

  //------------------------------------------------------------------------------
  // set parameters and equations
  //------------------------------------------------------------------------------
  long int sum_glbSpkCntPExc;
  int num_examples;
  int weight_variance_interval = _performance_update_interval;
  int response_rate_interval = _performance_update_interval;
  int theta_interval = _performance_update_interval;
  int weight_update_interval = _performance_update_interval;
  int save_connections_interval = _performance_update_interval;

  if (_NUM_EXAMPLES == auto_choose)
  {
    switch (_N)
    {
    case 100:
      num_examples = 60000 * 3;
      break;
    case 400:
      num_examples = 60000 * 5;
      break;
    case 1600:
      num_examples = 60000 * 7;
      break;
    case 6400:
      num_examples = 60000 * 15;
      break;
    case 10000:
      num_examples = 60000 * 20;
      break;
    default:
      num_examples = 60000 * 1;
      break;
    }
  }
  else
    num_examples = _NUM_EXAMPLES;

  int *assignments = new int[_NExc]; //assign labels to each neuron using assignments
  fill_n(assignments, _NExc, -1);
  // vector<vector<int>> result_monitor;//spike_rates of all samples
  // vector<int> spike_rates;//record spikes of each neuron when single samples
  // int result_monitor[_LABELS_CONST][_NExc] = {0}; //spike_rates of all samples
  int *assignments_dist = new int[10];
  fill_n(assignments_dist, 10, 0);
  /**********************revised at 2018-4-13************************/
  /*******assignments在用测试集评估时与训练集评估完全不同，防干扰********/
  int *assignmentsTestDataSet = new int[_NExc];
  fill_n(assignmentsTestDataSet, _NExc, -1);
  int *assignmentsTestDataSet_dist = new int[10];
  fill_n(assignmentsTestDataSet_dist, 10, 0);
  /*****************************************************************/
  int(*result_monitor)[_NExc] = new int[_LABELS_CONST][_NExc]; //所有样本的spike_rates
  fill_n(result_monitor[0], _LABELS_CONST * _NExc, 0);
  int(*cla_result_monitor)[_NCla] = new int[_LABELS_CONST][_NCla]; //所有样本的spike_rates
  fill_n(cla_result_monitor[0], _LABELS_CONST * _NCla, 0);
  int *spike_rates = new int[_NExc]; //单个样本，_NExc神经元各自发放数
  fill_n(spike_rates, _NExc, 0);
  int *cla_spike_rates = new int[_NCla]; //单个样本，_NCla神经元各自发放数
  fill_n(cla_spike_rates, _NCla, 0);
  // ------------------------------------------------------------------------------
  //  load MNIST
  // ------------------------------------------------------------------------------
  vector<vector<float>> images;
  vector<float> labels;
  vector<vector<float>> imagesTestDataSet;
  vector<float> labelsTestDataSet;
  vector<vector<float>> images_test;
  vector<float> labels_test;
  vector<float> one_images; //一幅图像
  clock_t start;            //clock()返回长整形数
  clock_t end;
  if (DATA != "cifar10/")
  {
    if (_Dataset_train)
    {
      cout << "**********train-images-idx3-ubyte**********" << endl;
      read_mnist_images(path + "train-images-idx3-ubyte", images);
      cout << "**********train-labels-idx1-ubyte**********" << endl;
      read_mnist_label(path + "train-labels-idx1-ubyte", labels);

      cout << "**********t10k-images-idx3-ubyte**********" << endl;
      read_mnist_images(path + "t10k-images-idx3-ubyte", imagesTestDataSet);
      cout << "**********t10k-labels-idx1-ubyte**********" << endl;
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labelsTestDataSet);
    }
    else
    {
      cout << "**********t10k-images-idx3-ubyte**********" << endl;
      read_mnist_images(path + "t10k-images-idx3-ubyte", images);

      cout << "**********t10k-labels-idx1-ubyte**********" << endl;
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labels);
    }
  }
  else
  {
#ifdef GRAYSCALE
    if (_Dataset_train)
    {
      read_cifar10_train(path, GRAYSCALE, images, labels);
      read_cifar10_test(path, GRAYSCALE, imagesTestDataSet, labelsTestDataSet);
    }
    else
    {
      read_cifar10_test(path, GRAYSCALE, images, labels);
    }
#endif
  }

  // ------------------------------------------------------------------------------
  //  准备输入图像
  // ------------------------------------------------------------------------------
  float input_intensity = START_INPUT_INTENSITY; //输入强度，256/8*input_intensity
  float *FR_khz = new float[_NPoi];              //用自带函数实现概率转换为阈值吧
  offsetPPoi = 0;                                //GeNN的泊松神经元自带参数，没用。
  uint64_t *CPUratesPPoi = new uint64_t[_NPoi];  //平台的bug，必须多写一句给ratesPPoi分配内存
  int size_PPoi = _NPoi * sizeof(uint64_t);
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPPoi, size_PPoi));
  fprintf(stdout, "allocated %lu elements for pattern.\n", size_PPoi / sizeof(uint64_t));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice));
  //cudaHostAlloc(&CPUratesPPoi, 1 * sizeof(unsigned int), cudaHostAllocPortable);
  //    deviceMemAllocate(&d_glbSpkCntPPoi, dd_glbSpkCntPPoi, 1 * sizeof(unsigned int));

  // ------------------------------------------------------------------------------
  //  准备分类层
  // ------------------------------------------------------------------------------
  float cla_input_intensity = _start_cla_input_intensity;
  float *cla_FR_khz = new float[_NCla];
  fill_n(cla_FR_khz, _NCla, 0.0);
  uint64_t *CPUratesPCla = new uint64_t[_NCla];
  int size_PCla = _NCla * sizeof(uint64_t);
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPCla, size_PCla));
  fprintf(stdout, "allocated %lu elements for pattern.\n", size_PCla / sizeof(uint64_t));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, size_PCla, cudaMemcpyHostToDevice));

  // ------------------------------------------------------------------------------
  //  重写参数
  // ------------------------------------------------------------------------------
  //重写gP2E初始值.最后一个参数是g_max乘以1000
  read_gP2E_from_file(gP2E);
  read_thetaPExc_from_file(thetaPExc);
  read_gE2C_from_file(gE2C);

  // get_rand_g(gE2C, _NExc * _NCla, _g_max_EC_init1000);

  //先编译model文件才有gE2I变量……
  float *tmp_g_EI = new float[_N * _N]; //dense型存储（对应到函数实现上）
  float *tmp_g_IE = new float[_N * _N]; //dense型存储（对应到函数实现上）
  //E——>I
  for (int i_dense = 0; i_dense < _N; i_dense++)   //行pre
    for (int j_dense = 0; j_dense < _N; j_dense++) //列post
    {
      if (i_dense == j_dense)
        tmp_g_EI[i_dense * _N + j_dense] = _g_EI; //大于1e-19就能分开 0.5有点大？
      else
        tmp_g_EI[i_dense * _N + j_dense] = 0.0; //大于1e-19就能分开
    }
  //I——>E
  for (int i_dense = 0; i_dense < _N; i_dense++)   //行pre
    for (int j_dense = 0; j_dense < _N; j_dense++) //列post
    {
      if (i_dense == j_dense)
        tmp_g_IE[i_dense * _N + j_dense] = 0.0; //大于1e-19就能分开
      else
        tmp_g_IE[i_dense * _N + j_dense] = _g_IE; //大于1e-19就能分开
    }
  setSparseConnectivityFromDense(gE2I, _preN_EI, _postN_EI, tmp_g_EI, &CE2I); //gI2E CE2I都是平台生成变量
  setSparseConnectivityFromDense(gI2E, _preN_IE, _postN_IE, tmp_g_IE, &CI2E);
  //重写seedPPoi
  float *tmp_p = new float[_NPoi];
  get_rand(tmp_p, _NPoi, 100000);
  for (int i = 0; i < _NPoi; i++)
  {
    seedPPoi[i] = tmp_p[i];
  }

  // ------------------------------------------------------------------------------
  //   重写参数后reset
  // ------------------------------------------------------------------------------
  copyStateToDevice();
  initmodel(); //只与稀疏连接有关
  copyCurrentSpikesToDevice();

// ------------------------------------------------------------------------------
//  输出至文件
// ------------------------------------------------------------------------------
#ifdef _file_recoder
  ofstream os_poi("./output/output_poi_V.dat");
  ofstream os_exc("./output/output_exc_V.dat");
  ofstream os_inh("./output/output_inh_V.dat");
  ofstream os_t2N_poi("./output/output_t2N_poi.dat"); //time2兴奋num
  ofstream os_t2N_exc("./output/output_t2N_exc.dat");
  ofstream os_t2N_inh("./output/output_t2N_inh.dat");
  ofstream os_visual("./output/output_visual.dat");
#endif
  // ------------------------------------------------------------------------------
  //  可视化突触权重！！！
  // ------------------------------------------------------------------------------
  auto PEVisual = new float[_NPoi_Sqrt * _NExc_Sqrt][_NPoi_Sqrt * _NExc_Sqrt];
  auto ECVisual = new float[_ECw_X][_ECw_Y];
  auto ECVisual_inferred = new float[_ECw_X][_ECw_Y];
  auto CEVisual = new float[_ECw_Y][_ECw_X];          //　added at 2017-12-19 09:05
  auto CEVisual_inferred = new float[_ECw_Y][_ECw_X]; //　added at 2017-12-19 09:05
#ifdef PLOT_ON
  GNUplot PEplotter;
  GNUplot ECplotter, ECplotter_inferred;
  GNUplot CEplotter, CEplotter_inferred;
  plot_visual_PEw_init(PEplotter);
  get_visual_PEw(PEVisual);
  write_visual_PEw_to_file(PEVisual);
  plot_visual_PEw(PEplotter);
  plot_visual_ECw_init(ECplotter);
  get_visual_ECw(ECVisual);
  write_visual_ECw_to_file(ECVisual);
  plot_visual_ECw(ECplotter);
  // plot_visual_ECw_inferred_init(ECplotter_inferred);
  // get_visual_ECw_inferred(ECVisual_inferred, assignments);
  // write_visual_ECw_inferred_to_file(ECVisual_inferred);
  // plot_visual_ECw_inferred(ECplotter_inferred);
#endif
  // ------------------------------------------------------------------------------
  //  收敛曲线（准确率、权重方差）、混淆矩阵、spike_monitor绘图
  // ------------------------------------------------------------------------------
  float *performance = new float[int(num_examples / _performance_update_interval)];
  float *performanceNowUseTrainData = new float[int(num_examples / _performance_update_interval)]; //revised at 2018-4-6
  float *performanceNowUseTestData = new float[int(num_examples / _performance_update_interval)];  //revised at 2018-4-6
  float *cla_performance = new float[int(num_examples / _performance_update_interval)];
  float *cla_performanceNowUseTrainData = new float[int(num_examples / _performance_update_interval)]; //revised at 2018-4-6
  float *cla_performanceNowUseTestData = new float[int(num_examples / _performance_update_interval)];  //revised at 2018-4-6
  fill_n(performance, int(num_examples / _performance_update_interval), 0);
  fill_n(cla_performance, int(num_examples / _performance_update_interval), 0);
  float variance[_NExc] = {0};
  float variance_E2C[_NExc] = {0};
  float confusion_m[11][11] = {0};
  float confusion_m_supervised[11][11] = {0}; //revised at 2018-05-13
  int offset = 0;
  int current_evaluation = 0;
#ifdef _spikes_monitor
  int(*spikes_real_time)[_NExc] = new int[(int)(_RunTime_singleImg / DT)][_NExc]; //实时spike_rates
  fill_n(spikes_real_time[0], _RunTime_singleImg / DT * _NExc, 0);
  int(*cla_spikes_real_time)[_NCla] = new int[(int)(_RunTime_singleImg / DT)][_NCla]; //实时spike_rates
  fill_n(cla_spikes_real_time[0], _RunTime_singleImg / DT * _NCla, 0);
#endif
  int *response_rate = new int[60];
  fill_n(response_rate, 60, 0);
  int *theta = new int[60];
  fill_n(theta, 60, 0);
  size_t size_thetaPExc = _NExc;
#ifdef PLOT_ON
  GNUplot plot_per;
  GNUplot plot_perNowUseTrainData;
  GNUplot plot_perNowUseTestData;
  GNUplot plot_v, plot_vE2C;
  GNUplot plot_confusion;
  GNUplot plot_confusion_supervised;
  GNUplot plot_v_dist, plot_vE2C_dist;
  GNUplot plot_assign_dist;
#ifdef _spikes_monitor
  GNUplot plot_spike;
  GNUplot plot_cla_spike;
#endif
  GNUplot plot_theta;
  GNUplot plot_response_rate;
  plot_performance_init(plot_per, num_examples);
  plot_performance_init(plot_perNowUseTrainData, num_examples); //revised at 2018-4-6
  plot_performance_init(plot_perNowUseTestData, num_examples);  //revised at 2018-4-6
  plot_variance_init(plot_v, num_examples / weight_variance_interval);
  plot_variance_E2C_init(plot_vE2C, num_examples / weight_variance_interval);
  plot_confusion_m_init(plot_confusion);
  plot_confusion_m_init(plot_confusion_supervised); //revised at 2018-05-13
  plot_variance_distribution_init(plot_v_dist);
  plot_variance_E2C_distribution_init(plot_vE2C_dist);
  plot_assignments_distribution_init(plot_assign_dist);
#ifdef _spikes_monitor
  plot_spikes_init(plot_spike);
  plot_cla_spikes_init(plot_cla_spike);
#endif
  plot_theta_init(plot_theta);
  plot_response_rates_init(plot_response_rate);
#endif

  // ------------------------------------------------------------------------------
  //  RUN
  // ------------------------------------------------------------------------------
  imageNum = 170000;
  sum_glbSpkCntPExc = 0;
  bool testDataEvaluateMode = true; // false;
  while (imageNum < num_examples)
  {

    // ------------------------------------------------------------------------------
    //  输入图像
    // ------------------------------------------------------------------------------
    one_images.clear();
    for (int i = 0; i < _NPoi; i++)
    {
      one_images.push_back(images[imageNum % 60000][i]);
    }
    for (int i = 0; i < _NPoi; i++) //图像接入
      FR_khz[i] = (float)one_images[i] / 8.0 / 1000.0 * input_intensity;
    convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, _NPoi);                           //单位是khz,为与DT配合
    CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

    // ------------------------------------------------------------------------------
    //  分类层
    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    //  350ms
    // ------------------------------------------------------------------------------
    sum_glbSpkCntPExc = 0;             //之前居然没归零，等于说<5没发挥作用，可能是不够光亮的原因之一？
    fill_n(spike_rates, _NExc, 0);     //记录发放的神经元 清零
    fill_n(cla_spike_rates, _NCla, 0); //记录发放的神经元 清零

    CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));

    CHECK_CUDA_ERRORS(cudaMemcpy(d_gP2E, gP2E, size_gP2E * sizeof(float), cudaMemcpyHostToDevice)); //注意把g写入GPU

    for (int dt = 0; dt < _RunTime_singleImg / DT; dt++)
    {

      stepTimeGPU();
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPExc, d_glbSpkCntPExc, sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPCla, d_glbSpkCntPCla, sizeof(unsigned int), cudaMemcpyDeviceToHost));

      //copySpikeNFromDevice();//需要查看变量时记得copyfrom//用SpikeN的话出错：非法cuda错误
      sum_glbSpkCntPExc += glbSpkCntPExc[0]; //延时的话是应该用[0]嘛？
#ifdef _file_recoder

      copyStateFromDevice();
      showGPU();

#endif

      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPExc, d_glbSpkPExc, glbSpkCntPExc[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPCla, d_glbSpkPCla, glbSpkCntPCla[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));

      for (int i = 0; i < glbSpkCntPExc[0]; i++) //记录发放的神经元
      {
        spike_rates[glbSpkPExc[i]]++;
      }
      for (int i = 0; i < glbSpkCntPCla[0]; i++) //记录发放的神经元
      {
        cla_spike_rates[glbSpkPCla[i]]++;
      }
#ifdef _spikes_monitor
      for (int i = 0; i < glbSpkCntPExc[0]; i++) //记录发放的神经元
      {
        spikes_real_time[dt][glbSpkPExc[i]] = 1;
      }
      for (int i = 0; i < glbSpkCntPCla[0]; i++) //记录发放的神经元
      {
        cla_spikes_real_time[dt][glbSpkPCla[i]] = 1;
      }
#endif
    }
#ifdef _spikes_monitor
    if (write_spike_to_file(spikes_real_time))
      plot_spikes(plot_spike);
    if (write_cla_spike_to_file(cla_spikes_real_time))
      plot_cla_spikes(plot_cla_spike);
    for (int dt = 0; dt < _RunTime_singleImg / DT; dt++) //复位发放的神经元
    {
      fill_n(spikes_real_time[dt], _NExc, 0);
      fill_n(cla_spikes_real_time[dt], _NCla, 0);
    }
#endif
    if (sum_glbSpkCntPExc < 60)
      response_rate[sum_glbSpkCntPExc] = response_rate[sum_glbSpkCntPExc] + 1;

    // ------------------------------------------------------------------------------
    //  是否增加输入强度？
    // ------------------------------------------------------------------------------
    // cout << "sum_glbSpkCntPExc<5?:" << sum_glbSpkCntPExc << " ";
    // cout << "input_intensity:" << input_intensity << endl;
    if (sum_glbSpkCntPExc < NUM_SPIKE_RESP && input_intensity < 32) // // revised by Xuhui Huang at 2018-04-02 / 2017-09-11 15:35
    {
      sum_glbSpkCntPExc = 0;
      input_intensity += 1;
      fill_n(FR_khz, _NPoi, 0);                                                                  //图像接入
      convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, _NPoi);                           //单位是khz 准备resting
      CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

      for (int dt = 0; dt < _RestingTime / DT; dt++)
      {
        stepTimeGPU(); //兴奋放电太少，复位状态，重新跑
      }
    }
    // ------------------------------------------------------------------------------
    //  150ms resting！！！
    // ------------------------------------------------------------------------------
    else
    {
      if (input_intensity >= 32)
      {
        // cout << "input_intensity:" << input_intensity << endl;
        cout << "imageNum without " << NUM_SPIKE_RESP << "-spike-response: " << (imageNum + 1) << endl; //  (imageNum + 1) : revised at 2018-04-25 15:30
      }
      for (int i = 0; i < _NExc; i++) //这个样本激发神经元存起来
        result_monitor[imageNum % _performance_update_interval][i] = spike_rates[i];

      // write_result_monitor_to_file(spike_rates, labels[imageNum % 60000]);              //SVM接口
      write_result_monitor_to_file_0518(spike_rates, labels[imageNum % 60000]);         //revised at 2018-05-18
      write_result_monitor_to_file_0518_cla(cla_spike_rates, labels[imageNum % 60000]); //revised at 2018-05-18

      for (int i = 0; i < _NCla; i++) //这个样本激发神经元存起来
        cla_result_monitor[imageNum % _performance_update_interval][i] = cla_spike_rates[i];
      if ((imageNum + 1) % 100 == 0 && imageNum > 0) //显示而已      //  (imageNum + 1) : revised at 2018-04-25 15:30
      {
        if (!testDataEvaluateMode)
          cout << "Training data - runs done: " << (imageNum + 1) << " -> " << num_examples << "; input_intensity: " << input_intensity << endl; //  (imageNum + 1) : revised at 2018-04-25 15:30
        else
          cout << "Test data for " << imageNum_train << "  - runs done: " << (imageNum + 1) << " -> " << 10000 << "; input_intensity: " << input_intensity << endl; //  (imageNum + 1) : revised at 2018-04-25 15:30
      }

      fill_n(FR_khz, _NPoi, 0);                                                                  //图像接入
      convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, _NPoi);                           //单位是khz 准备resting
      CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

      for (int dt = 0; dt < _RestingTime / DT; dt++) //resting！！！
      {

        stepTimeGPU();
      }

      input_intensity = START_INPUT_INTENSITY; //输入强度重置
      imageNum += 1;
    }
  }
  //------------------------------------------------------------------------------
  // save&plot results
  //------------------------------------------------------------------------------

  // ------------------------------------------------------------------------------
  //  Calculation accuracy！！！
  // ------------------------------------------------------------------------------
  // {
  //   //直接读取resultPopVecs10000.txt
  //   string str_gE2C;
  //   ifstream result_file("resultPopVecs10000.txt");
  //   if (result_file)
  //   {
  //     cout << "Read from resultPopVecs10000.txt..." << endl;
  //     for (int i = 0; i < 10000; i++)
  //     {
  //       for (int j = 0; j < 400; j++)
  //       {
  //         result_file >> str_gE2C;
  //         result_monitor[i][j] = str2float(str_gE2C);
  //       }
  //     }
  //     result_file.close();
  //   }
  //   else
  //   {
  //     cout << "open resultPopVecs10000.txt error!" << endl;
  //   }
  // }
  {
    offset = 0;
    current_evaluation = 0;
    cout << "calculate assignments..." << endl;
    get_new_assignments(assignments, offset, result_monitor, labels); //result_monitor:所有样本的spike_rates
    get_assignments_distribution(assignments, assignments_dist);
    write_assignments_distribution_to_file(assignments_dist);
#ifdef PLOT_ON
    plot_assignments_distribution(plot_assign_dist);
#endif
    if (whether_evaluation_or_not(assignments))
    {
      cout << "calculate performance&confusion_m..." << endl;
      get_performance(performance, offset, current_evaluation, result_monitor, assignments, labels);
      cout << "calculate assignments by last layer..." << endl;
      cla_get_performance(cla_performance, offset, current_evaluation, cla_result_monitor, labels);
      // write_performance_to_file(performance, cla_performance, current_evaluation, LastTrain);  // revised at 2017-10-18 17:00
      get_confusion_m(confusion_m, offset, result_monitor, assignments, labels);
      write_confusion_m_to_file(confusion_m, UNSUPERVISED);

      get_confusion_m_supervised(confusion_m_supervised, offset, cla_result_monitor, labels);
      write_confusion_m_to_file(confusion_m_supervised, SUPERVISED);
#ifdef PLOT_ON
      plot_performance(plot_per, FinalTest);
      plot_confusion_m(plot_confusion);
      plot_confusion_m(plot_confusion_supervised, SUPERVISED);
#endif

      // cout << "calculate assignments by last layer..." << endl;
      // cla_get_performance(cla_performance, offset, current_evaluation, cla_result_monitor, labels);
    }
    else
      cout << "maybe I'm saving something to evaluate...";
  }
// ------------------------------------------------------------------------------
//  DONE！！！
// ------------------------------------------------------------------------------
#ifdef _file_recoder
  os_poi.close();
  os_exc.close();
  os_inh.close();
  os_t2N_poi.close();
  os_t2N_exc.close();
  os_t2N_inh.close();
  os_visual.close();
#endif
  delete[] performance;
  delete[] result_monitor;
  delete[] cla_result_monitor;
  delete[] spike_rates;
  delete[] cla_spike_rates;
  delete[] assignments;
  delete[] PEVisual; // clean up memory
  delete[] ECVisual;
  delete[] ECVisual_inferred;
  delete[] tmp_g_EI;
  delete[] tmp_g_IE;
  delete[] FR_khz;
  delete[] cla_FR_khz;
  delete[] CPUratesPCla;
  cudaFreeHost(ratesPCla); //究竟哪个free呢？
  CHECK_CUDA_ERRORS(cudaFree(ratesPCla));

  delete[] CPUratesPPoi;
  cudaFreeHost(ratesPPoi); //究竟哪个free呢？
  CHECK_CUDA_ERRORS(cudaFree(ratesPPoi));
#ifdef _spikes_monitor
  delete[] spikes_real_time;
  delete[] cla_spikes_real_time;
#endif
  delete[] response_rate;
  delete[] theta;

  fprintf(stdout, "#testing end  ... \n");

  // cin.get();
  // exitGeNN();

  // ------------------------------------------------------------------------------
  //  程序计时
  // ------------------------------------------------------------------------------
  time_t t_end = time(NULL); //返回当前时间到1970年1月1日零点时间的差.秒
  cout << "t_start:" << t_start << endl;
  cout << "t_end:" << t_end << endl;
  cout << "time to use:" << t_end - t_start << "s = " << (t_end - t_start) / 3600.0 << "h" << endl;
  return 0;
}
