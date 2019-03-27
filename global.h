#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// #define test_mode
#define ID_device 0 //-1 // 选择GPU，-1表示autochoose
// #define _file_recoder
// #define _spikes_monitor
#define _train_layer_by_layer
// #define _gPE_read_from_file
// // #define svm_train

// #define DATA "mnist/"
#define DATA "fashion-mnist/"
// #define DATA "cifar10/"
// #define GRAYSCALE true
#define _N_Sqrt 10
#define _N (_N_Sqrt * _N_Sqrt)
#define _NExc _N
#define _NExc_Sqrt _N_Sqrt
#define _NInh _NExc
#define _NInh_Sqrt _N_Sqrt
#define _NPoi 784
#define _NPoi_Sqrt 28
#define _NUM_TRAINING_EXAMPLES 60000
#define _NCla 10
#define _ECw_X _NExc
#define _ECw_Y _NCla
// #define PLOT_ON
#define _RunTime_singleImg 350 //ms
#define _RestingTime 150       //ms
#define _NUM_TESTING_EXAMPLES 10000
#define _performance_update_interval 10000
#define auto_choose 0                                   //0表示根据网络大小调整样本集大小，否则接受指定样本集大小
#define _NUM_TRAINING_SL_ini _NUM_TRAINING_EXAMPLES * 0 // Starting Time for Supervised Learning (SL) - revised at 2017-10-16 16:50//revised at 2018-04-14
#define NUM_SPIKE_RESP 5                                //20

#define _LABELS_CONST 10000
#define _NUM_EXAMPLES auto_choose //
#define test_mode_neuron 0
#define _Dataset_train 1

#endif
