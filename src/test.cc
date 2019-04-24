#include "test.h"
#include "model.h"
#include "reader/mnist/mnist.h"
#include "reader/cifar10/cifar10.h"
#include "gnuplot.h"
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
  allocateE2I(connN_E2I);
  allocateI2E(connN_I2E);
  initialize();
  //void initModel();//Called after initializing the weight, otherwise it is not copied to the GPU.
  //initializeAllSparseArrays();//This function itself tells the user to initialize and then copy. Did not find the implementation of the initializeSparseArray function

  //------------------------------------------------------------------------------
  // set parameters and equations
  //------------------------------------------------------------------------------
  long int sum_glbSpkCntPExc;
  int num_samples;
  num_samples = NUM_SAMPLES;

  int *assignments = new int[NExc]; //assign labels to each neuron using assignments
  fill_n(assignments, NExc, -1);
  // vector<vector<int>> result_monitor;//spike_record of all samples
  // vector<int> spike_record;//record spikes of each neuron when single samples
  // int result_monitor[LABELS_CONST][NExc] = {0}; //spike_record of all samples
  int *assignments_dist = new int[10];
  fill_n(assignments_dist, 10, 0);

  /*******assignments在用测试集评估时与训练集评估完全不同，防干扰********/
  int *assignmentsTestDataSet = new int[NExc];
  fill_n(assignmentsTestDataSet, NExc, -1);
  int *assignmentsTestDataSet_dist = new int[10];
  fill_n(assignmentsTestDataSet_dist, 10, 0);

  int(*result_monitor)[NExc] = new int[LABELS_CONST][NExc]; //所有样本的spike_record
  fill_n(result_monitor[0], LABELS_CONST * NExc, 0);
  int(*cla_result_monitor)[NCla] = new int[LABELS_CONST][NCla]; //所有样本的spike_record
  fill_n(cla_result_monitor[0], LABELS_CONST * NCla, 0);
  int *spike_record = new int[NExc]; //单个样本，NExc神经元各自发放数
  fill_n(spike_record, NExc, 0);
  int *cla_spike_record = new int[NCla]; //单个样本，NCla神经元各自发放数
  fill_n(cla_spike_record, NCla, 0);
  // ------------------------------------------------------------------------------
  //  load MNIST
  // ------------------------------------------------------------------------------
  vector<vector<float>> images;
  vector<float> labels;
  vector<vector<float>> imagesTestDataSet;
  vector<float> labelsTestDataSet;
  vector<vector<float>> images_test;
  vector<float> labels_test;
  vector<float> one_image; //一幅图像
  clock_t start;           //clock()返回长整形数
  clock_t end;
  if (DATA != "cifar10/")
  {
    if (DATASET_TRAIN)
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
    if (DATASET_TRAIN)
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
  float input_intensity = INPUT_INTENSITY_INIT; //输入强度，256/8*input_intensity
  float *FR_khz = new float[NPoi];
  uint64_t *CPUratesPPoi = new uint64_t[NPoi]; //genn要求手动给ratesPPoi分配内存
  int size_PPoi = NPoi * sizeof(uint64_t);
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPPoi, size_PPoi));
  fprintf(stdout, "allocated %lu elements for pattern.\n", size_PPoi / sizeof(uint64_t));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice));
  //cudaHostAlloc(&CPUratesPPoi, 1 * sizeof(unsigned int), cudaHostAllocPortable);
  //    deviceMemAllocate(&d_glbSpkCntPPoi, dd_glbSpkCntPPoi, 1 * sizeof(unsigned int));

  // ------------------------------------------------------------------------------
  //  准备分类层
  // ------------------------------------------------------------------------------
  float cla_input_intensity = Cla_INPUT_INTENSITY_INIT;
  float *cla_FR_khz = new float[NCla];
  fill_n(cla_FR_khz, NCla, 0.0);
  uint64_t *CPUratesPCla = new uint64_t[NCla];
  int size_PCla = NCla * sizeof(uint64_t);
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

  // get_rand_g(gE2C, NExc * NCla, gEC_INIT_MAX_1000);

  //先编译model文件才有gE2I变量……
  float *tmp_g_EI = new float[N * N]; //dense型存储（对应到函数实现上）
  float *tmp_g_IE = new float[N * N]; //dense型存储（对应到函数实现上）
  //E——>I
  for (int i_dense = 0; i_dense < N; i_dense++)   //行pre
    for (int j_dense = 0; j_dense < N; j_dense++) //列post
    {
      if (i_dense == j_dense)
        tmp_g_EI[i_dense * N + j_dense] = g_EI; //大于1e-19就能分开 0.5有点大？
      else
        tmp_g_EI[i_dense * N + j_dense] = 0.0; //大于1e-19就能分开
    }
  //I——>E
  for (int i_dense = 0; i_dense < N; i_dense++)   //行pre
    for (int j_dense = 0; j_dense < N; j_dense++) //列post
    {
      if (i_dense == j_dense)
        tmp_g_IE[i_dense * N + j_dense] = 0.0; //大于1e-19就能分开
      else
        tmp_g_IE[i_dense * N + j_dense] = g_IE; //大于1e-19就能分开
    }
  setSparseConnectivityFromDense(gE2I, _preN_EI, _postN_EI, tmp_g_EI, &CE2I); //gI2E CE2I都是平台生成变量
  setSparseConnectivityFromDense(gI2E, _preN_IE, _postN_IE, tmp_g_IE, &CI2E);
  //重写seedPPoi
  float *tmp_p = new float[NPoi];
  get_rand(tmp_p, NPoi, 100000);
  for (int i = 0; i < NPoi; i++)
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
#ifdef FILE_RECODER
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
  auto PEVisual = new float[WIDTH * NExc_Sqrt][HEIGHT * NExc_Sqrt];
  auto ECVisual = new float[ECw_X][ECw_Y];
  auto ECVisual_inferred = new float[ECw_X][ECw_Y];
  auto CEVisual = new float[ECw_Y][ECw_X];          //　added at 2017-12-19 09:05
  auto CEVisual_inferred = new float[ECw_Y][ECw_X]; //　added at 2017-12-19 09:05
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
  float *performance = new float[int(num_samples / UPDATE_INTERVAL)];
  float *performanceNowUseTrainData = new float[int(num_samples / UPDATE_INTERVAL)];
  float *performanceNowUseTestData = new float[int(num_samples / UPDATE_INTERVAL)];
  float *cla_performance = new float[int(num_samples / UPDATE_INTERVAL)];
  float *cla_performanceNowUseTrainData = new float[int(num_samples / UPDATE_INTERVAL)];
  float *cla_performanceNowUseTestData = new float[int(num_samples / UPDATE_INTERVAL)];
  fill_n(performance, int(num_samples / UPDATE_INTERVAL), 0);
  fill_n(cla_performance, int(num_samples / UPDATE_INTERVAL), 0);
  float variance[NExc] = {0};
  float variance_E2C[NExc] = {0};
  float confusion_m[11][11] = {0};
  float confusion_m_supervised[11][11] = {0};
  int offset = 0;
  int current_evaluation = 0;
#ifdef SPIKES_MONITOR
  int(*spikes_real_time)[NExc] = new int[(int)(RUN_TIME / DT)][NExc]; //实时spike_record
  fill_n(spikes_real_time[0], RUN_TIME / DT * NExc, 0);
  int(*cla_spikes_real_time)[NCla] = new int[(int)(RUN_TIME / DT)][NCla]; //实时spike_record
  fill_n(cla_spikes_real_time[0], RUN_TIME / DT * NCla, 0);
#endif
  int *response_rate = new int[60];
  fill_n(response_rate, 60, 0);
  int *theta = new int[60];
  fill_n(theta, 60, 0);
  size_t size_thetaPExc = NExc;
#ifdef PLOT_ON
  GNUplot plot_per;
  GNUplot plot_perNowUseTrainData;
  GNUplot plot_perNowUseTestData;
  GNUplot plot_v, plot_vE2C;
  GNUplot plot_confusion;
  GNUplot plot_confusion_supervised;
  GNUplot plot_v_dist, plot_vE2C_dist;
  GNUplot plot_assign_dist;
#ifdef SPIKES_MONITOR
  GNUplot plot_spike;
  GNUplot plot_cla_spike;
#endif
  GNUplot plot_theta;
  GNUplot plot_response_rate;
  plot_performance_init(plot_per, num_samples);
  plot_performance_init(plot_perNowUseTrainData, num_samples);
  plot_performance_init(plot_perNowUseTestData, num_samples);
  plot_variance_init(plot_v, num_samples / UPDATE_INTERVAL);
  plot_variance_E2C_init(plot_vE2C, num_samples / UPDATE_INTERVAL);
  plot_confusion_m_init(plot_confusion);
  plot_confusion_m_init(plot_confusion_supervised);
  plot_variance_distribution_init(plot_v_dist);
  plot_variance_E2C_distribution_init(plot_vE2C_dist);
  plot_assignments_distribution_init(plot_assign_dist);
#ifdef SPIKES_MONITOR
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
  while (imageNum < num_samples)
  {

    // ------------------------------------------------------------------------------
    //  输入图像
    // ------------------------------------------------------------------------------
    one_image.clear();
    for (int i = 0; i < NPoi; i++)
    {
      one_image.push_back(images[imageNum % 60000][i]);
    }
    for (int i = 0; i < NPoi; i++) //图像接入
      FR_khz[i] = (float)one_image[i] / 8.0 / 1000.0 * input_intensity;
    convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);                            //单位是khz,为与DT配合
    CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

    // ------------------------------------------------------------------------------
    //  分类层
    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    //  350ms
    // ------------------------------------------------------------------------------
    sum_glbSpkCntPExc = 0;             //之前居然没归零，等于说<5没发挥作用，可能是不够光亮的原因之一？
    fill_n(spike_record, NExc, 0);     //记录发放的神经元 清零
    fill_n(cla_spike_record, NCla, 0); //记录发放的神经元 清零

    CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));

    CHECK_CUDA_ERRORS(cudaMemcpy(d_gP2E, gP2E, size_gP2E * sizeof(float), cudaMemcpyHostToDevice)); //注意把g写入GPU

    for (int dt = 0; dt < RUN_TIME / DT; dt++)
    {

      stepTimeGPU();
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPExc, d_glbSpkCntPExc, sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPCla, d_glbSpkCntPCla, sizeof(unsigned int), cudaMemcpyDeviceToHost));

      //copySpikeNFromDevice();//需要查看变量时记得copyfrom//用SpikeN的话出错：非法cuda错误
      sum_glbSpkCntPExc += glbSpkCntPExc[0]; //延时的话是应该用[0]嘛？
#ifdef FILE_RECODER

      copyStateFromDevice();
      showGPU();

#endif

      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPExc, d_glbSpkPExc, glbSpkCntPExc[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPCla, d_glbSpkPCla, glbSpkCntPCla[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));

      for (int i = 0; i < glbSpkCntPExc[0]; i++) //记录发放的神经元
      {
        spike_record[glbSpkPExc[i]]++;
      }
      for (int i = 0; i < glbSpkCntPCla[0]; i++) //记录发放的神经元
      {
        cla_spike_record[glbSpkPCla[i]]++;
      }
#ifdef SPIKES_MONITOR
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
#ifdef SPIKES_MONITOR
    if (write_spike_to_file(spikes_real_time, NExc, "spike.dat"))
      plot_spikes(plot_spike);
    if (write_spike_to_file(cla_spikes_real_time, NCla, "cla_spike.dat"))
      plot_cla_spikes(plot_cla_spike);
    for (int dt = 0; dt < RUN_TIME / DT; dt++) //复位发放的神经元
    {
      fill_n(spikes_real_time[dt], NExc, 0);
      fill_n(cla_spikes_real_time[dt], NCla, 0);
    }
#endif
    if (sum_glbSpkCntPExc < 60)
      response_rate[sum_glbSpkCntPExc] = response_rate[sum_glbSpkCntPExc] + 1;

    // ------------------------------------------------------------------------------
    //  是否增加输入强度？
    // ------------------------------------------------------------------------------
    // cout << "sum_glbSpkCntPExc<5?:" << sum_glbSpkCntPExc << " ";
    // cout << "input_intensity:" << input_intensity << endl;
    if (sum_glbSpkCntPExc < NUM_SPIKE_RESP && input_intensity < 32)
    {
      sum_glbSpkCntPExc = 0;
      input_intensity += 1;
      fill_n(FR_khz, NPoi, 0);                                                                   //图像接入
      convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);                            //单位是khz 准备rest
      CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

      for (int dt = 0; dt < REST_TIME / DT; dt++)
      {
        stepTimeGPU(); //兴奋放电太少，复位状态，重新跑
      }
    }
    // ------------------------------------------------------------------------------
    //  150ms rest
    // ------------------------------------------------------------------------------
    else
    {
      if (input_intensity >= 32)
      {
        // cout << "input_intensity:" << input_intensity << endl;
        cout << "imageNum without " << NUM_SPIKE_RESP << "-spike-response: " << (imageNum + 1) << endl;
      }
      for (int i = 0; i < NExc; i++) //这个样本激发神经元存起来
        result_monitor[imageNum % UPDATE_INTERVAL][i] = spike_record[i];

      // write_result_monitor_to_file(spike_record, labels[imageNum % 60000]);              //SVM接口
      write_result_monitor_to_file_0518(spike_record, labels[imageNum % 60000]);
      write_result_monitor_to_file_0518_cla(cla_spike_record, labels[imageNum % 60000]);

      for (int i = 0; i < NCla; i++) //这个样本激发神经元存起来
        cla_result_monitor[imageNum % UPDATE_INTERVAL][i] = cla_spike_record[i];
      if ((imageNum + 1) % 100 == 0 && imageNum > 0) //显示而已
      {
        if (!testDataEvaluateMode)
          cout << "Training data - runs done: " << (imageNum + 1) << " -> " << num_samples << "; input_intensity: " << input_intensity << endl;
        else
          cout << "Test data for " << imageNum_train << "  - runs done: " << (imageNum + 1) << " -> " << 10000 << "; input_intensity: " << input_intensity << endl;
      }

      fill_n(FR_khz, NPoi, 0);                                                                   //图像接入
      convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);                            //单位是khz 准备rest
      CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, size_PPoi, cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据

      for (int dt = 0; dt < REST_TIME / DT; dt++) //rest
      {

        stepTimeGPU();
      }

      input_intensity = INPUT_INTENSITY_INIT; //输入强度重置
      imageNum += 1;
    }
  }
  //------------------------------------------------------------------------------
  // save&plot results
  //------------------------------------------------------------------------------

  // ------------------------------------------------------------------------------
  //  Calculation accuracy
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
    get_new_assignments(assignments, offset, result_monitor, labels); //result_monitor:所有样本的spike_record
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
      // write_performance_to_file(performance, cla_performance, current_evaluation, LastTrain);
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
#ifdef FILE_RECODER
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
  delete[] spike_record;
  delete[] cla_spike_record;
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
#ifdef SPIKES_MONITOR
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
