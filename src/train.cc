#include "train.h"
#include "model.h"
#include "reader/caltech/caltech.h"
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
string path = "../../Downloads/dataset/" + string(DATA);

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
  int sum_glbSpkCntPExc = 0;
  int num_samples;
  int num_train_samples;
  int num_test_samples;
  int update_interval;
  int evaluation_interval;

  vector<int> assignments(NExc, -1); //assign labels to each neuron using assignments
  vector<int> assignments_dist(NUM_CLASS, 0);
  vector<int> assignmentsTestDataSet(NExc, -1);
  vector<int> assignmentsTestDataSet_dist(NUM_CLASS, 0);
  vector<vector<int>> result_monitor(LABELS_CONST, vector<int>(NExc, 0)); //所有样本的spike_record
  vector<vector<int>> cla_result_monitor(LABELS_CONST, vector<int>(NCla, 0));
  vector<int> spike_record(NExc, 0); //单个样本，NExc神经元响应
  vector<int> cla_spike_record(NCla, 0);
  // ------------------------------------------------------------------------------
  //  load dataset
  // ------------------------------------------------------------------------------
  vector<vector<float>> images;
  vector<float> labels;
  vector<vector<float>> images_test;
  vector<float> labels_test;
  clock_t start; //clock()返回长整形数
  clock_t end;
  get_inputdata(path, images, labels, images_test, labels_test);
  if (images.size() == labels.size() && images_test.size() == labels_test.size())
  {
    num_train_samples = images.size();
    num_test_samples = images_test.size();
    cout << "images size: " << images.size() << "  images_test size: " << images_test.size() << endl;
    update_interval = min(10000, num_train_samples);
    evaluation_interval = min(10000, num_test_samples);
    num_samples = NUM_SAMPLES;
  }
  else
    exit(1);

  // ------------------------------------------------------------------------------
  //  准备输入图像
  // ------------------------------------------------------------------------------
  float input_intensity = INPUT_INTENSITY_INIT; //输入强度，256/8*input_intensity
  vector<float> FR_khz(NPoi, 0);
  uint64_t *CPUratesPPoi = new uint64_t[NPoi]; //genn要求手动给ratesPPoi分配内存
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPPoi, NPoi * sizeof(uint64_t)));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice));
  //cudaHostAlloc(&CPUratesPPoi, 1 * sizeof(unsigned int), cudaHostAllocPortable);
  //deviceMemAllocate(&d_glbSpkCntPPoi, dd_glbSpkCntPPoi, 1 * sizeof(unsigned int));

  // ------------------------------------------------------------------------------
  //  准备分类层
  // ------------------------------------------------------------------------------
  float cla_input_intensity = Cla_INPUT_INTENSITY_INIT;
  vector<float> cla_FR_khz(NCla, 0);
  uint64_t *CPUratesPCla = new uint64_t[NCla];
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPCla, NCla * sizeof(uint64_t)));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice));

  // ------------------------------------------------------------------------------
  //  重写参数
  // ------------------------------------------------------------------------------
  //重写gP2E初始值.最后一个参数是g_max乘以1000
  get_rand_g(gP2E, NPoi * NExc, gPE_INIT_MAX_1000);
#ifdef READ_gPE_FROM_FILE
  read_gP2E_from_file(gP2E);
  read_thetaPExc_from_file(thetaPExc);
#endif
  // get_rand_g(gE2C, NExc * NCla, gEC_INIT_MAX_1000);
  rewrite_gEI_gIE();
  //重写seedPPoi
  get_rand(seedPPoi, NPoi, 100000);

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
  ofstream os_t2N_poi("./output/output_t2N_poi.dat");
  ofstream os_t2N_exc("./output/output_t2N_exc.dat");
  ofstream os_t2N_inh("./output/output_t2N_inh.dat");
  ofstream os_visual("./output/output_visual.dat");
#endif
  // ------------------------------------------------------------------------------
  //  可视化突触权重
  // ------------------------------------------------------------------------------
#ifdef PLOT_ON
  plot_visual_PEw_init(PEplotter);
  get_visual_PEw(PEVisual);
  write_visual_PEw_to_file(PEVisual);
  plot_visual_PEw(PEplotter);
  // plot_visual_ECw_init(ECplotter);
  // get_visual_ECw(ECVisual);
  // write_visual_ECw_to_file(ECVisual);
  // plot_visual_ECw(ECplotter);
  // plot_visual_ECw_inferred_init(ECplotter_inferred);
  // get_visual_ECw_inferred(ECVisual_inferred, assignments);
  // write_visual_ECw_inferred_to_file(ECVisual_inferred);
  // plot_visual_ECw_inferred(ECplotter_inferred);
#endif
  // ------------------------------------------------------------------------------
  //  收敛曲线（准确率、权重方差）、混淆矩阵、spike_monitor绘图
  // ------------------------------------------------------------------------------
  int performance_size = num_samples / update_interval;
  vector<float> performance(performance_size, 0);
  vector<float> cla_performance(performance_size, 0);
  vector<float> performanceNowUseTrainData(performance_size, 0);
  vector<float> performanceNowUseTestData(performance_size, 0);
  vector<float> cla_performanceNowUseTrainData(performance_size, 0);
  vector<float> cla_performanceNowUseTestData(performance_size, 0);

  vector<float> variance(NExc, 0);
  vector<float> variance_E2C(NExc, 0);
  vector<vector<float>> confusion_m(11, vector<float>(11, 0));
  vector<vector<float>> confusion_m_supervised(11, vector<float>(11, 0));
  int offset = 0;
  int current_evaluation = 0;

  vector<vector<int>> spikes_real_time((int)(RUN_TIME / DT), vector<int>(NExc, 0));
  vector<vector<int>> cla_spikes_real_time((int)(RUN_TIME / DT), vector<int>(NCla, 0));
  vector<int> response_rate(60, 0);
  vector<int> theta(60, 0);
  size_t size_thetaPExc = NExc;
#ifdef PLOT_ON
  plot_performance_init(plot_per, num_samples);
  plot_performance_init(plot_perNowUseTrainData, num_samples);
  plot_performance_init(plot_perNowUseTestData, num_samples);
  plot_variance_init(plot_v, num_samples / update_interval);
  plot_variance_E2C_init(plot_vE2C, num_samples / update_interval);
  plot_confusion_m_init(plot_confusion);
  plot_confusion_m_init(plot_confusion_supervised);
  plot_variance_distribution_init(plot_v_dist);
  plot_variance_E2C_distribution_init(plot_vE2C_dist);
  plot_assignments_distribution_init(plot_assign_dist);
  plot_theta_init(plot_theta);
  plot_response_rates_init(plot_response_rate);
#endif
#ifdef SPIKES_MONITOR //plot_spike如果声明和使用不在同一个#ifdef下会报错
  plot_spikes_init(plot_spike);
  plot_cla_spikes_init(plot_cla_spike);
#endif
  // ------------------------------------------------------------------------------
  //  RUN
  // ------------------------------------------------------------------------------
  imageNum = 0;
  bool testDataEvaluateMode = true; // false;
  while (imageNum < num_samples)
  {
    //imageNum集中更新
    static bool testDataStartOnce = true;
    if (testDataStartOnce || (((testDataEvaluateMode == false && imageNum % update_interval == 0) || (testDataEvaluateMode == true && imageNum % evaluation_interval == 0)) && (sum_glbSpkCntPExc >= NUM_SPIKE_RESP || input_intensity >= 32)))
    {
      testDataStartOnce = false;
      testDataEvaluateMode = !testDataEvaluateMode; //true-false-true-false...
      fill_n(testDataEvaluateModePExc, NExc, testDataEvaluateMode);
      fill_n(testDataEvaluateModeP2E, size_gP2E, testDataEvaluateMode);
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModePExc, testDataEvaluateModePExc, NExc * sizeof(bool), cudaMemcpyHostToDevice));
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModeP2E, testDataEvaluateModeP2E, size_gP2E * sizeof(bool), cudaMemcpyHostToDevice));
      if (imageNum >= NUM_TRAINING_SL_INI)
      {
        fill_n(testDataEvaluateModePCla, NCla, testDataEvaluateMode);
        fill_n(testDataEvaluateModeE2C, size_gE2C, testDataEvaluateMode);
        CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModePCla, testDataEvaluateModePCla, NCla * sizeof(bool), cudaMemcpyHostToDevice));
        CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModeE2C, testDataEvaluateModeE2C, size_gE2C * sizeof(bool), cudaMemcpyHostToDevice));
      }
      if (!testDataEvaluateMode) //false时表示已经跑完测试集，true时刚开始要跑测试集
      {
        int offsetNow = 0;
        static int current_evaluationNow = 0;
        cout << "\nEvaluating with TestDataSet after trained " << current_evaluationNow * update_interval << " samples" << endl;
        get_new_assignments(assignmentsTestDataSet, offsetNow, result_monitor, labels_test); //result_monitor:所有样本的spike_record
        get_assignments_distribution(assignmentsTestDataSet, assignmentsTestDataSet_dist);
        write_assignments_distribution_to_file(assignmentsTestDataSet_dist);

        get_performance(performanceNowUseTestData, offsetNow, current_evaluationNow, result_monitor, assignmentsTestDataSet, labels_test, update_interval);
        cla_get_performance(cla_performanceNowUseTestData, offsetNow, current_evaluationNow, cla_result_monitor, labels_test, update_interval);
        write_performance_to_file(performanceNowUseTestData, cla_performanceNowUseTestData, current_evaluationNow, NowTest);

        get_confusion_m(confusion_m, offsetNow, result_monitor, assignmentsTestDataSet, labels_test, update_interval);
        write_confusion_m_to_file(confusion_m, UNSUPERVISED);

        get_confusion_m_supervised(confusion_m_supervised, offsetNow, cla_result_monitor, labels_test, update_interval);
        write_confusion_m_to_file(confusion_m_supervised, SUPERVISED);

#ifdef PLOT_ON
        plot_assignments_distribution(plot_assign_dist);
        plot_performance(plot_perNowUseTestData, NowTest);
        plot_confusion_m(plot_confusion, UNSUPERVISED);
        plot_confusion_m(plot_confusion_supervised, SUPERVISED);
#endif
        current_evaluationNow++;
        imageNum = imageNum_train;
      }
      else
      {
        imageNum_train = imageNum;
        imageNum = 0;
      }
    }

    // ------------------------------------------------------------------------------
    //  输入图像
    // ------------------------------------------------------------------------------
    if (!testDataEvaluateMode)
      feed_to_networks(images[imageNum % num_train_samples], FR_khz, input_intensity);
    else
      feed_to_networks(images_test[imageNum % num_test_samples], FR_khz, input_intensity);

    // ------------------------------------------------------------------------------
    //  分类层
    // ------------------------------------------------------------------------------
    if (imageNum >= NUM_TRAINING_SL_INI)
    {
#ifdef TRAIN_LAYER_BY_LAYER
      static bool OnlyOnceInitCla = true;
      if (OnlyOnceInitCla) //训练到NUM_TRAINING_SL_INI个样本重置Cla层和gE2C的所有变量
      {
        cout << "Start to train Cla. in SL:" << NUM_TRAINING_SL_INI << endl;
        OnlyOnceInitCla = false;
        reset_Cla_para();
      }
#endif
      if (!testDataEvaluateMode)
        Cla_feed_to_networks(labels[imageNum % num_train_samples], cla_FR_khz, cla_input_intensity);
    }
    // ------------------------------------------------------------------------------
    //  350ms run
    // ------------------------------------------------------------------------------
    sum_glbSpkCntPExc = 0;
    spike_record.assign(NExc, 0); //记录发放的神经元 清零
    cla_spike_record.assign(NCla, 0);

    CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
    normalize_weights(gP2E);
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
        spike_record[glbSpkPExc[i]]++;
      for (int i = 0; i < glbSpkCntPCla[0]; i++) //记录发放的神经元
        cla_spike_record[glbSpkPCla[i]]++;
#ifdef SPIKES_MONITOR
      for (int i = 0; i < glbSpkCntPExc[0]; i++) //记录发放的神经元
        spikes_real_time[dt][glbSpkPExc[i]] = 1;
      for (int i = 0; i < glbSpkCntPCla[0]; i++) //记录发放的神经元
        cla_spikes_real_time[dt][glbSpkPCla[i]] = 1;
#endif
    }
#ifdef SPIKES_MONITOR
    if (write_spike_to_file(spikes_real_time, NExc, "spike.dat"))
      plot_spikes(plot_spike);
    if (write_spike_to_file(cla_spikes_real_time, NCla, "cla_spike.dat"))
      plot_cla_spikes(plot_cla_spike);
    spikes_real_time.assign(int(RUN_TIME / DT), vector<int>(NExc, 0)); //复位发放的神经元
    cla_spikes_real_time.assign(int(RUN_TIME / DT), vector<int>(NCla, 0));
#endif
    if (sum_glbSpkCntPExc < 60)
      response_rate[sum_glbSpkCntPExc] = response_rate[sum_glbSpkCntPExc] + 1;
    // #ifdef PLOT_ON
    //     if (imageNum > 0 && !testDataEvaluateMode) //测试集评估时不跑
    //     {
    //       if (imageNum % update_interval == 0) //实时准确率
    //       {
    //         static bool get_assignment_flag = false;
    //         offset = (imageNum % num_train_samples - update_interval) >= 0 ? (imageNum % num_train_samples - update_interval) : 50000;
    //         current_evaluation = imageNum / update_interval;
    //         if (get_assignment_flag && whether_evaluation_or_not(assignments)) //2w开始画准确率
    //         {
    //           cout << "calculate performance&confusion_m ..." << endl;
    //           get_visual_ECw_inferred(ECVisual_inferred, assignments);
    //           write_visual_ECw_inferred_to_file(ECVisual_inferred);
    //           plot_visual_ECw_inferred(ECplotter_inferred);
    //           // get_visual_CEw_inferred(CEVisual_inferred, assignments);
    //           // write_visual_CEw_inferred_to_file(CEVisual_inferred);
    //           // plot_visual_CEw_inferred(CEplotter_inferred);
    //         }
    //         get_assignment_flag = true;

    //         cout << "\nEvaluating with TrainDataSet when Training..." << endl;
    //         get_performance(performanceNowUseTrainData, offset, current_evaluation, result_monitor, assignments, labels);
    //         cla_get_performance(cla_performanceNowUseTrainData, offset, current_evaluation, cla_result_monitor, labels);
    //         write_performance_to_file(performanceNowUseTrainData, cla_performanceNowUseTrainData, current_evaluation, NowTrain);
    //         plot_performance(plot_perNowUseTrainData, NowTrain);
    //         cout << "end" << endl;

    //       }
    //     }
    // #endif

    // ------------------------------------------------------------------------------
    //  是否增加输入强度？
    // ------------------------------------------------------------------------------
    if (sum_glbSpkCntPExc < NUM_SPIKE_RESP && input_intensity < 32)
    {
      input_intensity += 1;
      reset_ratesPCla(cla_FR_khz);
      reset_ratesPPoi(FR_khz);
      for (int dt = 0; dt < REST_TIME / DT; dt++)
        stepTimeGPU(); //兴奋放电太少，复位状态，重新跑
    }
    // ------------------------------------------------------------------------------
    //  150ms rest
    // ------------------------------------------------------------------------------
    else
    {
      if (input_intensity >= 32)
      {
        cout << "imageNum without " << NUM_SPIKE_RESP << "-spike-response: " << (imageNum + 1) << endl;
      }
      result_monitor[imageNum % update_interval].assign(spike_record.begin(), spike_record.end()); //这个样本激发神经元存起来
      cla_result_monitor[imageNum % update_interval].assign(cla_spike_record.begin(), cla_spike_record.end());
      if ((imageNum + 1) % 100 == 0 && imageNum > 0) //显示而已
      {
        if (!testDataEvaluateMode)
          cout << "Training data - runs done: " << (imageNum + 1) << " -> " << num_samples << "; input_intensity: " << input_intensity << endl;
        else
          cout << "Test data for " << imageNum_train << "  - runs done: " << (imageNum + 1) << " -> " << num_test_samples << "; input_intensity: " << input_intensity << endl;
      }

      reset_ratesPCla(cla_FR_khz);
      reset_ratesPPoi(FR_khz);

      for (int dt = 0; dt < REST_TIME / DT; dt++) //rest
      {
        stepTimeGPU();
      }
      if ((imageNum + 1) % update_interval == 0 && imageNum > 0 && !testDataEvaluateMode)
      {
        CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERRORS(cudaMemcpy(thetaPExc, d_thetaPExc, NExc * sizeof(float), cudaMemcpyDeviceToHost));

        string fileid = "_" + to_string((imageNum + 1) / update_interval);
        save_gP2E("gP2E" + fileid);
        save_gE2C("gE2C" + fileid);
        save_theta("theta" + fileid);
      }
#ifdef PLOT_ON
      if (imageNum > 0 && !testDataEvaluateMode) //测试集评估时不跑
      {
        if ((imageNum + 1) % update_interval == 0) //权重方差
        {
          get_variance(variance, gP2E, NExc, NPoi, NORMAL);
          write_variance_to_file(variance, imageNum / update_interval);
          plot_variance(plot_v);
          plot_variance_distribution(plot_v_dist);
          get_variance(variance_E2C, gE2C, NExc, NCla, Cla_NORMAL);
          write_variance_gEC_to_file(variance_E2C, imageNum / update_interval);
          plot_variance_E2C(plot_vE2C);
          plot_variance_E2C_distribution(plot_vE2C_dist);
        }
        if (((imageNum + 1) % update_interval == 0) || ((imageNum + 1) % update_interval == 0))
        {

          CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
          CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));

          get_visual_PEw(PEVisual);
          write_visual_PEw_to_file(PEVisual);
          plot_visual_PEw(PEplotter);
          // get_visual_ECw(ECVisual);
          // write_visual_ECw_to_file(ECVisual);
          // plot_visual_ECw(ECplotter);
        }
        if ((imageNum + 1) % update_interval == 0)
        {
          write_vector_to_file(response_rate, 60, "response_rate.dat");
          plot_response_rates(plot_response_rate);
        }
        if ((imageNum + 1) % update_interval == 0)
        {

          CHECK_CUDA_ERRORS(cudaMemcpy(thetaPExc, d_thetaPExc, size_thetaPExc * sizeof(float), cudaMemcpyDeviceToHost));

          theta.assign(75, 0);
          for (int i = 0; i < size_thetaPExc; i++)
            if ((int)(thetaPExc[i] / 1) < 75)
              theta[(int)(thetaPExc[i] / 1)] += 1;
          write_vector_to_file(theta, 75, "theta.dat");
          // plot_thetas(plot_theta);
        }
      }
#endif

      input_intensity = INPUT_INTENSITY_INIT; //输入强度重置
      imageNum += 1;
    }
  }
  //------------------------------------------------------------------------------
  // save&plot results
  //------------------------------------------------------------------------------
  {
    cout << "save results..." << endl;

    CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
    CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
    CHECK_CUDA_ERRORS(cudaMemcpy(thetaPExc, d_thetaPExc, NExc * sizeof(float), cudaMemcpyDeviceToHost));

    save_gP2E(string("gP2E"));
    save_gE2C(string("gE2C"));
    save_theta(string("theta"));

#ifdef PLOT_ON
    get_visual_PEw(PEVisual);
    write_visual_PEw_to_file(PEVisual);
    plot_visual_PEw(PEplotter);
    // get_visual_ECw(ECVisual);
    // write_visual_ECw_to_file(ECVisual);
    // plot_visual_ECw(ECplotter);
    // get_visual_ECw_inferred(ECVisual_inferred, assignments);
    // write_visual_ECw_inferred_to_file(ECVisual_inferred);
    // plot_visual_ECw_inferred(ECplotter_inferred);
    get_variance(variance, gP2E, NExc, NPoi, NORMAL);
    write_variance_to_file(variance, imageNum / update_interval);
    plot_variance(plot_v);
    plot_variance_distribution(plot_v_dist);
    get_variance(variance, gE2C, NExc, NCla, Cla_NORMAL);
    write_variance_gEC_to_file(variance_E2C, imageNum / update_interval);
    plot_variance_E2C(plot_vE2C);
    plot_variance_E2C_distribution(plot_vE2C_dist);
#endif
  }

// ------------------------------------------------------------------------------
//  DONE
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

  // delete[] CPUratesPCla;
  cudaFreeHost(ratesPCla); //究竟哪个free呢？
  CHECK_CUDA_ERRORS(cudaFree(ratesPCla));

  // delete[] CPUratesPPoi;
  cudaFreeHost(ratesPPoi); //究竟哪个free呢？
  CHECK_CUDA_ERRORS(cudaFree(ratesPPoi));

  fprintf(stdout, "#training end  ... \n");

  // cin.get();
  // exitGeNN();

  //  timer
  time_t t_end = time(NULL); //返回当前时间到1970年1月1日零点时间的差.秒
  cout << "t_start:" << t_start << endl;
  cout << "t_end:" << t_end << endl;
  cout << "time to use:" << t_end - t_start << "s = " << (t_end - t_start) / 3600.0 << "h" << endl;
  return 0;
}

int str2int(string s)
{
  int num;
  stringstream ss(s);
  ss >> num;
  return num;
}

//-------------------------------------------------------------------------
/*! \brief Function to convert a firing rate (in kHz) 
to an integer of type uint64_t that can be used as a threshold for the GeNN random number generator to generate events with the given rate.
*/
//-------------------------------------------------------------------------
void convertRateToRandomNumberThreshold(vector<float> rateKHz_pattern, uint64_t *pattern, int Num)
{
  float fac = pow(2.0, (double)sizeof(uint64_t) * 8 - 16) * DT;
  for (int i = 0; i < Num; i++)
  {
    pattern[i] = (uint64_t)(rateKHz_pattern[i] * fac);
  }
}
void get_inputdata(string datapath, vector<vector<float>> &images, vector<float> &labels, vector<vector<float>> &images_test, vector<float> &labels_test)
{
  if (DATA == "cifar10/")
  {
#ifdef GRAYSCALE
    cout << "**********cifar10**********" << endl;
    if (DATASET_TRAIN)
    {
      read_cifar10_train(path, GRAYSCALE, images, labels);
      read_cifar10_test(path, GRAYSCALE, images_test, labels_test);
    }
    else
    {
      read_cifar10_test(path, GRAYSCALE, images, labels);
    }
#endif
  }
  else if (DATA == "caltech/")
  {
    cout << "**********caltech101-resized-dog**********" << endl;
    read_caltech("/home/hyz/Downloads/dataset/caltech101-resized-dog/", images, labels, images_test, labels_test);
  }
  else if (DATA == "fashion-mnist/")
  {
    cout << "**********fashion-mnist**********" << endl;
    if (DATASET_TRAIN)
    {
      // read_mnist_images(path + "train-images-idx3-ubyte", images);
      read_mnist_images(path + "train-images-idx3-ubyte-DoG-ON", images);
      read_mnist_label(path + "train-labels-idx1-ubyte", labels);

      // read_mnist_images(path + "t10k-images-idx3-ubyte", images_test);
      read_mnist_images(path + "t10k-images-idx3-ubyte-DoG-ON", images_test);
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labels_test);
    }
    else
    {
      read_mnist_images(path + "t10k-images-idx3-ubyte", images);
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labels);
    }
  }
  else
  {
    cout << "**********mnist**********" << endl;
    if (DATASET_TRAIN)
    {
      // read_mnist_images(path + "train-images-idx3-ubyte", images);
      read_mnist_images(path + "train-images-idx3-ubyte-DoG-ON", images);
      read_mnist_label(path + "train-labels-idx1-ubyte", labels);

      // read_mnist_images(path + "t10k-images-idx3-ubyte", images_test);
      read_mnist_images(path + "t10k-images-idx3-ubyte-DoG-ON", images_test);
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labels_test);
    }
    else
    {
      read_mnist_images(path + "t10k-images-idx3-ubyte", images);
      read_mnist_label(path + "t10k-labels-idx1-ubyte", labels);
    }
  }
}

void rewrite_gEI_gIE()
{
  //先编译model文件才有gE2I变量……
  float *g_EI_array = new float[preN_EI * postN_EI]; //dense型存储（对应到函数实现上）
  float *g_IE_array = new float[preN_IE * postN_IE]; //dense型存储（对应到函数实现上）
  //E——>I
  for (int i_dense = 0; i_dense < preN_EI; i_dense++)    //行pre
    for (int j_dense = 0; j_dense < postN_EI; j_dense++) //列post
    {
      if (i_dense == j_dense)
        g_EI_array[i_dense * postN_EI + j_dense] = g_EI; //大于1e-19就能分开
      else
        g_EI_array[i_dense * postN_EI + j_dense] = 0.0; //大于1e-19就能分开
    }
  //I——>E
  for (int i_dense = 0; i_dense < preN_IE; i_dense++)    //行pre
    for (int j_dense = 0; j_dense < postN_IE; j_dense++) //列post
    {
      if (i_dense == j_dense)
        g_IE_array[i_dense * postN_IE + j_dense] = 0.0; //大于1e-19就能分开
      else
        g_IE_array[i_dense * postN_IE + j_dense] = g_IE; //大于1e-19就能分开
    }
  setSparseConnectivityFromDense(gE2I, preN_EI, postN_EI, g_EI_array, &CE2I);
  setSparseConnectivityFromDense(gI2E, preN_IE, postN_IE, g_IE_array, &CI2E);

  delete[] g_EI_array;
  delete[] g_IE_array;
}
void reset_Cla_para()
{
  glbSpkCntPCla[0] = 0;

  fill_n(glbSpkPCla, NCla, 0);
  fill_n(sTPCla, NCla, -10); //???
  fill_n(VPCla, NCla, -100);
  fill_n(trace1PCla, NCla, 1);
  fill_n(trace2PCla, NCla, 1);
  fill_n(seedPCla, NCla, 1);

  pushPClaStateToDevice();
  pushPClaSpikesToDevice();
  pushPClaSpikeEventsToDevice();
  pushPClaCurrentSpikesToDevice();
  pushPClaCurrentSpikeEventsToDevice();

  fill_n(inSynE2C, NCla, 0);
  get_rand_g(gE2C, NExc * NCla, gEC_INIT_MAX_1000);
  pushE2CStateToDevice();
}
void feed_to_networks(vector<float> image, vector<float> &FR_khz, float input_intensity)
{
  static uint64_t *CPUratesPPoi = new uint64_t[NPoi];
  static vector<float> one_image;
  one_image.clear();
  for (int i = 0; i < NPoi; i++)
    one_image.push_back(image[i]);
  for (int i = 0; i < NPoi; i++)
    FR_khz[i] = (float)one_image[i] / 8.0 / 1000.0 * input_intensity;
  convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);                                          //单位是khz,为与DT配合
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据
}
void Cla_feed_to_networks(int label, vector<float> &cla_FR_khz, float cla_input_intensity)
{
  static uint64_t *CPUratesPCla = new uint64_t[NCla];
  cla_FR_khz.assign(NCla, 0);
  cla_FR_khz[label] = 1.0 * cla_input_intensity;
  convertRateToRandomNumberThreshold(cla_FR_khz, CPUratesPCla, NCla);                                      //单位是khz
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据
  CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
  cla_normalize_weights(gE2C);                                                                    //能不能在gpu里算这个？
  CHECK_CUDA_ERRORS(cudaMemcpy(d_gE2C, gE2C, size_gE2C * sizeof(float), cudaMemcpyHostToDevice)); //注意把g写入GPU
}
void reset_ratesPPoi(vector<float> &FR_khz)
{
  static uint64_t *CPUratesPPoi = new uint64_t[NPoi];
  FR_khz.assign(NPoi, 0);
  convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);                                          //单位是khz
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据
}
void reset_ratesPCla(vector<float> &cla_FR_khz)
{
  static uint64_t *CPUratesPCla = new uint64_t[NCla];
  cla_FR_khz.assign(NCla, 0);
  convertRateToRandomNumberThreshold(cla_FR_khz, CPUratesPCla, NCla);                                      //单位是khz
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice)); //之后记得尝试一次多准备些数据
}
