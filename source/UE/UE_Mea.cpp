/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_Mea.cpp
    \brief UE测量实现文件
    内容 ：UE_C类测量相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! 噪声功率是否做Alpha滤波，宏开关，0：关闭；1：打开
#define NOISE_ALPHA_FLAG 0
//! 噪声功率做Alpha滤波的系数
const float noise_alpha = 0.25f;
//! 多普勒扩展做Alpha滤波的系数
const float fd_alpha = 0.25f;

//! 基于CRS的第一次测量：信号功率、噪声功率、SINR、时偏、频偏、多普勒扩展、高铁标志、时域相关值、频域相关值
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CRS_Measurement_1st(int ue_cell_idx) {
  int i = 0;
  int j = 0;
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int PDP_Len = 0;
  float ave_factor = 0.0f;
  float acc_num = 0.0f;
  float power_scale = 0.0f;
  float max_value = 0.0f;
  int max_idx = 0;
  float Power_Acc = 0.0f;
  fvec Noise_Sample;
  int MaxPDPs_Num = 0;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  fvec PDP_tmp;
  cpxf temp = 0.0f;
  float ds = 0.0f;
  float ceta = 0.0f;
  float ceta_0 = 0.0f;
  float ceta_1 = 0.0f;
  ivec N_Segment;
  float PDP_Th = 0.0f;
  cfmats h_ifft;
  cfmats h_ifft_max;
  cfvec h_ifft_FOE;
  cpxf corr_0 = 0.0f;
  cpxf corr_1 = 0.0f;
  float corr_sqr = 0.0f;
  float corr_sqr_plus = 0.0f;
  fmat ceta_maxs;
  fmat P_maxs;
  cpxf AveValue = 0.0f;
  float Power_Ave = 0.0f;
  float Power_LOS = 0.0f;
  float Power_NLOS = 0.0f;
  int LOS_Flag = 0;
  ivec Max_LOS_Flag;
  fvec Acc_Power_LOS;
  fvec Acc_Power_NLOS;
  float R_tao0 = 0.0f;
  float R_tao0_plus = 0.0f;
  float R_taoa_abs = 0.0f;
  float R_taoa_abs_plus = 0.0f;
  cpxf R_taoa = 0.0f;
  cfvec fft_in_R_F;
  cfvec fft_out_R_F;
  float Acc_ceta = 0.0f;
  float Acc_P = 0.0f;
  ivec tmp_HST_SFN_Flag;
  int tmp_Flag = 0;
  bool Last_Flag = false;
  float delta_t = 0.0f;
  cfmat tmp_R_T;
  float Offset = 0.0f;
  float AGC_Factor = 0.0f;
  int TA_prime = 0;
  float fd_shift_tmp = 0.0f;
  float fd_tmp = 0.0f;
  if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
    //PDP_Len = 1 << CeilLog2(2 * Cell_Specific[ue_cell_idx].N_DL_RB);
    PDP_Len = (1536 == Cell_Specific[ue_cell_idx].N_FFT) ? 256 : (Cell_Specific[ue_cell_idx].N_FFT / 128 * 21);
    power_scale = MySqrt(PDP_Len);
    PDP_tmp.resize(PDP_Len);
    MaxPDPs_Num = MAXPDPSNUM;
    fft_in.resize(PDP_Len);
    fft_out.resize(PDP_Len);
    fft_in_R_F.resize(Cell_Specific[ue_cell_idx].N_FFT);
    fft_out_R_F.resize(Cell_Specific[ue_cell_idx].N_FFT);
    Noise_Sample.resize(UE_Specific[ue_cell_idx].RxNum);
    N_Segment.resize(UE_Specific[ue_cell_idx].RxNum);
    h_ifft.resize(UE_Specific[ue_cell_idx].RxNum * 2);
    h_ifft_max.resize(UE_Specific[ue_cell_idx].RxNum * 2);
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * 2); i++) {
      h_ifft[i].resize(CHE[ue_cell_idx].CRS_Num[0], PDP_Len);
      h_ifft_max[i].resize(CHE[ue_cell_idx].CRS_Num[0], MaxPDPs_Num);
    }
    h_ifft_FOE.resize(CHE[ue_cell_idx].CRS_Num[0]);
    ceta_maxs.resize(UE_Specific[ue_cell_idx].RxNum, MaxPDPs_Num);
    P_maxs.resize(UE_Specific[ue_cell_idx].RxNum, MaxPDPs_Num);
    Acc_Power_LOS.resize(UE_Specific[ue_cell_idx].RxNum);
    Acc_Power_LOS.setZero();
    Acc_Power_NLOS.resize(UE_Specific[ue_cell_idx].RxNum);
    Acc_Power_NLOS.setZero();
    Max_LOS_Flag.resize(UE_Specific[ue_cell_idx].RxNum);
    Max_LOS_Flag.setZero();
    tmp_HST_SFN_Flag.resize(UE_Specific[ue_cell_idx].RxNum);
    tmp_HST_SFN_Flag.setZero();
    tmp_R_T.resize(UE_Specific[ue_cell_idx].RxNum, R_T_Num);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //PDP
      ave_factor = 0;
      for (i = 0; i < PDP_Len; i++) {
        MEA[ue_cell_idx].CRS_PDP(r, i) = 0.0f;
      }
      for (p = 0; p < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2); p++) {
        for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
          fft_in.setZero();
          fft_in.segment(0, 2 * Cell_Specific[ue_cell_idx].N_DL_RB) = CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p].col(l_idx);
          fft.inv(fft_out, fft_in);
          fft_out *= power_scale;
          h_ifft[r * 2 + p].row(l_idx) = fft_out;
          for (i = 0; i < PDP_Len; i++) {
            MEA[ue_cell_idx].CRS_PDP(r, i) += (fft_out(i).real() * fft_out(i).real() + fft_out(i).imag() * fft_out(i).imag());
          }
          ave_factor += 1.0f;
        }
      }
      ave_factor = 1.0f / ave_factor / (float)(2.0f * Cell_Specific[ue_cell_idx].N_DL_RB);
      for (i = 0; i < PDP_Len; i++) {
        MEA[ue_cell_idx].CRS_PDP(r, i) *= ave_factor;
      }
      /*Power_Acc = 0.0f;
            for (i = 0; i < PDP_Len; i++)
               {
                   Power_Acc += MEA[ue_cell_idx].CRS_PDP(r, i);
               }
            for (i = 0; i < PDP_Len; i++)
               {
                   MEA[ue_cell_idx].CRS_PDP(r, i) /= Power_Acc;
               }*/
      //PDP Alpha Filter
      if (!MEA[ue_cell_idx].CRS_PDP_Alpha_1stFlag) {
        if ((AGC[r] - Last_AGC[r]) >= 0) {
          AGC_Factor = (float)(1 << (AGC[r] - Last_AGC[r]));
        } else {
          AGC_Factor = 1.0f / (float)(1 << (Last_AGC[r] - AGC[r]));
        }
        TA_prime = -round((float)(TA * Cell_Specific[ue_cell_idx].N_FFT) / 2048.0f) + round((float)(MEA[ue_cell_idx].CRS_PDP_lastTA * Cell_Specific[ue_cell_idx].N_FFT) / 2048.0f) + PDP_Len;
        for (i = 0; i < PDP_Len; i++) {
          MEA[ue_cell_idx].CRS_PDP(r, i) = MEA[ue_cell_idx].CRS_PDP_Alpha(r) * MEA[ue_cell_idx].CRS_PDP(r, i) + (1.0f - MEA[ue_cell_idx].CRS_PDP_Alpha(r)) * AGC_Factor * MEA[ue_cell_idx].CRS_PDP_last(r, (i + TA_prime) % PDP_Len);
        }
      }
      for (i = 0; i < PDP_Len; i++) {
        MEA[ue_cell_idx].CRS_PDP_last(r, i) = MEA[ue_cell_idx].CRS_PDP(r, i);
      }
      for (i = 0; i < PDP_Len; i++) {
        PDP_tmp(i) = MEA[ue_cell_idx].CRS_PDP(r, i);
      }
      //Find Max PDPs
      for (j = 0; j < MaxPDPs_Num; j += 3) {
        max_value = 0.0f;
        max_idx = 0;
        for (i = 0; i < PDP_Len; i++) {
          if (PDP_tmp(i) > max_value) {
            max_value = PDP_tmp(i);
            max_idx = i;
          }
        }
        MEA[ue_cell_idx].MaxPDPs_Idx(r, j) = max_idx;
        MEA[ue_cell_idx].MaxPDPs_Value(r, j) = max_value;
        MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 1) = (max_idx - 1 + PDP_Len) % PDP_Len;
        MEA[ue_cell_idx].MaxPDPs_Value(r, j + 1) = PDP_tmp(MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 1));
        MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 2) = (max_idx + 1 + PDP_Len) % PDP_Len;
        MEA[ue_cell_idx].MaxPDPs_Value(r, j + 2) = PDP_tmp(MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 2));
        PDP_tmp(MEA[ue_cell_idx].MaxPDPs_Idx(r, j)) = 0.0f;
        PDP_tmp(MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 1)) = 0.0f;
        PDP_tmp(MEA[ue_cell_idx].MaxPDPs_Idx(r, j + 2)) = 0.0f;
      }
      //Noise Power
      Power_Acc = 0.0f;
      acc_num = 0.0f;
      for (i = (MEA[ue_cell_idx].MaxPDPs_Idx(r, 0) - PDP_Len / 2); i < (MEA[ue_cell_idx].MaxPDPs_Idx(r, 0) - PDP_Len / 4); i++) {
        j = (i + PDP_Len) % PDP_Len;
        Power_Acc += MEA[ue_cell_idx].CRS_PDP(r, j);
        acc_num += 1.0f;
      }
      Noise_Sample(r) = Power_Acc / acc_num;
      Power_Acc = 0.0f;
      acc_num = 0.0f;
      for (i = 0; i < PDP_Len; i++) {
        if (PDP_tmp(i) > 0.0f && PDP_tmp(i) < (2.0f * Noise_Sample(r))) {
          Power_Acc += PDP_tmp(i);
          acc_num += 1.0f;
        }
      }
      if (Power_Acc > 0.0f) {
        Noise_Sample(r) = MIN(Power_Acc / acc_num, Noise_Sample(r));
      }
      Power_Acc = Noise_Sample(r) * (float)PDP_Len;
#if NOISE_ALPHA_FLAG > 0
      if (MEA[ue_cell_idx].CRS_N(r) > 0.0f) {
        MEA[ue_cell_idx].CRS_N(r) = (1.0f - noise_alpha) * MEA[ue_cell_idx].CRS_N(r) + noise_alpha * Power_Acc;
      } else {
        MEA[ue_cell_idx].CRS_N(r) = Power_Acc;
      }
#else
      MEA[ue_cell_idx].CRS_N(r) = Power_Acc;
#endif
      if (0.0f == MEA[ue_cell_idx].CRS_N(r)) {
        MEA[ue_cell_idx].CRS_N(r) = 1.0f;
        Noise_Sample(r) = 1.0f / (float)PDP_Len;
      }
      //Singal Power
      MEA[ue_cell_idx].CRS_S(r) = 0.0f;
      for (i = 0; i < PDP_Len; i++) {
        //MEA[ue_cell_idx].CRS_S(r) += MEA[ue_cell_idx].CRS_PDP(r, i);
        if (MEA[ue_cell_idx].CRS_PDP(r, i) > (2.0f * Noise_Sample(r))) {
          MEA[ue_cell_idx].CRS_S(r) += (MEA[ue_cell_idx].CRS_PDP(r, i) - Noise_Sample(r));
        }
      }
      if (0.0f == MEA[ue_cell_idx].CRS_S(r)) {
        MEA[ue_cell_idx].CRS_S(r) = MEA[ue_cell_idx].CRS_N(r) / ((float)Cell_Specific[ue_cell_idx].N_DL_RB);
      }
      //SNR
      MEA[ue_cell_idx].CRS_SNR(r) = 10.0f * log10f(MEA[ue_cell_idx].CRS_S(r) / MEA[ue_cell_idx].CRS_N(r));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("CRS_Measurement_1st: SNR[%d]=%f, S=%f, N=%f\n", r, MEA[ue_cell_idx].CRS_SNR(r), MEA[ue_cell_idx].CRS_S(r), MEA[ue_cell_idx].CRS_N(r));
      //PDP Alpha Filter
      MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) += (MEA[ue_cell_idx].MaxPDPs_Value(r, 0) / Noise_Sample(r));
      //IRT
      if (MEA[ue_cell_idx].CRS_SNR(r) > 5.0f) {
        temp = 0.0f;
        for (p = 0; p < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2); p++) {
          for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
            for (i = 0; i < Cell_Specific[ue_cell_idx].N_DL_RB - 1; i++) {
              temp += CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](i, l_idx) * conj(CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](i + 1, l_idx));
            }
            for (i = Cell_Specific[ue_cell_idx].N_DL_RB; i < 2 * Cell_Specific[ue_cell_idx].N_DL_RB - 1; i++) {
              temp += CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](i, l_idx) * conj(CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](i + 1, l_idx));
            }
          }
        }
        ceta = arg(temp);
        MEA[ue_cell_idx].IRT[r] = round(ceta * 2048.0f / (12.0f * PI));  //重心位置，以N_FFT=2048为基础，即单位Ts
      } else {
        MEA[ue_cell_idx].IRT[r] = round(MEA[ue_cell_idx].MaxPDPs_Idx(r, 0) * 2048.0f / (6.0f * (float)PDP_Len));
        if (MEA[ue_cell_idx].IRT[r] > 160.0f) {
          MEA[ue_cell_idx].IRT[r] = MEA[ue_cell_idx].IRT[r] - 341;
        }
      }
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("CRS_Measurement_1st: IRT[%d]=%d\n", r, MEA[ue_cell_idx].IRT[r]);
      //N_Segment
      //MEA[ue_cell_idx].IRT[r] = 0;
      N_Segment(r) = round(((float)MEA[ue_cell_idx].IRT[r] - 80.0f) * (6.0f * (float)PDP_Len) / 2048.0f);
      if (N_Segment(r) < 0) {
        N_Segment(r) += PDP_Len;
      } else {
        N_Segment(r) = PDP_Len;
      }
      //R_F
      fft_in_R_F.setZero();
      PDP_Th = MAX(2.0f * Noise_Sample(r), MEA[ue_cell_idx].MaxPDPs_Value(r, 0) / 128.0f);
      for (j = 0; j < MaxPDPs_Num; j++) {
        if (MEA[ue_cell_idx].MaxPDPs_Value(r, j) > PDP_Th || 0 == j) {
          i = round((float)MEA[ue_cell_idx].MaxPDPs_Idx(r, j) * (float)Cell_Specific[ue_cell_idx].N_FFT / (6.0f * (float)PDP_Len));
          if (MEA[ue_cell_idx].MaxPDPs_Idx(r, j) > N_Segment(r)) {
            i = MAX(MIN(i + Cell_Specific[ue_cell_idx].N_FFT - round((float)Cell_Specific[ue_cell_idx].N_FFT / 6.0f), Cell_Specific[ue_cell_idx].N_FFT - 1), 0);
          }
          fft_in_R_F(i).real(MAX(fft_in_R_F(i).real(), MEA[ue_cell_idx].MaxPDPs_Value(r, j)));
          //fft_in_R_F(i).real((fft_in_R_F(i).real() + MEA[ue_cell_idx].MaxPDPs_Value(r, j)));
        }
      }
#if 1 == IDEAL_H_METHOD
      for (i = 0; i < Cell_Specific[ue_cell_idx].N_FFT; i++) {
        fft_in_R_F(i) = MEA[ue_cell_idx].Ideal_PDP(r, i);
      }
#endif
      fft.fwd(fft_out_R_F, fft_in_R_F);
      for (i = 1; i < MIN(R_F_Num, Cell_Specific[ue_cell_idx].N_FFT); i++) {
        MEA[ue_cell_idx].R_F(r, i) = fft_out_R_F(i) / fft_out_R_F(0).real();
      }
      MEA[ue_cell_idx].R_F(r, 0) = 1.0f;
      //Freq Offset & Doppler & HST_SFN_Flag
      if (CHE[ue_cell_idx].CRS_Num[0] >= 3) {
        PDP_Th = MAX(10.0f * Noise_Sample(r), MEA[ue_cell_idx].MaxPDPs_Value(r, 0) / 128.0f);
        for (j = 0; j < MaxPDPs_Num; j++) {
          if (MEA[ue_cell_idx].MaxPDPs_Value(r, j) > PDP_Th || 0 == j) {
            corr_0 = 0.0f;
            corr_1 = 0.0f;
            if (MEA[ue_cell_idx].MaxPDPs_Idx(r, j) > N_Segment(r)) {
              Offset = 2.5f;
            } else {
              Offset = 0.0f;
            }
            for (p = 0; p < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2); p++) {
              h_ifft_max[r * 2 + p](0, j) = h_ifft[r * 2 + p](0, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
              h_ifft_max[r * 2 + p](2, j) = h_ifft[r * 2 + p](2, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
              if (0 == p) {
                h_ifft_max[r * 2 + p](1, j) = MyExpCpx_2PI((float)MEA[ue_cell_idx].MaxPDPs_Idx(r, j) / (2.0 * (float)PDP_Len) + Offset) * h_ifft[r * 2 + p](1, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
                if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                  h_ifft_max[r * 2 + p](3, j) = MyExpCpx_2PI((float)MEA[ue_cell_idx].MaxPDPs_Idx(r, j) / (2.0 * (float)PDP_Len) + Offset) * h_ifft[r * 2 + p](3, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
                }
              } else {
                h_ifft_max[r * 2 + p](1, j) = MyExpCpx_2PI(-(float)MEA[ue_cell_idx].MaxPDPs_Idx(r, j) / (2.0 * (float)PDP_Len) - Offset) * h_ifft[r * 2 + p](1, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
                if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                  h_ifft_max[r * 2 + p](3, j) = MyExpCpx_2PI(-(float)MEA[ue_cell_idx].MaxPDPs_Idx(r, j) / (2.0 * (float)PDP_Len) - Offset) * h_ifft[r * 2 + p](3, MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
                }
              }
              corr_0 += h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](1, j));
              if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j)) + h_ifft_max[r * 2 + p](3, j) * conj(h_ifft_max[r * 2 + p](1, j)));
              } else {
                corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j)));
              }
            }
            ds = (0.0005f / (float)Cell_Specific[ue_cell_idx].N_DL_symb);
            ceta_0 = arg(corr_0) / (2.0f * PI * ds * 3.0f);
            ceta_1 = arg(corr_1) / (2.0f * PI * 0.0005f);
            if ((MyFabs(ceta_0) > 800.0f || (MyFabs(ceta_0) > 500.0f && MyFabs(ceta_0 - ceta_1) > 500.0f)) && MEA[ue_cell_idx].MaxPDPs_Value(r, j) > PDP_Th) {
              ceta_maxs(r, j) = ceta_0;
            } else {
              ceta_maxs(r, j) = ceta_1;
            }
            P_maxs(r, j) = MEA[ue_cell_idx].MaxPDPs_Value(r, j);
            corr_0 = 0.0f;
            corr_1 = 0.0f;
            corr_sqr = 0.0f;
            corr_sqr_plus = 0.0f;
            LOS_Flag = 0;
            for (p = 0; p < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2); p++) {
              h_ifft_FOE[0] = h_ifft_max[r * 2 + p](0, j);
              h_ifft_FOE[1] = MyExpCpx_2PI(-ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 4.0f : 3.0f)) * h_ifft_max[r * 2 + p](1, j);
              h_ifft_FOE[2] = MyExpCpx_2PI(-ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 7.0f : 6.0f)) * h_ifft_max[r * 2 + p](2, j);
              if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                h_ifft_FOE[3] = MyExpCpx_2PI(-ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 11.0f : 10.0f)) * h_ifft_max[r * 2 + p](3, j);
              }
              AveValue = 0.0f;
              Power_Ave = 0.0f;
              for (i = 0; i < CHE[ue_cell_idx].CRS_Num[0]; i++) {
                AveValue += h_ifft_FOE[i];
                Power_Ave += (h_ifft_FOE[i].real() * h_ifft_FOE[i].real() + h_ifft_FOE[i].imag() * h_ifft_FOE[i].imag());
              }
              AveValue = AveValue / (float)CHE[ue_cell_idx].CRS_Num[0];
              Power_Ave = Power_Ave / (float)CHE[ue_cell_idx].CRS_Num[0];
              Power_LOS = AveValue.real() * AveValue.real() + AveValue.imag() * AveValue.imag();
              Power_NLOS = Power_Ave - Power_LOS;
              if (Power_LOS > (0.8f * Power_Ave)) {
                LOS_Flag++;
              }
              //MEA[ue_cell_idx].HST_SFN_Flag = false;
              if (Power_LOS > (0.8f * Power_Ave) && MEA[ue_cell_idx].HST_SFN_Flag) {
                h_ifft_max[r * 2 + p](0, j) -= AveValue;
                h_ifft_max[r * 2 + p](1, j) -= MyExpCpx_2PI(ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 4.0f : 3.0f)) * AveValue;
                h_ifft_max[r * 2 + p](2, j) -= MyExpCpx_2PI(ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 7.0f : 6.0f)) * AveValue;
                if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                  h_ifft_max[r * 2 + p](3, j) -= MyExpCpx_2PI(ceta_maxs(r, j) * ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 11.0f : 10.0f)) * AveValue;
                }
                if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                  corr_0 += (h_ifft_max[r * 2 + p](1, j) * conj(h_ifft_max[r * 2 + p](0, j)) + h_ifft_max[r * 2 + p](3, j) * conj(h_ifft_max[r * 2 + p](2, j))) * 2.0f * ave_factor;
                  corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j)) + h_ifft_max[r * 2 + p](3, j) * conj(h_ifft_max[r * 2 + p](1, j))) * 2.0f * ave_factor;
                  corr_sqr += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag() + h_ifft_max[r * 2 + p](3, j).real() * h_ifft_max[r * 2 + p](3, j).real() + h_ifft_max[r * 2 + p](3, j).imag() * h_ifft_max[r * 2 + p](3, j).imag()) * ave_factor;
                  corr_sqr_plus += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag() + h_ifft_max[r * 2 + p](3, j).real() * h_ifft_max[r * 2 + p](3, j).real() + h_ifft_max[r * 2 + p](3, j).imag() * h_ifft_max[r * 2 + p](3, j).imag()) * ave_factor;
                } else {
                  corr_0 += (h_ifft_max[r * 2 + p](1, j) * conj(h_ifft_max[r * 2 + p](0, j))) * 3.0f * ave_factor;
                  corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j))) * 3.0f * ave_factor;
                  corr_sqr += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag()) * 1.5f * ave_factor;
                  corr_sqr_plus += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag()) * 1.5f * ave_factor;
                }
                Acc_Power_LOS[r] += Power_LOS;
                Acc_Power_NLOS[r] += Power_NLOS;
              } else {
                if (4 == CHE[ue_cell_idx].CRS_Num[0]) {
                  corr_0 += (h_ifft_max[r * 2 + p](1, j) * conj(h_ifft_max[r * 2 + p](0, j)) + h_ifft_max[r * 2 + p](3, j) * conj(h_ifft_max[r * 2 + p](2, j))) * 2.0f * ave_factor;
                  corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j)) + h_ifft_max[r * 2 + p](3, j) * conj(h_ifft_max[r * 2 + p](1, j))) * 2.0f * ave_factor;
                  corr_sqr += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag() + h_ifft_max[r * 2 + p](3, j).real() * h_ifft_max[r * 2 + p](3, j).real() + h_ifft_max[r * 2 + p](3, j).imag() * h_ifft_max[r * 2 + p](3, j).imag()) * ave_factor;
                  corr_sqr_plus += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag() + h_ifft_max[r * 2 + p](3, j).real() * h_ifft_max[r * 2 + p](3, j).real() + h_ifft_max[r * 2 + p](3, j).imag() * h_ifft_max[r * 2 + p](3, j).imag()) * ave_factor;
                } else {
                  corr_0 += (h_ifft_max[r * 2 + p](1, j) * conj(h_ifft_max[r * 2 + p](0, j))) * 3.0f * ave_factor;
                  corr_1 += (h_ifft_max[r * 2 + p](2, j) * conj(h_ifft_max[r * 2 + p](0, j))) * 3.0f * ave_factor;
                  corr_sqr += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](1, j).real() * h_ifft_max[r * 2 + p](1, j).real() + h_ifft_max[r * 2 + p](1, j).imag() * h_ifft_max[r * 2 + p](1, j).imag()) * 1.5f * ave_factor;
                  corr_sqr_plus += (h_ifft_max[r * 2 + p](0, j).real() * h_ifft_max[r * 2 + p](0, j).real() + h_ifft_max[r * 2 + p](0, j).imag() * h_ifft_max[r * 2 + p](0, j).imag() + h_ifft_max[r * 2 + p](2, j).real() * h_ifft_max[r * 2 + p](2, j).real() + h_ifft_max[r * 2 + p](2, j).imag() * h_ifft_max[r * 2 + p](2, j).imag()) * 1.5f * ave_factor;
                }
                Acc_Power_NLOS[r] += Power_Ave;
              }
            }
            if (0 == j && MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2) == LOS_Flag) {
              Max_LOS_Flag[r] = 1;
            }
            if (10.0f * MEA[ue_cell_idx].MaxPDPs_Value(r, j) > MEA[ue_cell_idx].MaxPDPs_Value(r, 0)) {
              R_tao0 += MAX(corr_sqr - Noise_Sample(r), 0.0f) / Noise_Sample(r);
              R_tao0_plus += MAX(corr_sqr_plus - Noise_Sample(r), 0.0f) / Noise_Sample(r);
              R_taoa_abs += abs(corr_0) / Noise_Sample(r);
              R_taoa_abs_plus += abs(corr_1) / Noise_Sample(r);
              R_taoa += corr_0 / Noise_Sample(r);
              Acc_ceta += ceta_maxs(r, j) * MEA[ue_cell_idx].MaxPDPs_Value(r, j) / Noise_Sample(r);
              Acc_P += MEA[ue_cell_idx].MaxPDPs_Value(r, j) / Noise_Sample(r);
            }
            if (MyFabs(ceta_maxs(r, j) - ceta_maxs(r, 0)) > 800.0f && MIN(Cell_Specific[ue_cell_idx].CRSPortNum, 2) == LOS_Flag) {
              tmp_HST_SFN_Flag[r] = 1;
            }
          }
        }
      }
    }
    //PDP Alpha Filter
    if (MEA[ue_cell_idx].CRS_PDP_Alpha_1stFlag) {
      MEA[ue_cell_idx].CRS_PDP_Alpha_1stFlag = false;
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MEA[ue_cell_idx].CRS_PDP_lastAGC(r) = AGC[r];
    }
    MEA[ue_cell_idx].CRS_PDP_lastTA = TA;
    MEA[ue_cell_idx].CRS_PDP_Counter++;
    if (10 == MEA[ue_cell_idx].CRS_PDP_Counter) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) /= (float)MEA[ue_cell_idx].CRS_PDP_Counter;
        if (MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) > 20.0f) {
          MEA[ue_cell_idx].CRS_PDP_Alpha(r) = 1.0f;
        } else if (MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) > 10.0f) {
          MEA[ue_cell_idx].CRS_PDP_Alpha(r) = 0.25f;
        } else if (MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) > 5.0f) {
          MEA[ue_cell_idx].CRS_PDP_Alpha(r) = 0.125f;
        } else {
          MEA[ue_cell_idx].CRS_PDP_Alpha(r) = 0.0625f;
        }
        //MEA[ue_cell_idx].CRS_PDP_Alpha(r) = 0.0625f;
        MEA[ue_cell_idx].CRS_PDP_AccSNRTh(r) = 0.0f;
      }
      MEA[ue_cell_idx].CRS_PDP_Counter = 0;
    }
    //IRT
    max_value = 0.0f;
    max_idx = 0;
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      if (MEA[ue_cell_idx].CRS_SNR(r) > max_value) {
        MEA[ue_cell_idx].CRS_SNR(r) = max_value;
        max_idx = r;
      }
    }
    MEA[ue_cell_idx].IRT_Final = MEA[ue_cell_idx].IRT[max_idx];
    //HST_SFN_Flag & Freq Offset & Doppler & R_T
    Last_Flag = false;
    if (CHE[ue_cell_idx].CRS_Num[0] >= 3) {
      //HST_SFN_Flag
      tmp_Flag = 0;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        tmp_Flag += tmp_HST_SFN_Flag[r];
      }
      if (0 == tmp_Flag) {
        MEA[ue_cell_idx].HST_SFN_Counter--;
      } else {
        MEA[ue_cell_idx].HST_SFN_Counter += 2;
      }
      MEA[ue_cell_idx].HST_SFN_Counter = MAX(MEA[ue_cell_idx].HST_SFN_Counter, 0);
      MEA[ue_cell_idx].HST_SFN_Counter = MIN(MEA[ue_cell_idx].HST_SFN_Counter, 5000);
      if (MEA[ue_cell_idx].HST_SFN_Counter > 30) {
        MEA[ue_cell_idx].HST_SFN_Flag = true;
      } else {
        MEA[ue_cell_idx].HST_SFN_Flag = false;
      }
      tmp_Flag = 0;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        tmp_Flag += Max_LOS_Flag[r];
      }
      if (UE_Specific[ue_cell_idx].RxNum == tmp_Flag && MEA[ue_cell_idx].HST_SFN_Flag) {
        MEA[ue_cell_idx].HST_SFN_Delay_Counter = 0;
      } else if (MEA[ue_cell_idx].HST_SFN_Delay_Counter < 300 && MEA[ue_cell_idx].HST_SFN_Flag) {
        Last_Flag = true;
        MEA[ue_cell_idx].HST_SFN_Delay_Counter++;
        MEA[ue_cell_idx].HST_SFN_Delay_Counter = MIN(MEA[ue_cell_idx].HST_SFN_Delay_Counter, 300);
      }
    } else {
      Last_Flag = true;
    }
    //Last_Flag = false;
    //MEA[ue_cell_idx].HST_SFN_Flag = true;
    sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("CRS_Measurement_1st: HST_SFN_Flag=%d, Last_Flag=%d, HST_SFN_Counter=%d, HST_SFN_Delay_Counter=%d\n", (int)MEA[ue_cell_idx].HST_SFN_Flag, (int)Last_Flag, MEA[ue_cell_idx].HST_SFN_Counter, MEA[ue_cell_idx].HST_SFN_Delay_Counter);
    //Freq Offset
    if (!Last_Flag && Acc_P > 0.0f) {
      MEA[ue_cell_idx].Freq = Acc_ceta / Acc_P;
    } else {
      MEA[ue_cell_idx].Freq = 0.0f;
    }
    sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("CRS_Measurement_1st: Freq=%f, RxFreqOffset=%f\n", MEA[ue_cell_idx].Freq, Feedback[ue_cell_idx].RxFreqOffset);
    if (!MEA[ue_cell_idx].fd_shift_1stFlag) {
      MEA[ue_cell_idx].fd_shift = MEA[ue_cell_idx].fd_shift + Feedback[ue_cell_idx].RxFreqOffset - MEA[ue_cell_idx].last_RxFreqOffset;
    }
    MEA[ue_cell_idx].last_RxFreqOffset = Feedback[ue_cell_idx].RxFreqOffset;
    if (!Last_Flag) {
      //Doppler
      delta_t = ds * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 4.0f : 3.0f);
      if (R_tao0 > 0.0f) {
        R_taoa_abs = R_taoa_abs / R_tao0;
        R_taoa_abs_plus = R_taoa_abs_plus / R_tao0_plus;
        R_taoa = R_taoa / R_tao0;
      } else {
        R_taoa_abs = 2.0f;
        R_taoa_abs_plus = 2.0f;
      }
      if (R_taoa_abs_plus < 1.0f || R_taoa_abs < 1.0f) {
        if ((R_taoa_abs_plus < 1.0f && R_taoa_abs_plus > 0.9f) || R_taoa_abs >= 1.0f) {
          fd_tmp = Inverse_Besselj_2PI(R_taoa_abs_plus) / 0.0005f;
        } else {
          fd_tmp = Inverse_Besselj_2PI(R_taoa_abs) / delta_t;
        }
        MEA[ue_cell_idx].fd = fd_alpha * fd_tmp + (1.0f - fd_alpha) * MEA[ue_cell_idx].fd;
        MEA[ue_cell_idx].fd = MAX(MIN(MEA[ue_cell_idx].fd, 800.0f), 5.0f);
        if (MEA[ue_cell_idx].HST_SFN_Flag) {
          fd_shift_tmp = arg(R_taoa) / (2.0f * PI * delta_t);
          if ((MEA[ue_cell_idx].fd_shift_1stFlag || MEA[ue_cell_idx].fd_shift_Counter > 10) && MyFabs(MEA[ue_cell_idx].fd_shift - fd_shift_tmp) < 800.0f) {
            MEA[ue_cell_idx].fd_shift = fd_shift_tmp;
            MEA[ue_cell_idx].fd_shift_1stFlag = false;
            MEA[ue_cell_idx].fd_shift_Counter = 0;
          } else if (MyFabs(MEA[ue_cell_idx].fd_shift - fd_shift_tmp) < 800.0f) {
            MEA[ue_cell_idx].fd_shift = fd_alpha * fd_shift_tmp + (1.0f - fd_alpha) * MEA[ue_cell_idx].fd_shift;
            MEA[ue_cell_idx].fd_shift_Counter = 0;
          } else {
            MEA[ue_cell_idx].fd_shift_Counter++;
            MEA[ue_cell_idx].fd_shift_Counter = MIN(MEA[ue_cell_idx].fd_shift_Counter, 10);
          }
        } else {
          MEA[ue_cell_idx].fd_shift = 0.0f;
        }
      }
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("CRS_Measurement_1st: fd=%f, fd_shift=%f\n", MEA[ue_cell_idx].fd, MEA[ue_cell_idx].fd_shift);
      //if(MEA[ue_cell_idx].fd > 300)
      //    MEA[ue_cell_idx].fd = 300;
      //MEA[ue_cell_idx].fd = 300.0f;
      //R_T
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        /*for (j = 0; j < MaxPDPs_Num; j++)
                {
                    if (P_maxs(r, j) > 0.0f)
                    {
                        FilePrint("ceta_maxs(%d, %d)=%f %f %d\n", r, j, ceta_maxs(r, j), P_maxs(r, j), MEA[ue_cell_idx].MaxPDPs_Idx(r, j));
                    }
                }
                FilePrint("fd=%f %f\n", MEA[ue_cell_idx].fd, MEA[ue_cell_idx].fd_shift);
                FilePrint("Acc_Power_LOS=%f %f\n", Acc_Power_LOS[r], Acc_Power_NLOS[r]);*/
        MEA[ue_cell_idx].R_T(r, 0) = 1.0f;
        for (i = 1; i < R_T_Num; i++) {
          //MEA[ue_cell_idx].R_T(r, i) = Besselj(0, 2.0f * PI * MEA[ue_cell_idx].fd * ds * (float) i) * MyExpCpx_2PI(MEA[ue_cell_idx].fd_shift * ds * (float) i);
          MEA[ue_cell_idx].R_T(r, i) = Besselj_2PI(MEA[ue_cell_idx].fd * ds * (float)i) * MyExpCpx_2PI(MEA[ue_cell_idx].fd_shift * ds * (float)i);
        }
        if (MEA[ue_cell_idx].HST_SFN_Flag) {
          for (i = 0; i < R_T_Num; i++) {
            tmp_R_T(r, i) = 0.0f;
            for (j = 0; j < MaxPDPs_Num; j++) {
              if (P_maxs(r, j) > 0.0f) {
                tmp_R_T(r, i) += MyExpCpx_2PI(ceta_maxs(r, j) * (float)i * ds) * P_maxs(r, j);
              }
            }
          }
          for (i = 1; i < R_T_Num; i++) {
            tmp_R_T(r, i) = tmp_R_T(r, i) / tmp_R_T(r, 0).real();
          }
          tmp_R_T(r, 0) = 1.0f;
          for (i = 0; i < R_T_Num; i++) {
            //MEA[ue_cell_idx].R_T(r, i) = tmp_R_T(r, i);
            MEA[ue_cell_idx].R_T(r, i) = Acc_Power_LOS[r] / (Acc_Power_LOS[r] + Acc_Power_NLOS[r]) * tmp_R_T(r, i) +
                                         Acc_Power_NLOS[r] / (Acc_Power_LOS[r] + Acc_Power_NLOS[r]) * MEA[ue_cell_idx].R_T(r, i);
          }
        }
      }
    }
  }
}

//! 基于Port5 DMRS的第一次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port5_Measurement_1st(int ue_cell_idx) {
  int r = 0;
  int l_prime = 0;
  int m_prime = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MEA[ue_cell_idx].DMRS_S_Port5(r) = 0.0f;
      for (l_prime = 0; l_prime < CHE[ue_cell_idx].DMRS_Num_Port5; l_prime++) {
        for (m_prime = 0; m_prime < (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
          MEA[ue_cell_idx].DMRS_S_Port5(r) += (CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime).real() * CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime).real() +
                                               CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime).imag() * CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime).imag());
        }
      }
      MEA[ue_cell_idx].DMRS_S_Port5(r) = MEA[ue_cell_idx].DMRS_S_Port5(r) / ((float)(CHE[ue_cell_idx].DMRS_Num_Port5 * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size()));
      if (MEA[ue_cell_idx].DMRS_S_Port5(r) > 2.0f * MEA[ue_cell_idx].CRS_N_2nd(r)) {
        MEA[ue_cell_idx].DMRS_S_Port5(r) = MEA[ue_cell_idx].DMRS_S_Port5(r) - MEA[ue_cell_idx].CRS_N_2nd(r);
      }
      MEA[ue_cell_idx].DMRS_SNR_Port5(r) = 10.0f * log10f(MEA[ue_cell_idx].DMRS_S_Port5(r) / MEA[ue_cell_idx].CRS_N_2nd(r));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("DMRS_Port5_Measurement_1st: SNR[%d]=%f, S=%f\n", r, MEA[ue_cell_idx].DMRS_SNR_Port5(r), MEA[ue_cell_idx].DMRS_S_Port5(r));
    }
  }
}

//! 基于Port7~14 DMRS的第一次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port7to14_Measurement_1st(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int DMRS_Num = 0;
  int l_prime = 0;
  int m_prime = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int NZP_idx = -1;
  int i = 0;
  int TA_prime = 0;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type && S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
      DMRS_Num = 1;
    } else {
      DMRS_Num = 2;
    }
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
      TA_prime = round((float)((MEA[ue_cell_idx].last_IRT[NZP_idx] - TA) * Cell_Specific[ue_cell_idx].N_FFT) / 2048.0f);
      for (i = 1; i < MIN(R_F_Num, Cell_Specific[ue_cell_idx].N_FFT); i++) {
        MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, i) = MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, i) * MyExpCpx_2PI((float)(i * TA_prime) / (float)Cell_Specific[ue_cell_idx].N_FFT);
      }
      MEA[ue_cell_idx].last_IRT[NZP_idx] = TA;
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MEA[ue_cell_idx].DMRS_S_Port7to14(r) = 0.0f;
      for (p = (int)(UE_Specific[ue_cell_idx].Start_Port - Port7); p < ((int)(UE_Specific[ue_cell_idx].Start_Port - Port7) + UE_Specific[ue_cell_idx].N_L[DCI_idx]); p++) {
        for (l_prime = 0; l_prime < DMRS_Num; l_prime++) {
          for (m_prime = 0; m_prime < (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
            MEA[ue_cell_idx].DMRS_S_Port7to14(r) += (CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p](m_prime, l_prime).real() * CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p](m_prime, l_prime).real() +
                                                     CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p](m_prime, l_prime).imag() * CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p](m_prime, l_prime).imag());
          }
        }
      }
      MEA[ue_cell_idx].DMRS_S_Port7to14(r) = MEA[ue_cell_idx].DMRS_S_Port7to14(r) / (float)(DMRS_Num * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size());
      if (MEA[ue_cell_idx].DMRS_S_Port7to14(r) > 2.0f * MEA[ue_cell_idx].CRS_N_2nd(r)) {
        MEA[ue_cell_idx].DMRS_S_Port7to14(r) = MEA[ue_cell_idx].DMRS_S_Port7to14(r) - ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r));
      }
      MEA[ue_cell_idx].DMRS_SNR_Port7to14(r) = 10.0f * log10f(MEA[ue_cell_idx].DMRS_S_Port7to14(r) / ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("DMRS_Port7to14_Measurement_1st: SNR[%d]=%f, S=%f\n", r, MEA[ue_cell_idx].DMRS_SNR_Port7to14(r), MEA[ue_cell_idx].DMRS_S_Port7to14(r));
    }
  }
}

//! 基于EPDCCH DMRS的第一次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMRS_Measurement_1st(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int DMRS_Num = 0;
  int l_prime = 0;
  int m_prime = 0;
  int set_idx = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int NZP_idx = -1;
  int i = 0;
  int TA_prime = 0;
  if (UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type && S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
      DMRS_Num = 1;
    } else {
      DMRS_Num = 2;
    }
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
      TA_prime = round((float)((MEA[ue_cell_idx].last_IRT[NZP_idx] - TA) * Cell_Specific[ue_cell_idx].N_FFT) / 2048.0f);
      for (i = 1; i < MIN(R_F_Num, Cell_Specific[ue_cell_idx].N_FFT); i++) {
        MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, i) = MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, i) * MyExpCpx_2PI((float)(i * TA_prime) / (float)Cell_Specific[ue_cell_idx].N_FFT);
      }
      MEA[ue_cell_idx].last_IRT[NZP_idx] = TA;
    }
    for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        MEA[ue_cell_idx].DMRS_S_Port7to14(r) = 0.0f;
        for (p = 0; p < 4; p++) {
          for (l_prime = 0; l_prime < DMRS_Num; l_prime++) {
            for (m_prime = 0; m_prime < (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); m_prime++) {
              MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) += (CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](m_prime, l_prime).real() * CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](m_prime, l_prime).real() +
                                                             CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](m_prime, l_prime).imag() * CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](m_prime, l_prime).imag());
            }
          }
        }
        MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) = MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) / (float)(DMRS_Num * (NormalCP == Cell_Specific[ue_cell_idx].CP_Type ? 3 : 4) * (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size());
        if (MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) > 2.0f * MEA[ue_cell_idx].CRS_N_2nd(r)) {
          MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) = MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) - ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r));
        }
        MEA[ue_cell_idx].EPDCCH_DMS_SNR(r, set_idx) = 10.0f * log10f(MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx) / ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)));
        sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
        WithFile(c_string);
        FilePrint("EPDCCH_DMRS_Measurement_1st: set_idx=%d, SNR[%d]=%f, S=%f\n", set_idx, r, MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx), MEA[ue_cell_idx].EPDCCH_DMS_SNR(r, set_idx));
      }
    }
  }
}

//! 基于CSI RS的第一次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CSI_RS_Measurement_1st(int ue_cell_idx) {
  int i = 0;
  int j = 0;
  int r = 0;
  int p = 0;
  int PDP_Len = 0;
  float ave_factor = 0.0f;
  float acc_num = 0.0f;
  float power_scale = 0.0f;
  float max_value = 0.0f;
  int max_idx = 0;
  float Power_Acc = 0.0f;
  fvec Noise_Sample;
  int MaxPDPs_Num = 0;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  fvec PDP_tmp;
  cpxf temp = 0.0f;
  float ceta = 0.0f;
  ivec N_Segment;
  float PDP_Th = 0.0f;
  cfvec fft_in_R_F;
  cfvec fft_out_R_F;
  int NZP_idx = 0;
  if ((UE_Specific[ue_cell_idx].NZP_CSI_Set_Num > 0 || UE_Specific[ue_cell_idx].ZP_CSI_Set_Num > 0) && U != Cell_Specific[ue_cell_idx].SubframeType && S != Cell_Specific[ue_cell_idx].SubframeType) {
    PDP_Len = (1536 == Cell_Specific[ue_cell_idx].N_FFT) ? 128 : (Cell_Specific[ue_cell_idx].N_FFT * 21 / 256);
    power_scale = MySqrt(PDP_Len);
    PDP_tmp.resize(PDP_Len);
    MaxPDPs_Num = MIN(MAXPDPSNUM, PDP_Len);
    fft_in.resize(PDP_Len);
    fft_out.resize(PDP_Len);
    fft_in_R_F.resize(Cell_Specific[ue_cell_idx].N_FFT);
    fft_out_R_F.resize(Cell_Specific[ue_cell_idx].N_FFT);
    Noise_Sample.resize(UE_Specific[ue_cell_idx].RxNum);
    N_Segment.resize(UE_Specific[ue_cell_idx].RxNum);
    for (NZP_idx = 0; NZP_idx < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; NZP_idx++) {
      if (UE_Specific[ue_cell_idx].CSI_Flag_NZP[NZP_idx]) {
        MEA[ue_cell_idx].last_IRT[NZP_idx] = TA;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          //PDP
          ave_factor = 0;
          for (i = 0; i < PDP_Len; i++) {
            MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i) = 0.0f;
          }
          for (p = 0; p < UE_Specific[ue_cell_idx].CSIPortNum_NZP[NZP_idx]; p++) {
            fft_in.setZero();
            fft_in.segment(0, Cell_Specific[ue_cell_idx].N_DL_RB) = CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p].col(NZP_idx);
            fft.inv(fft_out, fft_in);
            fft_out *= power_scale;
            for (i = 0; i < PDP_Len; i++) {
              MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i) += (fft_out(i).real() * fft_out(i).real() + fft_out(i).imag() * fft_out(i).imag());
            }
            ave_factor += 1.0f;
          }
          ave_factor = 1.0f / ave_factor / (float)Cell_Specific[ue_cell_idx].N_DL_RB;
          for (i = 0; i < PDP_Len; i++) {
            MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i) *= ave_factor;
            PDP_tmp(i) = MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i);
          }
          //Find Max PDPs
          for (j = 0; j < MaxPDPs_Num; j += 3) {
            max_value = 0.0f;
            max_idx = 0;
            for (i = 0; i < PDP_Len; i++) {
              if (PDP_tmp(i) > max_value) {
                max_value = PDP_tmp(i);
                max_idx = i;
              }
            }
            MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j) = max_idx;
            MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, j) = max_value;
            MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 1) = (max_idx - 1 + PDP_Len) % PDP_Len;
            MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, j + 1) = PDP_tmp(MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 1));
            MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 2) = (max_idx + 1 + PDP_Len) % PDP_Len;
            MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, j + 2) = PDP_tmp(MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 2));
            PDP_tmp(MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j)) = 0.0f;
            PDP_tmp(MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 1)) = 0.0f;
            PDP_tmp(MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j + 2)) = 0.0f;
          }
          //Noise Power
          Power_Acc = 0.0f;
          acc_num = 0.0f;
          for (i = (MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, 0) - PDP_Len / 2); i < (MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, 0) - PDP_Len / 4); i++) {
            j = (i + PDP_Len) % PDP_Len;
            Power_Acc += MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, j);
            acc_num += 1.0f;
          }
          Noise_Sample(r) = Power_Acc / acc_num;
          Power_Acc = 0.0f;
          acc_num = 0.0f;
          for (i = 0; i < PDP_Len; i++) {
            if (PDP_tmp(i) > 0.0f && PDP_tmp(i) < (2.0f * Noise_Sample(r))) {
              Power_Acc += PDP_tmp(i);
              acc_num += 1.0f;
            }
          }
          if (Power_Acc > 0.0f) {
            Noise_Sample(r) = MIN(Power_Acc / acc_num, Noise_Sample(r));
          }
          Power_Acc = Noise_Sample(r) * (float)PDP_Len;
#if NOISE_ALPHA_FLAG > 0
          if (MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) > 0.0f) {
            MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) = (1.0f - noise_alpha) * MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) + noise_alpha * Power_Acc;
          } else {
            MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) = Power_Acc;
          }
#else
          MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) = Power_Acc;
#endif
          if (0.0f == MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx)) {
            MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) = 1.0f;
          }
          //Singal Power
          MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx) = 0.0f;
          for (i = 0; i < PDP_Len; i++) {
            if (MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i) > (2.0f * Noise_Sample(r))) {
              MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx) += (MEA[ue_cell_idx].CSI_RS_PDP[NZP_idx](r, i) - Noise_Sample(r));
            }
          }
          if (0.0f == MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx)) {
            MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx) = MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) / ((float)Cell_Specific[ue_cell_idx].N_DL_RB);
          }
          //SNR
          MEA[ue_cell_idx].CSI_RS_SNR(r, NZP_idx) = 10.0f * log10f(MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx) / MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx));
          sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          FilePrint("CSI_RS_Measurement_1st: NZP_idx=%d, SNR[%d]=%f, S=%f, N=%f\n", NZP_idx, r, MEA[ue_cell_idx].CSI_RS_SNR(r, NZP_idx), MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx), MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx));
          //IRT
          temp = 0.0f;
          for (p = 0; p < UE_Specific[ue_cell_idx].CSIPortNum_NZP[NZP_idx]; p++) {
            for (i = 0; i < Cell_Specific[ue_cell_idx].N_DL_RB / 2 - 1; i++) {
              temp += CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](i, NZP_idx) * conj(CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](i + 1, NZP_idx));
            }
            for (i = Cell_Specific[ue_cell_idx].N_DL_RB / 2; i < Cell_Specific[ue_cell_idx].N_DL_RB - 1; i++) {
              temp += CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](i, NZP_idx) * conj(CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](i + 1, NZP_idx));
            }
          }
          ceta = arg(temp);
          //N_Segment
          N_Segment(r) = round(ceta * (float)Cell_Specific[ue_cell_idx].N_FFT / (24.0f * PI) * ((float)PDP_Len / 2048.0f * 12.0f) - 36.0f * ((float)PDP_Len / 2048.0f * 12.0f));
          if (N_Segment(r) < 0) {
            N_Segment(r) += PDP_Len;
          }
          //R_F
          fft_in_R_F.setZero();
          PDP_Th = MAX(2.0f * Noise_Sample(r), MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, 0) / 128.0f);
          for (j = 0; j < MaxPDPs_Num; j++) {
            if (MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, j) > PDP_Th || 0 == j) {
              i = round((float)MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j) * (float)Cell_Specific[ue_cell_idx].N_FFT / (12.0f * (float)PDP_Len));
              if (MEA[ue_cell_idx].CSI_MaxPDPs_Idx[NZP_idx](r, j) > N_Segment(r)) {
                i = i + Cell_Specific[ue_cell_idx].N_FFT - round((float)Cell_Specific[ue_cell_idx].N_FFT / 12.0f);
              }
              fft_in_R_F(i).real(MAX(fft_in_R_F(i).real(), MEA[ue_cell_idx].CSI_MaxPDPs_Value[NZP_idx](r, j)));
            }
          }
          fft.fwd(fft_out_R_F, fft_in_R_F);
          for (i = 1; i < MIN(R_F_Num, Cell_Specific[ue_cell_idx].N_FFT); i++) {
            MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, i) = fft_out_R_F(i) / fft_out_R_F(0).real();
          }
          MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, 0) = 1.0f;
        }
      }
    }
  }
}

//! 基于CRS的第二次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CRS_Measurement_2nd(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int k = 0;
  int m = 0;
  int subband_idx = 0;
  int RI_idx = 0;
  int RI_start_idx = 0;
  int RI_end_idx = 0;
  int PMI_idx = 0;
  const int v[4][4] = {{0, 3, 0, 3}, {3, 0, 3, 0}, {0, 3, 0, 0}, {3, 6, 0, 0}};
  int v_shift = 0;
  int acc_num = 0;
  int RS_Pos[4][4];
  cpxf c_temp = 0.0f;
  float temp[2];
  cfmat h;      //h(r, p)
  cfmat h_eff;  //h(r, v)
  cfmat Ruu;
  cfmat Ryy_Inv;
  cfmat W;
  cfmat MMSE_H;
  float noise = 0.0f;
  float interference = 0.0f;
  int vv = 0;
  int vv_prime = 0;
  if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
    RS_Pos[0][0] = RS_Pos[1][0] = 0;
    RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[0][2] = RS_Pos[1][2] = Cell_Specific[ue_cell_idx].N_DL_symb;
    RS_Pos[0][3] = RS_Pos[1][3] = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[2][0] = RS_Pos[3][0] = 1;
    RS_Pos[2][1] = RS_Pos[3][1] = Cell_Specific[ue_cell_idx].N_DL_symb + 1;
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 6;
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //CRS_N_2nd & CRS_S_2nd
      temp[0] = 0.0f;
      temp[1] = 0.0f;
      acc_num = 0;
      for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
        for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
          for (m = 0; m < (2 * Cell_Specific[ue_cell_idx].N_DL_RB); m++) {
            k = 6 * m + (v[p][l_idx] + v_shift) % 6;
            c_temp = (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](m, l_idx) - CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]));
            temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            c_temp = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]);
            temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            acc_num++;
          }
        }
      }
      MEA[ue_cell_idx].CRS_N_2nd(r) = temp[0] / (float)acc_num;
      MEA[ue_cell_idx].CRS_S_2nd(r) = temp[1] / (float)acc_num;
      if (0.0f == MEA[ue_cell_idx].CRS_S_2nd(r)) {
        MEA[ue_cell_idx].CRS_N_2nd(r) = 0.001f;
        MEA[ue_cell_idx].CRS_S_2nd(r) = 1.0f;
      } else if (0.0f == MEA[ue_cell_idx].CRS_N_2nd(r)) {
        MEA[ue_cell_idx].CRS_N_2nd(r) = MEA[ue_cell_idx].CRS_S_2nd(r) / 1000.0f;
      }
      MEA[ue_cell_idx].CRS_SNR_2nd(r) = 10.0f * log10f(MEA[ue_cell_idx].CRS_S_2nd(r) / MEA[ue_cell_idx].CRS_N_2nd(r));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("CRS_Measurement_2nd: SNR[%d]=%f, S=%f, N=%f\n", r, MEA[ue_cell_idx].CRS_SNR_2nd(r), MEA[ue_cell_idx].CRS_S_2nd(r), MEA[ue_cell_idx].CRS_N_2nd(r));
    }
    //Subband Noise
    for (subband_idx = 0; subband_idx < (Feedback[ue_cell_idx].N_Subband - 1); subband_idx++) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        temp[0] = 0.0f;
        acc_num = 0;
        for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
          for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
            for (m = (2 * subband_idx * Feedback[ue_cell_idx].k_Subband); m < (2 * (subband_idx + 1) * Feedback[ue_cell_idx].k_Subband); m++) {
              k = 6 * m + (v[p][l_idx] + v_shift) % 6;
              c_temp = (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](m, l_idx) - CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]));
              temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
              acc_num++;
            }
          }
        }
        MEA[ue_cell_idx].Subband_N(r, subband_idx) = temp[0] / (float)acc_num;
      }
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      temp[0] = 0.0f;
      acc_num = 0;
      for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
        for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
          for (m = (2 * (Feedback[ue_cell_idx].N_Subband - 1) * Feedback[ue_cell_idx].k_Subband); m < (2 * Cell_Specific[ue_cell_idx].N_DL_RB); m++) {
            k = 6 * m + (v[p][l_idx] + v_shift) % 6;
            c_temp = (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](m, l_idx) - CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]));
            temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            acc_num++;
          }
        }
      }
      MEA[ue_cell_idx].Subband_N(r, Feedback[ue_cell_idx].N_Subband - 1) = temp[0] / (float)acc_num;
    }
    //Subband SNR
    if (Simuctrl.CQI_Feedback_Flag || Simuctrl.RI_Feedback_Flag || Simuctrl.PMI_Feedback_Flag) {
      if (1 == Cell_Specific[ue_cell_idx].CRSPortNum || Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) {
        for (subband_idx = 0; subband_idx < (Feedback[ue_cell_idx].N_Subband - 1); subband_idx++) {
          MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx) = 0.0f;
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            temp[1] = 0.0f;
            acc_num = 0;
            for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
              for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
                for (m = (2 * subband_idx * Feedback[ue_cell_idx].k_Subband); m < (2 * (subband_idx + 1) * Feedback[ue_cell_idx].k_Subband); m++) {
                  k = 6 * m + (v[p][l_idx] + v_shift) % 6;
                  c_temp = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]);
                  temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                  acc_num++;
                }
              }
            }
            MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx) += (temp[1] / (float)acc_num / MEA[ue_cell_idx].Subband_N(r, subband_idx));
          }
          MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx) /= (float)UE_Specific[ue_cell_idx].RxNum;
        }
        MEA[ue_cell_idx].Subband_SNR[0](0, Feedback[ue_cell_idx].N_Subband - 1) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          temp[1] = 0.0f;
          acc_num = 0;
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
              for (m = (2 * (Feedback[ue_cell_idx].N_Subband - 1) * Feedback[ue_cell_idx].k_Subband); m < (2 * Cell_Specific[ue_cell_idx].N_DL_RB); m++) {
                k = 6 * m + (v[p][l_idx] + v_shift) % 6;
                c_temp = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[p][l_idx]);
                temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                acc_num++;
              }
            }
          }
          MEA[ue_cell_idx].Subband_SNR[0](0, Feedback[ue_cell_idx].N_Subband - 1) += (temp[1] / (float)acc_num / MEA[ue_cell_idx].Subband_N(r, Feedback[ue_cell_idx].N_Subband - 1));
        }
        MEA[ue_cell_idx].Subband_SNR[0](0, Feedback[ue_cell_idx].N_Subband - 1) /= (float)UE_Specific[ue_cell_idx].RxNum;
      } else {
        h.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].CRSPortNum);
        h_eff.resize(UE_Specific[ue_cell_idx].RxNum, MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum));
        Ruu.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].RxNum);
        RI_start_idx = (Simuctrl.RI_Feedback_Flag) ? 0 : Feedback[ue_cell_idx].RI;
        RI_end_idx = (Simuctrl.RI_Feedback_Flag) ? MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum) : (Feedback[ue_cell_idx].RI + 1);
        for (subband_idx = 0; subband_idx < (Feedback[ue_cell_idx].N_Subband - 1); subband_idx++) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            Ruu(r, r) = MEA[ue_cell_idx].Subband_N(r, subband_idx);
          }
          for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
            for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
              for (vv = 0; vv < (RI_idx + 1); vv++) {
                MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) = 0.0f;
              }
            }
          }
          for (m = (2 * subband_idx * Feedback[ue_cell_idx].k_Subband); m < (2 * (subband_idx + 1) * Feedback[ue_cell_idx].k_Subband); m++) {
            k = 6 * m + (v[0][0] + v_shift) % 6;
            for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[0]; l_idx++) {
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                  h(r, p) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[0][l_idx]);
                }
              }
              for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
                W.resize(RI_idx + 1, UE_Specific[ue_cell_idx].RxNum);
                MMSE_H.resize(RI_idx + 1, RI_idx + 1);
                for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
                  if (2 == Cell_Specific[ue_cell_idx].CRSPortNum) {
                    if (0 == RI_idx) {
                      h_eff = h * Codebook_2P1v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else {
                      h_eff = h * Codebook_2P2v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    }
                  } else if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
                    if (UE_Specific[ue_cell_idx].alternativeCodeBookEnabledFor4TX_r12) {
                      if (0 == RI_idx) {
                        h_eff = h * Codebook_4P1v_CSI[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else if (1 == RI_idx) {
                        h_eff = h * Codebook_4P2v_CSI[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else if (2 == RI_idx) {
                        h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else {
                        h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      }
                    } else {
                      if (0 == RI_idx) {
                        h_eff = h * Codebook_4P1v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else if (1 == RI_idx) {
                        h_eff = h * Codebook_4P2v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else if (2 == RI_idx) {
                        h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      } else {
                        h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                      }
                    }
                  }
                  Hermitian_Matrix_INV(h_eff * h_eff.adjoint() + Ruu, Ryy_Inv);
                  W = h_eff.adjoint() * Ryy_Inv;
                  MMSE_H = W * h_eff;
                  for (vv = 0; vv < (RI_idx + 1); vv++) {
                    noise = 0.0f;
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      noise += (W(vv, r).real() * W(vv, r).real() + W(vv, r).imag() * W(vv, r).imag()) * MEA[ue_cell_idx].Subband_N(r, subband_idx);
                    }
                    interference = 0.0f;
                    for (vv_prime = 0; vv_prime < (RI_idx + 1); vv_prime++) {
                      if (vv_prime != vv) {
                        interference += (MMSE_H(vv, vv_prime).real() * MMSE_H(vv, vv_prime).real() + MMSE_H(vv, vv_prime).imag() * MMSE_H(vv, vv_prime).imag());
                      }
                    }
                    noise = 1.0f / (noise + interference);
                    MEA[ue_cell_idx].Subband_SNR[RI_idx](UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx), vv * Feedback[ue_cell_idx].N_Subband + subband_idx) += MMSE_H(vv, vv).real() * MMSE_H(vv, vv).real() * noise;
                  }
                }
              }
            }
          }
          for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
            for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
              for (vv = 0; vv < (RI_idx + 1); vv++) {
                MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) /= (float)(2 * Feedback[ue_cell_idx].k_Subband * CHE[ue_cell_idx].CRS_Num[0]);
              }
            }
          }
        }
        for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
          for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
            for (vv = 0; vv < (RI_idx + 1); vv++) {
              MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + Feedback[ue_cell_idx].N_Subband - 1) = 0.0f;
            }
          }
        }
        for (m = (2 * (Feedback[ue_cell_idx].N_Subband - 1) * Feedback[ue_cell_idx].k_Subband); m < (2 * Cell_Specific[ue_cell_idx].N_DL_RB); m++) {
          k = 6 * m + (v[0][0] + v_shift) % 6;
          for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[0]; l_idx++) {
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                h(r, p) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, RS_Pos[0][l_idx]);
              }
            }
            for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
              W.resize(RI_idx + 1, UE_Specific[ue_cell_idx].RxNum);
              MMSE_H.resize(RI_idx + 1, RI_idx + 1);
              for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
                if (2 == Cell_Specific[ue_cell_idx].CRSPortNum) {
                  if (0 == RI_idx) {
                    h_eff = h * Codebook_2P1v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                  } else {
                    h_eff = h * Codebook_2P2v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                  }
                } else if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
                  if (UE_Specific[ue_cell_idx].alternativeCodeBookEnabledFor4TX_r12) {
                    if (0 == RI_idx) {
                      h_eff = h * Codebook_4P1v_CSI[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else if (1 == RI_idx) {
                      h_eff = h * Codebook_4P2v_CSI[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else if (2 == RI_idx) {
                      h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else {
                      h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    }
                  } else {
                    if (0 == RI_idx) {
                      h_eff = h * Codebook_4P1v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else if (1 == RI_idx) {
                      h_eff = h * Codebook_4P2v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else if (2 == RI_idx) {
                      h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    } else {
                      h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx)];
                    }
                  }
                }
                Hermitian_Matrix_INV(h_eff * h_eff.adjoint() + Ruu, Ryy_Inv);
                W = h_eff.adjoint() * Ryy_Inv;
                MMSE_H = W * h_eff;
                for (vv = 0; vv < (RI_idx + 1); vv++) {
                  noise = 0.0f;
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    noise += (W(vv, r).real() * W(vv, r).real() + W(vv, r).imag() * W(vv, r).imag()) * MEA[ue_cell_idx].Subband_N(r, Feedback[ue_cell_idx].N_Subband - 1);
                  }
                  interference = 0.0f;
                  for (vv_prime = 0; vv_prime < (RI_idx + 1); vv_prime++) {
                    if (vv_prime != vv) {
                      interference += (MMSE_H(vv, vv_prime).real() * MMSE_H(vv, vv_prime).real() + MMSE_H(vv, vv_prime).imag() * MMSE_H(vv, vv_prime).imag());
                    }
                  }
                  noise = 1.0f / (noise + interference);
                  MEA[ue_cell_idx].Subband_SNR[RI_idx](UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx), vv * Feedback[ue_cell_idx].N_Subband + Feedback[ue_cell_idx].N_Subband - 1) += MMSE_H(vv, vv).real() * MMSE_H(vv, vv).real() * noise;
                }
              }
            }
          }
        }
        for (RI_idx = RI_start_idx; RI_idx < RI_end_idx; RI_idx++) {
          for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
            for (vv = 0; vv < (RI_idx + 1); vv++) {
              MEA[ue_cell_idx].Subband_SNR[RI_idx](UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](PMI_idx), vv * Feedback[ue_cell_idx].N_Subband + Feedback[ue_cell_idx].N_Subband - 1) /= (float)(2 * Feedback[ue_cell_idx].k_LastSubband * CHE[ue_cell_idx].CRS_Num[0]);
            }
          }
        }
      }
    }
#if 1 == EESM_TEST
    if (UE_Specific[ue_cell_idx].RB_Assignment[UE_Specific[ue_cell_idx].DCI_idx].size() > 0) {
      sprintf(c_string, "%s/%s_%s_EESM_SubbandSNR", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
        FilePrint("%f\n", MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx));
        //FilePrint("%f\n", 10.0f * log10f(MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx)));
      }
    }
#endif
#if 1 == CQI_TEST
    float SNR_eff = 0.0f;
    if (UE_Specific[ue_cell_idx].RB_Assignment[UE_Specific[ue_cell_idx].DCI_idx].size() > 0) {
      SNR_eff = 0.0f;
      for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
        SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[0](0, subband_idx) / CQI_Beta[UE_Specific[ue_cell_idx].CQI[0][UE_Specific[ue_cell_idx].DCI_idx] - 1]));
      }
      Result[ue_cell_idx].CQI_SNR_eff[loop] += -CQI_Beta[UE_Specific[ue_cell_idx].CQI[0][UE_Specific[ue_cell_idx].DCI_idx] - 1] * MyLog(SNR_eff / (float)Feedback[ue_cell_idx].N_Subband);
      Result[ue_cell_idx].CQI_SNR_eff_Num[loop]++;
    }
#endif
  }
}

//! 基于Port5 DMRS的第二次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port5_Measurement_2nd(int ue_cell_idx) {
  int r = 0;
  int l_prime = 0;
  ivec l;
  int m_prime = 0;
  int n_PRB = 0;
  int v_shift = 0;
  int k_prime = 0;
  int k = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int acc_num = 0;
  cpxf c_temp = 0.0f;
  float temp[2];
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      l.resize(4);
      l << 3, 6, 9, 12;
    } else {
      l.resize(3);
      l << 4, 7, 10;
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //DMRS_N_2nd_Port5 & DMRS_S_2nd_Port5
      temp[0] = 0.0f;
      temp[1] = 0.0f;
      acc_num = 0;
      for (l_prime = 0; l_prime < CHE[ue_cell_idx].DMRS_Num_Port5; l_prime++) {
        if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
          for (m_prime = 0; m_prime < 3 * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][m_prime / 3];
            k_prime = (0 == (l_prime & 1)) ? (4 * m_prime + v_shift) : (4 * m_prime + ((2 + v_shift) & 3));
            k = k_prime % Cell_Specific[ue_cell_idx].N_RB_sc + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB;
            c_temp = (CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime) - CHE[ue_cell_idx].DMRS_H_T_Port5[r](k, l[l_prime]));
            temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            c_temp = CHE[ue_cell_idx].DMRS_H_T_Port5[r](k, l[l_prime]);
            temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            acc_num++;
          }
        } else {
          for (m_prime = 0; m_prime < 4 * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][m_prime / 4];
            k_prime = (1 != l_prime) ? (3 * m_prime + v_shift) : (3 * m_prime + ((2 + v_shift) % 3));
            k = k_prime % Cell_Specific[ue_cell_idx].N_RB_sc + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB;
            c_temp = (CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime) - CHE[ue_cell_idx].DMRS_H_T_Port5[r](k, l[l_prime]));
            temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            c_temp = CHE[ue_cell_idx].DMRS_H_T_Port5[r](k, l[l_prime]);
            temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
            acc_num++;
          }
        }
      }
      MEA[ue_cell_idx].DMRS_N_2nd_Port5(r) = temp[0] / (float)acc_num;
      MEA[ue_cell_idx].DMRS_S_2nd_Port5(r) = temp[1] / (float)acc_num;
      if (0.0f == MEA[ue_cell_idx].DMRS_S_2nd_Port5(r)) {
        MEA[ue_cell_idx].DMRS_N_2nd_Port5(r) = 0.001f;
        MEA[ue_cell_idx].DMRS_S_2nd_Port5(r) = 1.0f;
      } else if (0.0f == MEA[ue_cell_idx].DMRS_N_2nd_Port5(r)) {
        MEA[ue_cell_idx].DMRS_N_2nd_Port5(r) = MEA[ue_cell_idx].DMRS_S_2nd_Port5(r) / 1000.0f;
      }
      MEA[ue_cell_idx].DMRS_SNR_2nd_Port5(r) = 10.0f * log10f(MEA[ue_cell_idx].DMRS_S_2nd_Port5(r) / MEA[ue_cell_idx].DMRS_N_2nd_Port5(r));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("DMRS_Port5_Measurement_2nd: SNR[%d]=%f, S=%f, N=%f\n", r, MEA[ue_cell_idx].DMRS_SNR_2nd_Port5(r), MEA[ue_cell_idx].DMRS_S_2nd_Port5(r), MEA[ue_cell_idx].DMRS_N_2nd_Port5(r));
    }
  }
}

//! 基于Port7~14 DMRS的第二次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port7to14_Measurement_2nd(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_prime = 0;
  int i = 0;
  ivec l;
  int m_prime = 0;
  int n_PRB = 0;
  int k_prime = 0;
  int k = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int acc_num = 0;
  cpxf c_temp = 0.0f;
  float temp[2];
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      l.resize(2);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 5;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 9;
      } else {
        l << 5, 12;
      }
    } else {
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l.resize(1);
        l << 4;
      } else {
        l.resize(2);
        l << 4, 10;
      }
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //DMRS_N_2nd_Port7to14 & DMRS_S_2nd_Port7to14
      temp[0] = 0.0f;
      temp[1] = 0.0f;
      acc_num = 0;
      for (p = (int)UE_Specific[ue_cell_idx].Start_Port; p < ((int)UE_Specific[ue_cell_idx].Start_Port + UE_Specific[ue_cell_idx].N_L[DCI_idx]); p++) {
        if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
          for (l_prime = 0; l_prime < 2; l_prime++) {
            for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); i++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i];
              for (m_prime = 0; m_prime < 3; m_prime++) {
                k_prime = ((int)Port7 == p || (int)Port8 == p || (int)Port11 == p || (int)Port13 == p) ? 1 : 0;
                k = 5 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                c_temp = (CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](3 * i + m_prime, l_prime) - CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](k, l[l_prime]));
                temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                c_temp = CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](k, l[l_prime]);
                temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                acc_num++;
              }
            }
          }
        } else {
          for (l_prime = 0; l_prime < (int)l.size(); l_prime++) {
            for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); i++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i];
              for (m_prime = 0; m_prime < 4; m_prime++) {
                k_prime = (l(l_prime) <= 5) ? 1 : 2;
                k = 3 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                c_temp = (CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](4 * i + m_prime, l_prime) - CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](k, l[l_prime]));
                temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                c_temp = CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](k, l[l_prime]);
                temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                acc_num++;
              }
            }
          }
        }
      }
      MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r) = temp[0] / (float)acc_num;
      MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r) = temp[1] / (float)acc_num;
      if (0.0f == MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r)) {
        MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r) = 0.001f;
        MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r) = 1.0f;
      } else if (0.0f == MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r)) {
        MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r) = MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r) / 1000.0f;
      }
      MEA[ue_cell_idx].DMRS_SNR_2nd_Port7to14(r) = 10.0f * log10f(MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r) / MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("DMRS_Port7to14_Measurement_2nd: SNR[%d]=%f, S=%f, N=%f\n", r, MEA[ue_cell_idx].DMRS_SNR_2nd_Port7to14(r), MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r), MEA[ue_cell_idx].DMRS_N_2nd_Port7to14(r));
    }
  }
}

//! EPDCCH DMRS的第二次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMRS_Measurement_2nd(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_prime = 0;
  int i = 0;
  ivec l;
  int m_prime = 0;
  int n_PRB = 0;
  int k_prime = 0;
  int k = 0;
  int acc_num = 0;
  cpxf c_temp = 0.0f;
  float temp[2];
  int set_idx = 0;
  if (UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      l.resize(2);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 5;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 9;
      } else {
        l << 5, 12;
      }
    } else {
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l.resize(1);
        l << 4;
      } else {
        l.resize(2);
        l << 4, 10;
      }
    }
    for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        //EPDCCH_DMRS_N_2nd & EPDCCH_DMRS_S_2nd
        temp[0] = 0.0f;
        temp[1] = 0.0f;
        acc_num = 0;
        for (p = (int)Port107; p < ((int)Port107 + 4); p++) {
          if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
            for (l_prime = 0; l_prime < 2; l_prime++) {
              for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
                n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][i];
                for (m_prime = 0; m_prime < 3; m_prime++) {
                  k_prime = ((int)Port107 == p || (int)Port108 == p) ? 1 : 0;
                  k = 5 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                  c_temp = (CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](3 * i + m_prime, l_prime) - CHE[ue_cell_idx].EPDCCH_DMRS_H_T[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](k, l[l_prime]));
                  temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                  c_temp = CHE[ue_cell_idx].EPDCCH_DMRS_H_T[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](k, l[l_prime]);
                  temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                  acc_num++;
                }
              }
            }
          } else {
            for (l_prime = 0; l_prime < (int)l.size(); l_prime++) {
              for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
                n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][i];
                for (m_prime = 0; m_prime < 4; m_prime++) {
                  k_prime = (l(l_prime) <= 5) ? 1 : 2;
                  k = 3 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                  c_temp = (CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](4 * i + m_prime, l_prime) - CHE[ue_cell_idx].EPDCCH_DMRS_H_T[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](k, l[l_prime]));
                  temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                  c_temp = CHE[ue_cell_idx].EPDCCH_DMRS_H_T[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](k, l[l_prime]);
                  temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
                  acc_num++;
                }
              }
            }
          }
        }
        MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx) = temp[0] / (float)acc_num;
        MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd(r, set_idx) = temp[1] / (float)acc_num;
        if (0.0f == MEA[ue_cell_idx].DMRS_S_2nd_Port7to14(r, set_idx)) {
          MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx) = 0.001f;
          MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd(r, set_idx) = 1.0f;
        } else if (0.0f == MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx)) {
          MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx) = MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd(r, set_idx) / 1000.0f;
        }
        MEA[ue_cell_idx].EPDCCH_DMRS_SNR_2nd(r, set_idx) = 10.0f * log10f(MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd(r, set_idx) / MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx));
        sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
        WithFile(c_string);
        FilePrint("EPDCCH_DMRS_Measurement_2nd: set_idx=%d, SNR[%d]=%f, S=%f, N=%f\n", set_idx, r, MEA[ue_cell_idx].EPDCCH_DMRS_SNR_2nd(r, set_idx), MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd(r, set_idx), MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd(r, set_idx));
      }
    }
  }
}

//! 基于CSI RS的第二次测量
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CSI_RS_Measurement_2nd(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int k_prime = 0;
  int k = 0;
  int m = 0;
  int NZP_idx = 0;
  int acc_num = 0;
  cpxf c_temp = 0.0f;
  float temp[2];
  if ((UE_Specific[ue_cell_idx].NZP_CSI_Set_Num > 0 || UE_Specific[ue_cell_idx].ZP_CSI_Set_Num > 0) && U != Cell_Specific[ue_cell_idx].SubframeType && S != Cell_Specific[ue_cell_idx].SubframeType) {
    for (NZP_idx = 0; NZP_idx < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; NZP_idx++) {
      if (UE_Specific[ue_cell_idx].CSI_Flag_NZP[NZP_idx]) {
        if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
          k_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
        } else {
          k_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
        }
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          //CSI_RS_N_2nd & CSI_RS_S_2nd
          temp[0] = 0.0f;
          temp[1] = 0.0f;
          acc_num = 0;
          for (p = (int)Port15; p < ((int)Port15 + UE_Specific[ue_cell_idx].CSIPortNum_NZP[NZP_idx]); p++) {
            for (m = 0; m < Cell_Specific[ue_cell_idx].N_DL_RB; m++) {
              k = k_prime + 12 * m + ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? CSI_k_offset_NormalCP[p - (int)Port15] : CSI_k_offset_ExtendedCP[p - (int)Port15]);
              c_temp = (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCSIRSPORTNUM + p - (int)Port15](m, NZP_idx) - CHE[ue_cell_idx].CSI_RS_H_F[r * MAXCSIRSPORTNUM + p - (int)Port15](k, NZP_idx));
              temp[0] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
              c_temp = CHE[ue_cell_idx].CSI_RS_H_F[r * MAXCSIRSPORTNUM + p - (int)Port15](k, NZP_idx);
              temp[1] += (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag());
              acc_num++;
            }
          }
          MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) = temp[0] / (float)acc_num;
          MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx) = temp[1] / (float)acc_num;
          if (0.0f == MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx)) {
            MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) = 0.001f;
            MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx) = 1.0f;
          } else if (0.0f == MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx)) {
            MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) = MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx) / 1000.0f;
          }
          MEA[ue_cell_idx].CSI_RS_SNR_2nd(r, NZP_idx) = 10.0f * log10f(MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx) / MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx));
          sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          FilePrint("CSI_RS_Measurement_2nd: NZP_idx=%d, SNR[%d]=%f, S=%f, N=%f\n", NZP_idx, r, MEA[ue_cell_idx].CSI_RS_SNR_2nd(r, NZP_idx), MEA[ue_cell_idx].CSI_RS_S_2nd(r, NZP_idx), MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx));
        }
      }
    }
  }
}
