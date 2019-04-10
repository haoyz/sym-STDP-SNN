#ifndef _FILE_RECORDER_H_
#define _FILE_RECORDER_H_

#define showGPU()                                                                                                 \
  {                                                                                                               \
    os_poi << t << " ";                                                                                           \
    os_exc << t << " ";                                                                                           \
    os_inh << t << " ";                                                                                           \
    os_t2N_poi << t << " ";                                                                                       \
    os_t2N_exc << t << " ";                                                                                       \
    os_t2N_inh << t << " ";                                                                                       \
    os_visual << t << " ";                                                                                        \
    for (int j = 0; j < 1; j++)                                                                                   \
    {                                                                                                             \
      os_poi << VPPoi[j] << " ";                                                                                  \
      /*os_poi << theRndPPoi[j] << " ";                                                                           \
      os_poi << CPUratesPPoi[j] << " ";                                                                           \
      os_poi << spikeTimePPoi[j] << " ";   */                                                                     \
      os_poi << endl;                                                                                             \
                                                                                                                  \
      os_exc << VPExc[j] << " ";                                                                                  \
      /*os_exc << inSynP2E[j] * ((0.00000f) - VPExc[j]) << " ";                                                   \
      os_exc << inSynI2E[j] * ((-100.000f) - VPExc[j]) << " ";                                                    \
      os_exc << inSynE2I[j] * ((0.000f) - VPInh[j]) << " ";                                                       \
      os_exc << inSynP2E[j] << " ";                                                                               \
      os_exc << inSynI2E[j] << " ";                                                                               \
      os_exc << inSynE2I[j] << " ";                                                                               \
      os_exc << thetaPExc[j] << " ";                                                                              \
      os_exc << timerPExc[j] << " ";    */                                                                        \
      os_exc << endl;                                                                                             \
                                                                                                                  \
      os_inh << VPInh[j] << " ";                                                                                  \
      /*  os_inh << inSynE2I[j] * ((0.00000f) - VPInh[j]) << " ";                                                 \
        os_inh << inSynE2I[j] << " ";                                                                             \
        os_inh << gE2I[j] << " ";   */                                                                            \
      os_inh << endl;                                                                                             \
                                                                                                                  \
      os_t2N_poi << glbSpkCntPPoi[0] << " ";                                                                      \
      for (int i = 0; i < glbSpkCntPPoi[0]; i++)                                                                  \
      {                                                                                                           \
        os_t2N_poi << glbSpkPPoi[i] << " ";                                                                       \
      }                                                                                                           \
      os_t2N_poi << endl;                                                                                         \
                                                                                                                  \
      for (int i = 0; i < glbSpkCntPExc[0]; i++)                                                                  \
      {                                                                                                           \
        os_t2N_exc << glbSpkPExc[i] << " ";                                                                       \
      }                                                                                                           \
      os_t2N_exc << endl;                                                                                         \
                                                                                                                  \
      for (int i = 0; i < glbSpkCntPInh[0]; i++)                                                                  \
      {                                                                                                           \
        os_t2N_inh << glbSpkPInh[i] << " ";                                                                       \
      }                                                                                                           \
      os_t2N_inh << endl;                                                                                         \
                                                                                                                  \
      /*if (dt % (int)(20 / DT) == 0)                                                                             \
      {                                                                                                           \
        int i_v = 0;                                                                                              \
        int j_v = 0;                                                                                              \
        for (int i = 0; i < NExc; i++)                                                                            \
        {                                                                                                         \
          i_v = (i / NExc_Sqrt) * WIDTH;//怎么排由我决定，认为一列一列排，i是列数，j是行数 \
          j_v = (i % NExc_Sqrt) * HEIGHT;                                                                         \
          for (int j = 0; j < NPoi; j++)                                                                          \
          {                                                                                                       \
            Visual[i_v + j / HEIGHT][j_v + j % WIDTH] = gP2E[i + j * NExc];                                       \
          }                                                                                                       \
        }                                                                                                         \
        os_visual << endl;                                                                                        \
        for (int i = 0; i < WIDTH * NExc_Sqrt; i++)                                                               \
        {                                                                                                         \
          for (int j = 0; j < HEIGHT * NExc_Sqrt; j++)                                                            \
          {                                                                                                       \
            os_visual << Visual[i][j] << " ";                                                                     \
          }                                                                                                       \
          os_visual << endl;                                                                                      \
        }                                                                                                         \
      }  */                                                                                                       \
    }                                                                                                             \
  }

#endif