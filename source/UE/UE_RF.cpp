/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_RF.cpp
    \brief UE RF实现文件
    内容 ：UE_C类RF相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! UE RF处理过程：提前接收、加热噪声、RC滤波、ADC、AGC、AFC
/*!
  \param Channel ： 连接的无线信道，输入
  \return 无返回值
*/
void UE_C::RF(Channel_C &Channel) {
  int ue_cell_idx = 0;
  int r = 0;
  int s = 0;
  cpxf w = 0.0f;
  float RSSI = 0.0f;
  int RSSI_Len = 0;
  float AGC_Factor = 0.0f;
  int TA_prime = 0;
  int fir_idx = 0;
  int offset = 0;
#if IDEAL_H_METHOD > 0
  int l = 0;
  int s_prime = 0;
  int s_prime_prime = 0;
  int s_prime_prime_prime = 0;
#endif
  if (MaxOverSample_DataLen > 0) {
    TA_prime = round((float)(TA * Cell_Specific[0].OverSample_N_FFT) / 2048.0f);
    offset = RC_FIR_LEN >> 1;
    for (r = 0; r < UE_Specific[0].RxNum; r++) {
      Wire_Signal.row(r).segment(0, RC_FIR_LEN + MaxOverSample_DataLen) = Channel.RxData.row(r).segment(TA_prime + MAXTASAMPLES - offset, RC_FIR_LEN + MaxOverSample_DataLen);
    }
    if (MidCorr == UE_Specific[0].ChanCorrInd || HighCorr == UE_Specific[0].ChanCorrInd) {
      if (2 == UE_Specific[0].RxNum) {
        for (s = 0; s < (RC_FIR_LEN + MaxOverSample_DataLen); s++) {
          Wire_Signal.col(s) = Corr_HIGH_2 * Wire_Signal.col(s);
        }
      } else if (4 == UE_Specific[0].RxNum) {
        for (s = 0; s < (RC_FIR_LEN + MaxOverSample_DataLen); s++) {
          Wire_Signal.col(s) = Corr_HIGH_4 * Wire_Signal.col(s);
        }
      }
#if IDEAL_H_METHOD > 0
      if (2 == UE_Specific[0].RxNum) {
        for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
          for (s = 0; s < (Cell_Specific[ue_cell_idx].OverSample_N_FFT * Cell_Specific[ue_cell_idx].DL_Symb_Num); s++) {
            Channel.Channel_Specific[ue_cell_idx].Ideal_h[s].block(0, 0, 2, Channel.Channel_Specific[ue_cell_idx].TxNum) = Corr_HIGH_2 * Channel.Channel_Specific[ue_cell_idx].Ideal_h[s].block(0, 0, 2, Channel.Channel_Specific[ue_cell_idx].TxNum);
          }
        }
      } else if (4 == UE_Specific[0].RxNum) {
        for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
          for (s = 0; s < (Cell_Specific[ue_cell_idx].OverSample_N_FFT * Cell_Specific[ue_cell_idx].DL_Symb_Num); s++) {
            Channel.Channel_Specific[ue_cell_idx].Ideal_h[s].block(0, 0, 2, Channel.Channel_Specific[ue_cell_idx].TxNum) = Corr_HIGH_4 * Channel.Channel_Specific[ue_cell_idx].Ideal_h[s].block(0, 0, 2, Channel.Channel_Specific[ue_cell_idx].TxNum);
          }
        }
      }
#endif
    }
    for (r = 0; r < UE_Specific[0].RxNum; r++) {
      for (s = 0; s < (RC_FIR_LEN + MaxOverSample_DataLen); s++) {
        Wire_Signal(r, s) += gauss_cpx(0.0f, SQRT_2_RECIP, &seed);
      }
    }
    if (Simuctrl.RC_FIR_Flag) {
      for (r = 0; r < UE_Specific[0].RxNum; r++) {
        for (s = 0; s < MaxOverSample_DataLen; s++) {
          Wire_Signal_FIR(r, s) = RC_FIR_Coef[0] * Wire_Signal(r, offset + s);
          for (fir_idx = 1; fir_idx <= offset; fir_idx++) {
            Wire_Signal_FIR(r, s) += RC_FIR_Coef[fir_idx] * (Wire_Signal(r, offset + s - fir_idx) + Wire_Signal(r, offset + s + fir_idx));
          }
        }
      }
    } else {
      Wire_Signal_FIR.block(0, 0, UE_Specific[0].RxNum, MaxOverSample_DataLen) = Wire_Signal.block(0, offset, UE_Specific[0].RxNum, MaxOverSample_DataLen);
    }
#if IDEAL_H_METHOD > 0
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
      if (Simuctrl.RC_FIR_Flag) {
        for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
          for (s = 0; s < Cell_Specific[ue_cell_idx].OverSample_N_FFT; s++) {
            s_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + s;
            CHE[ue_cell_idx].Ideal_h[s_prime] = RC_FIR_Coef[0] * Channel.Channel_Specific[ue_cell_idx].Ideal_h[s_prime];
            for (fir_idx = 1; fir_idx <= offset; fir_idx++) {
              s_prime_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + (s - fir_idx + Cell_Specific[ue_cell_idx].OverSample_N_FFT) % Cell_Specific[ue_cell_idx].OverSample_N_FFT;
              s_prime_prime_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + (s + fir_idx + Cell_Specific[ue_cell_idx].OverSample_N_FFT) % Cell_Specific[ue_cell_idx].OverSample_N_FFT;
              CHE[ue_cell_idx].Ideal_h[s_prime] += RC_FIR_Coef[fir_idx] * (Channel.Channel_Specific[ue_cell_idx].Ideal_h[s_prime_prime] + Channel.Channel_Specific[ue_cell_idx].Ideal_h[s_prime_prime_prime]);
            }
          }
        }
      } else {
        for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
          for (s = 0; s < Cell_Specific[ue_cell_idx].OverSample_N_FFT; s++) {
            s_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + s;
            s_prime_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + (s + TA_prime + Cell_Specific[ue_cell_idx].OverSample_N_FFT) % Cell_Specific[ue_cell_idx].OverSample_N_FFT;
            CHE[ue_cell_idx].Ideal_h[s_prime] = Channel.Channel_Specific[ue_cell_idx].Ideal_h[s_prime_prime];
          }
        }
      }
    }
#endif
    if (Simuctrl.AGC_Flag) {
      RSSI_Len = MIN(256, MaxOverSample_DataLen);
      for (r = 0; r < UE_Specific[0].RxNum; r++) {
        RSSI = 0.0f;
        for (s = 0; s < RSSI_Len; s++) {
          RSSI += (Wire_Signal_FIR(r, s).real() * Wire_Signal_FIR(r, s).real() + Wire_Signal_FIR(r, s).imag() * Wire_Signal_FIR(r, s).imag());
        }
        if (((float)Simuctrl.ADC_AVEPower * (float)RSSI_Len) > RSSI) {
          AGC[r] = Log2(round(((float)Simuctrl.ADC_AVEPower * (float)RSSI_Len) / RSSI)) / 2;
          AGC_Factor = (float)(1 << AGC[r]);
        } else {
          AGC[r] = -CeilLog2(round(RSSI / ((float)Simuctrl.ADC_AVEPower * (float)RSSI_Len))) / 2;
          AGC_Factor = 1.0f / (float)(1 << (-AGC[r]));
        }
        for (s = 0; s < MaxOverSample_DataLen; s++) {
          Wire_Signal_FIR(r, s).real((float)round(Wire_Signal_FIR(r, s).real() * AGC_Factor));
          Wire_Signal_FIR(r, s).imag((float)round(Wire_Signal_FIR(r, s).imag() * AGC_Factor));
          if (Wire_Signal_FIR(r, s).real() > Simuctrl.ADC_MAX) {
            Wire_Signal_FIR(r, s).real(Simuctrl.ADC_MAX);
          } else if (Wire_Signal_FIR(r, s).real() < Simuctrl.ADC_MIN) {
            Wire_Signal_FIR(r, s).real(Simuctrl.ADC_MIN);
          }
          if (Wire_Signal_FIR(r, s).imag() > Simuctrl.ADC_MAX) {
            Wire_Signal_FIR(r, s).imag(Simuctrl.ADC_MAX);
          } else if (Wire_Signal_FIR(r, s).imag() < Simuctrl.ADC_MIN) {
            Wire_Signal_FIR(r, s).imag(Simuctrl.ADC_MIN);
          }
        }
#if IDEAL_H_METHOD > 0
        for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
          for (s = 0; s < (Cell_Specific[ue_cell_idx].OverSample_N_FFT * Cell_Specific[ue_cell_idx].DL_Symb_Num); s++) {
            CHE[ue_cell_idx].Ideal_h[s].row(r) = AGC_Factor * CHE[ue_cell_idx].Ideal_h[s].row(r);
          }
        }
#endif
      }
    }
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
      if (ServingCell == UE_Specific[ue_cell_idx].CellType) {
        if (0.0f != Feedback[ue_cell_idx].RxFreqOffset) {
          Phase[ue_cell_idx] += (double)Feedback[ue_cell_idx].RxFreqOffset * (double)(Simuctrl.n_subframe - last_n_subframe[ue_cell_idx] - 1) * 0.001;
#if IDEAL_H_METHOD > 0
          for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
            w = MyExpCpx_2PI((float)(Phase[ue_cell_idx] + (double)Feedback[ue_cell_idx].RxFreqOffset * (double)(IdealSamplePos[ue_cell_idx][l] + TA_prime) * Cell_Specific[ue_cell_idx].OverSample_unit));
            for (s = 0; s < Cell_Specific[ue_cell_idx].OverSample_N_FFT; s++) {
              s_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + s;
              CHE[ue_cell_idx].Ideal_h[s_prime] = w * CHE[ue_cell_idx].Ideal_h[s_prime];
            }
          }
#endif
          for (s = 0; s < MaxOverSample_DataLen; s++) {
            Phase[ue_cell_idx] += (double)Feedback[ue_cell_idx].RxFreqOffset * Cell_Specific[ue_cell_idx].OverSample_unit;
            w = MyExpCpx_2PI((float)Phase[ue_cell_idx]);
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              Cell_Signal[ue_cell_idx](r, s) = w * Wire_Signal_FIR(r, s);
            }
          }
          Phase[ue_cell_idx] += (double)(Cell_Specific[ue_cell_idx].OverSample_Num - MaxOverSample_DataLen) * Cell_Specific[ue_cell_idx].OverSample_unit;
          last_n_subframe[ue_cell_idx] = Simuctrl.n_subframe;
        } else {
          Cell_Signal[ue_cell_idx].block(0, 0, UE_Specific[ue_cell_idx].RxNum, MaxOverSample_DataLen) = Wire_Signal_FIR.block(0, 0, UE_Specific[ue_cell_idx].RxNum, MaxOverSample_DataLen);
        }
      }
    }
  }
}
