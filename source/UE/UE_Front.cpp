/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_Front.cpp
    \brief UE数字前端实现文件
    内容 ：UE_C类数字前端相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! CP移除，并做FFT变换
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::RemoveCP_FFT(int ue_cell_idx) {
  int r = 0;
  int l = 0;
  int Acc_Sample = 0;
  Eigen::FFT<float> fft;
  cfvec fft_in(Cell_Specific[ue_cell_idx].OverSample_N_FFT);
  cfvec fft_out(Cell_Specific[ue_cell_idx].OverSample_N_FFT);
  float power_scale = 0.0f;
  int rb_idx = 0;
  int sc = 0;
  float sigma = 0.0f;
#if IDEAL_H_METHOD > 0
  int p = 0;
  int s = 0;
  int s_prime = 0;
  MEA[ue_cell_idx].Ideal_PDP.setZero();
#endif
  power_scale = 1.0f / MySqrt((float)Cell_Specific[ue_cell_idx].OverSample_N_FFT);
  for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
#if IDEAL_H_METHOD > 0
      for (p = 0; p < Cell_Specific[ue_cell_idx].TxNum; p++) {
        for (s = 0; s < Cell_Specific[ue_cell_idx].OverSample_N_FFT; s++) {
          s_prime = l * Cell_Specific[ue_cell_idx].OverSample_N_FFT + (s - MIN(TA_Target_prime, 0) + Cell_Specific[ue_cell_idx].OverSample_N_FFT) % Cell_Specific[ue_cell_idx].OverSample_N_FFT;
          fft_in(s) = CHE[ue_cell_idx].Ideal_h[s_prime](r, p);
          if (p < 2) {
            MEA[ue_cell_idx].Ideal_PDP(r, s) += (fft_in(s).real() * fft_in(s).real() + fft_in(s).imag() * fft_in(s).imag());
          }
        }
        fft.fwd(fft_out, fft_in);
        CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + p].col(l).segment(0, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2) = fft_out.segment(Cell_Specific[ue_cell_idx].OverSample_N_FFT - Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2);
        CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + p].col(l).segment(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2) = fft_out.segment(1, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2);
      }
#endif
      if (TA_Target_prime >= 0)  //eq TA_Target set to 0
      {
        fft_in = Cell_Signal[ue_cell_idx].row(r).segment(Acc_Sample + Cell_Specific[ue_cell_idx].OverSample_N_CP_l[l], Cell_Specific[ue_cell_idx].OverSample_N_FFT);
      } else {
        fft_in.segment(0, Cell_Specific[ue_cell_idx].OverSample_N_FFT + TA_Target_prime) = Cell_Signal[ue_cell_idx].row(r).segment(Acc_Sample + Cell_Specific[ue_cell_idx].OverSample_N_CP_l[l] - TA_Target_prime, Cell_Specific[ue_cell_idx].OverSample_N_FFT + TA_Target_prime);
        fft_in.segment(Cell_Specific[ue_cell_idx].OverSample_N_FFT + TA_Target_prime, -TA_Target_prime) = Cell_Signal[ue_cell_idx].row(r).segment(Acc_Sample + Cell_Specific[ue_cell_idx].OverSample_N_CP_l[l], -TA_Target_prime);
      }
      fft.fwd(fft_out, fft_in);
      fft_out *= power_scale;
      RE[r].col(l).segment(0, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2) = fft_out.segment(Cell_Specific[ue_cell_idx].OverSample_N_FFT - Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2);
      RE[r].col(l).segment(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2) = fft_out.segment(1, Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2);
      if (Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
        for (rb_idx = 0; rb_idx < Cell_Specific[ue_cell_idx].N_DL_RB; rb_idx++) {
          if (UE_Specific[ue_cell_idx].I_ot_Offset[rb_idx] < 0.0f) {
            sigma = SQRT_2_RECIP * powf(10.0f, -UE_Specific[ue_cell_idx].I_ot_Offset[rb_idx] / 20.0f);
            for (sc = 0; sc < Cell_Specific[ue_cell_idx].N_RB_sc; sc++) {
              RE[r](rb_idx * Cell_Specific[ue_cell_idx].N_RB_sc + sc, l) += gauss_cpx(0.0f, sigma, &seed);
            }
          }
        }
      }
    }
    Acc_Sample += (Cell_Specific[ue_cell_idx].OverSample_N_CP_l[l] + Cell_Specific[ue_cell_idx].OverSample_N_FFT);
  }
}
