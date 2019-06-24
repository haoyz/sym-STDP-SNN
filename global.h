#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// #define TEST_MODE
#define ID_DEVICE 0 //-1 // 选择GPU，-1表示AUTO_CHOOSE
// #define FILE_RECODER
#define SPIKES_MONITOR
#define TRAIN_LAYER_BY_LAYER
#define READ_gPE_FROM_FILE
// // #define SVM_TRAIN
#define PLOT_ON

// #define DATA "caltech/"
// #define WIDTH 240
// #define HEIGHT 160
// #define NUM_CLASS 2
// #define DATA "mnist/"
#define DATA "fashion-mnist/"
#define WIDTH 28
#define HEIGHT 28
#define NUM_CLASS 10
// #define DATA "cifar10/"
// #define GRAYSCALE true
#define NPoi (WIDTH * HEIGHT)
#define N_SQRT 20
#define NExc (N_SQRT * N_SQRT)
#define NExc_Sqrt N_SQRT
#define NInh NExc
#define NInh_Sqrt N_SQRT
#define NCla NUM_CLASS
#define ECw_X NExc
#define ECw_Y NCla
#define RUN_TIME 350
#define REST_TIME 150
#define NUM_SPIKE_RESP 5 //20

#define LABELS_CONST 10000
#define NUM_SAMPLES 60000 * 15
#define UPDATE_INTERVAL 10000 //有时候想自动化结果反而困难了
#define EVALUATION_INTERVAL 10000
// #define UPDATE_INTERVAL NUM_TRAIN_SAMPLES
// #define EVALUATION_INTERVAL NUM_TEST_SAMPLES
#define UPDATE_TOTAL (NUM_SAMPLES / UPDATE_INTERVAL)
#define TEST_MODE_NEURON 0
#define DATASET_TRAIN 1
#define NUM_TRAINING_SL_INI NUM_SAMPLES * 0 //0.5 // Starting Time for Supervised Learning (SL)

#endif
