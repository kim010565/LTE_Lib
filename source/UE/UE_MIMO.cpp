/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_MIMO.cpp
    \brief UE解MIMO实现文件
    内容 ：UE_C类解MIMO相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! PSS检测
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PSS_DT(int ue_cell_idx) {
  int i = 0;
  int j = 0;
  int PSS_Candidated_idx = 0;
  int r = 0;
  int Candidated_N_2_ID = 0;
  int Candidated_Pos = 0;
  float Noise = 0;
  float max_value = 0;
  cpxf temp1, temp2, temp3;
  const float *FilterCoef = NULL;
  int SampleDownFactor = 0;
  int offset = 0;
  float AGC_Factor = 0.0f;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  fvec sqr_corr;
  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
    if ((AGC[r] - Last_AGC[r]) >= 0) {
      AGC_Factor = (float)(1 << (AGC[r] - Last_AGC[r]));
    } else {
      AGC_Factor = 1.0f / (float)(1 << (Last_AGC[r] - AGC[r]));
    }
    for (i = 0; i < 5; i++) {
      Cell_Signal_5ms[ue_cell_idx].row(r).segment(i * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
          AGC_Factor * Cell_Signal_5ms[ue_cell_idx].row(r).segment((i + 1) * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num);
    }
    Cell_Signal_5ms[ue_cell_idx].row(r).segment(5 * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
        Cell_Signal[ue_cell_idx].row(r).segment(0, Cell_Specific[ue_cell_idx].OverSample_Num);
  }
  Counter_5ms++;
  if (6 == Counter_5ms) {
    //SampleDown To 1.4M
    if (30720 == Cell_Specific[ue_cell_idx].OverSample_Num) {
      FilterCoef = FilterCoef_20Mto1p4M;
      SampleDownFactor = 16;
    } else if (23040 == Cell_Specific[ue_cell_idx].OverSample_Num) {
      FilterCoef = FilterCoef_15Mto1p4M;
      SampleDownFactor = 12;
    } else if (15360 == Cell_Specific[ue_cell_idx].OverSample_Num) {
      FilterCoef = FilterCoef_10Mto1p4M;
      SampleDownFactor = 8;
    } else if (7680 == Cell_Specific[ue_cell_idx].OverSample_Num) {
      FilterCoef = FilterCoef_5Mto1p4M;
      SampleDownFactor = 4;
    } else if (3840 == Cell_Specific[ue_cell_idx].OverSample_Num) {
      FilterCoef = FilterCoef_3Mto1p4M;
      SampleDownFactor = 2;
    }
    if (1920 != Cell_Specific[ue_cell_idx].OverSample_Num) {
      offset = 16 / SampleDownFactor;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        for (i = 0; i < (5 * 1920 + 128 + 128); i++) {
          Cell_Signal_1p4M_5ms[ue_cell_idx](r, offset + i) = FilterCoef[0] * Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i);
          for (j = 1; j <= 16; j++) {
            Cell_Signal_1p4M_5ms[ue_cell_idx](r, offset + i) += FilterCoef[j] * (Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i - j) + Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i + j));
          }
        }
      }
    } else {
      Cell_Signal_1p4M_5ms[ue_cell_idx] = Cell_Signal_5ms[ue_cell_idx];
    }
    fft_in.resize(2048);
    sqr_corr.resize(1920);
    Noise = 0.0f;
    for (PSS_Candidated_idx = 0; PSS_Candidated_idx < 9; PSS_Candidated_idx++) {
      for (i = 0; i < 6; i++) {
        sqr_corr.setZero();
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          if (i < 5) {
            fft_in = Cell_Signal_1p4M_5ms[ue_cell_idx].row(r).segment(1920 * i, 2048);
          } else {
            fft_in.setZero();
            fft_in.segment(0, 1920) = Cell_Signal_1p4M_5ms[ue_cell_idx].row(r).segment(1920 * i, 1920);
          }
          fft.fwd(fft_out, fft_in);
          fft_in.array() = PSS_FFT_d_2048[PSS_Candidated_idx].array() * fft_out.array();
          fft.inv(fft_out, fft_in);
          if (i < 5) {
            for (j = 0; j < 1920; j++) {
              sqr_corr(j) += fft_out(j).real() * fft_out(j).real() + fft_out(j).imag() * fft_out(j).imag();
              Noise += sqr_corr(j);
            }
          } else {
            for (j = 0; j < 128; j++) {
              sqr_corr(j) += fft_out(j).real() * fft_out(j).real() + fft_out(j).imag() * fft_out(j).imag();
              Noise += sqr_corr(j);
            }
          }
        }
        if (i < 5) {
          for (j = 0; j < 1920; j++) {
            if (sqr_corr(j) > max_value) {
              Candidated_N_2_ID = PSS_Candidated_idx;
              Candidated_Pos = 1920 * i + j;
              max_value = sqr_corr(j);
            }
          }
        } else {
          for (j = 0; j < 128; j++) {
            if (sqr_corr(j) > max_value) {
              Candidated_N_2_ID = PSS_Candidated_idx;
              Candidated_Pos = 1920 * i + j;
              max_value = sqr_corr(j);
            }
          }
        }
      }
    }
    Noise = Noise / (9.0f * (5.0f * 1920.0f + 128.0f));
    Result[ue_cell_idx].PSS_DT_Flag = false;
    MEA[ue_cell_idx].Freq = 0.0f;
    if (max_value > (8.0f * Noise)) {
      Result[ue_cell_idx].PSS_DT_Flag = true;
      Result[ue_cell_idx].N_2_ID = Candidated_N_2_ID % 3;
      if (Candidated_Pos < 500) {
        Candidated_Pos = Candidated_Pos + 5 * 1920;
      }
      Result[ue_cell_idx].PSS_Pos = Candidated_Pos;
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("PSS_DT: N_2_ID=%d, PSS_Pos=%d, ", Result[ue_cell_idx].N_2_ID, Result[ue_cell_idx].PSS_Pos);
      //Freq
      if (Candidated_N_2_ID >= 6) {
        MEA[ue_cell_idx].Freq = 15000;
      } else if (Candidated_N_2_ID >= 3) {
        MEA[ue_cell_idx].Freq = -15000;
      }
      fft_in.resize(128);
      temp3 = 0.0f;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        if (0.0f != MEA[ue_cell_idx].Freq) {
          for (i = 0; i < 128; i++) {
            fft_in(i) = Cell_Signal_1p4M_5ms[ue_cell_idx](r, i + Candidated_Pos) * MyExpCpx_2PI(MEA[ue_cell_idx].Freq * 0.001f / 1920.0f * (float)i);
          }
        } else {
          fft_in = Cell_Signal_1p4M_5ms[ue_cell_idx].row(r).segment(Candidated_Pos, 128);
        }
        for (i = 0; i < 64; i++) {
          temp1 = PSS_FFT_d_128[Result[ue_cell_idx].N_2_ID](i) * fft_in(i);
          temp2 = PSS_FFT_d_128[Result[ue_cell_idx].N_2_ID](128 - 64 + i) * fft_in(128 - 64 + i);
          temp3 += temp1 * conj(temp2);
        }
      }
      MEA[ue_cell_idx].Freq += arg(temp3) / (2.0f * PI * 0.001f / 1920.0f * (128.0f - 64.0f));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("Freq=%f\n", MEA[ue_cell_idx].Freq);
      //CHE
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        for (i = 0; i < 128; i++) {
          fft_in(i) = Cell_Signal_1p4M_5ms[ue_cell_idx](r, i + Candidated_Pos) * MyExpCpx_2PI(MEA[ue_cell_idx].Freq * 0.001f / 1920.0f * (float)i);
        }
        fft.fwd(fft_out, fft_in);
        for (i = 0; i < 31; i++) {
          CHE[ue_cell_idx].PSS_H(r, i) = conj(PSS_d[Result[ue_cell_idx].N_2_ID](i)) * fft_out(128 - 31 + i);
          CHE[ue_cell_idx].PSS_H(r, i + 31) = conj(PSS_d[Result[ue_cell_idx].N_2_ID](i + 31)) * fft_out(1 + i);
        }
      }
    }
  }
}

//! SSS检测
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::SSS_DT(int ue_cell_idx) {
  const int PSS_SSS_Offset_FDD[2] = {-137, -160};
  const int PSS_SSS_Offset_TDD[2] = {-412, -480};
  const int *Sample_Offset = 0;
  int r = 0;
  int i = 0;
  int j = 0;
  int SSS_Candidated_idx = 0;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  cfvec d;
  float max_value = 0.0f;
  int max_idx = 0;
  cpxf temp = 0.0f;
  float sqr_value = 0.0f;
  int m_prime[2] = {0};
  int m_prime_prime[2] = {0};
  int delta_m = 0;
  const int N_1_ID_offset[7] = {0, 30, 59, 87, 114, 140, 165};
  float noise = 0.0f;
  float metric[2];
  int SF = 0;
  cpxf w = 0.0f;
  int m0 = 0;
  int m1 = 0;
  cfvecs fft_out_PSS;
  cfvecs fft_out_SSS;
  fvec PDP;
  Result[ue_cell_idx].SSS_DT_Flag = false;
  if (Result[ue_cell_idx].PSS_DT_Flag && 6 == Counter_5ms) {
    fft_in.resize(128);
    d.resize(62);
    Sample_Offset = (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? PSS_SSS_Offset_FDD : PSS_SSS_Offset_TDD;
    metric[0] = 0.0f;
    metric[1] = 0.0f;
    for (SSS_Candidated_idx = 0; SSS_Candidated_idx < 2; SSS_Candidated_idx++)  //NoramlCP, ExtendedCP
    {
      d.setZero();
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        for (i = 0; i < 128; i++) {
          fft_in(i) = Cell_Signal_1p4M_5ms[ue_cell_idx](r, i + Result[ue_cell_idx].PSS_Pos + Sample_Offset[SSS_Candidated_idx]) * MyExpCpx_2PI(MEA[ue_cell_idx].Freq * 0.001f / 1920.0f * (float)i);
        }
        fft.fwd(fft_out, fft_in);
        for (i = 0; i < 31; i++) {
          d(i) += conj(CHE[ue_cell_idx].PSS_H(r, i)) * fft_out(128 - 31 + i);  //低SNR下，需要通过多帧相干合并来提升性能；需要注意SF=0，SF=5的奇偶变换
          d(i + 31) += conj(CHE[ue_cell_idx].PSS_H(r, i + 31)) * fft_out(1 + i);
        }
      }
      //c0, c1
      for (i = 0; i < 31; i++) {
        d(2 * i) = d(2 * i) * conj(SSS_c((i + Result[ue_cell_idx].N_2_ID) % 31));
        d(2 * i + 1) = d(2 * i + 1) * conj(SSS_c((i + Result[ue_cell_idx].N_2_ID + 3) % 31));
      }
      //max d(2n) => m0 or m1
      max_value = 0.0f;
      max_idx = 0;
      noise = 0.0f;
      for (i = 0; i < 31; i++)  //Candidated m0 or m1
      {
        temp = 0.0f;
        for (j = 0; j < 31; j++) {
          temp += d(2 * j) * conj(SSS_s((j + i) % 31));
        }
        sqr_value = temp.real() * temp.real() + temp.imag() * temp.imag();
        noise += sqr_value;
        if (sqr_value > max_value) {
          max_value = sqr_value;
          max_idx = i;
        }
      }
      noise = (noise - max_value) / 30.0f;
      metric[SSS_Candidated_idx] += max_value / noise;
      m_prime[SSS_Candidated_idx] = max_idx;
      //z0, z1
      for (i = 0; i < 31; i++) {
        d(2 * i + 1) = d(2 * i + 1) * conj(SSS_z((i + (max_idx & 7)) % 31));
      }
      //max d(2n+1) => m0 or m1
      max_value = 0.0f;
      max_idx = 0;
      noise = 0.0f;
      for (i = 0; i < 31; i++)  //Candidated m0 or m1
      {
        temp = 0.0f;
        for (j = 0; j < 31; j++) {
          temp += d(2 * j + 1) * conj(SSS_s((j + i) % 31));
        }
        sqr_value = temp.real() * temp.real() + temp.imag() * temp.imag();
        noise += sqr_value;
        if (sqr_value > max_value) {
          max_value = sqr_value;
          max_idx = i;
        }
      }
      noise = (noise - max_value) / 30.0f;
      metric[SSS_Candidated_idx] += max_value / noise;
      m_prime_prime[SSS_Candidated_idx] = max_idx;
    }
    if (metric[0] >= (2.0f * metric[1])) {
      SSS_Candidated_idx = 0;
      Result[ue_cell_idx].CP_Type = NormalCP;
      if (m_prime[0] < m_prime_prime[0]) {
        SF = 0;
        delta_m = m_prime_prime[0] - m_prime[0];
        if (delta_m >= 1 && delta_m <= 7) {
          Result[ue_cell_idx].N_1_ID = m_prime[0] + N_1_ID_offset[delta_m - 1];
          Result[ue_cell_idx].SSS_DT_Flag = true;
        } else {
          Result[ue_cell_idx].SSS_DT_Flag = false;
        }
      } else {
        SF = 5;
        delta_m = m_prime[0] - m_prime_prime[0];
        if (delta_m >= 1 && delta_m <= 7) {
          Result[ue_cell_idx].N_1_ID = m_prime_prime[0] + N_1_ID_offset[delta_m - 1];
          Result[ue_cell_idx].SSS_DT_Flag = true;
        } else {
          Result[ue_cell_idx].SSS_DT_Flag = false;
        }
      }
    } else if (metric[1] >= (2.0f * metric[0])) {
      SSS_Candidated_idx = 1;
      Result[ue_cell_idx].CP_Type = ExtendedCP;
      if (m_prime[1] < m_prime_prime[1]) {
        SF = 0;
        delta_m = m_prime_prime[1] - m_prime[1];
        if (delta_m >= 1 && delta_m <= 7) {
          Result[ue_cell_idx].N_1_ID = m_prime[1] + N_1_ID_offset[delta_m - 1];
          Result[ue_cell_idx].SSS_DT_Flag = true;
        } else {
          Result[ue_cell_idx].SSS_DT_Flag = false;
        }
      } else {
        SF = 5;
        delta_m = m_prime[1] - m_prime_prime[1];
        if (delta_m >= 1 && delta_m <= 7) {
          Result[ue_cell_idx].N_1_ID = m_prime_prime[1] + N_1_ID_offset[delta_m - 1];
          Result[ue_cell_idx].SSS_DT_Flag = true;
        } else {
          Result[ue_cell_idx].SSS_DT_Flag = false;
        }
      }
    }
    if (Result[ue_cell_idx].SSS_DT_Flag) {
      Result[ue_cell_idx].N_cell_ID = 3 * Result[ue_cell_idx].N_1_ID + Result[ue_cell_idx].N_2_ID;
      //CHE
      m0 = SSS_m0_m1[Result[ue_cell_idx].N_1_ID][0];
      m1 = SSS_m0_m1[Result[ue_cell_idx].N_1_ID][1];
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        for (i = 0; i < 128; i++) {
          fft_in(i) = Cell_Signal_1p4M_5ms[ue_cell_idx](r, i + Result[ue_cell_idx].PSS_Pos + Sample_Offset[SSS_Candidated_idx]) * MyExpCpx_2PI(MEA[ue_cell_idx].Freq * 0.001f / 1920.0f * (float)i);
        }
        fft.fwd(fft_out, fft_in);
        for (i = 0; i < 31; i++) {
          d(i) = fft_out(128 - 31 + i);
          d(i + 31) = fft_out(1 + i);
        }
        if (0 == SF) {
          for (i = 0; i < 31; i++) {
            CHE[ue_cell_idx].SSS_H(r, 2 * i) = conj(SSS_s((i + m0) % 31) * SSS_c((i + Result[ue_cell_idx].N_2_ID) % 31)) * d(2 * i);
            CHE[ue_cell_idx].SSS_H(r, 2 * i + 1) = conj(SSS_s((i + m1) % 31) * SSS_c((i + Result[ue_cell_idx].N_2_ID + 3) % 31) * SSS_z((i + (m0 & 7)) % 31)) * d(2 * i + 1);
          }
        } else {
          for (i = 0; i < 31; i++) {
            CHE[ue_cell_idx].SSS_H(r, 2 * i) = conj(SSS_s((i + m1) % 31) * SSS_c((i + Result[ue_cell_idx].N_2_ID) % 31)) * d(2 * i);
            CHE[ue_cell_idx].SSS_H(r, 2 * i + 1) = conj(SSS_s((i + m0) % 31) * SSS_c((i + Result[ue_cell_idx].N_2_ID + 3) % 31) * SSS_z((i + (m1 & 7)) % 31)) * d(2 * i + 1);
          }
        }
      }
      //Freq
      fft_in.resize(64);
      fft_in.setZero();
      fft_out_PSS.resize(UE_Specific[ue_cell_idx].RxNum);
      fft_out_SSS.resize(UE_Specific[ue_cell_idx].RxNum);
      PDP.resize(64);
      PDP.setZero();
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        for (i = 0; i < 62; i++) {
          fft_in(i) = CHE[ue_cell_idx].PSS_H(r, i);
        }
        fft.inv(fft_out_PSS[r], fft_in);
        for (i = 0; i < 62; i++) {
          fft_in(i) = CHE[ue_cell_idx].SSS_H(r, i);
        }
        fft.inv(fft_out_SSS[r], fft_in);
        for (i = 0; i < 64; i++) {
          PDP(i) += fft_out_PSS[r](i).real() * fft_out_PSS[r](i).real() + fft_out_PSS[r](i).imag() * fft_out_PSS[r](i).imag();
          PDP(i) += fft_out_SSS[r](i).real() * fft_out_SSS[r](i).real() + fft_out_SSS[r](i).imag() * fft_out_SSS[r](i).imag();
        }
      }
      max_value = 0.0f;
      max_idx = 0;
      for (i = 0; i < 64; i++) {
        if (max_value < PDP(i)) {
          max_value = PDP(i);
          max_idx = i;
        }
      }
      temp = 0.0f;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        temp += fft_out_SSS[r](max_idx) * conj(fft_out_PSS[r](max_idx));
      }
      MEA[ue_cell_idx].Freq = arg(temp) / (2.0f * PI * 0.001f / 1920.0f * (-Sample_Offset[SSS_Candidated_idx]));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("SSS_DT: SF=%d, CP_Type=%s, N_1_ID=%d, N_cell_ID=%d, Freq=%f\n", SF, (0 == SSS_Candidated_idx) ? "NormalCP" : "ExtendedCP", Result[ue_cell_idx].N_1_ID, Result[ue_cell_idx].N_cell_ID, MEA[ue_cell_idx].Freq);
    }
    Feedback[ue_cell_idx].RxFreqOffset += MEA[ue_cell_idx].Freq;
    for (i = 5 * Cell_Specific[ue_cell_idx].OverSample_Num; i < 6 * Cell_Specific[ue_cell_idx].OverSample_Num; i++) {
      Phase[ue_cell_idx] += (double)MEA[ue_cell_idx].Freq * Cell_Specific[ue_cell_idx].OverSample_unit;
      w = MyExpCpx_2PI((float)Phase[ue_cell_idx]);
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        Cell_Signal_5ms[ue_cell_idx](r, i) *= w;
      }
    }
  }
  if (6 == Counter_5ms) {
    Counter_5ms = 1;
  }
}

//! PBCH盲检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PBCH_DMP_MIMO_DCC_Blind(int ue_cell_idx) {
  const int RS_Num[4] = {2, 2, 1, 1};
  const int v[4][2] = {{0, 3}, {3, 0}, {3, 0}, {6, 0}};
  int i = 0;
  int j = 0;
  int r = 0;
  int l = 0;
  int p = 0;
  int subframe_idx = 0;
  int l_idx = 0;
  int v_shift = 0;
  int n_s = 0;
  uint32 c_init = 0;
  int v_mod_6 = 0;
  int m = 0;
  int k = 0;
  int m_prime = 0;
  int offset = 0;
  int k_offset = 0;
  uint32 cinit = 0;
  cpxf temp = 0.0f;
  int RS_Pos[4][2];
  int offset_Candidated = 0;
  int SFN_Low2bit = 0;
  int offset_seg = 0;
  const int start_idx_const[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 2, 1}, {0, 3, 2, 1}};    //start_idx_const[Counter_5ms-1][offset_Candidated]
  const int offset_idx_const[4][4] = {{0, 1, 2, 3}, {0, 1, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};   //offset_idx_const[Counter_5ms-1][offset_Candidated]
  const int SFN_Low2bit_const[4][4] = {{0, 1, 2, 3}, {1, 2, 3, 0}, {2, 3, 0, 1}, {3, 0, 1, 2}};  //SFN_Low2bit_const[Counter_5ms-1][offset_Candidated]
  int start_idx = 0;
  int end_idx = 0;
  int offset_idx = 0;
  const float *FilterCoef = NULL;
  int SampleDownFactor = 0;
  float AGC_Factor = 0.0f;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  bvec c;
  cfmats LS_H;           //LS_H[r*4 + p](l, sc)
  cfmats CHE_H;          //CHE_H[r*4 + p](l, sc)
  cfmat MIMO_symbs_in;   //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;         //MIMO_H[r](p, idx)
  cfvec MIMO_symbs_out;  //MIMO_symbs_out(idx)
  cfmat h;               //h(r, p)
  if (0 == Cell_Specific[ue_cell_idx].n_subframe) {
    if (Counter_5ms >= 0 && Counter_5ms < 4) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        if ((AGC[r] - Last_AGC_PBCH[r]) >= 0) {
          AGC_Factor = (float)(1 << (AGC[r] - Last_AGC_PBCH[r]));
        } else {
          AGC_Factor = 1.0f / (float)(1 << (Last_AGC_PBCH[r] - AGC[r]));
        }
        for (i = 0; i < Counter_5ms; i++) {
          Cell_Signal_5ms[ue_cell_idx].row(r).segment(i * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
              AGC_Factor * Cell_Signal_5ms[ue_cell_idx].row(r).segment((i)*Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num);
        }
        Cell_Signal_5ms[ue_cell_idx].row(r).segment(Counter_5ms * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
            Cell_Signal[ue_cell_idx].row(r).segment(0, Cell_Specific[ue_cell_idx].OverSample_Num);
      }
      Last_AGC_PBCH[r] = AGC[r];
    } else if (4 == Counter_5ms) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        if ((AGC[r] - Last_AGC_PBCH[r]) >= 0) {
          AGC_Factor = 1.0f / (float)(1 << (AGC[r] - Last_AGC_PBCH[r]));
          for (i = 0; i < 3; i++) {
            Cell_Signal_5ms[ue_cell_idx].row(r).segment(i * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
                Cell_Signal_5ms[ue_cell_idx].row(r).segment((i + 1) * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num);
          }
          Cell_Signal_5ms[ue_cell_idx].row(r).segment(3 * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
              AGC_Factor * Cell_Signal[ue_cell_idx].row(r).segment(0, Cell_Specific[ue_cell_idx].OverSample_Num);
        } else {
          AGC_Factor = 1.0f / (float)(1 << (Last_AGC_PBCH[r] - AGC[r]));
          for (i = 0; i < 3; i++) {
            Cell_Signal_5ms[ue_cell_idx].row(r).segment(i * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
                AGC_Factor * Cell_Signal_5ms[ue_cell_idx].row(r).segment((i + 1) * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num);
          }
          Cell_Signal_5ms[ue_cell_idx].row(r).segment(3 * Cell_Specific[ue_cell_idx].OverSample_Num, Cell_Specific[ue_cell_idx].OverSample_Num) =
              Cell_Signal[ue_cell_idx].row(r).segment(0, Cell_Specific[ue_cell_idx].OverSample_Num);
        }
      }
      Last_AGC_PBCH[r] = AGC[r];
    }
    Counter_5ms++;
    if (Counter_5ms >= 4) {
      Counter_5ms = 4;
    }
    if (Counter_5ms > 0) {
      //SampleDown To 1.4M
      if (30720 == Cell_Specific[ue_cell_idx].OverSample_Num) {
        FilterCoef = FilterCoef_20Mto1p4M;
        SampleDownFactor = 16;
      } else if (23040 == Cell_Specific[ue_cell_idx].OverSample_Num) {
        FilterCoef = FilterCoef_15Mto1p4M;
        SampleDownFactor = 12;
      } else if (15360 == Cell_Specific[ue_cell_idx].OverSample_Num) {
        FilterCoef = FilterCoef_10Mto1p4M;
        SampleDownFactor = 8;
      } else if (7680 == Cell_Specific[ue_cell_idx].OverSample_Num) {
        FilterCoef = FilterCoef_5Mto1p4M;
        SampleDownFactor = 4;
      } else if (3840 == Cell_Specific[ue_cell_idx].OverSample_Num) {
        FilterCoef = FilterCoef_3Mto1p4M;
        SampleDownFactor = 2;
      }
      if (1920 != Cell_Specific[ue_cell_idx].OverSample_Num) {
        offset = 16 / SampleDownFactor;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (i = 0; i < (Counter_5ms * 1920); i++) {
            Cell_Signal_1p4M_5ms[ue_cell_idx](r, offset + i) = FilterCoef[0] * Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i);
            for (j = 1; j <= 16; j++) {
              Cell_Signal_1p4M_5ms[ue_cell_idx](r, offset + i) += FilterCoef[j] * (Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i - j) + Cell_Signal_5ms[ue_cell_idx](r, 16 + SampleDownFactor * i + j));
            }
          }
        }
      } else {
        Cell_Signal_1p4M_5ms[ue_cell_idx] = Cell_Signal_5ms[ue_cell_idx];
      }
      fft_in.resize(128);
      LS_H.resize(UE_Specific[ue_cell_idx].RxNum * 4);
      CHE_H.resize(UE_Specific[ue_cell_idx].RxNum * 4);
      for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * 4); i++) {
        LS_H[i].resize(12, 2);
        CHE_H[i].resize(6 * 12, 5);
      }
      //Front & CHE
      RS_Pos[0][0] = RS_Pos[1][0] = Cell_Specific[ue_cell_idx].N_DL_symb;
      RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 3;
      RS_Pos[2][0] = RS_Pos[3][0] = Cell_Specific[ue_cell_idx].N_DL_symb + 1;
      v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 6;
      MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 960 : 864);
      MIMO_symbs_in.setZero();
      MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        MIMO_H[r].resize(4, (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 960 : 864);
        MIMO_H[r].setZero();
      }
      MIMO_symbs_out.resize((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 960 : 864);
      h.resize(UE_Specific[ue_cell_idx].RxNum, 4);
      for (subframe_idx = 0; subframe_idx < Counter_5ms; subframe_idx++) {
        for (l = Cell_Specific[ue_cell_idx].N_DL_symb; l < (Cell_Specific[ue_cell_idx].N_DL_symb + 5); l++) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
              fft_in = Cell_Signal_1p4M_5ms[ue_cell_idx].row(r).segment(subframe_idx * 1920 + 960 + 10 + (l - Cell_Specific[ue_cell_idx].N_DL_symb) * 137, 128);
            } else {
              fft_in = Cell_Signal_1p4M_5ms[ue_cell_idx].row(r).segment(subframe_idx * 1920 + 960 + 32 + (l - Cell_Specific[ue_cell_idx].N_DL_symb) * 160, 128);
            }
            fft.fwd(fft_out, fft_in);
            RE[r].col(l).segment(0, 6 * 12 / 2) = fft_out.segment(128 - 6 * 12 / 2, 6 * 12 / 2);
            RE[r].col(l).segment(6 * 12 / 2, 6 * 12 / 2) = fft_out.segment(1, 6 * 12 / 2);
          }
        }
        for (p = 0; p < 4; p++) {
          for (l_idx = 0; l_idx < RS_Num[p]; l_idx++) {
            l = RS_Pos[p][l_idx];
            n_s = 1;
            c_init = 1024 * (7 * (n_s + 1) + (l % Cell_Specific[ue_cell_idx].N_DL_symb) + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1) + 2 * Cell_Specific[ue_cell_idx].N_cell_ID + Cell_Specific[ue_cell_idx].N_CP;
            PNSequence_Gen(c_init, c, 4 * N_MAX_DL_RB);
            v_mod_6 = (v[p][l_idx] + v_shift) % 6;
            offset = N_MAX_DL_RB - 6;
            for (m = 0; m < 2 * 6; m++) {
              k = 6 * m + v_mod_6;
              m_prime = 2 * (m + offset);
              temp = cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                LS_H[r * 4 + p](m, l_idx) = RE[r](k, l) * temp;
              }
            }
            k = (v[p][l_idx] + v_shift) % 6;
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              for (i = 0; i < (6 * 12); i++) {
                if (i <= k) {
                  CHE_H[r * 4 + p](i, l - Cell_Specific[ue_cell_idx].N_DL_symb) = LS_H[r * 4 + p](0, l_idx);
                } else if (i >= (k + (6 * 11))) {
                  CHE_H[r * 4 + p](i, l - Cell_Specific[ue_cell_idx].N_DL_symb) = LS_H[r * 4 + p](11, l_idx);
                } else {
                  j = (i - k) % 6;
                  CHE_H[r * 4 + p](i, l - Cell_Specific[ue_cell_idx].N_DL_symb) = (float)(6 - j) / 6.0f * LS_H[r * 4 + p]((i - k) / 6, l_idx) + (float)j / 6.0f * LS_H[r * 4 + p]((i - k) / 6 + 1, l_idx);
                }
              }
            }
          }
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            if (p < 2) {
              if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
                CHE_H[r * 4 + p].col(1) = 0.75f * CHE_H[r * 4 + p].col(0) + 0.25 * CHE_H[r * 4 + p].col(4);
                CHE_H[r * 4 + p].col(2) = 0.5f * CHE_H[r * 4 + p].col(0) + 0.5f * CHE_H[r * 4 + p].col(4);
                CHE_H[r * 4 + p].col(3) = 0.25f * CHE_H[r * 4 + p].col(0) + 0.75f * CHE_H[r * 4 + p].col(4);
              } else {
                CHE_H[r * 4 + p].col(1) = (2.0f / 3.0f) * CHE_H[r * 4 + p].col(0) + (1.0f / 3.0f) * CHE_H[r * 4 + p].col(3);
                CHE_H[r * 4 + p].col(2) = (1.0f / 3.0f) * CHE_H[r * 4 + p].col(0) + (2.0f / 3.0f) * CHE_H[r * 4 + p].col(3);
              }
            } else {
              CHE_H[r * 4 + p].col(0) = CHE_H[r * 4 + p].col(1);
              CHE_H[r * 4 + p].col(2) = CHE_H[r * 4 + p].col(1);
              CHE_H[r * 4 + p].col(3) = CHE_H[r * 4 + p].col(1);
            }
          }
        }
        //DMP
        k_offset = 6 * 12 / 2 - 36;
        v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
        i = subframe_idx * ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 240 : 216);
        for (l = Cell_Specific[ue_cell_idx].N_DL_symb; l < (Cell_Specific[ue_cell_idx].N_DL_symb + 4); l++) {
          for (k = k_offset; k < (k_offset + 72); k++) {
            if (v_shift != (k % 3) || (l >= (Cell_Specific[ue_cell_idx].N_DL_symb + 2) && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || ((Cell_Specific[ue_cell_idx].N_DL_symb + 2) == l && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type)) {
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                MIMO_symbs_in(r, i) = RE[r](k, l);
                for (p = 0; p < 4; p++) {
                  MIMO_H[r](p, i) = CHE_H[r * 4 + p](k, l - Cell_Specific[ue_cell_idx].N_DL_symb);
                }
              }
              i++;
            }
          }
        }
      }
      //Blind DT: Single_Antenna
      offset_seg = ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 240 : 216);
      Result[ue_cell_idx].PBCH_Blind_Flag = false;
      for (offset_Candidated = 0; offset_Candidated < 4; offset_Candidated++) {
        if (Result[ue_cell_idx].PBCH_Blind_Flag) {
          break;
        }
        //MIMO
        MIMO_symbs_out.setZero();
        start_idx = start_idx_const[Counter_5ms - 1][offset_Candidated];
        offset_idx = offset_idx_const[Counter_5ms - 1][offset_Candidated];
        end_idx = Counter_5ms;
        SFN_Low2bit = SFN_Low2bit_const[Counter_5ms - 1][offset_Candidated];
        for (i = start_idx * offset_seg, j = offset_idx * offset_seg; i < end_idx * offset_seg; i++, j++) {
          MIMO_symbs_out(j) = 0.0f;
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            MIMO_symbs_out(j) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i));
          }
        }
        //LLR
        DeModulation_QPSK(MIMO_symbs_out, V_softbits);
        //DeScrambling
        cinit = Cell_Specific[ue_cell_idx].N_cell_ID;
        DeScrambling(cinit, V_softbits);
        //DCC
        PBCH_DCC_Blind(ue_cell_idx, 1, SFN_Low2bit);
      }
      //Blind DT: Transmit_Diversity 2X
      for (offset_Candidated = 0; offset_Candidated < 4; offset_Candidated++) {
        if (Result[ue_cell_idx].PBCH_Blind_Flag) {
          break;
        }
        //MIMO
        MIMO_symbs_out.setZero();
        start_idx = start_idx_const[Counter_5ms - 1][offset_Candidated];
        offset_idx = offset_idx_const[Counter_5ms - 1][offset_Candidated];
        end_idx = Counter_5ms;
        SFN_Low2bit = SFN_Low2bit_const[Counter_5ms - 1][offset_Candidated];
        for (i = start_idx * offset_seg / 2, j = offset_idx * offset_seg / 2; i < end_idx * offset_seg / 2; i++, j++) {
          MIMO_symbs_out(2 * i) = 0.0f;
          MIMO_symbs_out(2 * i + 1) = 0.0f;
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            for (p = 0; p < 2; p++) {
              h(r, p) = (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
            }
            MIMO_symbs_out(2 * j) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1))));
            MIMO_symbs_out(2 * j + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1)));
          }
        }
        //LLR
        DeModulation_QPSK(MIMO_symbs_out, V_softbits);
        //DeScrambling
        cinit = Cell_Specific[ue_cell_idx].N_cell_ID;
        DeScrambling(cinit, V_softbits);
        //DCC
        PBCH_DCC_Blind(ue_cell_idx, 2, SFN_Low2bit);
      }
      //Blind DT: Transmit_Diversity 4X
      for (offset_Candidated = 0; offset_Candidated < 4; offset_Candidated++) {
        if (Result[ue_cell_idx].PBCH_Blind_Flag) {
          break;
        }
        //MIMO
        MIMO_symbs_out.setZero();
        start_idx = start_idx_const[Counter_5ms - 1][offset_Candidated];
        offset_idx = offset_idx_const[Counter_5ms - 1][offset_Candidated];
        end_idx = Counter_5ms;
        SFN_Low2bit = SFN_Low2bit_const[Counter_5ms - 1][offset_Candidated];
        for (i = start_idx * offset_seg / 4, j = offset_idx * offset_seg / 4; i < end_idx * offset_seg / 4; i++, j++) {
          MIMO_symbs_out(4 * i) = 0.0f;
          MIMO_symbs_out(4 * i + 1) = 0.0f;
          MIMO_symbs_out(4 * i + 2) = 0.0f;
          MIMO_symbs_out(4 * i + 3) = 0.0f;
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            for (p = 0; p < 4; p++) {
              if (0 == p || 2 == p) {
                h(r, p) = (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
              } else {
                h(r, p) = (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
              }
            }
            MIMO_symbs_out(4 * j) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1))));
            MIMO_symbs_out(4 * j + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1)));
            MIMO_symbs_out(4 * j + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3))));
            MIMO_symbs_out(4 * j + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3)));
          }
        }
        //LLR
        DeModulation_QPSK(MIMO_symbs_out, V_softbits);
        //DeScrambling
        cinit = Cell_Specific[ue_cell_idx].N_cell_ID;
        DeScrambling(cinit, V_softbits);
        //DCC
        PBCH_DCC_Blind(ue_cell_idx, 4, SFN_Low2bit);
      }
    }
  }
}

//! PBCH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PBCH_DMP_MIMO_DCC(int ue_cell_idx) {
  int i = 0;
  int j = 0;
  int r = 0;
  int l = 0;
  int p = 0;
  int vshift = 0;
  int k = 0;
  int offset_seg = 0;
  int k_offset = 0;
  uint32 cinit = 0;
  int SFN_Low2bit = 0;
  cfmat MIMO_symbs_in;  //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;        //MIMO_H[r](p, idx)
  cfmat h;              //h(r, p)
  fvec R_sigma2;
  if (0 == Cell_Specific[ue_cell_idx].n_subframe) {
    SFN_Low2bit = Cell_Specific[ue_cell_idx].n_f & 3;
    MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 240 : 216);
    MIMO_symbs_in.setZero();
    MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MIMO_H[r].resize(Cell_Specific[ue_cell_idx].CRSPortNum, (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 240 : 216);
      MIMO_H[r].setZero();
    }
    R_sigma2.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      R_sigma2(r) = 1.0f / MEA[ue_cell_idx].CRS_N_2nd(r);
    }
    h.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].CRSPortNum);
    //DMP
    k_offset = Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2 - 36;
    vshift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    i = 0;
    for (l = Cell_Specific[ue_cell_idx].N_DL_symb; l < (Cell_Specific[ue_cell_idx].N_DL_symb + 4); l++) {
      for (k = k_offset; k < (k_offset + 72); k++) {
        if (vshift != (k % 3) || (l >= (Cell_Specific[ue_cell_idx].N_DL_symb + 2) && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || ((Cell_Specific[ue_cell_idx].N_DL_symb + 2) == l && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type)) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            MIMO_symbs_in(r, i) = RE[r](k, l);
            for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
              MIMO_H[r](p, i) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, l);
            }
          }
          i++;
        }
        if (CRS != RE_Type(k, l)) {
          RE_Type(k, l) = PBCH;
        }
      }
    }
    //MIMO
    if (0 == SFN_Low2bit) {
      Result[ue_cell_idx].MIMO_symbs_out.setZero();
    }
    offset_seg = ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 240 : 216);
    if (1 == Cell_Specific[ue_cell_idx].CRSPortNum)  //Single_Antenna_Port
    {
      for (i = 0, j = SFN_Low2bit * offset_seg; i < offset_seg; i++, j++) {
        Result[ue_cell_idx].MIMO_symbs_out(j) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          Result[ue_cell_idx].MIMO_symbs_out(j) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i) * R_sigma2(r));
        }
      }
    } else if (2 == Cell_Specific[ue_cell_idx].CRSPortNum)  //2x Transmit_Diversity
    {
      for (i = 0, j = SFN_Low2bit * offset_seg / 2; i < offset_seg / 2; i++, j++) {
        Result[ue_cell_idx].MIMO_symbs_out(2 * j) = 0.0f;
        Result[ue_cell_idx].MIMO_symbs_out(2 * j + 1) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < 2; p++) {
            h(r, p) = (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
          }
          Result[ue_cell_idx].MIMO_symbs_out(2 * j) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1)) * R_sigma2(r)));
          Result[ue_cell_idx].MIMO_symbs_out(2 * j + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1) * R_sigma2(r)));
        }
      }
    } else  //4x Transmit_Diversity
    {
      for (i = 0, j = SFN_Low2bit * offset_seg / 4; i < offset_seg / 4; i++, j++) {
        Result[ue_cell_idx].MIMO_symbs_out(4 * j) = 0.0f;
        Result[ue_cell_idx].MIMO_symbs_out(4 * j + 1) = 0.0f;
        Result[ue_cell_idx].MIMO_symbs_out(4 * j + 2) = 0.0f;
        Result[ue_cell_idx].MIMO_symbs_out(4 * j + 3) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < 4; p++) {
            if (0 == p || 2 == p) {
              h(r, p) = (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
            } else {
              h(r, p) = (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
            }
          }
          Result[ue_cell_idx].MIMO_symbs_out(4 * j) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1)) * R_sigma2(r)));
          Result[ue_cell_idx].MIMO_symbs_out(4 * j + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1) * R_sigma2(r)));
          Result[ue_cell_idx].MIMO_symbs_out(4 * j + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3)) * R_sigma2(r)));
          Result[ue_cell_idx].MIMO_symbs_out(4 * j + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3) * R_sigma2(r)));
        }
      }
    }
    if (3 == SFN_Low2bit) {
      //LLR
      DeModulation_QPSK(Result[ue_cell_idx].MIMO_symbs_out, V_softbits);
      //DeScrambling
      cinit = Cell_Specific[ue_cell_idx].N_cell_ID;
      DeScrambling(cinit, V_softbits);
      //DCC
      PBCH_DCC(ue_cell_idx, Cell_Specific[ue_cell_idx].CRSPortNum);
    }
  }
}

//! PCFICH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PCFICH_DMP_MIMO_DCC(int ue_cell_idx) {
  uint32 cinit = 0;
  int i = 0;
  int j = 0;
  int p = 0;
  int r = 0;
  int k = 0;
  int k_bar = 0;
  int k_offset = 0;
  int vshift = 0;
  cfmat MIMO_symbs_in;   //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;         //MIMO_H[r](p, idx)
  cfvec MIMO_symbs_out;  //MIMO_symbs_out(idx)
  cfmat h;               //h(r, p)
  fvec R_sigma2;
  //DMP
  if (U != Cell_Specific[ue_cell_idx].SubframeType) {
    k_bar = Cell_Specific[ue_cell_idx].N_RB_sc / 2 * (Cell_Specific[ue_cell_idx].N_cell_ID % (2 * Cell_Specific[ue_cell_idx].N_DL_RB));
    vshift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, 16);
    MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MIMO_H[r].resize(Cell_Specific[ue_cell_idx].CRSPortNum, 16);
    }
    R_sigma2.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      R_sigma2(r) = 1.0f / MEA[ue_cell_idx].CRS_N_2nd(r);
    }
    for (i = 0; i < 4; i++) {
      k_offset = k_bar + (i * Cell_Specific[ue_cell_idx].N_DL_RB / 2) * Cell_Specific[ue_cell_idx].N_RB_sc / 2;
      j = 0;
      for (k = k_offset; k < (k_offset + 6); k++) {
        if (vshift != (k % 3)) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            MIMO_symbs_in(r, 4 * i + j) = RE[r](k, 0);
            for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
              MIMO_H[r](p, 4 * i + j) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, 0);
            }
          }
          RE_Type(k, 0) = PCFICH;
          j++;
        }
      }
    }
    //MIMO
    MIMO_symbs_out.resize(16);
    h.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].CRSPortNum);
    if (1 == Cell_Specific[ue_cell_idx].CRSPortNum)  //Single_Antenna_Port
    {
      for (i = 0; i < 16; i++) {
        MIMO_symbs_out(i) = 0;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          MIMO_symbs_out(i) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i) * R_sigma2(r));
        }
      }
    } else if (2 == Cell_Specific[ue_cell_idx].CRSPortNum)  //2x Transmit_Diversity
    {
      for (i = 0; i < 8; i++) {
        MIMO_symbs_out(2 * i) = 0.0f;
        MIMO_symbs_out(2 * i + 1) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            h(r, p) = (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
          }
          MIMO_symbs_out(2 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1))) * R_sigma2(r));
          MIMO_symbs_out(2 * i + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1)) * R_sigma2(r));
        }
      }
    } else  //4x Transmit_Diversity
    {
      for (i = 0; i < 4; i++) {
        MIMO_symbs_out(4 * i) = 0.0f;
        MIMO_symbs_out(4 * i + 1) = 0.0f;
        MIMO_symbs_out(4 * i + 2) = 0.0f;
        MIMO_symbs_out(4 * i + 3) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            if (0 == p || 2 == p) {
              h(r, p) = (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
            } else {
              h(r, p) = (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
            }
          }
          MIMO_symbs_out(4 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1))) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1)) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3))) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3)) * R_sigma2(r));
        }
      }
    }
    //LLR
    DeModulation_QPSK(MIMO_symbs_out, V_softbits);
    //DeScrambling
    cinit = (((Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1)) << 9) + Cell_Specific[ue_cell_idx].N_cell_ID;
    DeScrambling(cinit, V_softbits);
    //DCC
    PCFICH_DCC(ue_cell_idx);
  }
}

//! PHICH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PHICH_DMP_MIMO_DCC(int ue_cell_idx) {
  const float N_g_map[4] = {1.0f / 6.0f, 1.0f / 2.0f, 1.0f, 2.0f};
  float N_g = N_g_map[Cell_Specific[ue_cell_idx].phich_Resource];
  int N_group_PHICH = 0;
  int N_PHICH_SF = 0;
  int M_symb = 0;
  int i = 0;
  int j = 0;
  int p = 0;
  int r = 0;
  uint32 cinit = 0;
  int m_prime = 0;
  int m_prime_num = 0;
  int l_prime = 0;
  int n[3] = {0, 0, 0};
  std::vector<int> n_REG[3];
  int k_prime = 0;
  int k_prime_offset = 0;
  int n_bar_i = 0;
  int k_bar = 0;
  int vshift = 0;
  int REG_Idx0 = 0;
  int REG_Idx1 = 0;
  int resource_idx = 0;
  int PHICH_idx = 0;
  cfvec z;
  cfvec d;
  bvec c;
  cfmat y;
  cfmat MIMO_symbs_in;   //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;         //MIMO_H[r](p, idx)
  cfvec MIMO_symbs_out;  //MIMO_symbs_out(idx)
  cfmat h;               //h(r, p)
  fvec R_sigma2;
  if (U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      N_group_PHICH = ceil(N_g * ((float)Cell_Specific[ue_cell_idx].N_DL_RB / 8.0f));
      N_PHICH_SF = 4;
    } else {
      N_group_PHICH = 2 * ceil(N_g * ((float)Cell_Specific[ue_cell_idx].N_DL_RB / 8.0f));
      N_PHICH_SF = 2;
    }
    M_symb = N_PHICH_SF * 3;
    if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType) {
      N_group_PHICH = N_group_PHICH * PHICH_mi[(int)Cell_Specific[ue_cell_idx].ULDLConfiguration][Cell_Specific[ue_cell_idx].n_subframe];
    }
    if (N_group_PHICH > 0) {
      n[0] = Cell_Specific[ue_cell_idx].N_DL_RB * 2 - 4;
      n[1] = (4 == Cell_Specific[ue_cell_idx].CRSPortNum) ? (Cell_Specific[ue_cell_idx].N_DL_RB * 2) : (Cell_Specific[ue_cell_idx].N_DL_RB * 3);
      n[2] = Cell_Specific[ue_cell_idx].N_DL_RB * 3;
      n_REG[0].resize(n[0]);
      n_REG[1].resize(n[1]);
      n_REG[2].resize(n[2]);
      vshift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
      k_bar = Cell_Specific[ue_cell_idx].N_RB_sc / 2 * (Cell_Specific[ue_cell_idx].N_cell_ID % (2 * Cell_Specific[ue_cell_idx].N_DL_RB));
      REG_Idx0 = 0;
      REG_Idx1 = k_bar / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (Cell_Specific[ue_cell_idx].N_DL_RB / 2) * Cell_Specific[ue_cell_idx].N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 1] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (2 * Cell_Specific[ue_cell_idx].N_DL_RB / 2) * Cell_Specific[ue_cell_idx].N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 2] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (3 * Cell_Specific[ue_cell_idx].N_DL_RB / 2) * Cell_Specific[ue_cell_idx].N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 3] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = Cell_Specific[ue_cell_idx].N_DL_RB * 2;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 4] = i;
      }
      for (i = 0; i < n[1]; i++) {
        n_REG[1][i] = i;
      }
      for (i = 0; i < n[2]; i++) {
        n_REG[2][i] = i;
      }
      d.resize(M_symb);
      z.resize(3);
      //DMP
      m_prime_num = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? N_group_PHICH : (N_group_PHICH / 2);
      for (m_prime = 0; m_prime < m_prime_num; m_prime++) {
        for (i = 0; i < 3; i++) {
          if (Normal_Duration == Cell_Specific[ue_cell_idx].phich_Duration) {
            l_prime = 0;
            n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else if (Cell_Specific[ue_cell_idx].MBSFN_Subframe_Flag) {
            l_prime = (m_prime / 2 + i + 1) & 1;
            n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType && (1 == Cell_Specific[ue_cell_idx].n_subframe || 6 == Cell_Specific[ue_cell_idx].n_subframe)) {
            l_prime = (m_prime / 2 + i + 1) & 1;
            n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else {
            l_prime = i;
            n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          }
          if (0 == l_prime || (1 == l_prime && 4 == Cell_Specific[ue_cell_idx].CRSPortNum)) {
            k_prime_offset = n_REG[l_prime][n_bar_i] * 6;
            for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 6); k_prime++) {
              if (vshift != (k_prime % 3)) {
                RE_Type(k_prime, l_prime) = PHICH;
              }
            }
          } else {
            k_prime_offset = n_REG[l_prime][n_bar_i] * 4;
            for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 4); k_prime++) {
              RE_Type(k_prime, l_prime) = PHICH;
            }
          }
        }
      }
      MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, 12);
      MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        MIMO_H[r].resize(Cell_Specific[ue_cell_idx].CRSPortNum, 12);
      }
      R_sigma2.resize(UE_Specific[ue_cell_idx].RxNum);
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        R_sigma2(r) = 1.0f / MEA[ue_cell_idx].CRS_N_2nd(r);
      }
      PHICH_idx = UE_Specific[ue_cell_idx].PHICH_idx;
      for (resource_idx = 0; resource_idx < (int)UE_Specific[ue_cell_idx].PHICH_Wanted_Pattern[PHICH_idx].size(); resource_idx++) {
        if (Wanted_User == UE_Specific[ue_cell_idx].PHICH_Wanted_Pattern[PHICH_idx][resource_idx]) {
          //DMP
          m_prime = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? UE_Specific[ue_cell_idx].n_group_PHICH[PHICH_idx][resource_idx] : (UE_Specific[ue_cell_idx].n_group_PHICH[PHICH_idx][resource_idx] / 2);
          for (i = 0; i < 3; i++) {
            if (Normal_Duration == Cell_Specific[ue_cell_idx].phich_Duration) {
              l_prime = 0;
              n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
            } else if (Cell_Specific[ue_cell_idx].MBSFN_Subframe_Flag) {
              l_prime = (m_prime / 2 + i + 1) & 1;
              n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
            } else if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType && (1 == Cell_Specific[ue_cell_idx].n_subframe || 6 == Cell_Specific[ue_cell_idx].n_subframe)) {
              l_prime = (m_prime / 2 + i + 1) & 1;
              n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
            } else {
              l_prime = i;
              n_bar_i = ((Cell_Specific[ue_cell_idx].N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
            }
            if (0 == l_prime || (1 == l_prime && 4 == Cell_Specific[ue_cell_idx].CRSPortNum)) {
              k_prime_offset = n_REG[l_prime][n_bar_i] * 6;
              j = 0;
              for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 6); k_prime++) {
                if (vshift != (k_prime % 3)) {
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    MIMO_symbs_in(r, 4 * i + j) = RE[r](k_prime, l_prime);
                    for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                      MIMO_H[r](p, 4 * i + j) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k_prime, l_prime);
                    }
                  }
                  RE_Type(k_prime, l_prime) = PHICH;
                  j++;
                }
              }
            } else {
              k_prime_offset = n_REG[l_prime][n_bar_i] * 4;
              for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 4); k_prime++) {
                j = k_prime - k_prime_offset;
                for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                  MIMO_symbs_in(r, 4 * i + j) = RE[r](k_prime, l_prime);
                  for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                    MIMO_H[r](p, 4 * i + j) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k_prime, l_prime);
                  }
                }
                RE_Type(k_prime, l_prime) = PHICH;
              }
            }
          }
          //MIMO
          MIMO_symbs_out.resize(12);
          h.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].CRSPortNum);
          if (1 == Cell_Specific[ue_cell_idx].CRSPortNum)  //Single_Antenna_Port
          {
            for (i = 0; i < 12; i++) {
              MIMO_symbs_out(i) = 0.0f;
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                MIMO_symbs_out(i) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i) * R_sigma2(r));
              }
            }
          } else if (2 == Cell_Specific[ue_cell_idx].CRSPortNum)  //2x Transmit_Diversity
          {
            for (i = 0; i < 6; i++) {
              MIMO_symbs_out(2 * i) = 0.0f;
              MIMO_symbs_out(2 * i + 1) = 0.0f;
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                  h(r, p) = (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
                }
                MIMO_symbs_out(2 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1))) * R_sigma2(r));
                MIMO_symbs_out(2 * i + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1)) * R_sigma2(r));
              }
            }
          } else  //4x Transmit_Diversity
          {
            for (i = 0; i < 3; i++) {
              MIMO_symbs_out(4 * i) = 0.0f;
              MIMO_symbs_out(4 * i + 1) = 0.0f;
              MIMO_symbs_out(4 * i + 2) = 0.0f;
              MIMO_symbs_out(4 * i + 3) = 0.0f;
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                  if (0 == p || 2 == p) {
                    h(r, p) = (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
                  } else {
                    h(r, p) = (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
                  }
                }
                if ((0 == ((i + UE_Specific[ue_cell_idx].n_group_PHICH[PHICH_idx][resource_idx]) & 1) && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || (0 == ((i + (UE_Specific[ue_cell_idx].n_group_PHICH[PHICH_idx][resource_idx] / 2)) & 1) && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type)) {
                  MIMO_symbs_out(4 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1))) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1)) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 2) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 3))) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 3) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 3)) * R_sigma2(r));
                } else {
                  MIMO_symbs_out(4 * i) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 1))) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 1) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 1)) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3))) * R_sigma2(r));
                  MIMO_symbs_out(4 * i + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3)) * R_sigma2(r));
                }
              }
            }
          }
          //DeScrambling & DeSpread
          if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
            d = MIMO_symbs_out;
          } else if (0 == (UE_Specific[ue_cell_idx].n_group_PHICH[PHICH_idx][resource_idx] & 1)) {
            for (i = 0; i < M_symb / 2; i++) {
              d(2 * i) = MIMO_symbs_out(4 * i);
              d(2 * i + 1) = MIMO_symbs_out(4 * i + 1);
            }
          } else {
            for (i = 0; i < M_symb / 2; i++) {
              d(2 * i) = MIMO_symbs_out(4 * i + 2);
              d(2 * i + 1) = MIMO_symbs_out(4 * i + 3);
            }
          }
          cinit = (((Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1)) << 9) + Cell_Specific[ue_cell_idx].N_cell_ID;
          PNSequence_Gen(cinit, c, M_symb);
          z.setZero();
          if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
            for (i = 0; i < M_symb; i++) {
              z(i / N_PHICH_SF) += d(i) * PHICH_Orth_Seq_NormalCP(UE_Specific[ue_cell_idx].n_seq_PHICH[PHICH_idx][resource_idx], i % N_PHICH_SF) * (1.0f - 2.0f * (float)c(i));
            }
          } else {
            for (i = 0; i < M_symb; i++) {
              z(i / N_PHICH_SF) += d(i) * PHICH_Orth_Seq_ExtendedCP(UE_Specific[ue_cell_idx].n_seq_PHICH[PHICH_idx][resource_idx], i % N_PHICH_SF) * (1.0f - 2.0f * (float)c(i));
            }
          }
          z = z / N_PHICH_SF;
          //LLR
          DeModulation_BPSK(z, V_softbits);
          //DCC
          PHICH_DCC(ue_cell_idx);
        }
      }
    }
  }
}

//! PDCCH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PDCCH_DMP_MIMO_DCC(int ue_cell_idx) {
  int N_REG = 0;
  int i = 0;
  int l = 0;
  int k = 0;
  uint32 cinit = 0;
  int M_tot = 0;
  int M_quad = 0;
  int p = 0;
  int vshift = 0;
  int r = 0;
  int row = 0;
  int col = 0;
  const int C_TC_subblock = 32;
  int R_TC_subblock = 0;
  int N_D = 0;
  const int P_pattern[] = {1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30};
  cfmat MIMO_symbs_in;      //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;            //MIMO_H[r](p, idx)
  cfvec MIMO_symbs_out;     //MIMO_symbs_out(idx)
  cfmat h;                  //h(r, p)
  cfmat MIMO_symbs_in_tmp;  //MIMO_symbs_in(r, idx)
  cfmats MIMO_H_tmp;        //MIMO_H[r](p, idx)
  fvec R_sigma2;
  int Ctrl_Symb_Num = 0;
  if (U != Cell_Specific[ue_cell_idx].SubframeType) {
    N_REG = 0;
    Ctrl_Symb_Num = (S == Cell_Specific[ue_cell_idx].SubframeType) ? Cell_Specific[ue_cell_idx].Ctrl_Symb_Num_Special : Cell_Specific[ue_cell_idx].Ctrl_Symb_Num;
    for (l = 0; l < Ctrl_Symb_Num; l++) {
      for (k = 0; k < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); k++) {
        if (PCFICH == RE_Type(k, l) || PHICH == RE_Type(k, l)) {
          N_REG++;
        }
      }
    }
    N_REG = -N_REG / 4;
    N_REG += (Cell_Specific[ue_cell_idx].N_DL_RB * 2);
    if (Ctrl_Symb_Num > 1) {
      N_REG += ((4 == Cell_Specific[ue_cell_idx].CRSPortNum) ? (2 * Cell_Specific[ue_cell_idx].N_DL_RB) : (3 * Cell_Specific[ue_cell_idx].N_DL_RB));
    }
    if (Ctrl_Symb_Num > 2) {
      N_REG += (Cell_Specific[ue_cell_idx].N_DL_RB * 3);
    }
    if (Ctrl_Symb_Num > 3) {
      N_REG += ((4 == Cell_Specific[ue_cell_idx].CRSPortNum) ? (2 * Cell_Specific[ue_cell_idx].N_DL_RB) : (3 * Cell_Specific[ue_cell_idx].N_DL_RB));
    }
    M_tot = N_REG * 8;
    M_quad = M_tot / 2 / 4;
    //DMP
    MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, M_tot / 2);
    MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MIMO_H[r].resize(Cell_Specific[ue_cell_idx].CRSPortNum, M_tot / 2);
    }
    R_sigma2.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      R_sigma2(r) = 1.0f / MEA[ue_cell_idx].CRS_N_2nd(r);
    }
    MIMO_symbs_in_tmp.resize(UE_Specific[ue_cell_idx].RxNum, M_tot / 2);
    MIMO_H_tmp.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MIMO_H_tmp[r].resize(Cell_Specific[ue_cell_idx].CRSPortNum, M_tot / 2);
    }
    vshift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    i = 0;
    for (k = 0; k < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); k++) {
      for (l = 0; l < Ctrl_Symb_Num; l++) {
        if (0 == l || (1 == l && 4 == Cell_Specific[ue_cell_idx].CRSPortNum) || (3 == l && 4 == Cell_Specific[ue_cell_idx].CRSPortNum)) {
          if (PCFICH != RE_Type(k, l) && PHICH != RE_Type(k, l) && vshift != (k % 3)) {
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              MIMO_symbs_in(r, i) = RE[r](k, l);
              for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                MIMO_H[r](p, i) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, l);
              }
            }
            RE_Type(k, l) = PDCCH;
            i++;
          }
        } else {
          if (PCFICH != RE_Type(k, l) && PHICH != RE_Type(k, l)) {
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              MIMO_symbs_in(r, i) = RE[r](k, l);
              for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                MIMO_H[r](p, i) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, l);
              }
            }
            RE_Type(k, l) = PDCCH;
            i++;
          }
        }
      }
    }
    for (i = 0; i < M_quad; i++) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        MIMO_symbs_in_tmp(r, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad)) = MIMO_symbs_in(r, 4 * i);
        MIMO_symbs_in_tmp(r, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 1) = MIMO_symbs_in(r, 4 * i + 1);
        MIMO_symbs_in_tmp(r, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 2) = MIMO_symbs_in(r, 4 * i + 2);
        MIMO_symbs_in_tmp(r, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 3) = MIMO_symbs_in(r, 4 * i + 3);
        for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
          MIMO_H_tmp[r](p, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad)) = MIMO_H[r](p, 4 * i);
          MIMO_H_tmp[r](p, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 1) = MIMO_H[r](p, 4 * i + 1);
          MIMO_H_tmp[r](p, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 2) = MIMO_H[r](p, 4 * i + 2);
          MIMO_H_tmp[r](p, 4 * ((i + Cell_Specific[ue_cell_idx].N_cell_ID) % M_quad) + 3) = MIMO_H[r](p, 4 * i + 3);
        }
      }
    }
    R_TC_subblock = ceil((float)M_quad / (float)C_TC_subblock);
    N_D = R_TC_subblock * C_TC_subblock - M_quad;
    row = 0;
    col = 0;
    k = 0;
    i = 0;
    while (k < M_quad) {
      i = P_pattern[col];
      for (row = 0; row < R_TC_subblock; row++) {
        if (i >= N_D) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            MIMO_symbs_in(r, 4 * (i - N_D)) = MIMO_symbs_in_tmp(r, 4 * k);
            MIMO_symbs_in(r, 4 * (i - N_D) + 1) = MIMO_symbs_in_tmp(r, 4 * k + 1);
            MIMO_symbs_in(r, 4 * (i - N_D) + 2) = MIMO_symbs_in_tmp(r, 4 * k + 2);
            MIMO_symbs_in(r, 4 * (i - N_D) + 3) = MIMO_symbs_in_tmp(r, 4 * k + 3);
            for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
              MIMO_H[r](p, 4 * (i - N_D)) = MIMO_H_tmp[r](p, 4 * k);
              MIMO_H[r](p, 4 * (i - N_D) + 1) = MIMO_H_tmp[r](p, 4 * k + 1);
              MIMO_H[r](p, 4 * (i - N_D) + 2) = MIMO_H_tmp[r](p, 4 * k + 2);
              MIMO_H[r](p, 4 * (i - N_D) + 3) = MIMO_H_tmp[r](p, 4 * k + 3);
            }
          }
          k++;
        }
        if (k == M_quad) {
          break;
        }
        i = i + C_TC_subblock;
      }
      col = col + 1;
    }
    //MIMO
    MIMO_symbs_out.resize(M_tot / 2);
    h.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].CRSPortNum);
    if (1 == Cell_Specific[ue_cell_idx].CRSPortNum)  //Single_Antenna_Port
    {
      for (i = 0; i < (M_tot / 2); i++) {
        MIMO_symbs_out(i) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          MIMO_symbs_out(i) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i) * R_sigma2(r));
        }
      }
    } else if (2 == Cell_Specific[ue_cell_idx].CRSPortNum)  //2x Transmit_Diversity
    {
      for (i = 0; i < (M_tot / 2 / 2); i++) {
        MIMO_symbs_out(2 * i) = 0.0f;
        MIMO_symbs_out(2 * i + 1) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            h(r, p) = (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
          }
          MIMO_symbs_out(2 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1))) * R_sigma2(r));
          MIMO_symbs_out(2 * i + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1)) * R_sigma2(r));
        }
      }
    } else  //4x Transmit_Diversity
    {
      for (i = 0; i < (M_tot / 2 / 4); i++) {
        MIMO_symbs_out(4 * i) = 0.0f;
        MIMO_symbs_out(4 * i + 1) = 0.0f;
        MIMO_symbs_out(4 * i + 2) = 0.0f;
        MIMO_symbs_out(4 * i + 3) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            if (0 == p || 2 == p) {
              h(r, p) = (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
            } else {
              h(r, p) = (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
            }
          }
          MIMO_symbs_out(4 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1))) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1)) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3))) * R_sigma2(r));
          MIMO_symbs_out(4 * i + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3)) * R_sigma2(r));
        }
      }
    }
    //LLR
    DeModulation_QPSK(MIMO_symbs_out, V_softbits);
    //DeScrambling
    cinit = (Cell_Specific[ue_cell_idx].n_subframe << 9) + Cell_Specific[ue_cell_idx].N_cell_ID;
    DeScrambling(cinit, V_softbits);
    //DCC
    PDCCH_DCC(ue_cell_idx);
  }
}

//! EPDCCH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMP_MIMO_DCC(int ue_cell_idx) {
}

//! PDSCH检测，解MIMO相关
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PDSCH_DMP_MIMO_DCC(int ue_cell_idx) {
  uint32 cinit = 0;
  int i = 0;
  int j = 0;
  int p = 0;
  int v = 0;
  int v_prime = 0;
  int r = 0;
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int sc = 0;
  int G_symb = 0;
  int G_prime = 0;
  int N_L_1cw = 0;
  int cw_idx = 0;
  int P_idx = (4 == Cell_Specific[ue_cell_idx].CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 0 : 1;
  float power_scale = 0.0f;
  float row_a = 0.0f;
  float row_b = 0.0f;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  int idx_offset = 0;
  int DCI_idx = 0;
  cfmat MIMO_symbs_in;    //MIMO_symbs_in(r, idx)
  cfmats MIMO_H;          //MIMO_H[r](p, idx)
  fmat MIMO_N_Recip;      //MIMO_N_Recip(r, idx)
  fvec MIMO_N_Recip_Ave;  //MIMO_N_Recip_Ave(idx)
  cfvecs MIMO_symbs_out;  //MIMO_symbs_out[cw_idx](idx)
  fvecs MIMO_SNR;         //MIMO_SNR[cw_idx](idx)
  cfmat h;                //h(r, p)
  cfmat h_eff;            //h(r, v)
  cfmat Ruu;
  cfmat Ryy_Inv;
  cfmat W;
  cfvec Y;
  cfvec X;
  cfmat MMSE_H;
  float noise = 0.0f;
  float interference = 0.0f;
  fvec v_SNR;
  ivec v_SNR_idx;
  Eigen::HouseholderQR<cfmat> QR;
  cfmat Q;
  cfmat R;
  cfmat h_eff_sort;
  ivec Qm_sort;
  cfvec Z;
  int MaxQm = 0;
  int M = 0;
  int Max_L = 0;
  ivecs candidated_idxs;
  int L_idx = 0;
  int L = 0;
  int M_idx = 0;
  cpxf Z_temp = 0.0f;
  cpxf c_temp = 0.0f;
  fvec metrics;
  fvec acc_metrics;
  fvec acc_metrics_temp;
  float metric0 = 0.0f;
  float metric1 = 0.0f;
  ivec ML_idxs;
  const cpxf *Constellation = NULL;
  cfmat C_temp;
  cfmat C;
  fvecs V_softbits_tmp;
  int Ctrl_Symb_Num = 0;
  int TD_idx = 0;
  int Tx_Num = 0;
  DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (TM7 == UE_Specific[ue_cell_idx].TM && DCI_Format_1 == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
      Tx_Num = 1;
    } else if ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx])) {
      Tx_Num = MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]);
    } else {
      Tx_Num = Cell_Specific[ue_cell_idx].CRSPortNum;
      row_a = powf(10.0f, UE_Specific[ue_cell_idx].p_a / 10.0f);
      if (MU_MIMO == UE_Specific[ue_cell_idx].Transmission_Scheme) {
        row_a = row_a * ((0 == UE_Specific[ue_cell_idx].Downlink_power_offset[DCI_idx]) ? 0.5f : 1.0f);
      }
      row_b = row_a * row_b_div_a[(1 == Cell_Specific[ue_cell_idx].CRSPortNum) ? 0 : 1][Cell_Specific[ue_cell_idx].p_b];
      if (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) {
        row_a_v = MySqrt(row_a) * MySqrt((float)MIN(2, Cell_Specific[ue_cell_idx].CRSPortNum));
        row_b_v = MySqrt(row_b) * MySqrt((float)MIN(2, Cell_Specific[ue_cell_idx].CRSPortNum));
      } else if (Port0 == UE_Specific[ue_cell_idx].Start_Port) {
        row_a_v = MySqrt(row_a) * MySqrt((float)Cell_Specific[ue_cell_idx].CRSPortNum);
        row_b_v = MySqrt(row_b) * MySqrt((float)Cell_Specific[ue_cell_idx].CRSPortNum);
      }
    }
    Ctrl_Symb_Num = (S == Cell_Specific[ue_cell_idx].SubframeType) ? Cell_Specific[ue_cell_idx].Ctrl_Symb_Num_Special : Cell_Specific[ue_cell_idx].Ctrl_Symb_Num;
    //DMP
    G_symb = 0;
    for (l = Ctrl_Symb_Num; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
      for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
        for (sc = 0; sc < Cell_Specific[ue_cell_idx].N_RB_sc; sc++) {
          k = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] * Cell_Specific[ue_cell_idx].N_RB_sc + sc;
          if (EMPTY_RE == RE_Type(k, l) || PDSCH == RE_Type(k, l)) {
            G_symb = G_symb + 1;
          }
        }
      }
    }
    MIMO_symbs_in.resize(UE_Specific[ue_cell_idx].RxNum, G_symb);
    MIMO_H.resize(UE_Specific[ue_cell_idx].RxNum);
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      MIMO_H[r].resize(Tx_Num, G_symb);
    }
    MIMO_N_Recip.resize(UE_Specific[ue_cell_idx].RxNum, G_symb);
    MIMO_N_Recip_Ave.resize(G_symb);
    G_symb = 0;
    for (l = Ctrl_Symb_Num; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
      power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l]) ? row_a_v : row_b_v;
      for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
        for (sc = 0; sc < Cell_Specific[ue_cell_idx].N_RB_sc; sc++) {
          k = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] * Cell_Specific[ue_cell_idx].N_RB_sc + sc;
          if (EMPTY_RE == RE_Type(k, l) || PDSCH == RE_Type(k, l)) {
            MIMO_N_Recip_Ave(G_symb) = 0.0f;
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              MIMO_symbs_in(r, G_symb) = RE[r](k, l);
              if (TM7 == UE_Specific[ue_cell_idx].TM && DCI_Format_1 == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
                MIMO_H[r](0, G_symb) = CHE[ue_cell_idx].DMRS_H_T_Port5[r](k, l);
              } else if ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx])) {
                for (p = 0; p < Tx_Num; p++) {
                  MIMO_H[r](p, G_symb) = CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p](k, l);
                }
              } else {
                for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                  MIMO_H[r](p, G_symb) = CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](k, l) * power_scale;
                }
              }
              MIMO_N_Recip(r, G_symb) = 1.0f / MEA[ue_cell_idx].Subband_N(r, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] / Feedback[ue_cell_idx].k_Subband);
              MIMO_N_Recip_Ave(G_symb) += (MEA[ue_cell_idx].Subband_N(r, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] / Feedback[ue_cell_idx].k_Subband));
            }
            MIMO_N_Recip_Ave(G_symb) = 1.0f / (MIMO_N_Recip_Ave(G_symb) / (float)UE_Specific[ue_cell_idx].RxNum);
            G_symb = G_symb + 1;
          }
        }
      }
    }
    G_prime = G_symb;
    if (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) {
      G_prime = G_prime / 2;
    }
    //MIMO
    MIMO_symbs_out.resize(UE_Specific[ue_cell_idx].codeword_num[DCI_idx]);
    MIMO_SNR.resize(UE_Specific[ue_cell_idx].codeword_num[DCI_idx]);
    for (cw_idx = 0; cw_idx < UE_Specific[ue_cell_idx].codeword_num[DCI_idx]; cw_idx++) {
      N_L_1cw = (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) ? 2 : UE_Specific[ue_cell_idx].v[cw_idx];
      MIMO_symbs_out[cw_idx].resize(N_L_1cw * G_prime);
      MIMO_SNR[cw_idx].resize(N_L_1cw * G_prime);
    }
    h.resize(UE_Specific[ue_cell_idx].RxNum, Tx_Num);
    if (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) {
      h_eff.resize(UE_Specific[ue_cell_idx].RxNum * 2, 2);
    } else {
      h_eff.resize(UE_Specific[ue_cell_idx].RxNum, MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
    }
    Ruu.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].RxNum);
    //Ruu.resize(UE_Specific[ue_cell_idx].N_L[DCI_idx], UE_Specific[ue_cell_idx].N_L[DCI_idx]);
    if (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) {
      Y.resize(UE_Specific[ue_cell_idx].RxNum * 2);
    } else {
      Y.resize(UE_Specific[ue_cell_idx].RxNum);
    }
    W.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]), UE_Specific[ue_cell_idx].RxNum);
    //W.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].N_L[DCI_idx]);
    MMSE_H.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]), MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
    if (Simuctrl.RML_Flag || (Simuctrl.TD_RML_Flag && Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme)) {
      if (2 == UE_Specific[ue_cell_idx].codeword_num[DCI_idx]) {
        MaxQm = MAX(UE_Specific[ue_cell_idx].Qm[0], UE_Specific[ue_cell_idx].Qm[1]);
      } else {
        MaxQm = UE_Specific[ue_cell_idx].Qm[0];
        UE_Specific[ue_cell_idx].v[1] = 1;
        UE_Specific[ue_cell_idx].Qm[1] = UE_Specific[ue_cell_idx].Qm[0];
      }
      if (2 == MaxQm) {
        M = 4;
        Max_L = 4;
      } else if (4 == MaxQm) {
        M = 16;
        Max_L = 16;
      } else if (6 == MaxQm) {
        M = 64;
        Max_L = 64;
      } else {
        M = 256;
        Max_L = 256;
      }
      candidated_idxs.resize(M);
      metrics.resize(Max_L);
      acc_metrics.resize(M);
      acc_metrics_temp.resize(Max_L * M);
      ML_idxs.resize(M);
      v_SNR.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      v_SNR_idx.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      h_eff_sort.resize(UE_Specific[ue_cell_idx].RxNum, MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      Qm_sort.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      Z.resize(MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      C_temp.resize(M, MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      C.resize(M, MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]));
      V_softbits_tmp.resize(UE_Specific[ue_cell_idx].codeword_num[DCI_idx]);
      for (cw_idx = 0; cw_idx < UE_Specific[ue_cell_idx].codeword_num[DCI_idx]; cw_idx++) {
        V_softbits_tmp[cw_idx].resize(UE_Specific[ue_cell_idx].v[cw_idx] * UE_Specific[ue_cell_idx].Qm[cw_idx] * G_prime);
      }
      for (M_idx = 0; M_idx < M; M_idx++) {
        candidated_idxs[M_idx].resize(Max_L);
      }
    }
    if (Single_Antenna_Port == UE_Specific[ue_cell_idx].Transmission_Scheme &&
        !((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx])))  //Single_Antenna_Port
    {
      for (i = 0; i < G_symb; i++) {
        MIMO_symbs_out[0](i) = 0.0f;
        MIMO_SNR[0](i) = 0.0f;
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          MIMO_symbs_out[0](i) += (conj(MIMO_H[r](0, i)) * MIMO_symbs_in(r, i) * MIMO_N_Recip(r, i));
          MIMO_SNR[0](i) += ((MIMO_H[r](0, i).real() * MIMO_H[r](0, i).real() + MIMO_H[r](0, i).imag() * MIMO_H[r](0, i).imag()) * MIMO_N_Recip(r, i));
        }
        MIMO_symbs_out[0](i) = MIMO_symbs_out[0](i) / MIMO_SNR[0](i);
      }
    } else if (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme)  //Transmit_Diversity
    {
      if (2 == Cell_Specific[ue_cell_idx].CRSPortNum) {
        if (!Simuctrl.TD_RML_Flag) {
          for (i = 0; i < (G_symb / 2); i++) {
            MIMO_symbs_out[0](2 * i) = 0.0f;
            MIMO_symbs_out[0](2 * i + 1) = 0.0f;
            MIMO_SNR[0](2 * i) = 0.0f;
            MIMO_SNR[0](2 * i + 1) = 0.0f;
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                h(r, p) = 0.5f * SQRT_2_RECIP * (MIMO_H[r](p, 2 * i) + MIMO_H[r](p, 2 * i + 1));
              }
              MIMO_symbs_out[0](2 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i) + h(r, 1) * conj(MIMO_symbs_in(r, 2 * i + 1))) * MIMO_N_Recip(r, 2 * i));
              MIMO_symbs_out[0](2 * i + 1) += ((-h(r, 1) * conj(MIMO_symbs_in(r, 2 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 2 * i + 1)) * MIMO_N_Recip(r, 2 * i));
              MIMO_SNR[0](2 * i) += ((h(r, 0).real() * h(r, 0).real() + h(r, 0).imag() * h(r, 0).imag() +
                                      h(r, 1).real() * h(r, 1).real() + h(r, 1).imag() * h(r, 1).imag()) *
                                     MIMO_N_Recip(r, 2 * i));
              MIMO_SNR[0](2 * i + 1) += ((h(r, 0).real() * h(r, 0).real() + h(r, 0).imag() * h(r, 0).imag() +
                                          h(r, 1).real() * h(r, 1).real() + h(r, 1).imag() * h(r, 1).imag()) *
                                         MIMO_N_Recip(r, 2 * i));
            }
            MIMO_symbs_out[0](2 * i) = MIMO_symbs_out[0](2 * i) / MIMO_SNR[0](2 * i);
            MIMO_symbs_out[0](2 * i + 1) = MIMO_symbs_out[0](2 * i + 1) / MIMO_SNR[0](2 * i + 1);
          }
        } else {
          for (i = 0; i < (G_symb / 2); i++) {
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              h_eff(2 * r, 0) = SQRT_2_RECIP * MIMO_H[r](0, 2 * i);
              h_eff(2 * r + 1, 0) = SQRT_2_RECIP * conj(MIMO_H[r](1, 2 * i + 1));
              h_eff(2 * r, 1) = -SQRT_2_RECIP * MIMO_H[r](1, 2 * i);
              h_eff(2 * r + 1, 1) = SQRT_2_RECIP * conj(MIMO_H[r](0, 2 * i + 1));
              Y(2 * r) = MIMO_symbs_in(r, 2 * i);
              Y(2 * r + 1) = conj(MIMO_symbs_in(r, 2 * i + 1));
            }
            QR.compute(h_eff);
            Q = QR.householderQ();
            R = QR.matrixQR().template triangularView<Eigen::Upper>();
            Z = Q.adjoint() * Y;
            v = 1;
            L = Get_MLD_candidated_idxs(UE_Specific[ue_cell_idx].Qm[0], conj(Z(v)), conj(R(v, v)), candidated_idxs[0], &Constellation);
            for (L_idx = 0; L_idx < L; L_idx++) {
              c_temp = conj(Z(v)) - conj(R(v, v)) * Constellation[candidated_idxs[0](L_idx)];
              metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(2 * i);
            }
            idx_offset = UE_Specific[ue_cell_idx].Qm[0] * (UE_Specific[ue_cell_idx].v[0] * i + v);
            cw_idx = 0;
            for (j = 0; j < UE_Specific[ue_cell_idx].Qm[0]; j++) {
              V_softbits_tmp[cw_idx](idx_offset + j) = (0 == ((candidated_idxs[0](0) >> (UE_Specific[ue_cell_idx].Qm[0] - 1 - j)) & 1)) ? (metrics(j + 1) - metrics(0)) : (metrics(0) - metrics(j + 1));
            }
            if (L <= M) {
              for (M_idx = 0; M_idx < L; M_idx++) {
                acc_metrics(M_idx) = metrics(M_idx);
                C(M_idx, v) = conj(Constellation[candidated_idxs[0](M_idx)]);
              }
              for (M_idx = L; M_idx < M; M_idx++) {
                acc_metrics(M_idx) = FLT_MAX;
                C(M_idx, v) = cpxf(FLT_MAX, FLT_MAX);
              }
            } else {
              Find_MDL_M_metrics(metrics, L, ML_idxs, acc_metrics);
              for (M_idx = 0; M_idx < M; M_idx++) {
                C(M_idx, v) = conj(Constellation[candidated_idxs[0](ML_idxs(M_idx))]);
              }
            }
            v = 0;
            for (M_idx = 0; M_idx < M; M_idx++) {
              Z_temp = Z(v);
              for (v_prime = (v + 1); v_prime < 2; v_prime++) {
                Z_temp -= R(v, v_prime) * C(M_idx, v_prime);
              }
              L = Get_MLD_candidated_idxs(UE_Specific[ue_cell_idx].Qm[0], Z_temp, R(v, v), candidated_idxs[M_idx], &Constellation);
              L = UE_Specific[ue_cell_idx].Qm[0] + 1;
              for (L_idx = 0; L_idx < L; L_idx++) {
                k = L_idx * M + M_idx;
                c_temp = Z_temp - R(v, v) * Constellation[candidated_idxs[M_idx](L_idx)];
                metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(2 * i);
                acc_metrics_temp(k) = acc_metrics(M_idx) + metrics(L_idx);
              }
            }
            idx_offset = UE_Specific[ue_cell_idx].Qm[0] * (UE_Specific[ue_cell_idx].v[0] * i + v);
            cw_idx = 0;
            for (j = 0; j < UE_Specific[ue_cell_idx].Qm[0]; j++) {
              l = UE_Specific[ue_cell_idx].Qm[0] - 1 - j;
              if (0 == ((candidated_idxs[0](0) >> l) & 1)) {
                metric0 = acc_metrics_temp(0);
                metric1 = acc_metrics_temp((j + 1) * M);
              } else {
                metric0 = acc_metrics_temp((j + 1) * M);
                metric1 = acc_metrics_temp(0);
              }
              k = (j + 1) * M;
              for (M_idx = 1; M_idx < M; M_idx++) {
                if (0 == ((candidated_idxs[M_idx](0) >> l) & 1)) {
                  metric0 = MIN(acc_metrics_temp(M_idx), metric0);
                  metric1 = MIN(acc_metrics_temp(k + M_idx), metric1);
                } else {
                  metric0 = MIN(acc_metrics_temp(k + M_idx), metric0);
                  metric1 = MIN(acc_metrics_temp(M_idx), metric1);
                }
              }
              V_softbits_tmp[cw_idx](idx_offset + j) = metric1 - metric0;
            }
          }
        }
      } else {
        if (!Simuctrl.TD_RML_Flag) {
          for (i = 0; i < (G_symb / 4); i++) {
            MIMO_symbs_out[0](4 * i) = 0.0f;
            MIMO_symbs_out[0](4 * i + 1) = 0.0f;
            if ((4 * i + 3) < G_symb) {
              MIMO_symbs_out[0](4 * i + 2) = 0.0f;
              MIMO_symbs_out[0](4 * i + 3) = 0.0f;
            }
            MIMO_SNR[0](4 * i) = 0.0f;
            MIMO_SNR[0](4 * i + 1) = 0.0f;
            if ((4 * i + 3) < G_symb) {
              MIMO_SNR[0](4 * i + 2) = 0.0f;
              MIMO_SNR[0](4 * i + 3) = 0.0f;
            }
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
                if (0 == p || 2 == p) {
                  h(r, p) = 0.5f * SQRT_2_RECIP * (MIMO_H[r](p, 4 * i) + MIMO_H[r](p, 4 * i + 1));
                } else {
                  h(r, p) = 0.5f * SQRT_2_RECIP * (MIMO_H[r](p, 4 * i + 2) + MIMO_H[r](p, 4 * i + 3));
                }
              }
              MIMO_symbs_out[0](4 * i) += ((conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i) + h(r, 2) * conj(MIMO_symbs_in(r, 4 * i + 1))) * MIMO_N_Recip(r, 4 * i));
              MIMO_symbs_out[0](4 * i + 1) += ((-h(r, 2) * conj(MIMO_symbs_in(r, 4 * i)) + conj(h(r, 0)) * MIMO_symbs_in(r, 4 * i + 1)) * MIMO_N_Recip(r, 4 * i));
              if ((4 * i + 3) < G_symb) {
                MIMO_symbs_out[0](4 * i + 2) += ((conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 2) + h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 3))) * MIMO_N_Recip(r, 4 * i));
                MIMO_symbs_out[0](4 * i + 3) += ((-h(r, 3) * conj(MIMO_symbs_in(r, 4 * i + 2)) + conj(h(r, 1)) * MIMO_symbs_in(r, 4 * i + 3)) * MIMO_N_Recip(r, 4 * i));
              }
              MIMO_SNR[0](4 * i) += ((h(r, 0).real() * h(r, 0).real() + h(r, 0).imag() * h(r, 0).imag() +
                                      h(r, 2).real() * h(r, 2).real() + h(r, 2).imag() * h(r, 2).imag()) *
                                     MIMO_N_Recip(r, 4 * i));
              MIMO_SNR[0](4 * i + 1) += ((h(r, 0).real() * h(r, 0).real() + h(r, 0).imag() * h(r, 0).imag() +
                                          h(r, 2).real() * h(r, 2).real() + h(r, 2).imag() * h(r, 2).imag()) *
                                         MIMO_N_Recip(r, 4 * i));
              if ((4 * i + 3) < G_symb) {
                MIMO_SNR[0](4 * i + 2) += ((h(r, 1).real() * h(r, 1).real() + h(r, 1).imag() * h(r, 1).imag() +
                                            h(r, 3).real() * h(r, 3).real() + h(r, 3).imag() * h(r, 3).imag()) *
                                           MIMO_N_Recip(r, 4 * i));
                MIMO_SNR[0](4 * i + 3) += ((h(r, 1).real() * h(r, 1).real() + h(r, 1).imag() * h(r, 1).imag() +
                                            h(r, 3).real() * h(r, 3).real() + h(r, 3).imag() * h(r, 3).imag()) *
                                           MIMO_N_Recip(r, 4 * i));
              }
            }
            MIMO_symbs_out[0](4 * i) = MIMO_symbs_out[0](4 * i) / MIMO_SNR[0](4 * i);
            MIMO_symbs_out[0](4 * i + 1) = MIMO_symbs_out[0](4 * i + 1) / MIMO_SNR[0](4 * i + 1);
            if ((4 * i + 3) < G_symb) {
              MIMO_symbs_out[0](4 * i + 2) = MIMO_symbs_out[0](4 * i + 2) / MIMO_SNR[0](4 * i + 2);
              MIMO_symbs_out[0](4 * i + 3) = MIMO_symbs_out[0](4 * i + 3) / MIMO_SNR[0](4 * i + 3);
            }
          }
        } else {
          for (i = 0; i < (G_symb / 4); i++) {
            for (TD_idx = 0; TD_idx <= 2; TD_idx += 2) {
              if ((4 * i + 1 + TD_idx) < G_symb) {
                for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                  h_eff(2 * r, 0) = SQRT_2_RECIP * MIMO_H[r]((TD_idx >> 1), 4 * i + TD_idx);
                  h_eff(2 * r + 1, 0) = SQRT_2_RECIP * conj(MIMO_H[r](2 + (TD_idx >> 1), 4 * i + 1 + TD_idx));
                  h_eff(2 * r, 1) = -SQRT_2_RECIP * MIMO_H[r](2 + (TD_idx >> 1), 4 * i + TD_idx);
                  h_eff(2 * r + 1, 1) = SQRT_2_RECIP * conj(MIMO_H[r]((TD_idx >> 1), 4 * i + 1 + TD_idx));
                  Y(2 * r) = MIMO_symbs_in(r, 4 * i + TD_idx);
                  Y(2 * r + 1) = conj(MIMO_symbs_in(r, 4 * i + 1 + TD_idx));
                }
                QR.compute(h_eff);
                Q = QR.householderQ();
                R = QR.matrixQR().template triangularView<Eigen::Upper>();
                Z = Q.adjoint() * Y;
                v = 1;
                L = Get_MLD_candidated_idxs(UE_Specific[ue_cell_idx].Qm[0], conj(Z(v)), conj(R(v, v)), candidated_idxs[0], &Constellation);
                for (L_idx = 0; L_idx < L; L_idx++) {
                  c_temp = conj(Z(v)) - conj(R(v, v)) * Constellation[candidated_idxs[0](L_idx)];
                  metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(4 * i);
                }
                idx_offset = UE_Specific[ue_cell_idx].Qm[0] * (UE_Specific[ue_cell_idx].v[0] * i + v + TD_idx);
                cw_idx = 0;
                for (j = 0; j < UE_Specific[ue_cell_idx].Qm[0]; j++) {
                  V_softbits_tmp[cw_idx](idx_offset + j) = (0 == ((candidated_idxs[0](0) >> (UE_Specific[ue_cell_idx].Qm[0] - 1 - j)) & 1)) ? (metrics(j + 1) - metrics(0)) : (metrics(0) - metrics(j + 1));
                }
                if (L <= M) {
                  for (M_idx = 0; M_idx < L; M_idx++) {
                    acc_metrics(M_idx) = metrics(M_idx);
                    C(M_idx, v) = conj(Constellation[candidated_idxs[0](M_idx)]);
                  }
                  for (M_idx = L; M_idx < M; M_idx++) {
                    acc_metrics(M_idx) = FLT_MAX;
                    C(M_idx, v) = cpxf(FLT_MAX, FLT_MAX);
                  }
                } else {
                  Find_MDL_M_metrics(metrics, L, ML_idxs, acc_metrics);
                  for (M_idx = 0; M_idx < M; M_idx++) {
                    C(M_idx, v) = conj(Constellation[candidated_idxs[0](ML_idxs(M_idx))]);
                  }
                }
                v = 0;
                for (M_idx = 0; M_idx < M; M_idx++) {
                  Z_temp = Z(v);
                  for (v_prime = (v + 1); v_prime < 2; v_prime++) {
                    Z_temp -= R(v, v_prime) * C(M_idx, v_prime);
                  }
                  L = Get_MLD_candidated_idxs(UE_Specific[ue_cell_idx].Qm[0], Z_temp, R(v, v), candidated_idxs[M_idx], &Constellation);
                  L = UE_Specific[ue_cell_idx].Qm[0] + 1;
                  for (L_idx = 0; L_idx < L; L_idx++) {
                    k = L_idx * M + M_idx;
                    c_temp = Z_temp - R(v, v) * Constellation[candidated_idxs[M_idx](L_idx)];
                    metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(4 * i);
                    acc_metrics_temp(k) = acc_metrics(M_idx) + metrics(L_idx);
                  }
                }
                idx_offset = UE_Specific[ue_cell_idx].Qm[0] * (UE_Specific[ue_cell_idx].v[0] * i + v + TD_idx);
                cw_idx = 0;
                for (j = 0; j < UE_Specific[ue_cell_idx].Qm[0]; j++) {
                  l = UE_Specific[ue_cell_idx].Qm[0] - 1 - j;
                  if (0 == ((candidated_idxs[0](0) >> l) & 1)) {
                    metric0 = acc_metrics_temp(0);
                    metric1 = acc_metrics_temp((j + 1) * M);
                  } else {
                    metric0 = acc_metrics_temp((j + 1) * M);
                    metric1 = acc_metrics_temp(0);
                  }
                  k = (j + 1) * M;
                  for (M_idx = 1; M_idx < M; M_idx++) {
                    if (0 == ((candidated_idxs[M_idx](0) >> l) & 1)) {
                      metric0 = MIN(acc_metrics_temp(M_idx), metric0);
                      metric1 = MIN(acc_metrics_temp(k + M_idx), metric1);
                    } else {
                      metric0 = MIN(acc_metrics_temp(k + M_idx), metric0);
                      metric1 = MIN(acc_metrics_temp(M_idx), metric1);
                    }
                  }
                  V_softbits_tmp[cw_idx](idx_offset + j) = metric1 - metric0;
                }
              }
            }
          }
        }
      }
    } else  //Large_Delay_CDD or Closed_Loop_SM or TM8/9/10
    {
      for (i = 0; i < G_symb; i++) {
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          Ruu(r, r) = MIMO_N_Recip(r, i);
        }
        if (Large_Delay_CDD == UE_Specific[ue_cell_idx].Transmission_Scheme || Closed_Loop_SM == UE_Specific[ue_cell_idx].Transmission_Scheme || MU_MIMO == UE_Specific[ue_cell_idx].Transmission_Scheme) {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
              h(r, p) = MIMO_H[r](p, i);
            }
            Y(r) = MIMO_symbs_in(r, i);
          }
          if (Large_Delay_CDD == UE_Specific[ue_cell_idx].Transmission_Scheme) {
            if (2 == Cell_Specific[ue_cell_idx].CRSPortNum) {
              h_eff = h * Codebook_2P2v[0] * Codebook_D_2v[i & 1] * Codebook_U_2v;
            } else if (2 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
              h_eff = h * Codebook_4P2v[((i / UE_Specific[ue_cell_idx].N_L[DCI_idx]) & 3) + 12] * Codebook_D_2v[i & 1] * Codebook_U_2v;
            } else if (3 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
              h_eff = h * Codebook_4P3v[((i / UE_Specific[ue_cell_idx].N_L[DCI_idx]) & 3) + 12] * Codebook_D_3v[i % 3] * Codebook_U_3v;
            } else {
              h_eff = h * Codebook_4P4v[((i / UE_Specific[ue_cell_idx].N_L[DCI_idx]) & 3) + 12] * Codebook_D_4v[i & 3] * Codebook_U_4v;
            }
          } else {
            if (2 == Cell_Specific[ue_cell_idx].CRSPortNum) {
              if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                h_eff = h * Codebook_2P1v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
              } else {
                h_eff = h * Codebook_2P2v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
              }
            } else if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
              if (UE_Specific[ue_cell_idx].alternativeCodeBookEnabledFor4TX_r12) {
                if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P1v_CSI[UE_Specific[ue_cell_idx].Codebook_index0(i) * 16 + UE_Specific[ue_cell_idx].Codebook_index1(i)];
                } else if (2 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P2v_CSI[UE_Specific[ue_cell_idx].Codebook_index0(i) * 16 + UE_Specific[ue_cell_idx].Codebook_index1(i)];
                } else if (3 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].Codebook_index1(i)];
                } else {
                  h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].Codebook_index1(i)];
                }
              } else {
                if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P1v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
                } else if (2 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P2v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
                } else if (3 == UE_Specific[ue_cell_idx].N_L[DCI_idx]) {
                  h_eff = h * Codebook_4P3v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
                } else {
                  h_eff = h * Codebook_4P4v[UE_Specific[ue_cell_idx].Codebook_index0(i)];
                }
              }
            }
          }
        } else {
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            for (p = 0; p < Tx_Num; p++) {
              h_eff(r, p) = MIMO_H[r](p, i);
            }
            Y(r) = MIMO_symbs_in(r, i);
          }
        }
        if (!Simuctrl.RML_Flag) {
          Hermitian_Matrix_INV(h_eff * h_eff.adjoint() + Ruu, Ryy_Inv);
          W = h_eff.adjoint() * Ryy_Inv;
          X = W * Y;
          MMSE_H = W * h_eff;
          if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx] &&
              ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]))) {
            v = (int)(UE_Specific[ue_cell_idx].Start_Port - Port7);
            noise = 0.0f;
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              noise += (W(v, r).real() * W(v, r).real() + W(v, r).imag() * W(v, r).imag()) * MIMO_N_Recip(r, i);
            }
            interference = 0.0f;
            for (v_prime = 0; v_prime < 2; v_prime++) {
              if (v_prime != v) {
                interference += (MMSE_H(v, v_prime).real() * MMSE_H(v, v_prime).real() + MMSE_H(v, v_prime).imag() * MMSE_H(v, v_prime).imag());
              }
            }
            noise = 1.0f / (noise + interference);
            MIMO_SNR[0](i) = MMSE_H(v, v).real() * MMSE_H(v, v).real() * noise;
            MIMO_symbs_out[0](i) = X(v) / MMSE_H(v, v).real();
          } else {
            for (v = 0; v < UE_Specific[ue_cell_idx].N_L[DCI_idx]; v++) {
              noise = 0.0f;
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                noise += (W(v, r).real() * W(v, r).real() + W(v, r).imag() * W(v, r).imag()) * MIMO_N_Recip(r, i);
              }
              interference = 0.0f;
              for (v_prime = 0; v_prime < UE_Specific[ue_cell_idx].N_L[DCI_idx]; v_prime++) {
                if (v_prime != v) {
                  interference += (MMSE_H(v, v_prime).real() * MMSE_H(v, v_prime).real() + MMSE_H(v, v_prime).imag() * MMSE_H(v, v_prime).imag());
                }
              }
              noise = 1.0f / (noise + interference);
              if (v < UE_Specific[ue_cell_idx].v[0]) {
                MIMO_SNR[0](UE_Specific[ue_cell_idx].v[0] * i + v) = MMSE_H(v, v).real() * MMSE_H(v, v).real() * noise;
                MIMO_symbs_out[0](UE_Specific[ue_cell_idx].v[0] * i + v) = X(v) / MMSE_H(v, v).real();
              } else {
                MIMO_SNR[1](UE_Specific[ue_cell_idx].v[1] * i + (v - UE_Specific[ue_cell_idx].v[0])) = MMSE_H(v, v).real() * MMSE_H(v, v).real() * noise;
                MIMO_symbs_out[1](UE_Specific[ue_cell_idx].v[1] * i + (v - UE_Specific[ue_cell_idx].v[0])) = X(v) / MMSE_H(v, v).real();
              }
            }
          }
          /*Hermitian_Matrix_INV(h_eff.adjoint() * h_eff + Ruu, Ryy_Inv);
                    W = Ryy_Inv * h_eff.adjoint();
                    X = W * Y;
                    MMSE_H = W * h_eff;
                    for (v = 0; v < UE_Specific[ue_cell_idx].N_L[DCI_idx]; v++)
                    {
                        if (v < UE_Specific[ue_cell_idx].v[0])
                        {
                            MIMO_SNR[0](UE_Specific[ue_cell_idx].v[0] * i + v) = 1.0f / (Ruu(v, v).real() * Ryy_Inv(v, v).real());
                            MIMO_symbs_out[0](UE_Specific[ue_cell_idx].v[0] * i + v) = X(v) * (1.0f + MIMO_SNR[0](UE_Specific[ue_cell_idx].v[0] * i + v)) / MIMO_SNR[0](UE_Specific[ue_cell_idx].v[0] * i + v);
                        }
                        else
                        {
                            MIMO_SNR[1](UE_Specific[ue_cell_idx].v[1]  * i + (v - UE_Specific[ue_cell_idx].v[0])) = 1.0f / (Ruu(v, v).real() * Ryy_Inv(v, v).real());
                            MIMO_symbs_out[1](UE_Specific[ue_cell_idx].v[1]  * i + (v - UE_Specific[ue_cell_idx].v[0])) = X(v) * (1.0f + MIMO_SNR[1](UE_Specific[ue_cell_idx].v[1]  * i + (v - UE_Specific[ue_cell_idx].v[0]))) / MIMO_SNR[1](UE_Specific[ue_cell_idx].v[1]  * i + (v - UE_Specific[ue_cell_idx].v[0]));
                        }
                    }*/
        } else {
          /*MMSE_H = h_eff.adjoint() * h_eff;
                    for (v = 0; v < UE_Specific[ue_cell_idx].N_L[DCI_idx]; v++)
                    {
                        noise = 0.0f;
                        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++)
                        {
                            noise += (h_eff(r, v).real() * h_eff(r, v).real() + h_eff(r, v).imag() * h_eff(r, v).imag()) * MEA[ue_cell_idx].CRS_N_2nd(r);
                        }
                        interference = 0.0f;
                        for (v_prime = 0; v_prime < UE_Specific[ue_cell_idx].N_L[DCI_idx]; v_prime++)
                        {
                            if (v_prime != v)
                            {
                                interference += (MMSE_H(v, v_prime).real() * MMSE_H(v, v_prime).real() + MMSE_H(v, v_prime).imag() * MMSE_H(v, v_prime).imag());
                            }
                        }
                        v_SNR(v) = MMSE_H(v, v).real() * MMSE_H(v, v).real() / (noise + interference);
                    }
                    BubbleSortIdx(v_SNR, v_SNR_idx);*/
          if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx] &&
              ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]))) {
            if (Port7 == UE_Specific[ue_cell_idx].Start_Port) {
              v_SNR_idx(0) = 1;
              v_SNR_idx(1) = 0;
            } else {
              v_SNR_idx(0) = 0;
              v_SNR_idx(1) = 1;
            }
          } else {
            for (v = 0; v < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); v++) {
              //v_SNR_idx(v) = UE_Specific[ue_cell_idx].N_L[DCI_idx] - 1 - v;
              v_SNR_idx(v) = v;
            }
          }
          for (v = 0; v < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); v++) {
            h_eff_sort.col(v) = h_eff.col(v_SNR_idx(v));
            if (1 == UE_Specific[ue_cell_idx].codeword_num[DCI_idx] || v_SNR_idx(v) < UE_Specific[ue_cell_idx].v[0]) {
              Qm_sort(v) = UE_Specific[ue_cell_idx].Qm[0];
            } else {
              Qm_sort(v) = UE_Specific[ue_cell_idx].Qm[1];
            }
          }
          QR.compute(h_eff_sort);
          Q = QR.householderQ();
          R = QR.matrixQR().template triangularView<Eigen::Upper>();
          Z = Q.adjoint() * Y;
          v = (MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]) - 1);
          L = Get_MLD_candidated_idxs(Qm_sort(v), Z(v), R(v, v), candidated_idxs[0], &Constellation);
          for (L_idx = 0; L_idx < L; L_idx++) {
            c_temp = Z(v) - R(v, v) * Constellation[candidated_idxs[0](L_idx)];
            metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(i);
          }
          if (1 == UE_Specific[ue_cell_idx].N_L[DCI_idx] &&
              ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]))) {
            if (v_SNR_idx(v) < UE_Specific[ue_cell_idx].v[0]) {
              idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[0] * i + v_SNR_idx(v));
            } else {
              idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[1] * i + (v_SNR_idx(v) - UE_Specific[ue_cell_idx].v[0]));
            }
            cw_idx = 0;
            for (j = 0; j < Qm_sort(v); j++) {
              V_softbits_tmp[cw_idx](idx_offset + j) = (0 == ((candidated_idxs[0](0) >> (Qm_sort(v) - 1 - j)) & 1)) ? (metrics(j + 1) - metrics(0)) : (metrics(0) - metrics(j + 1));
            }
          } else {
            if (v_SNR_idx(v) < UE_Specific[ue_cell_idx].v[0]) {
              idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[0] * i + v_SNR_idx(v));
              cw_idx = 0;
            } else {
              idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[1] * i + (v_SNR_idx(v) - UE_Specific[ue_cell_idx].v[0]));
              cw_idx = 1;
            }
            for (j = 0; j < Qm_sort(v); j++) {
              V_softbits_tmp[cw_idx](idx_offset + j) = (0 == ((candidated_idxs[0](0) >> (Qm_sort(v) - 1 - j)) & 1)) ? (metrics(j + 1) - metrics(0)) : (metrics(0) - metrics(j + 1));
            }
            if (L <= M) {
              for (M_idx = 0; M_idx < L; M_idx++) {
                acc_metrics(M_idx) = metrics(M_idx);
                C(M_idx, v) = Constellation[candidated_idxs[0](M_idx)];
              }
              for (M_idx = L; M_idx < M; M_idx++) {
                acc_metrics(M_idx) = FLT_MAX;
                C(M_idx, v) = cpxf(FLT_MAX, FLT_MAX);
              }
            } else {
              Find_MDL_M_metrics(metrics, L, ML_idxs, acc_metrics);
              for (M_idx = 0; M_idx < M; M_idx++) {
                C(M_idx, v) = Constellation[candidated_idxs[0](ML_idxs(M_idx))];
              }
            }
            for (v = (MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]) - 2); v >= 0; v--) {
              for (M_idx = 0; M_idx < M; M_idx++) {
                Z_temp = Z(v);
                for (v_prime = (v + 1); v_prime < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); v_prime++) {
                  Z_temp -= R(v, v_prime) * C(M_idx, v_prime);
                }
                L = Get_MLD_candidated_idxs(Qm_sort(v), Z_temp, R(v, v), candidated_idxs[M_idx], &Constellation);
                if (0 == v) {
                  L = Qm_sort(v) + 1;
                }
                for (L_idx = 0; L_idx < L; L_idx++) {
                  k = L_idx * M + M_idx;
                  c_temp = Z_temp - R(v, v) * Constellation[candidated_idxs[M_idx](L_idx)];
                  metrics(L_idx) = (c_temp.real() * c_temp.real() + c_temp.imag() * c_temp.imag()) * MIMO_N_Recip_Ave(i);
                  acc_metrics_temp(k) = acc_metrics(M_idx) + metrics(L_idx);
                }
              }
              if (v_SNR_idx(v) < UE_Specific[ue_cell_idx].v[0]) {
                idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[0] * i + v_SNR_idx(v));
                cw_idx = 0;
              } else {
                idx_offset = Qm_sort(v) * (UE_Specific[ue_cell_idx].v[1] * i + (v_SNR_idx(v) - UE_Specific[ue_cell_idx].v[0]));
                cw_idx = 1;
              }
              for (j = 0; j < Qm_sort(v); j++) {
                l = Qm_sort(v) - 1 - j;
                if (0 == ((candidated_idxs[0](0) >> l) & 1)) {
                  metric0 = acc_metrics_temp(0);
                  metric1 = acc_metrics_temp((j + 1) * M);
                } else {
                  metric0 = acc_metrics_temp((j + 1) * M);
                  metric1 = acc_metrics_temp(0);
                }
                k = (j + 1) * M;
                for (M_idx = 1; M_idx < M; M_idx++) {
                  if (0 == ((candidated_idxs[M_idx](0) >> l) & 1)) {
                    metric0 = MIN(acc_metrics_temp(M_idx), metric0);
                    metric1 = MIN(acc_metrics_temp(k + M_idx), metric1);
                  } else {
                    metric0 = MIN(acc_metrics_temp(k + M_idx), metric0);
                    metric1 = MIN(acc_metrics_temp(M_idx), metric1);
                  }
                }
                V_softbits_tmp[cw_idx](idx_offset + j) = metric1 - metric0;
              }
              if (v > 0) {
                Find_MDL_M_metrics(acc_metrics_temp, M * L, ML_idxs, acc_metrics);
                for (M_idx = 0; M_idx < M; M_idx++) {
                  for (v_prime = (v + 1); v_prime < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); v_prime++) {
                    C_temp(M_idx, v_prime) = C(M_idx, v_prime);
                  }
                }
                for (M_idx = 0; M_idx < M; M_idx++) {
                  for (v_prime = (v + 1); v_prime < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); v_prime++) {
                    C(M_idx, v_prime) = C_temp(ML_idxs(M_idx) % M, v_prime);
                  }
                  C(M_idx, v) = Constellation[candidated_idxs[M_idx](ML_idxs(M_idx) / M)];
                }
              }
            }
          }
        }
      }
    }
  }
  for (cw_idx = 0; cw_idx < UE_Specific[ue_cell_idx].codeword_num[DCI_idx]; cw_idx++) {
    //LLR
    if (ReTransmit[ue_cell_idx].LastNDI[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] != UE_Specific[ue_cell_idx].NDI[cw_idx][DCI_idx]) {
      ReTransmit[ue_cell_idx].LastQm[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = UE_Specific[ue_cell_idx].Qm[cw_idx];
    } else {
      UE_Specific[ue_cell_idx].Qm[cw_idx] = ReTransmit[ue_cell_idx].LastQm[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID];
    }
    if (Simuctrl.RML_Flag || (Simuctrl.TD_RML_Flag && Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme)) {
      V_softbits.resize(V_softbits_tmp[cw_idx].size());
      V_softbits = V_softbits_tmp[cw_idx];
    } else {
      /*WithFile("DeModulation");
            for (i = 0; i < (int)MIMO_symbs_out[cw_idx].size(); i++)
            {
                if (MIMO_symbs_out[cw_idx](i).real() > 4.0f * D_64QAM)
                {
                    MIMO_symbs_out[cw_idx](i).real(4.0f * D_64QAM);
                }
                else if (MIMO_symbs_out[cw_idx](i).real() < -4.0f * D_64QAM)
                {
                    MIMO_symbs_out[cw_idx](i).real(-4.0f * D_64QAM);
                }
                if (MIMO_symbs_out[cw_idx](i).imag() > 4.0f * D_64QAM)
                {
                    MIMO_symbs_out[cw_idx](i).imag(4.0f * D_64QAM);
                }
                else if (MIMO_symbs_out[cw_idx](i).imag() < -4.0f * D_64QAM)
                {
                    MIMO_symbs_out[cw_idx](i).imag(-4.0f * D_64QAM);
                }
                FilePrint("%f %f\n", MIMO_symbs_out[cw_idx](i).real(), MIMO_symbs_out[cw_idx](i).imag());
            }*/
      if (2 == UE_Specific[ue_cell_idx].Qm[cw_idx]) {
        DeModulation_QPSK(MIMO_symbs_out[cw_idx], V_softbits);
      } else if (4 == UE_Specific[ue_cell_idx].Qm[cw_idx]) {
        DeModulation_16QAM(MIMO_symbs_out[cw_idx], D_16QAM, V_softbits);
      } else if (6 == UE_Specific[ue_cell_idx].Qm[cw_idx]) {
        DeModulation_64QAM(MIMO_symbs_out[cw_idx], D_64QAM, V_softbits);
      } else {
        DeModulation_256QAM(MIMO_symbs_out[cw_idx], D_256QAM, V_softbits);
      }
      N_L_1cw = (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) ? 2 : UE_Specific[ue_cell_idx].v[cw_idx];
      for (i = 0; i < (N_L_1cw * G_prime); i++) {
        idx_offset = UE_Specific[ue_cell_idx].Qm[cw_idx] * i;
        for (j = 0; j < UE_Specific[ue_cell_idx].Qm[cw_idx]; j++) {
          V_softbits(idx_offset + j) = V_softbits(idx_offset + j) * MIMO_SNR[cw_idx](i);
        }
      }
    }
    /*WithFile("LLR");
        for (i = 0; i < (int)V_softbits.size(); i++)
        {
            //FilePrint("%d %f\n", V_softbits(i) > 0.0f ? 0 : 1, V_softbits(i));
            FilePrint("%d\n", V_softbits(i) > 0.0f ? 0 : 1);
        }*/
    //DeScrambling
    cinit = (UE_Specific[ue_cell_idx].RNTI << 14) + (cw_idx << 13) + (Cell_Specific[ue_cell_idx].n_subframe << 9) + Cell_Specific[ue_cell_idx].N_cell_ID;
    DeScrambling(cinit, V_softbits);
    //DCC
    PDSCH_DCC(ue_cell_idx, cw_idx, G_prime);
  }
}
