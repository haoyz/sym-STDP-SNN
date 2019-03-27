#ifndef _FILE_RECORDER_H_
#define _FILE_RECORDER_H_

#define showGPU()                                                                     \
  {                                                                                   \
    os_poi << t << " ";                                                               \
    os_exc << t << " ";                                                               \
    os_inh << t << " ";                                                               \
    os_t2N_poi << t << " ";                                                           \
    os_t2N_exc << t << " ";                                                           \
    os_t2N_inh << t << " ";                                                           \
    os_visual << t << " ";                                                            \
    for (int j = 0; j < 1; j++)                                                       \
    {                                                                                 \
      os_poi << VPPoi[j] << " ";                                                      \
      /*os_poi << theRndPPoi[j] << " ";                                               \
      os_poi << CPUratesPPoi[j] << " ";                                               \
      os_poi << spikeTimePPoi[j] << " ";   */                                         \
      os_poi << endl;                                                                 \
                                                                                      \
      os_exc << VPExc[j] << " ";                                                      \
      /*os_exc << inSynP2E[j] * ((0.00000f) - VPExc[j]) << " ";                       \
      os_exc << inSynI2E[j] * ((-100.000f) - VPExc[j]) << " ";                        \
      os_exc << inSynE2I[j] * ((0.000f) - VPInh[j]) << " ";                           \
      os_exc << inSynP2E[j] << " ";                                                   \
      os_exc << inSynI2E[j] << " ";                                                   \
      os_exc << inSynE2I[j] << " ";                                                   \
      os_exc << thetaPExc[j] << " ";                                                  \
      os_exc << timerPExc[j] << " ";    */                                            \
      os_exc << endl;                                                                 \
                                                                                      \
      os_inh << VPInh[j] << " ";                                                      \
      /*  os_inh << inSynE2I[j] * ((0.00000f) - VPInh[j]) << " ";                     \
        os_inh << inSynE2I[j] << " ";                                                 \
        os_inh << gE2I[j] << " ";   */                                                \
      os_inh << endl;                                                                 \
                                                                                      \
      os_t2N_poi << glbSpkCntPPoi[0] << " ";                                          \
      for (int i = 0; i < glbSpkCntPPoi[0]; i++)                                      \
      {                                                                               \
        os_t2N_poi << glbSpkPPoi[i] << " ";                                           \
      }                                                                               \
      os_t2N_poi << endl;                                                             \
                                                                                      \
      for (int i = 0; i < glbSpkCntPExc[0]; i++)                                      \
      {                                                                               \
        os_t2N_exc << glbSpkPExc[i] << " ";                                           \
      }                                                                               \
      os_t2N_exc << endl;                                                             \
                                                                                      \
      for (int i = 0; i < glbSpkCntPInh[0]; i++)                                      \
      {                                                                               \
        os_t2N_inh << glbSpkPInh[i] << " ";                                           \
      }                                                                               \
      os_t2N_inh << endl;                                                             \
                                                                                      \
      /*if (dt % (int)(20 / DT) == 0)                                                 \
      {                                                                               \
        int i_v = 0;                                                                  \
        int j_v = 0;                                                                  \
        for (int i = 0; i < _NExc; i++)                                               \
        {                                                                             \
          i_v = (i / _NExc_Sqrt) * _NPoi_Sqrt;                                        \
          j_v = (i % _NExc_Sqrt) * _NPoi_Sqrt;                                        \
          for (int j = 0; j < _NPoi; j++)                                             \
          {                                                                           \
            Visual[i_v + j / _NPoi_Sqrt][j_v + j % _NPoi_Sqrt] = gP2E[i + j * _NExc]; \
          }                                                                           \
        }                                                                             \
        os_visual << endl;                                                            \
        for (int i = 0; i < _NPoi_Sqrt * _NExc_Sqrt; i++)                             \
        {                                                                             \
          for (int j = 0; j < _NPoi_Sqrt * _NExc_Sqrt; j++)                           \
          {                                                                           \
            os_visual << Visual[i][j] << " ";                                         \
          }                                                                           \
          os_visual << endl;                                                          \
        }                                                                             \
      }  */                                                                           \
    }                                                                                 \
  }

#endif