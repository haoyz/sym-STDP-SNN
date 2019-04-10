#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// #define test_mode
#define ID_device 0 //-1 // 选择GPU，-1表示autochoose
// #define FILE_RECODER
// #define SPIKES_MONITOR
#define TRAIN_LAYER_BY_LAYER
// #define READ_gPE_FROM_FILE
// // #define svm_train
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
#define N_SQRT 10
#define NExc (N_SQRT * N_SQRT)
#define NExc_Sqrt N_SQRT
#define NInh NExc
#define NInh_Sqrt N_SQRT
#define NCla NUM_CLASS
#define _ECw_X NExc
#define _ECw_Y NCla
#define RUN_TIME 350     //ms
#define REST_TIME 150    //ms
#define auto_choose 0    //0表示根据网络大小调整样本集大小，否则接受指定样本集大小
#define NUM_SPIKE_RESP 5 //20

#define _LABELS_CONST 10000
#define NUM_SAMPLES 60000 * 3
#define test_mode_neuron 0
#define _Dataset_train 1
#define _NUM_TRAINING_SL_ini NUM_SAMPLES * 0 //0.5 // Starting Time for Supervised Learning (SL)

#endif
