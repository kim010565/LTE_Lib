/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_CHE.cpp
    \brief UE信道估计实现文件
    内容 ：UE_C类信道估计相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"
#define Corr_Calc 0  //!< 1: Enable; 0: Disable

//! CRS LS信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CRS_LSCheEst(int ue_cell_idx) {
  const int RS_Num[4] = {4, 4, 2, 2};
  int RS_Pos[4][4];
  int n_s = 0;
  uint32 c_init = 0;
  int l = 0;
  int k = 0;
  const int v[4][4] = {{0, 3, 0, 3}, {3, 0, 3, 0}, {0, 3, 0, 0}, {3, 6, 0, 0}};
  int p = 0;
  int m = 0;
  int m_prime = 0;
  int l_idx = 0;
  int v_shift = 0;
  bvec c;
  int v_mod_6 = 0;
  int offset = 0;
  int r = 0;
  cpxf temp = 0.0f;
  if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
    RS_Pos[0][0] = RS_Pos[1][0] = 0;
    RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[0][2] = RS_Pos[1][2] = Cell_Specific[ue_cell_idx].N_DL_symb;
    RS_Pos[0][3] = RS_Pos[1][3] = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[2][0] = RS_Pos[3][0] = 1;
    RS_Pos[2][1] = RS_Pos[3][1] = Cell_Specific[ue_cell_idx].N_DL_symb + 1;
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 6;
    for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
      for (l_idx = 0; l_idx < RS_Num[p]; l_idx++) {
        if (RS_Pos[p][l_idx] < Cell_Specific[ue_cell_idx].DL_Symb_Num) {
          CHE[ue_cell_idx].CRS_Num[p] = l_idx + 1;
          l = RS_Pos[p][l_idx];
          n_s = 2 * Cell_Specific[ue_cell_idx].n_subframe + RS_Pos[p][l_idx] / Cell_Specific[ue_cell_idx].N_DL_symb;
          c_init = 1024 * (7 * (n_s + 1) + (l % Cell_Specific[ue_cell_idx].N_DL_symb) + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1) + 2 * Cell_Specific[ue_cell_idx].N_cell_ID + Cell_Specific[ue_cell_idx].N_CP;
          PNSequence_Gen(c_init, c, 4 * N_MAX_DL_RB);
          v_mod_6 = (v[p][l_idx] + v_shift) % 6;
          offset = N_MAX_DL_RB - Cell_Specific[ue_cell_idx].N_DL_RB;
          for (m = 0; m < 2 * Cell_Specific[ue_cell_idx].N_DL_RB; m++) {
            k = 6 * m + v_mod_6;
            m_prime = 2 * (m + offset);
            temp = cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
            for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
              CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](m, l_idx) = RE[r](k, l) * temp;
#if 2 == IDEAL_H_METHOD
              CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](m, l_idx) = CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](k, l);
#endif
            }
            RE_Type(k, l) = CRS;
          }
        }
      }
    }
  }
}

//! Port5 DMRS LS信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port5_LSCheEst(int ue_cell_idx) {
  int l_prime = 0;
  ivec l;
  int k_prime = 0;
  int k = 0;
  int m_prime = 0;
  int m = 0;
  uint32 c_init = 0;
  int v_shift = 0;
  int n_PRB = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  bvec c;
  int r = 0;
  cpxf temp = 0.0f;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    c_init = 65536 * (Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1) + UE_Specific[ue_cell_idx].RNTI;
    PNSequence_Gen(c_init, c, 24 * UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size());
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      l.resize(4);
      l << 3, 6, 9, 12;
      for (l_prime = 0; l_prime < 4; l_prime++) {
        if (l(l_prime) < Cell_Specific[ue_cell_idx].DL_Symb_Num) {
          CHE[ue_cell_idx].DMRS_Num_Port5 = l_prime + 1;
          for (m_prime = 0; m_prime < 3 * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][m_prime / 3];
            k_prime = (0 == (l_prime & 1)) ? (4 * m_prime + v_shift) : (4 * m_prime + ((2 + v_shift) & 3));
            k = k_prime % Cell_Specific[ue_cell_idx].N_RB_sc + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB;
            m = 2 * (3 * l_prime * UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() + m_prime);
            if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port5 == RE_Type(k, l(l_prime))) {
              temp = cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime) = RE[r](k, l(l_prime)) * temp;
              }
              RE_Type(k, l(l_prime)) = DMRS_Port5;
            }
          }
        }
      }
    } else {
      l.resize(3);
      l << 4, 7, 10;
      for (l_prime = 0; l_prime < 3; l_prime++) {
        if (l(l_prime) < Cell_Specific[ue_cell_idx].DL_Symb_Num) {
          CHE[ue_cell_idx].DMRS_Num_Port5 = l_prime + 1;
          for (m_prime = 0; m_prime < 4 * (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][m_prime / 4];
            k_prime = (1 != l_prime) ? (3 * m_prime + v_shift) : (3 * m_prime + ((2 + v_shift) % 3));
            k = k_prime % Cell_Specific[ue_cell_idx].N_RB_sc + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB;
            m = 2 * (4 * l_prime * UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() + m_prime);
            if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port5 == RE_Type(k, l(l_prime))) {
              temp = cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
              for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                CHE[ue_cell_idx].DMRS_LS_H_Port5[r](m_prime, l_prime) = RE[r](k, l(l_prime)) * temp;
              }
              RE_Type(k, l(l_prime)) = DMRS_Port5;
            }
          }
        }
      }
    }
  }
}

//! Port7~14 DMRS LS信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port7to14_LSCheEst(int ue_cell_idx) {
  int l_prime = 0;
  ivec l;
  int k_prime = 0;
  int k = 0;
  int m_prime = 0;
  int m = 0;
  uint32 c_init = 0;
  int n_PRB = 0;
  int p = 0;
  int i = 0;
  float w = 0.0f;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  bvec c;
  int r = 0;
  cpxf temp = 0.0f;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (-1 == UE_Specific[ue_cell_idx].n_DMRS_ID[UE_Specific[ue_cell_idx].n_SCID[DCI_idx]] || DCI_Format_1A == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
      c_init = 65536 * (Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * Cell_Specific[ue_cell_idx].N_cell_ID + 1) + UE_Specific[ue_cell_idx].n_SCID[DCI_idx];
    } else {
      c_init = 65536 * (Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * UE_Specific[ue_cell_idx].n_DMRS_ID[UE_Specific[ue_cell_idx].n_SCID[DCI_idx]] + 1) + UE_Specific[ue_cell_idx].n_SCID[DCI_idx];
    }
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      PNSequence_Gen(c_init, c, 24 * N_MAX_DL_RB);
      l.resize(4);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 3, 5, 6;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 3, 9, 10;
      } else {
        l << 5, 6, 12, 13;
      }
      for (p = (int)Port7; p < ((int)Port7 + MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx])); p++) {
        for (l_prime = 0; l_prime < 4; l_prime++) {
          for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); i++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i];
            for (m_prime = 0; m_prime < 3; m_prime++) {
              k_prime = ((int)Port7 == p || (int)Port8 == p || (int)Port11 == p || (int)Port13 == p) ? 1 : 0;
              k = 5 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
              w = (0 == ((m_prime + n_PRB) & 1)) ? Seq_w_NormalCP[p - (int)Port7][l_prime] : Seq_w_NormalCP[p - (int)Port7][3 - l_prime];
              m = 2 * (3 * l_prime * N_MAX_DL_RB + 3 * n_PRB + m_prime);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port7_14 == RE_Type(k, l(l_prime))) {
                temp = w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                if (UE_Specific[ue_cell_idx].N_L[DCI_idx] <= 4) {
                  if (0 == (l_prime & 1)) {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](3 * i + m_prime, l_prime / 2) = RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  } else {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](3 * i + m_prime, l_prime / 2) += RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  }
                } else {
                  if (0 == l_prime) {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](3 * i + m_prime, 0) = RE[r](k, l(l_prime)) * temp * 0.25f;
                    }
                  } else {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](3 * i + m_prime, 0) += RE[r](k, l(l_prime)) * temp * 0.25f;
                    }
                  }
                }
                RE_Type(k, l(l_prime)) = DMRS_Port7_14;
              }
            }
          }
        }
      }
    } else {
      PNSequence_Gen(c_init, c, 32 * N_MAX_DL_RB);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l.resize(2);
        l << 4, 5;
      } else {
        l.resize(4);
        l << 4, 5, 10, 11;
      }
      for (p = (int)Port7; p < ((int)Port7 + MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx])); p++) {
        for (l_prime = 0; l_prime < (int)l.size(); l_prime++) {
          for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); i++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i];
            for (m_prime = 0; m_prime < 4; m_prime++) {
              k_prime = (l(l_prime) <= 5) ? 1 : 2;
              k = 3 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
              w = (0 == (m_prime & 1)) ? Seq_w_NormalCP[p - (int)Port7][l_prime] : Seq_w_NormalCP[p - (int)Port7][1 - l_prime];
              m = 2 * (4 * l_prime * N_MAX_DL_RB + 4 * n_PRB + m_prime);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port7_14 == RE_Type(k, l(l_prime))) {
                temp = w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                if (0 == (l_prime & 1)) {
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](4 * i + m_prime, l_prime / 2) = RE[r](k, l(l_prime)) * temp * 0.5f;
                  }
                } else {
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](4 * i + m_prime, l_prime / 2) += RE[r](k, l(l_prime)) * temp * 0.5f;
                  }
                }
                RE_Type(k, l(l_prime)) = DMRS_Port7_14;
              }
            }
          }
        }
      }
    }
  }
}

//! EPDCCH DMRS LS信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMRS_LSCheEst(int ue_cell_idx) {
  int l_prime = 0;
  ivec l;
  int k_prime = 0;
  int k = 0;
  int m_prime = 0;
  int m = 0;
  uint32 c_init = 0;
  int n_PRB = 0;
  int p = 0;
  int i = 0;
  int set_idx = 0;
  float w = 0.0f;
  bvec c;
  int r = 0;
  cpxf temp = 0.0f;
  if (UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      l.resize(4);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 3, 5, 6;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l << 2, 3, 9, 10;
      } else {
        l << 5, 6, 12, 13;
      }
      for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
        c_init = 65536 * (Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * UE_Specific[ue_cell_idx].n_EPDCCH_ID[set_idx] + 1) + 2;
        PNSequence_Gen(c_init, c, 24 * N_MAX_DL_RB);
        for (p = (int)Port107; p < ((int)Port107 + 4); p++) {
          for (l_prime = 0; l_prime < 4; l_prime++) {
            for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][i];
              for (m_prime = 0; m_prime < 3; m_prime++) {
                k_prime = ((int)Port107 == p || (int)Port108 == p) ? 1 : 0;
                k = 5 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                w = (0 == ((m_prime + n_PRB) & 1)) ? Seq_w_NormalCP[p - (int)Port107][l_prime] : Seq_w_NormalCP[p - (int)Port107][3 - l_prime];
                m = 2 * (3 * l_prime * N_MAX_DL_RB + 3 * n_PRB + m_prime);
                if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port107_110 == RE_Type(k, l(l_prime))) {
                  temp = w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                  if (0 == (l_prime & 1)) {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](3 * i + m_prime, l_prime / 2) = RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  } else {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](3 * i + m_prime, l_prime / 2) += RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  }
                  RE_Type(k, l(l_prime)) = DMRS_Port107_110;
                }
              }
            }
          }
        }
      }
    } else {
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        l.resize(2);
        l << 4, 5;
      } else {
        l.resize(4);
        l << 4, 5, 10, 11;
      }
      for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
        c_init = 65536 * (Cell_Specific[ue_cell_idx].n_subframe + 1) * (2 * UE_Specific[ue_cell_idx].n_EPDCCH_ID[set_idx] + 1) + 2;
        PNSequence_Gen(c_init, c, 32 * N_MAX_DL_RB);
        for (p = (int)Port107; p < ((int)Port107 + 2); p++) {
          for (l_prime = 0; l_prime < l.size(); l_prime++) {
            for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][i];
              for (m_prime = 0; m_prime < 4; m_prime++) {
                k_prime = (l(l_prime) <= 5) ? 1 : 2;
                k = 3 * m_prime + Cell_Specific[ue_cell_idx].N_RB_sc * n_PRB + k_prime;
                w = (0 == (m_prime & 1)) ? Seq_w_NormalCP[p - (int)Port107][l_prime] : Seq_w_NormalCP[p - (int)Port107][1 - l_prime];
                m = 2 * (4 * l_prime * N_MAX_DL_RB + 4 * n_PRB + m_prime);
                if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port107_110 == RE_Type(k, l(l_prime))) {
                  temp = w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                  if (0 == (l_prime & 1)) {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](4 * i + m_prime, l_prime / 2) = RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  } else {
                    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](4 * i + m_prime, l_prime / 2) += RE[r](k, l(l_prime)) * temp * 0.5f;
                    }
                  }
                  RE_Type(k, l(l_prime)) = DMRS_Port107_110;
                }
              }
            }
          }
        }
      }
    }
  }
}

//! CSI RS LS信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CSI_RS_LSCheEst(int ue_cell_idx) {
  int l_prime_prime = 0;
  int l_prime = 0;
  ivec l;
  int k_prime = 0;
  int k = 0;
  int m_prime = 0;
  int m = 0;
  uint32 c_init = 0;
  int p = 0;
  float w = 0.0f;
  bvec c;
  int NZP_idx = 0;
  int T_CSI_RS = 0;
  int Delta_CSI_RS = 0;
  int n_s = 0;
  int r = 0;
  cpxf temp = 0.0f;
  if ((UE_Specific[ue_cell_idx].NZP_CSI_Set_Num > 0 || UE_Specific[ue_cell_idx].ZP_CSI_Set_Num > 0) && U != Cell_Specific[ue_cell_idx].SubframeType && S != Cell_Specific[ue_cell_idx].SubframeType) {
    for (NZP_idx = 0; NZP_idx < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; NZP_idx++) {
      UE_Specific[ue_cell_idx].CSI_Flag_NZP[NZP_idx] = false;
      Get_T_CSI_RS_Delta_CSI_RS(UE_Specific[ue_cell_idx].I_CSI_RS_NZP[NZP_idx], &T_CSI_RS, &Delta_CSI_RS);
      if (0 == ((10 * Cell_Specific[ue_cell_idx].n_f + Cell_Specific[ue_cell_idx].n_subframe - Delta_CSI_RS + T_CSI_RS) % T_CSI_RS)) {
        UE_Specific[ue_cell_idx].CSI_Flag_NZP[NZP_idx] = true;
        l.resize(2);
        if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
          k_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][1];
          n_s = CSI_RS_Signal_Config_NormalCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][2];
          if (UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx] <= 19) {
            l(0) = l_prime + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
            l(1) = l_prime + 1 + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
          } else {
            l(0) = l_prime + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
            l(1) = l_prime + 2 + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
          }
        } else {
          k_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][1];
          n_s = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][2];
          l(0) = l_prime + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
          l(1) = l_prime + 1 + n_s * Cell_Specific[ue_cell_idx].N_DL_symb;
        }
        for (p = (int)Port15; p < ((int)Port15 + UE_Specific[ue_cell_idx].CSIPortNum_NZP[NZP_idx]); p++) {
          for (l_prime_prime = 0; l_prime_prime < 2; l_prime_prime++) {
            c_init = 1024 * (7 * (n_s + 1) + (l(l_prime_prime) % Cell_Specific[ue_cell_idx].N_DL_symb) + 1) * (2 * UE_Specific[ue_cell_idx].N_CSI_ID_NZP[NZP_idx] + 1) + 2 * UE_Specific[ue_cell_idx].N_CSI_ID_NZP[NZP_idx] + Cell_Specific[ue_cell_idx].N_CP;
            PNSequence_Gen(c_init, c, 2 * N_MAX_DL_RB);
            if ((int)Port15 == p || (int)Port17 == p || (int)Port19 == p || (int)Port21 == p) {
              w = 1.0f;
            } else {
              w = (0 == l_prime_prime) ? 1.0f : -1.0f;
            }
            for (m = 0; m < Cell_Specific[ue_cell_idx].N_DL_RB; m++) {
              m_prime = 2 * (m + (N_MAX_DL_RB - Cell_Specific[ue_cell_idx].N_DL_RB) / 2);
              k = k_prime + 12 * m + ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? CSI_k_offset_NormalCP[p - (int)Port15] : CSI_k_offset_ExtendedCP[p - (int)Port15]);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || CSI_RS == RE_Type(k, l(l_prime))) {
                temp = w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), -SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
                if (0 == l_prime_prime) {
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCSIRSPORTNUM + p - (int)Port15](m, NZP_idx) = RE[r](k, l(l_prime)) * temp * 0.5f;
                  }
                } else {
                  for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
                    CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCSIRSPORTNUM + p - (int)Port15](m, NZP_idx) += RE[r](k, l(l_prime)) * temp * 0.5f;
                  }
                }
                RE_Type(k, l(l_prime)) = CSI_RS;
              }
            }
          }
        }
      }
    }
  }
}

//! CRS 频域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CRS_FilterF(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int v_shift = 0;
  float SNR_Recip = 0.0f;
  cpxf temp = 0.0f;
  cpxf scale_tmp = 0.0f;
  int N_DL_SC = 0;
  int N_F = 0;
  const int v[4][4] = {{0, 3, 0, 3}, {3, 0, 3, 0}, {0, 3, 0, 0}, {3, 6, 0, 0}};
#if 3 == IDEAL_H_METHOD
  int RS_Pos[4][4];
#endif
  cfmat FAC, FAC_INV, FCC, FCoef;
  if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
#if 3 == IDEAL_H_METHOD
    RS_Pos[0][0] = RS_Pos[1][0] = 0;
    RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[0][2] = RS_Pos[1][2] = Cell_Specific[ue_cell_idx].N_DL_symb;
    RS_Pos[0][3] = RS_Pos[1][3] = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[2][0] = RS_Pos[3][0] = 1;
    RS_Pos[2][1] = RS_Pos[3][1] = Cell_Specific[ue_cell_idx].N_DL_symb + 1;
#endif
    N_F = MIN(N_FilterF, 2 * Cell_Specific[ue_cell_idx].N_DL_RB);
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 6;
    N_DL_SC = Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc;
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      SNR_Recip = MEA[ue_cell_idx].CRS_N(r) / MEA[ue_cell_idx].CRS_S(r);
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      //FAC
      FAC.resize(N_F, N_F);
      for (i = 0; i < N_F; i++) {
        FAC(i, i) = 1.0f + SNR_Recip;
        for (j = i + 1; j < N_F; j++) {
          FAC(j, i) = MEA[ue_cell_idx].R_F(r, (j - i) * 6);
          FAC(i, j) = conj(FAC(j, i));
        }
      }
      //INV FAC
      Hermitian_Matrix_INV(FAC, FAC_INV);
      //FCC
      FCC.resize(N_F * 6 + 5, N_F);
      for (i = 0; i < (N_F * 6 + 5); i++) {
        for (j = 0; j < N_F; j++) {
          k = i - (j * 6 + 5);
          FCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_F(r, k) : conj(MEA[ue_cell_idx].R_F(r, -k));
        }
      }
      //FCoef
      FCoef.resize(N_F * 6 + 5, N_F);
      FCoef = FCC * FAC_INV;
      //FilterF_Noise_Scale
      MEA[ue_cell_idx].FilterF_Noise_Scale(r) = 0.0f;
      for (i = 0; i < N_F; i++) {
        MEA[ue_cell_idx].FilterF_Noise_Scale(r) += (FCoef((N_F * 6 / 2) + 5, i).real() * FCoef((N_F * 6 / 2) + 5, i).real() + FCoef((N_F * 6 / 2) + 5, i).imag() * FCoef((N_F * 6 / 2) + 5, i).imag());
      }
      MEA[ue_cell_idx].FilterF_Noise_Scale(r) = MIN(1.0f / MEA[ue_cell_idx].FilterF_Noise_Scale(r), N_F);
      MEA[ue_cell_idx].FilterF_Noise_Scale(r) = MAX(MEA[ue_cell_idx].FilterF_Noise_Scale(r), 1.0f);
      //Interp
      for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
        for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
          k = 5 - (v[p][l_idx] + v_shift) % 6;
          for (i = 0; i < N_DL_SC; i++) {
            temp = 0.0f;
            scale_tmp = 0.0f;
            if ((i + k) < (N_F * 6 / 2))  //Part1
            {
              for (j = 0; j < N_F; j++) {
                temp += (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](j, l_idx) * FCoef((i + k), j));
                scale_tmp += (conj(FCC((i + k), j)) * FCoef((i + k), j));
              }
            } else if ((i + k) > (N_DL_SC + 5 - (N_F * 6 / 2)))  //Part 3
            {
              for (j = 0; j < N_F; j++) {
                temp += (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p](2 * Cell_Specific[ue_cell_idx].N_DL_RB - N_F + j, l_idx) * FCoef((i + k) + N_F * 6 - N_DL_SC, j));
                scale_tmp += (conj(FCC((i + k) + N_F * 6 - N_DL_SC, j)) * FCoef((i + k) + N_F * 6 - N_DL_SC, j));
              }
            } else  //Part 2
            {
              for (j = 0; j < N_F; j++) {
                temp += (CHE[ue_cell_idx].CRS_LS_H[r * MAXCRSPORTNUM + p]((i + k) / 6 - N_F / 2 + j, l_idx) * FCoef(((i + k) % 6) + (N_F * 6 / 2), j));
                scale_tmp += (conj(FCC(((i + k) % 6) + (N_F * 6 / 2), j)) * FCoef(((i + k) % 6) + (N_F * 6 / 2), j));
              }
            }
            CHE[ue_cell_idx].CRS_H_F[r * MAXCRSPORTNUM + p](i, l_idx) = temp / scale_tmp.real();
            //CHE[ue_cell_idx].CRS_H_F[r * MAXCRSPORTNUM + p](i, l_idx) = temp;
#if 3 == IDEAL_H_METHOD
            CHE[ue_cell_idx].CRS_H_F[r * MAXCRSPORTNUM + p](i, l_idx) = CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](i, RS_Pos[p][l_idx]);
#endif
          }
        }
      }
    }
  }
}

//! Port5 DMRS频域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port5_FilterF(int ue_cell_idx) {
  int r = 0;
  int l_idx = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int v_shift = 0;
  float SNR_Recip = 0.0f;
  cpxf temp = 0.0f;
  int N0 = 0;
  int N_F = 0;
  int DMRS_Num = CHE[ue_cell_idx].DMRS_Num_Port5;
  int rb_idx = 0;
  int n_PRB = 0;
  cfmat FAC, FAC_INV, FCC, FCoef;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    N0 = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 4 : 3;
    N_F = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 3 : 4;
    v_shift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      SNR_Recip = MEA[ue_cell_idx].CRS_N_2nd(r) / MEA[ue_cell_idx].DMRS_S_Port5(r);
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      //FAC
      FAC.resize(N_F, N_F);
      for (i = 0; i < N_F; i++) {
        FAC(i, i) = 1.0f + SNR_Recip;
        for (j = i + 1; j < N_F; j++) {
          FAC(j, i) = MEA[ue_cell_idx].R_F(r, (j - i) * N0);
          FAC(i, j) = conj(FAC(j, i));
        }
      }
      //INV FAC
      Hermitian_Matrix_INV(FAC, FAC_INV);
      //FCC
      FCC.resize(N_F * N0 + N0 - 1, N_F);
      for (i = 0; i < (N_F * N0 + N0 - 1); i++) {
        for (j = 0; j < N_F; j++) {
          k = i - (j + 1) * N0 + 1;
          FCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_F(r, k) : conj(MEA[ue_cell_idx].R_F(r, -k));
        }
      }
      //FCoef
      FCoef.resize(N_F * N0 + N0 - 1, N_F);
      FCoef = FCC * FAC_INV;
      //FilterF_Noise_Scale
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r) = 0.0f;
      for (i = 0; i < N_F; i++) {
        MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r) += (FCoef((N_F * N0 / 2) + N0 - 1, i).real() * FCoef((N_F * N0 / 2) + N0 - 1, i).real() + FCoef((N_F * N0 / 2) + N0 - 1, i).imag() * FCoef((N_F * N0 / 2) + N0 - 1, i).imag());
      }
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r) = MIN(1.0f / MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r), N_F);
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r) = MAX(MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r), 1.0f);
      //Interp
      for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
        if (1 == (l_idx & 1)) {
          k = N0 - 1 - (2 + v_shift) % N0;
        } else {
          k = N0 - 1;
        }
        for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
          n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx];
          for (i = 0; i < Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
            temp = 0.0f;
            for (j = 0; j < N_F; j++) {
              temp += (CHE[ue_cell_idx].DMRS_LS_H_Port5[r](rb_idx * N_F + j, l_idx) * FCoef((i + k), j));
            }
            CHE[ue_cell_idx].DMRS_H_F_Port5[r](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx) = temp;
          }
        }
      }
    }
  }
}

//! Port7~14 DMRS频域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port7to14_FilterF(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  float SNR_Recip = 0.0f;
  cpxf temp = 0.0f;
  int N0 = 0;
  int N_F = 0;
  int Offset = 0;
  int DMRS_Num = 0;
  int rb_idx = 0;
  int P_prime = 0;
  int Granularity_idx = 0;
  cfmat FAC[3], FAC_INV[3], FCC[3], FCoef[3];
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  vector<int> PRG_size;
  vector<int> PRG_startPRB;
  int counter_size = 0;
  int PRG_idx = 0;
  int NZP_idx = -1;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    N0 = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 5 : 3;
    N_F = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 3 : 4;
    Offset = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 0 : 1;
    if (ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type && S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
      DMRS_Num = 1;
    } else {
      DMRS_Num = 2;
    }
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
    }
    if (TM7 == UE_Specific[ue_cell_idx].TM || TM8 == UE_Specific[ue_cell_idx].TM || (TM9 == UE_Specific[ue_cell_idx].TM && !UE_Specific[ue_cell_idx].PMI_RI_Report) || (TM10 == UE_Specific[ue_cell_idx].TM && !UE_Specific[ue_cell_idx].PMI_RI_Report)) {
      P_prime = 1;
    } else {
      if (Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        P_prime = 1;
      } else if (Cell_Specific[ue_cell_idx].N_DL_RB <= 28) {
        P_prime = 2;
      } else if (Cell_Specific[ue_cell_idx].N_DL_RB <= 63) {
        P_prime = 3;
      } else {
        P_prime = 2;
      }
    }
    for (rb_idx = 0, counter_size = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
      counter_size++;
      if ((P_prime - 1) == (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] % P_prime) || (int)(UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() - 1) == rb_idx) {
        PRG_size.push_back(counter_size);
        PRG_startPRB.push_back(UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx] - (counter_size - 1));
        counter_size = 0;
      }
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      SNR_Recip = ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)) / MEA[ue_cell_idx].DMRS_S_Port7to14(r);
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      for (Granularity_idx = 0; Granularity_idx < P_prime; Granularity_idx++) {
        //FAC
        FAC[Granularity_idx].resize((Granularity_idx + 1) * N_F, (Granularity_idx + 1) * N_F);
        for (i = 0; i < ((Granularity_idx + 1) * N_F); i++) {
          FAC[Granularity_idx](i, i) = 1.0f + SNR_Recip;
          for (j = i + 1; j < ((Granularity_idx + 1) * N_F); j++) {
            FAC[Granularity_idx](j, i) = (NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, (j - i) * N0) : MEA[ue_cell_idx].R_F(r, (j - i) * N0);
            FAC[Granularity_idx](i, j) = conj(FAC[Granularity_idx](j, i));
          }
        }
        //INV FAC
        Hermitian_Matrix_INV(FAC[Granularity_idx], FAC_INV[Granularity_idx]);
        //FCC
        FCC[Granularity_idx].resize((Granularity_idx + 1) * N_F * N0 + 1, (Granularity_idx + 1) * N_F);
        for (i = 0; i < ((Granularity_idx + 1) * N_F * N0 + 1); i++) {
          for (j = 0; j < (Granularity_idx + 1) * N_F; j++) {
            k = i - j * N0 - 1 - Offset;
            if (NZP_idx >= 0) {
              FCC[Granularity_idx](i, j) = (k >= 0) ? MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, k) : conj(MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, -k));
            } else {
              FCC[Granularity_idx](i, j) = (k >= 0) ? MEA[ue_cell_idx].R_F(r, k) : conj(MEA[ue_cell_idx].R_F(r, -k));
            }
          }
        }
        //FCoef
        FCoef[Granularity_idx].resize((Granularity_idx + 1) * N_F * N0 + 1, (Granularity_idx + 1) * N_F);
        FCoef[Granularity_idx] = FCC[Granularity_idx] * FAC_INV[Granularity_idx];
      }
      //FilterF_Noise_Scale
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r) = 0.0f;
      for (i = 0; i < P_prime * N_F; i++) {
        MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r) += (FCoef[P_prime - 1]((P_prime * N_F * N0 / 2) + 1, i).real() * FCoef[P_prime - 1]((P_prime * N_F * N0 / 2) + 1, i).real() + FCoef[P_prime - 1]((P_prime * N_F * N0 / 2) + 1, i).imag() * FCoef[P_prime - 1]((P_prime * N_F * N0 / 2) + 1, i).imag());
      }
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r) = MIN(1.0f / MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r), P_prime * N_F);
      MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r) = MAX(MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r), 1.0f);
      //Interp
      for (p = (int)Port7; p < ((int)Port7 + MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx])); p++) {
        for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
          if ((((int)Port9 == p || (int)Port10 == p || (int)Port12 == p || (int)Port14 == p) && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || ((0 == l_idx) && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type)) {
            k = 1;
          } else {
            k = 0;
          }
          for (PRG_idx = 0; PRG_idx < (int)PRG_size.size(); PRG_idx++) {
            for (i = 0; i < PRG_size[PRG_idx] * Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
              temp = 0.0f;
              for (j = 0; j < PRG_size[PRG_idx] * N_F; j++) {
                temp += (CHE[ue_cell_idx].DMRS_LS_H_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](PRG_idx * PRG_size[PRG_idx] * N_F + j, l_idx) * FCoef[PRG_size[PRG_idx] - 1]((i + k), j));
              }
              CHE[ue_cell_idx].DMRS_H_F_Port7to14[r * MAXDMRSPORTNUM + p - (int)Port7](PRG_startPRB[PRG_idx] * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx) = temp;
            }
          }
        }
      }
    }
  }
}

//! EPDCCH DMRS频域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMRS_FilterF(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  float SNR_Recip = 0.0f;
  cpxf temp = 0.0f;
  int N0 = 0;
  int N_F = 0;
  int Offset = 0;
  int n_PRB = 0;
  int DMRS_Num = 0;
  int rb_idx = 0;
  int set_idx = 0;
  cfmats FAC, FAC_INV, FCoef;
  cfmat FCC;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int NZP_idx = -1;
  if (UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    N0 = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 5 : 3;
    N_F = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 3 : 4;
    Offset = (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 0 : 1;
    if (ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type && S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
      DMRS_Num = 1;
    } else {
      DMRS_Num = 2;
    }
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      FAC.resize(UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
      FAC_INV.resize(UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
      for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
        //SNR RECIP
        SNR_Recip = ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)) / MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx);
        SNR_Recip = MAX(SNR_Recip, 0.001f);
        //FAC
        FAC[set_idx].resize(N_F, N_F);
        for (i = 0; i < N_F; i++) {
          FAC[set_idx](i, i) = 1.0f + SNR_Recip;
          for (j = i + 1; j < N_F; j++) {
            FAC[set_idx](j, i) = (NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, (j - i) * N0) : MEA[ue_cell_idx].R_F(r, (j - i) * N0);
            FAC[set_idx](i, j) = conj(FAC[set_idx](j, i));
          }
        }
        //INV FAC
        Hermitian_Matrix_INV(FAC[set_idx], FAC_INV[set_idx]);
      }
      //FCC
      FCC.resize(N_F * N0 + 1, N_F);
      for (i = 0; i < (N_F * N0 + 1); i++) {
        for (j = 0; j < N_F; j++) {
          k = i - j * N0 - 1 - Offset;
          if (NZP_idx >= 0) {
            FCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, k) : conj(MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, -k));
          } else {
            FCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_F(r, k) : conj(MEA[ue_cell_idx].R_F(r, -k));
          }
        }
      }
      FCoef.resize(UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
      for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
        //FCoef
        FCoef[set_idx].resize(N_F * N0 + 1, N_F);
        FCoef[set_idx] = FCC * FAC_INV[set_idx];
        //FilterF_Noise_Scale
        MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx) = 0.0f;
        for (i = 0; i < N_F; i++) {
          MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx) += (FCoef[set_idx]((N_F * N0 / 2) + 1, i).real() * FCoef[set_idx]((N_F * N0 / 2) + 1, i).real() + FCoef[set_idx]((N_F * N0 / 2) + 1, i).imag() * FCoef[set_idx]((N_F * N0 / 2) + 1, i).imag());
        }
        MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx) = MIN(1.0f / MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx), N_F);
        MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx) = MAX(MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx), 1.0f);
      }
      //Interp
      for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
        for (p = (int)Port107; p < ((int)Port107 + 4); p++) {
          for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
            if ((((int)Port109 == p || (int)Port110) == p && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || ((0 == l_idx) && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type)) {
              k = 1;
            } else {
              k = 0;
            }
            for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); rb_idx++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][rb_idx];
              for (i = 0; i < Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
                temp = 0.0f;
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](rb_idx * N_F + j, l_idx) * FCoef[set_idx]((i + k), j));
                }
                CHE[ue_cell_idx].EPDCCH_DMRS_H_F[set_idx * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].RxNum + r * MAXEPDCCHRSPORTNUM + p - (int)Port107](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx) = temp;
              }
            }
          }
        }
      }
    }
  }
}

//! CSI RS频域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CSI_RS_FilterF(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int NZP_idx = 0;
  int k_prime = 0;
  int N_F = 0;
  float SNR_Recip = 0.0f;
  cpxf temp = 0.0f;
  int N_DL_SC_quad = 0;
  cfmat FAC, FAC_INV, FCC, FCoef;
  if ((UE_Specific[ue_cell_idx].NZP_CSI_Set_Num > 0 || UE_Specific[ue_cell_idx].ZP_CSI_Set_Num > 0) && U != Cell_Specific[ue_cell_idx].SubframeType && S != Cell_Specific[ue_cell_idx].SubframeType) {
    for (NZP_idx = 0; NZP_idx < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; NZP_idx++) {
      if (UE_Specific[ue_cell_idx].CSI_Flag_NZP[NZP_idx]) {
        N_F = MIN(N_FilterF, Cell_Specific[ue_cell_idx].N_DL_RB / 2);
        N_DL_SC_quad = Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 4;
        if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
          k_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
        } else {
          k_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[ue_cell_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
        }
        for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          //SNR RECIP
          SNR_Recip = MEA[ue_cell_idx].CSI_RS_N(r, NZP_idx) / MEA[ue_cell_idx].CSI_RS_S(r, NZP_idx);
          SNR_Recip = MAX(SNR_Recip, 0.001f);
          //FAC
          FAC.resize(N_F, N_F);
          for (i = 0; i < N_F; i++) {
            FAC(i, i) = 1.0f + SNR_Recip;
            for (j = i + 1; j < N_F; j++) {
              FAC(j, i) = MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, (j - i) * 12);
              FAC(i, j) = conj(FAC(j, i));
            }
          }
          //INV FAC
          Hermitian_Matrix_INV(FAC, FAC_INV);
          //FCC
          FCC.resize(N_F * 12 + 11, N_F);
          for (i = 0; i < (N_F * 12 + 11); i++) {
            for (j = 0; j < N_F; j++) {
              k = i - (j + 1) * 12 + 1;
              FCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, k) : conj(MEA[ue_cell_idx].CSI_R_F[NZP_idx](r, -k));
            }
          }
          //FCoef
          FCoef.resize(N_F * 12 + 11, N_F);
          FCoef = FCC * FAC_INV;
          //Interp
          for (p = 0; p < UE_Specific[ue_cell_idx].CSIPortNum_NZP[NZP_idx]; p++) {
            k = 11 - (k_prime + ((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? CSI_k_offset_NormalCP[p - (int)Port15] : CSI_k_offset_ExtendedCP[p - (int)Port15]));
            //Half: N_DL_SC_quad-1
            for (i = 0; i < N_DL_SC_quad; i++) {
              temp = 0.0f;
              if ((i + k) < (N_F * 12 / 2))  //Part1
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](j, NZP_idx) * FCoef((i + k), j));
                }
              } else if ((i + k) > (N_DL_SC_quad + 11 - (N_F * 12 / 2)))  //Part 3
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](Cell_Specific[ue_cell_idx].N_DL_RB / 2 - N_F + j, NZP_idx) * FCoef((i + k) + N_F * 12 - N_DL_SC_quad, j));
                }
              } else  //Part 2
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p]((i + k) / 12 - N_F / 2 + j, NZP_idx) * FCoef(((i + k) % 12) + (N_F * 12 / 2), j));
                }
              }
              CHE[ue_cell_idx].CSI_RS_H_F[r * MAXCRSPORTNUM + p](i, NZP_idx) = temp;
            }
            //Half: N_DL_SC_quad~2*N_DL_SC_quad-1
            for (i = 0; i < N_DL_SC_quad; i++) {
              temp = 0.0f;
              if ((i + k) < (N_F * 12 / 2))  //Part1
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](Cell_Specific[ue_cell_idx].N_DL_RB / 2 + j, NZP_idx) * FCoef((i + k), j));
                }
              } else if ((i + k) > (N_DL_SC_quad + 11 - (N_F * 12 / 2)))  //Part 3
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](Cell_Specific[ue_cell_idx].N_DL_RB - N_F + j, NZP_idx) * FCoef((i + k) + N_F * 12 - N_DL_SC_quad, j));
                }
              } else  //Part 2
              {
                for (j = 0; j < N_F; j++) {
                  temp += (CHE[ue_cell_idx].CSI_RS_LS_H[r * MAXCRSPORTNUM + p](Cell_Specific[ue_cell_idx].N_DL_RB / 2 + (i + k) / 12 - N_F / 2 + j, NZP_idx) * FCoef(((i + k) % 12) + (N_F * 12 / 2), j));
                }
              }
              CHE[ue_cell_idx].CSI_RS_H_F[r * MAXCRSPORTNUM + p](i + N_DL_SC_quad, NZP_idx) = temp;
            }
          }
        }
      }
    }
  }
}

//! CRS时域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::CRS_FilterT(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int l_idx = 0;
  int l = 0;
  int sc = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  float SNR_Recip = 0.0f;
  int RS_Pos[4][4];
  cpxf temp = 0.0f;
  cpxf scale_tmp = 0.0f;
  cfmat TAC[2], TAC_INV[2], TCC[2], TCoef[2];
  if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
    RS_Pos[0][0] = RS_Pos[1][0] = 0;
    RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[0][2] = RS_Pos[1][2] = Cell_Specific[ue_cell_idx].N_DL_symb;
    RS_Pos[0][3] = RS_Pos[1][3] = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 3;
    RS_Pos[2][0] = RS_Pos[3][0] = 1;
    RS_Pos[2][1] = RS_Pos[3][1] = Cell_Specific[ue_cell_idx].N_DL_symb + 1;
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      //MEA[ue_cell_idx].FilterF_Noise_Scale(r) = 1.0f;
      SNR_Recip = MEA[ue_cell_idx].CRS_N(r) / (MEA[ue_cell_idx].FilterF_Noise_Scale(r) * MEA[ue_cell_idx].CRS_S(r));
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      //TAC[0] for Port 0/1
      TAC[0].resize(CHE[ue_cell_idx].CRS_Num[0], CHE[ue_cell_idx].CRS_Num[0]);
      for (i = 0; i < CHE[ue_cell_idx].CRS_Num[0]; i++) {
        TAC[0](i, i) = 1.0f + SNR_Recip;
        for (j = i + 1; j < CHE[ue_cell_idx].CRS_Num[0]; j++) {
          TAC[0](j, i) = MEA[ue_cell_idx].R_T(r, RS_Pos[0][j] - RS_Pos[0][i]);
          TAC[0](i, j) = conj(TAC[0](j, i));
        }
      }
      //TAC[1] for Port 2/3
      if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
        TAC[1].resize(CHE[ue_cell_idx].CRS_Num[2], CHE[ue_cell_idx].CRS_Num[2]);
        for (i = 0; i < CHE[ue_cell_idx].CRS_Num[2]; i++) {
          TAC[1](i, i) = 1.0f + SNR_Recip;
          for (j = i + 1; j < CHE[ue_cell_idx].CRS_Num[2]; j++) {
            TAC[1](j, i) = MEA[ue_cell_idx].R_T(r, RS_Pos[2][j] - RS_Pos[2][i]);
            TAC[1](i, j) = conj(TAC[1](j, i));
          }
        }
      }
      //INV TAC
      Hermitian_Matrix_INV(TAC[0], TAC_INV[0]);
      if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
        Hermitian_Matrix_INV(TAC[1], TAC_INV[1]);
      }
      //TCC[0] for Port 0/1
      TCC[0].resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, CHE[ue_cell_idx].CRS_Num[0]);
      for (i = 0; i < Cell_Specific[ue_cell_idx].DL_Symb_Num; i++) {
        for (j = 0; j < CHE[ue_cell_idx].CRS_Num[0]; j++) {
          k = i - RS_Pos[0][j];
          TCC[0](i, j) = (k >= 0) ? MEA[ue_cell_idx].R_T(r, k) : conj(MEA[ue_cell_idx].R_T(r, -k));
        }
      }
      //TCC[1] for Port 2/3
      if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
        TCC[1].resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, CHE[ue_cell_idx].CRS_Num[2]);
        for (i = 0; i < Cell_Specific[ue_cell_idx].DL_Symb_Num; i++) {
          for (j = 0; j < CHE[ue_cell_idx].CRS_Num[2]; j++) {
            k = i - RS_Pos[2][j];
            TCC[1](i, j) = (k >= 0) ? MEA[ue_cell_idx].R_T(r, k) : conj(MEA[ue_cell_idx].R_T(r, -k));
          }
        }
      }
      //FCoef
      TCoef[0].resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, CHE[ue_cell_idx].CRS_Num[0]);
      TCoef[0] = TCC[0] * TAC_INV[0];
      if (4 == Cell_Specific[ue_cell_idx].CRSPortNum) {
        TCoef[1].resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, CHE[ue_cell_idx].CRS_Num[2]);
        TCoef[1] = TCC[1] * TAC_INV[1];
      }
      //Interp
      for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
        if (p < 2) {
          for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
            for (sc = 0; sc < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); sc++) {
              temp = 0.0f;
              for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
                temp += CHE[ue_cell_idx].CRS_H_F[r * MAXCRSPORTNUM + p](sc, l_idx) * TCoef[0](l, l_idx);
              }
              scale_tmp = 0.0f;
              for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
                scale_tmp += conj(TCC[0](l, l_idx)) * TCoef[0](l, l_idx);
              }
              CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = temp / scale_tmp.real();
              //CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = temp;
#if 4 == IDEAL_H_METHOD
              /*WithFile("CRS_H_T");
                            FilePrint("%d %d %d %d %f %f, %f %f\n", r, p, l, sc,
                                            CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l).real(),
                                            CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l).imag(),
                                            CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real(),
                                            CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());*/
              CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l);
#endif
            }
          }
        } else {
          for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
            for (sc = 0; sc < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); sc++) {
              temp = 0.0f;
              for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
                temp += CHE[ue_cell_idx].CRS_H_F[r * MAXCRSPORTNUM + p](sc, l_idx) * TCoef[1](l, l_idx);
              }
              scale_tmp = 0.0f;
              for (l_idx = 0; l_idx < CHE[ue_cell_idx].CRS_Num[p]; l_idx++) {
                scale_tmp += conj(TCC[1](l, l_idx)) * TCoef[1](l, l_idx);
              }
              CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = temp / scale_tmp.real();
              //CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = temp;
#if 4 == IDEAL_H_METHOD
              /*WithFile("CRS_H_T");
                            FilePrint("%d %d %d %d %f %f, %f %f\n", r, p, l, sc,
                                            CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l).real(),
                                            CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l).imag(),
                                            CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real(),
                                            CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());*/
              CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l) = CHE[ue_cell_idx].Ideal_H_T[r * MAXCRSPORTNUM + PortToPhyAntenna[p]](sc, l);
#endif
            }
          }
        }
      }
    }
#if 1 == Corr_Calc
    //MIMO Rx Corr Calc
    float E_ab = 0.0f;
    float E_a = 0.0f;
    float E_b = 0.0f;
    float E_a2 = 0.0f;
    float E_b2 = 0.0f;
    int E_Counter = 0;
    float lu = 0.0f;
    if (UE_Specific[ue_cell_idx].RxNum > 1) {
      lu = 0.0f;
      for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
        for (r = 1; r < UE_Specific[ue_cell_idx].RxNum; r++) {
          E_ab = 0.0f;
          E_a = 0.0f;
          E_b = 0.0f;
          E_a2 = 0.0f;
          E_b2 = 0.0f;
          E_Counter = 0;
          for (p = 0; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
            for (sc = 0; sc < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); sc++) {
              E_ab += (CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_a += (CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).imag());
              E_b += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_a2 += (CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[(r - 1) * MAXCRSPORTNUM + p](sc, l).imag());
              E_b2 += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_Counter += 2;
            }
          }
          E_ab = E_ab / (float)E_Counter;
          E_a = E_a / (float)E_Counter;
          E_b = E_b / (float)E_Counter;
          E_a2 = E_a2 / (float)E_Counter;
          E_b2 = E_b2 / (float)E_Counter;
          lu += (E_ab - E_a * E_b) / MySqrt((E_a2 - E_a * E_a) * (E_b2 - E_b * E_b));
        }
      }
      lu = MyFabs(lu / (float)((UE_Specific[ue_cell_idx].RxNum - 1) * Cell_Specific[ue_cell_idx].DL_Symb_Num));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("Rx Corr: lu = %f\n", lu);
    }
    //MIMO Tx Corr Calc
    if (Cell_Specific[ue_cell_idx].CRSPortNum > 1) {
      lu = 0.0f;
      for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
        for (p = 1; p < Cell_Specific[ue_cell_idx].CRSPortNum; p++) {
          E_ab = 0.0f;
          E_a = 0.0f;
          E_b = 0.0f;
          E_a2 = 0.0f;
          E_b2 = 0.0f;
          E_Counter = 0;
          for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
            for (sc = 0; sc < (Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc); sc++) {
              E_ab += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_a += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).imag());
              E_b += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_a2 += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p - 1](sc, l).imag());
              E_b2 += (CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).real() + CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag() * CHE[ue_cell_idx].CRS_H_T[r * MAXCRSPORTNUM + p](sc, l).imag());
              E_Counter += 2;
            }
          }
          E_ab = E_ab / (float)E_Counter;
          E_a = E_a / (float)E_Counter;
          E_b = E_b / (float)E_Counter;
          E_a2 = E_a2 / (float)E_Counter;
          E_b2 = E_b2 / (float)E_Counter;
          lu += (E_ab - E_a * E_b) / MySqrt((E_a2 - E_a * E_a) * (E_b2 - E_b * E_b));
        }
      }
      lu = MyFabs(lu / (float)((Cell_Specific[ue_cell_idx].CRSPortNum - 1) * Cell_Specific[ue_cell_idx].DL_Symb_Num));
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("Tx Corr: lu = %f\n", lu);
    }
#endif
  }
}

//! Port5 DMRS时域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port5_FilterT(int ue_cell_idx) {
  int r = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int n_PRB = 0;
  int l_idx = 0;
  int DMRS_Num = CHE[ue_cell_idx].DMRS_Num_Port5;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  float SNR_Recip = 0.0f;
  ivec RS_Pos;
  cpxf temp = 0.0f;
  cfmat TAC, TAC_INV, TCC, TCoef;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      RS_Pos.resize(4);
      RS_Pos << 3, 6, 9, 12;
    } else {
      RS_Pos.resize(3);
      RS_Pos << 4, 7, 10;
    }
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      SNR_Recip = MEA[ue_cell_idx].CRS_N_2nd(r) / (MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5(r) * MEA[ue_cell_idx].DMRS_S_Port5(r));
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      //TAC
      TAC.resize(DMRS_Num, DMRS_Num);
      for (i = 0; i < DMRS_Num; i++) {
        TAC(i, i) = 1.0f + SNR_Recip;
        for (j = i + 1; j < DMRS_Num; j++) {
          TAC(j, i) = MEA[ue_cell_idx].R_T(r, RS_Pos[j] - RS_Pos[i]);
          TAC(i, j) = conj(TAC(j, i));
        }
      }
      //INV TAC
      Hermitian_Matrix_INV(TAC, TAC_INV);
      //TCC
      TCC.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
      for (i = 0; i < Cell_Specific[ue_cell_idx].DL_Symb_Num; i++) {
        for (j = 0; j < DMRS_Num; j++) {
          k = i - RS_Pos(j);
          TCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_T(r, k) : conj(MEA[ue_cell_idx].R_T(r, -k));
        }
      }
      //FCoef
      TCoef.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
      TCoef = TCC * TAC_INV;
      //Interp
      for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
        for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
          n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx];
          for (i = 0; i < Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
            temp = 0;
            for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
              temp += CHE[ue_cell_idx].DMRS_H_F_Port5[r](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx) * TCoef(l, l_idx);
            }
            CHE[ue_cell_idx].DMRS_H_T_Port5[r](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l) = temp;
          }
        }
      }
    }
  }
}

//! Port7~14 DMRS时域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::DMRS_Port7to14_FilterT(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int n_PRB = 0;
  int l_idx = 0;
  int DMRS_Num = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  float SNR_Recip = 0.0f;
  ivec RS_Pos;
  cpxf temp = 0.0f;
  cfmat TAC, TAC_INV, TCC, TCoef;
  int NZP_idx = -1;
  if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
    }
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      RS_Pos.resize(4);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos << 2, 3, 5, 6;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos << 2, 3, 9, 10;
      } else {
        RS_Pos << 5, 6, 12, 13;
      }
    } else {
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos.resize(2);
        RS_Pos << 4, 5;
      } else {
        RS_Pos.resize(4);
        RS_Pos << 4, 5, 10, 11;
      }
    }
    DMRS_Num = (int)RS_Pos.size();
    for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
      //SNR RECIP
      SNR_Recip = ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)) / (MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14(r) * MEA[ue_cell_idx].DMRS_S_Port7to14(r));
      SNR_Recip = MAX(SNR_Recip, 0.001f);
      //TAC
      TAC.resize(DMRS_Num, DMRS_Num);
      for (i = 0; i < DMRS_Num; i++) {
        TAC(i, i) = 1.0f + SNR_Recip;
        for (j = i + 1; j < DMRS_Num; j++) {
          TAC(j, i) = MEA[ue_cell_idx].R_T(r, RS_Pos[j] - RS_Pos[i]);
          TAC(i, j) = conj(TAC(j, i));
        }
      }
      //INV TAC
      Hermitian_Matrix_INV(TAC, TAC_INV);
      //TCC
      TCC.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
      for (i = 0; i < Cell_Specific[ue_cell_idx].DL_Symb_Num; i++) {
        for (j = 0; j < DMRS_Num; j++) {
          k = i - RS_Pos(j);
          TCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_T(r, k) : conj(MEA[ue_cell_idx].R_T(r, -k));
        }
      }
      //FCoef
      TCoef.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
      TCoef = TCC * TAC_INV;
      //Interp
      for (p = 0; p < MAX(2, UE_Specific[ue_cell_idx].N_L[DCI_idx]); p++) {
        for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
          for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
            n_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][rb_idx];
            for (i = 0; i < Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
              temp = 0;
              for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
                temp += CHE[ue_cell_idx].DMRS_H_F_Port7to14[r * MAXDMRSPORTNUM + p](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx / 2) * TCoef(l, l_idx);
              }
              CHE[ue_cell_idx].DMRS_H_T_Port7to14[r * MAXDMRSPORTNUM + p](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l) = temp;
            }
          }
        }
      }
    }
  }
}

//! EPDCCH DMRS时域滤波信道估计
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DMRS_FilterT(int ue_cell_idx) {
  int r = 0;
  int p = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int n_PRB = 0;
  int l_idx = 0;
  int DMRS_Num = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  float SNR_Recip = 0.0f;
  ivec RS_Pos;
  cpxf temp = 0.0f;
  cfmat TAC, TAC_INV, TCC, TCoef;
  int NZP_idx = -1;
  int set_idx = 0;
  if (UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
    if (TM10 == UE_Specific[ue_cell_idx].TM && QCL_TypeB == UE_Specific[ue_cell_idx].QCL_Type) {
      NZP_idx = UE_Specific[ue_cell_idx].QCL_NZP_CSI_idx[UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]];
    }
    if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
      RS_Pos.resize(4);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos << 2, 3, 5, 6;
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos << 2, 3, 9, 10;
      } else {
        RS_Pos << 5, 6, 12, 13;
      }
    } else {
      if (S == Cell_Specific[ue_cell_idx].SubframeType && (SS_CFG1 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration)) {
        RS_Pos.resize(2);
        RS_Pos << 4, 5;
      } else {
        RS_Pos.resize(4);
        RS_Pos << 4, 5, 10, 11;
      }
    }
    DMRS_Num = (int)RS_Pos.size();
    for (set_idx = 0; set_idx < UE_Specific[ue_cell_idx].EPDCCH_Set_Num; set_idx++) {
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        //SNR RECIP
        SNR_Recip = ((NZP_idx >= 0) ? MEA[ue_cell_idx].CSI_RS_N_2nd(r, NZP_idx) : MEA[ue_cell_idx].CRS_N_2nd(r)) / (MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale(r, set_idx) * MEA[ue_cell_idx].EPDCCH_DMRS_S(r, set_idx));
        SNR_Recip = MAX(SNR_Recip, 0.001f);
        //TAC
        TAC.resize(DMRS_Num, DMRS_Num);
        for (i = 0; i < DMRS_Num; i++) {
          TAC(i, i) = 1.0f + SNR_Recip;
          for (j = i + 1; j < DMRS_Num; j++) {
            TAC(j, i) = MEA[ue_cell_idx].R_T(r, RS_Pos[j] - RS_Pos[i]);
            TAC(i, j) = conj(TAC(j, i));
          }
        }
        //INV TAC
        Hermitian_Matrix_INV(TAC, TAC_INV);
        //TCC
        TCC.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
        for (i = 0; i < Cell_Specific[ue_cell_idx].DL_Symb_Num; i++) {
          for (j = 0; j < DMRS_Num; j++) {
            k = i - RS_Pos(j);
            TCC(i, j) = (k >= 0) ? MEA[ue_cell_idx].R_T(r, k) : conj(MEA[ue_cell_idx].R_T(r, -k));
          }
        }
        //FCoef
        TCoef.resize(Cell_Specific[ue_cell_idx].DL_Symb_Num, DMRS_Num);
        TCoef = TCC * TAC_INV;
        //Interp
        for (p = 0; p < UE_Specific[ue_cell_idx].N_L[DCI_idx]; p++) {
          for (l = 0; l < Cell_Specific[ue_cell_idx].DL_Symb_Num; l++) {
            for (rb_idx = 0; rb_idx < (int)UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx].size(); rb_idx++) {
              n_PRB = UE_Specific[ue_cell_idx].RB_Assignment_EPDCCH[set_idx][set_idx];
              for (i = 0; i < Cell_Specific[ue_cell_idx].N_RB_sc; i++) {
                temp = 0;
                for (l_idx = 0; l_idx < DMRS_Num; l_idx++) {
                  temp += CHE[ue_cell_idx].EPDCCH_DMRS_H_F[r * MAXEPDCCHRSPORTNUM + p](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l_idx / 2) * TCoef(l, l_idx);
                }
                CHE[ue_cell_idx].EPDCCH_DMRS_H_T[r * MAXEPDCCHRSPORTNUM + p](n_PRB * Cell_Specific[ue_cell_idx].N_RB_sc + i, l) = temp;
              }
            }
          }
        }
      }
    }
  }
}
