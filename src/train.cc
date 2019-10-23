#include "train.h"
#include "model.h"
#include "reader/mnist/mnist.h"
#include "gnuplot.h"
#include "drafting.h"
#include "file_recorder.h"
#include "evaluation.h"
#include "file_read.h"

#include <cuda_runtime.h>
#include <fstream>
#include <iostream>
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
string datapath = "data/" + string(DATA);

int main()
{
  //  timer
  time_t t_start = time(NULL);
  // required by GeNN
  allocateMem();
  allocateE2I(connN_E2I);
  allocateI2E(connN_I2E);
  initialize();

  //------------------------------------------------------------------------------
  // set parameters and equations
  //------------------------------------------------------------------------------
  int sum_glbSpkCntPExc = 0;
  int num_train_samples;
  int num_test_samples;

  vector<int> assignments(NExc, -1); //assign labels to each neuron using assignments
  vector<int> assignments_dist(NUM_CLASS, 0);
  // vector<int> assignmentsTestDataSet(NExc, -1);
  // vector<int> assignmentsTestDataSet_dist(NUM_CLASS, 0);
  vector<vector<int>> test_result_monitor(NUM_TEST_SAMPLES, vector<int>(NExc, 0));   
  vector<vector<int>> train_result_monitor(NUM_TRAIN_SAMPLES, vector<int>(NExc, 0)); 
  vector<vector<int>> cla_result_monitor(NUM_TEST_SAMPLES, vector<int>(NCla, 0));
  vector<int> spike_record(NExc, 0); 
  vector<int> cla_spike_record(NCla, 0);
  // ------------------------------------------------------------------------------
  //  load dataset
  // ------------------------------------------------------------------------------
  vector<vector<float>> images;
  vector<float> labels;
  vector<vector<float>> images_test;
  vector<float> labels_test;
  clock_t start; 
  clock_t end;
  get_inputdata(datapath, images, labels, images_test, labels_test);
  if (images.size() == labels.size() && images_test.size() == labels_test.size())
  {
    num_train_samples = images.size();
    num_test_samples = images_test.size();
    cout << "images size: " << images.size() << "  images_test size: " << images_test.size() << endl;
  }
  else
    exit(1);

  // ------------------------------------------------------------------------------
  //  input images initialization
  // ------------------------------------------------------------------------------
  float input_intensity = INPUT_INTENSITY_INIT; // 256.0 / 8 * input_intensity
  vector<float> FR_khz(NPoi, 0);
  uint64_t *CPUratesPPoi = new uint64_t[NPoi]; // required by genn
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPPoi, NPoi * sizeof(uint64_t)));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice));
  //cudaHostAlloc(&CPUratesPPoi, 1 * sizeof(unsigned int), cudaHostAllocPortable);
  //deviceMemAllocate(&d_glbSpkCntPPoi, dd_glbSpkCntPPoi, 1 * sizeof(unsigned int));

  // ------------------------------------------------------------------------------
  //  supervision layer initialization
  // ------------------------------------------------------------------------------
  float cla_input_intensity = Cla_INPUT_INTENSITY_INIT;
  vector<float> cla_FR_khz(NCla, 0);
  uint64_t *CPUratesPCla = new uint64_t[NCla];
  CHECK_CUDA_ERRORS(cudaMalloc((void **)&ratesPCla, NCla * sizeof(uint64_t)));
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice));

  // ------------------------------------------------------------------------------
  //  parameters initialization 
  // ------------------------------------------------------------------------------
  get_rand_g(gP2E, NPoi * NExc, gPE_INIT_MAX_1000);
#ifdef FIXED_HIDDEN_LAYER
  /***parameters of 400 excitatory neurons for mnist***/
  read_gP2E_from_file("./weights/mnist_400/gP2E", gP2E);
  read_thetaPExc_from_file("./weights/mnist_400/theta", thetaPExc);
#endif
  // get_rand_g(gE2C, NExc * NCla, gEC_INIT_MAX_1000);
  rewrite_gEI_gIE();
  get_rand(seedPPoi, NPoi, 100000);

  // ------------------------------------------------------------------------------
  //  reset model
  // ------------------------------------------------------------------------------
  copyStateToDevice();
  initmodel(); // need by sparse connection
  copyCurrentSpikesToDevice();

  // ------------------------------------------------------------------------------
  //  output data to file
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
  //  visualization
  // ------------------------------------------------------------------------------
#ifdef PLOT_ON
  Drafting PEw{init_PEw, plot_PEw, true};
  get_visual_PEw(PEVisual);
  write_visual_PEw_to_file(PEVisual);
  Drafting ECw{init_ECw, plot_ECw, true};
  get_visual_ECw(ECVisual);
  write_visual_ECw_to_file(ECVisual);
#endif
  // ------------------------------------------------------------------------------
  //  performance, confusion matrix and spike monitor
  // ------------------------------------------------------------------------------
  vector<float> performance(UPDATE_TOTAL, 0);
  vector<float> cla_performance(UPDATE_TOTAL, 0);
  vector<float> performanceNowUseTrainData(UPDATE_TOTAL, 0);
  vector<float> performanceNowUseTestData(UPDATE_TOTAL, 0);
  vector<float> cla_performanceNowUseTrainData(UPDATE_TOTAL, 0);
  vector<float> cla_performanceNowUseTestData(UPDATE_TOTAL, 0);

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
  Drafting perNowUseTestData{init_performance, plot_performance};
  Drafting varianceP2E{init_varianceP2E, plot_varianceP2E};
  Drafting varianceE2C{init_varianceE2C, plot_varianceE2C};
  Drafting usl_confusion{init_confusion, plot_usl_confusion, true};
  Drafting sl_confusion{init_confusion, plot_sl_confusion, true};
  Drafting varianceP2E_distribution{init_varianceP2E_distribution, plot_varianceP2E_distribution, true};
  Drafting varianceE2C_distribution{init_varianceE2C_distribution, plot_varianceE2C_distribution, true};
  Drafting assign_dist{init_assign_dist, plot_assign_dist};
  Drafting thetas{init_theta, plot_theta};
  Drafting response_rates{init_response_rates, plot_response_rates};
#endif
#ifdef SPIKES_MONITOR 
  Drafting spikes{init_spikes, plot_spikes};
  Drafting Claspikes{init_Claspikes, plot_Claspikes};
#endif

  // ------------------------------------------------------------------------------
  //  RUN
  // ------------------------------------------------------------------------------
  imageNum = 0;
  bool testDataEvaluateMode = true; // false;
  bool test_begin;
  while (imageNum < NUM_SAMPLES)
  {
    // imageNum update
    static bool testDataStartOnce = true;
    test_begin = imageNum > 0 && imageNum % UPDATE_INTERVAL == 0;
    // test_begin = (imageNum <= 11 * NUM_TRAIN_SAMPLES && imageNum % NUM_TRAIN_SAMPLES == 0) || (imageNum > 11 * NUM_TRAIN_SAMPLES && imageNum % UPDATE_INTERVAL == 0);
    if (testDataStartOnce || (((testDataEvaluateMode == false && test_begin) || (testDataEvaluateMode == true && imageNum % EVALUATION_INTERVAL == 0)) && (sum_glbSpkCntPExc >= NUM_SPIKE_RESP || input_intensity >= 32)))
    {
      testDataStartOnce = false;
      testDataEvaluateMode = !testDataEvaluateMode; //true-false-true-false...
#ifndef FIXED_HIDDEN_LAYER
      fill_n(testDataEvaluateModePExc, NExc, testDataEvaluateMode);
      fill_n(testDataEvaluateModeP2E, size_gP2E, testDataEvaluateMode);
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModePExc, testDataEvaluateModePExc, NExc * sizeof(bool), cudaMemcpyHostToDevice));
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModeP2E, testDataEvaluateModeP2E, size_gP2E * sizeof(bool), cudaMemcpyHostToDevice));
#endif

      fill_n(testDataEvaluateModePCla, NCla, testDataEvaluateMode);
      fill_n(testDataEvaluateModeE2C, size_gE2C, testDataEvaluateMode);
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModePCla, testDataEvaluateModePCla, NCla * sizeof(bool), cudaMemcpyHostToDevice));
      CHECK_CUDA_ERRORS(cudaMemcpy(d_testDataEvaluateModeE2C, testDataEvaluateModeE2C, size_gE2C * sizeof(bool), cudaMemcpyHostToDevice));

      if (!testDataEvaluateMode) //'false' means run the test set. 'true' means start running test set
      {
        static int current_evaluationNow = 0;
        cout << "\nEvaluating with TestDataSet after trained " << current_evaluationNow * UPDATE_INTERVAL << " samples" << endl;
        get_new_assignments(assignments, train_result_monitor, labels); 
        get_assignments_distribution(assignments, assignments_dist);
        write_assignments_distribution_to_file(assignments_dist);

        get_performance(performance, current_evaluationNow, test_result_monitor, assignments, labels_test);
        cla_get_performance(cla_performance, current_evaluationNow, cla_result_monitor, labels_test);
        write_performance_to_file(performance, cla_performance, current_evaluationNow, NowTest);

        get_confusion_m(confusion_m, test_result_monitor, assignments, labels_test);
        write_confusion_m_to_file(confusion_m, UNSUPERVISED);

        get_confusion_m_supervised(confusion_m_supervised, cla_result_monitor, labels_test);
        write_confusion_m_to_file(confusion_m_supervised, SUPERVISED);

#ifdef PLOT_ON
        assign_dist.update();
        perNowUseTestData.update();
        usl_confusion.update();
        sl_confusion.update();
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
    //  input images
    // ------------------------------------------------------------------------------
    if (!testDataEvaluateMode)
      feed_to_networks(images[imageNum % num_train_samples], FR_khz, input_intensity);
    else
      feed_to_networks(images_test[imageNum % num_test_samples], FR_khz, input_intensity);

    // ------------------------------------------------------------------------------
    //  supervision layer
    // ------------------------------------------------------------------------------
    if (imageNum >= NUM_TRAINING_SL_INI)
    {
      static bool OnlyOnceInitCla = true;
      if (OnlyOnceInitCla) // Reset supervision layer parameters when imageNum = NUM_TRAINING_SL_INI
      {
        cout << "Start to train Cla. in SL:" << NUM_TRAINING_SL_INI << endl;
        OnlyOnceInitCla = false;
        reset_Cla_para();
      }
      if (!testDataEvaluateMode)
        Cla_feed_to_networks(labels[imageNum % num_train_samples], cla_FR_khz, cla_input_intensity);
    }
    // ------------------------------------------------------------------------------
    //  350ms run
    // ------------------------------------------------------------------------------
    sum_glbSpkCntPExc = 0;
    spike_record.assign(NExc, 0); // clean spike record
    cla_spike_record.assign(NCla, 0);

    CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
    normalize_weights(gP2E);
    CHECK_CUDA_ERRORS(cudaMemcpy(d_gP2E, gP2E, size_gP2E * sizeof(float), cudaMemcpyHostToDevice)); 

    for (int dt = 0; dt < RUN_TIME / DT; dt++)
    {
      stepTimeGPU();
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPExc, d_glbSpkCntPExc, sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkCntPCla, d_glbSpkCntPCla, sizeof(unsigned int), cudaMemcpyDeviceToHost));

      sum_glbSpkCntPExc += glbSpkCntPExc[0]; 
#ifdef FILE_RECODER
      copyStateFromDevice();
      showGPU();
#endif

      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPExc, d_glbSpkPExc, glbSpkCntPExc[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));
      CHECK_CUDA_ERRORS(cudaMemcpy(glbSpkPCla, d_glbSpkPCla, glbSpkCntPCla[0] * sizeof(unsigned int), cudaMemcpyDeviceToHost));

      for (int i = 0; i < glbSpkCntPExc[0]; i++) 
        spike_record[glbSpkPExc[i]]++;
      for (int i = 0; i < glbSpkCntPCla[0]; i++) 
        cla_spike_record[glbSpkPCla[i]]++;
#ifdef SPIKES_MONITOR
      for (int i = 0; i < glbSpkCntPExc[0]; i++) 
        spikes_real_time[dt][glbSpkPExc[i]] = 1;
      for (int i = 0; i < glbSpkCntPCla[0]; i++) 
        cla_spikes_real_time[dt][glbSpkPCla[i]] = 1;
#endif
    }
#ifdef SPIKES_MONITOR
    if (write_spike_to_file(spikes_real_time, NExc, "spike.dat"))
      spikes.update();
    if (write_spike_to_file(cla_spikes_real_time, NCla, "cla_spike.dat"))
      Claspikes.update();
    spikes_real_time.assign(int(RUN_TIME / DT), vector<int>(NExc, 0)); 
    cla_spikes_real_time.assign(int(RUN_TIME / DT), vector<int>(NCla, 0));
#endif
    if (sum_glbSpkCntPExc < 60)
      response_rate[sum_glbSpkCntPExc] = response_rate[sum_glbSpkCntPExc] + 1;

    // ------------------------------------------------------------------------------
    //  Each sample is presented to a network for 350ms, then we reset the network for 150ms. 
    //  When the excitatory neurons in the hidden layer fire less than five spikes within 350 ms, 
    //  in addition of reset, we also increase the input intensity by 32Hz and do not update sample ID, 
    //  which means this sample will be presented to network again in higher rates for another 350ms, 
    //  until the input rates too high, i.e., "input_intensity < 32".
    // ------------------------------------------------------------------------------
    if (sum_glbSpkCntPExc < NUM_SPIKE_RESP && input_intensity < 32)
    {
      input_intensity += 1;// increase the input intensity by 32Hz.
      reset_ratesPCla(cla_FR_khz);
      reset_ratesPPoi(FR_khz);// reset the FR_khz to 0, i.e., no spikes input
      for (int dt = 0; dt < REST_TIME / DT; dt++)// reset the networks for 150ms
        stepTimeGPU();
    }
    // ------------------------------------------------------------------------------
    //  150ms rest
    //  Reset input_intensity to INPUT_INTENSITY_INIT and update sample ID
    // ------------------------------------------------------------------------------
    else
    {
      if (input_intensity >= 32)
      {
        cout << "imageNum without " << NUM_SPIKE_RESP << "-spike-response: " << (imageNum + 1) << endl;
      }
      if (!testDataEvaluateMode)
        train_result_monitor[imageNum % NUM_TRAIN_SAMPLES].assign(spike_record.begin(), spike_record.end()); 
      else
      {
        test_result_monitor[imageNum % NUM_TEST_SAMPLES].assign(spike_record.begin(), spike_record.end()); 
        cla_result_monitor[imageNum % NUM_TEST_SAMPLES].assign(cla_spike_record.begin(), cla_spike_record.end());
      }
      if ((imageNum + 1) % 100 == 0 && imageNum > 0) 
      {
        if (!testDataEvaluateMode)
          cout << "Training data - runs done: " << (imageNum + 1) << " -> " << NUM_SAMPLES << "; input_intensity: " << input_intensity << endl;
        else
          cout << "Test data for " << imageNum_train << "  - runs done: " << (imageNum + 1) << " -> " << num_test_samples << "; input_intensity: " << input_intensity << endl;
      }

      reset_ratesPCla(cla_FR_khz);
      reset_ratesPPoi(FR_khz);// reset the FR_khz to 0, i.e., no spikes input
      for (int dt = 0; dt < REST_TIME / DT; dt++)// reset the networks for 150ms
      {
        stepTimeGPU();
      }

      if ((imageNum + 1) % UPDATE_INTERVAL == 0 && imageNum > 0 && !testDataEvaluateMode) 
      {
        CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERRORS(cudaMemcpy(thetaPExc, d_thetaPExc, NExc * sizeof(float), cudaMemcpyDeviceToHost));

        string fileid = "_" + to_string((imageNum + 1) / UPDATE_INTERVAL);
        save_gP2E("gP2E" + fileid);
        save_gE2C("gE2C" + fileid);
        save_theta("theta" + fileid);
        // write_result_monitor_to_file(test_result_monitor);
#ifdef PLOT_ON
        get_variance(variance, gP2E, NExc, NPoi, NORMAL);
        write_variance_to_file(variance, imageNum / UPDATE_INTERVAL);
        varianceP2E.update();
        varianceP2E_distribution.update();

        // get_variance(variance_E2C, gE2C, NExc, NCla, Cla_NORMAL);
        get_variance(variance_E2C, gE2C, NCla, NExc, Cla_NORMAL);
        write_variance_gEC_to_file(variance_E2C, imageNum / UPDATE_INTERVAL);
        varianceE2C.update();
        varianceE2C_distribution.update();

        CHECK_CUDA_ERRORS(cudaMemcpy(gP2E, d_gP2E, size_gP2E * sizeof(float), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
        get_visual_PEw(PEVisual);
        write_visual_PEw_to_file(PEVisual);
        PEw.update();
        get_visual_ECw(ECVisual);
        write_visual_ECw_to_file(ECVisual);
        ECw.update();

        write_vector_to_file(response_rate, 60, "response_rate.dat");
        response_rates.update();

        CHECK_CUDA_ERRORS(cudaMemcpy(thetaPExc, d_thetaPExc, size_thetaPExc * sizeof(float), cudaMemcpyDeviceToHost));
        theta.assign(75, 0);
        for (int i = 0; i < size_thetaPExc; i++)
          if ((int)(thetaPExc[i] / 1) < 75)
            theta[(int)(thetaPExc[i] / 1)] += 1;
        write_vector_to_file(theta, 75, "theta.dat");
        thetas.update();
#endif
      }

      input_intensity = INPUT_INTENSITY_INIT; // Reset input_intensity to INPUT_INTENSITY_INIT
      imageNum += 1;// update sample ID
    }
  }
  //------------------------------------------------------------------------------
  // save & plot
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
    PEw.update();
    get_visual_ECw(ECVisual);
    write_visual_ECw_to_file(ECVisual);
    ECw.update();
    get_variance(variance, gP2E, NExc, NPoi, NORMAL);
    write_variance_to_file(variance, imageNum / UPDATE_INTERVAL);
    varianceP2E.update();
    varianceP2E_distribution.update();
    // get_variance(variance, gE2C, NExc, NCla, Cla_NORMAL);
    get_variance(variance_E2C, gE2C, NCla, NExc, Cla_NORMAL);
    write_variance_gEC_to_file(variance_E2C, imageNum / UPDATE_INTERVAL);
    varianceE2C.update();
    varianceE2C_distribution.update();
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
  cudaFreeHost(ratesPCla); 
  CHECK_CUDA_ERRORS(cudaFree(ratesPCla));

  // delete[] CPUratesPPoi;
  cudaFreeHost(ratesPPoi); 
  CHECK_CUDA_ERRORS(cudaFree(ratesPPoi));

  fprintf(stdout, "#training end  ... \n");

  // cin.get();
  // exitGeNN();

  //  timer
  time_t t_end = time(NULL); 
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
void get_rand_g(float *p, long int n, int g_max) 
{
  long int i;                  
  srand((unsigned)time(NULL)); 
  for (i = 0; i < n; i++)
    p[i] = rand() % g_max / 1000.0; 
}
void get_rand(uint64_t *p, long int n, int max)
{
  long int i;
  srand((unsigned)time(NULL));
  for (i = 0; i < n; i++)
    p[i] = (uint64_t)(rand() % (max)); 
}
void get_inputdata(string path, vector<vector<float>> &images, vector<float> &labels, vector<vector<float>> &images_test, vector<float> &labels_test)
{
  if (DATA == "fashion-mnist/")
  {
    cout << "**********fashion-mnist**********" << endl;
    if (DATASET_TRAIN)
    {
      read_mnist_images(path + "train-images-idx3-ubyte_normalized_to30", images);
      read_mnist_label(path + "train-labels-idx1-ubyte", labels);

      read_mnist_images(path + "t10k-images-idx3-ubyte_normalized_to30", images_test);
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
      read_mnist_images(path + "train-images-idx3-ubyte", images);
      read_mnist_label(path + "train-labels-idx1-ubyte", labels);

      read_mnist_images(path + "t10k-images-idx3-ubyte", images_test);
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
  float *g_EI_array = new float[preN_EI * postN_EI]; 
  float *g_IE_array = new float[preN_IE * postN_IE]; 
  //E——>I
  for (int i_dense = 0; i_dense < preN_EI; i_dense++)
    for (int j_dense = 0; j_dense < postN_EI; j_dense++)
    {
      if (i_dense == j_dense)
        g_EI_array[i_dense * postN_EI + j_dense] = g_EI;
      else
        g_EI_array[i_dense * postN_EI + j_dense] = 0.0; 
    }
  //I——>E
  for (int i_dense = 0; i_dense < preN_IE; i_dense++) 
    for (int j_dense = 0; j_dense < postN_IE; j_dense++)
    {
      if (i_dense == j_dense)
        g_IE_array[i_dense * postN_IE + j_dense] = 0.0; 
      else
        g_IE_array[i_dense * postN_IE + j_dense] = g_IE;
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
  fill_n(sTPCla, NCla, -10); //last spike time
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
  convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi); 
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice)); 
}
void Cla_feed_to_networks(int label, vector<float> &cla_FR_khz, float cla_input_intensity)
{
  static uint64_t *CPUratesPCla = new uint64_t[NCla];
  cla_FR_khz.assign(NCla, 0);
  cla_FR_khz[label] = 1.0 * cla_input_intensity;
  convertRateToRandomNumberThreshold(cla_FR_khz, CPUratesPCla, NCla);                                      
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice)); 
  CHECK_CUDA_ERRORS(cudaMemcpy(gE2C, d_gE2C, size_gE2C * sizeof(float), cudaMemcpyDeviceToHost));
  cla_normalize_weights(gE2C);                                                                    
  CHECK_CUDA_ERRORS(cudaMemcpy(d_gE2C, gE2C, size_gE2C * sizeof(float), cudaMemcpyHostToDevice)); 
}
void reset_ratesPPoi(vector<float> &FR_khz)
{
  static uint64_t *CPUratesPPoi = new uint64_t[NPoi];
  FR_khz.assign(NPoi, 0);
  convertRateToRandomNumberThreshold(FR_khz, CPUratesPPoi, NPoi);
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPPoi, CPUratesPPoi, NPoi * sizeof(uint64_t), cudaMemcpyHostToDevice)); 
}
void reset_ratesPCla(vector<float> &cla_FR_khz)
{
  static uint64_t *CPUratesPCla = new uint64_t[NCla];
  cla_FR_khz.assign(NCla, 0);
  convertRateToRandomNumberThreshold(cla_FR_khz, CPUratesPCla, NCla);
  CHECK_CUDA_ERRORS(cudaMemcpy(ratesPCla, CPUratesPCla, NCla * sizeof(uint64_t), cudaMemcpyHostToDevice)); 
}
