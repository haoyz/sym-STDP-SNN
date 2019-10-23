#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// #define TEST_MODE
#define ID_DEVICE 0 //-1 // choose GPU. '-1' represents 'AUTO_CHOOSE'
// #define FILE_RECODER
// #define SPIKES_MONITOR
#define FIXED_HIDDEN_LAYER
// // #define SVM_TRAIN
#define PLOT_ON

/***setting for mnist***/
#define DATA "mnist/"
/***setting for fashion-mnist***/
// #define DATA "fashion-mnist/"

#define WIDTH 28
#define HEIGHT 28
#define NUM_CLASS 10

#define NPoi (WIDTH * HEIGHT)

/***parameters of 10000 excitatory neurons for mnist***/
// #define N_SQRT 100
/***parameters of 6400 excitatory neurons for mnist***/
// #define N_SQRT 80
/***parameters of 1600 excitatory neurons for mnist***/
// #define N_SQRT 40
/***parameters of 400 excitatory neurons for mnist***/
#define N_SQRT 20
/***parameters of 100 excitatory neurons for mnist***/
// #define N_SQRT 10
/***parameters of 6400 excitatory neurons for fashion-mnist***/
// #define N_SQRT 80
/***parameters of 400 excitatory neurons for fashion-mnist***/
// #define N_SQRT 20

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

#define NUM_TRAIN_SAMPLES 60000
#define NUM_TEST_SAMPLES 10000
#define NUM_SAMPLES 60000 * 20
#define UPDATE_INTERVAL 10000 
#define EVALUATION_INTERVAL 10000
#define UPDATE_TOTAL (NUM_SAMPLES / UPDATE_INTERVAL)
#define TEST_MODE_NEURON 0
#define DATASET_TRAIN 1
#define NUM_TRAINING_SL_INI NUM_SAMPLES * 0 //0.5 // Starting Time for Supervised Learning (SL)

#endif
