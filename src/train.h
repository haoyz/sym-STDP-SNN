#ifndef _TRAIN_H_
#define _TRAIN_H_

#include "../global.h"
#include <stddef.h> //vscode下必须有

#define DT 0.500000f
#define N_rand 256 //精度。

#define _preN_EI _N
#define _postN_EI _N
#define _preN_IE _N
#define _postN_IE _N
#define _connN_E2I _N           //一对一
#define _connN_I2E _N *(_N - 1) //一对多

#define START_INPUT_INTENSITY 2
#define _start_cla_input_intensity 0.1 // 10 //khz //revised at 2018-04-27 //revised at 2018-4-14

#define LastTrain 0 //revised at 2018-4-14
#define NowTrain 1
#define NowTest 2
#define FinalTest 0

#define UNSUPERVISED 0 //revised at 2018-5-13
#define SUPERVISED 1

extern long int imageNum;
#endif