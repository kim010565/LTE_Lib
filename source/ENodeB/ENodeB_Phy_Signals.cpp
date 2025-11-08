/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file ENodeB_Phy_Signals.cpp
    \brief ENodeB_Phy_Signals实现文件
    内容 ：Cell_C类物理信号相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/ENodeB/ENodeB.h"

//! PSS产生。出自 ： 3GPP 36.211 6.11.1节
/*!
  \return 无返回值
*/
void Cell_C::PSS_Gen() {
  int k = -31 + Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2;
  int l = Cell_Specific.N_DL_symb - 1;
  if (FDD == Cell_Specific.FrameStructureType && (0 == Cell_Specific.n_subframe || 5 == Cell_Specific.n_subframe)) {
    l = Cell_Specific.N_DL_symb - 1;
    RE[PortToPhyAntenna[(int)Port0]].block(k, l, 62, 1) += PSS_d[Cell_Specific.N_2_ID];
    RE_Type.block(k - 5, l, 72, 1).setConstant(PSS);
  } else if (TDD == Cell_Specific.FrameStructureType && (1 == Cell_Specific.n_subframe || 6 == Cell_Specific.n_subframe)) {
    l = 2;
    RE[PortToPhyAntenna[(int)Port0]].block(k, l, 62, 1) += PSS_d[Cell_Specific.N_2_ID];
    RE_Type.block(k - 5, l, 72, 1).setConstant(PSS);
  }
}

//! SSS产生。出自 ： 3GPP 36.211 6.11.2节
/*!
  \return 无返回值
*/
void Cell_C::SSS_Gen() {
  int k = -31 + Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2;
  int l = Cell_Specific.N_DL_symb - 2;
  int m0 = SSS_m0_m1[Cell_Specific.N_1_ID][0];
  int m1 = SSS_m0_m1[Cell_Specific.N_1_ID][1];
  cfvec d_n(62);
  int n = 0;
  if ((FDD == Cell_Specific.FrameStructureType || TDD == Cell_Specific.FrameStructureType) && (0 == Cell_Specific.n_subframe || 5 == Cell_Specific.n_subframe)) {
    if (FDD == Cell_Specific.FrameStructureType) {
      l = Cell_Specific.N_DL_symb - 2;
    } else {
      l = Cell_Specific.N_DL_symb + Cell_Specific.N_DL_symb - 1;
    }
    d_n.setZero();
    if (0 == Cell_Specific.n_subframe) {
      for (n = 0; n <= 30; n++) {
        d_n(2 * n) = SSS_s((n + m0) % 31) * SSS_c((n + Cell_Specific.N_2_ID) % 31);
        d_n(2 * n + 1) = SSS_s((n + m1) % 31) * SSS_c((n + Cell_Specific.N_2_ID + 3) % 31) * SSS_z((n + (m0 & 7)) % 31);
      }
    } else {
      for (n = 0; n <= 30; n++) {
        d_n(2 * n) = SSS_s((n + m1) % 31) * SSS_c((n + Cell_Specific.N_2_ID) % 31);
        d_n(2 * n + 1) = SSS_s((n + m0) % 31) * SSS_c((n + Cell_Specific.N_2_ID + 3) % 31) * SSS_z((n + (m1 & 7)) % 31);
      }
    }
    RE[PortToPhyAntenna[(int)Port0]].block(k, l, 62, 1) += d_n;
    RE_Type.block(k - 5, l, 72, 1).setConstant(SSS);
  }
}

//! CRS产生。出自 ： 3GPP 36.211 6.10.1节
/*!
  \return 无返回值
*/
void Cell_C::CRS_Gen() {
  const Port_Type_E Port[4] = {Port0, Port1, Port2, Port3};
  const int RS_Num[4] = {4, 4, 2, 2};
  int RS_Pos[4][4];
  int n_s = 0;
  int c_init = 0;
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
  if (15000 == Cell_Specific.delta_f && U != Cell_Specific.SubframeType) {
    RS_Pos[0][0] = RS_Pos[1][0] = 0;
    RS_Pos[0][1] = RS_Pos[1][1] = Cell_Specific.N_DL_symb - 3;
    RS_Pos[0][2] = RS_Pos[1][2] = Cell_Specific.N_DL_symb;
    RS_Pos[0][3] = RS_Pos[1][3] = Cell_Specific.N_DL_symb + Cell_Specific.N_DL_symb - 3;
    RS_Pos[2][0] = RS_Pos[3][0] = 1;
    RS_Pos[2][1] = RS_Pos[3][1] = Cell_Specific.N_DL_symb + 1;
    v_shift = Cell_Specific.N_cell_ID % 6;
    for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
      for (l_idx = 0; l_idx < RS_Num[p]; l_idx++) {
        if (RS_Pos[p][l_idx] < Cell_Specific.DL_Symb_Num) {
          l = RS_Pos[p][l_idx];
          n_s = 2 * Cell_Specific.n_subframe + RS_Pos[p][l_idx] / Cell_Specific.N_DL_symb;
          c_init = 1024 * (7 * (n_s + 1) + (l % Cell_Specific.N_DL_symb) + 1) * (2 * Cell_Specific.N_cell_ID + 1) + 2 * Cell_Specific.N_cell_ID + Cell_Specific.N_CP;
          PNSequence_Gen(c_init, c, 4 * N_MAX_DL_RB);
          v_mod_6 = (v[p][l_idx] + v_shift) % 6;
          offset = N_MAX_DL_RB - Cell_Specific.N_DL_RB;
          for (m = 0; m < 2 * Cell_Specific.N_DL_RB; m++) {
            k = 6 * m + v_mod_6;
            m_prime = 2 * (m + offset);
            RE[PortToPhyAntenna[(int)Port[p]]](k, l) += cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
            RE_Type(k, l) = CRS;
          }
        }
      }
    }
  }
}

//! Port5的DMRS产生（TM7）。出自 ： 3GPP 36.211 6.10.3节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::DMRS_Port5_Gen(int cell_ue_idx) {
  int l_prime = 0;
  ivec l;
  int k_prime = 0;
  int k = 0;
  int m_prime = 0;
  int m = 0;
  uint32 c_init = 0;
  int v_shift = 0;
  int n_PRB = 0;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  bvec c;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  int subband_idx = 0;
  int Codebook_index = 0;
  cpxf temp = 0.0f;
  if (UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific.SubframeType) {
    row_a_v = powf(10.0f, Cell_Specific.PDSCH_RA / 20.0f);
    row_b_v = powf(10.0f, Cell_Specific.PDSCH_RB / 20.0f);
    v_shift = Cell_Specific.N_cell_ID % 3;
    c_init = 65536 * (Cell_Specific.n_subframe + 1) * (2 * Cell_Specific.N_cell_ID + 1) + UE_Specific[cell_ue_idx].RNTI;
    PNSequence_Gen(c_init, c, 24 * UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size());
    if (NormalCP == Cell_Specific.CP_Type) {
      l.resize(4);
      l << 3, 6, 9, 12;
      for (l_prime = 0; l_prime < 4; l_prime++) {
        power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l(l_prime)]) ? row_a_v : row_b_v;
        for (m_prime = 0; m_prime < 3 * (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
          n_PRB = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][m_prime / 3];
          k_prime = (0 == (l_prime & 1)) ? (4 * m_prime + v_shift) : (4 * m_prime + ((2 + v_shift) & 3));
          k = k_prime % Cell_Specific.N_RB_sc + Cell_Specific.N_RB_sc * n_PRB;
          m = 2 * (3 * l_prime * UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() + m_prime);
          subband_idx = n_PRB / Feedback[cell_ue_idx].k_Subband;
          Codebook_index = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
          if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port5 == RE_Type(k, l(l_prime))) {
            temp = power_scale * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
            RE[PortToPhyAntenna[(int)Port5]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](0) * temp;
            RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](1) * temp;
            RE_Type(k, l(l_prime)) = DMRS_Port5;
          }
        }
      }
    } else {
      l.resize(3);
      l << 4, 7, 10;
      for (l_prime = 0; l_prime < 3; l_prime++) {
        power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l(l_prime)]) ? row_a_v : row_b_v;
        for (m_prime = 0; m_prime < 4 * (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); m_prime++) {
          n_PRB = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][m_prime / 4];
          k_prime = (1 != l_prime) ? (3 * m_prime + v_shift) : (3 * m_prime + ((2 + v_shift) % 3));
          k = k_prime % Cell_Specific.N_RB_sc + Cell_Specific.N_RB_sc * n_PRB;
          m = 2 * (4 * l_prime * UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() + m_prime);
          subband_idx = n_PRB / Feedback[cell_ue_idx].k_Subband;
          Codebook_index = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
          if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port5 == RE_Type(k, l(l_prime))) {
            temp = power_scale * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
            RE[PortToPhyAntenna[(int)Port5]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](0) * temp;
            RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](1) * temp;
            RE_Type(k, l(l_prime)) = DMRS_Port5;
          }
        }
      }
    }
  }
}

//! Port7to14的DMRS产生（TM8/9/10）。出自 ： 3GPP 36.211 6.10.3节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::DMRS_Port7to14_Gen(int cell_ue_idx) {
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
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  bvec c;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  int subband_idx = 0;
  int Codebook_index = 0;
  cpxf temp = 0.0f;
  if (UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific.SubframeType) {
    row_a_v = powf(10.0f, Cell_Specific.PDSCH_RA / 20.0f);
    row_b_v = powf(10.0f, Cell_Specific.PDSCH_RB / 20.0f);
    if (-1 == UE_Specific[cell_ue_idx].n_DMRS_ID[UE_Specific[cell_ue_idx].n_SCID[DCI_idx]] || DCI_Format_1A == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
      c_init = 65536 * (Cell_Specific.n_subframe + 1) * (2 * Cell_Specific.N_cell_ID + 1) + UE_Specific[cell_ue_idx].n_SCID[DCI_idx];
    } else {
      c_init = 65536 * (Cell_Specific.n_subframe + 1) * (2 * UE_Specific[cell_ue_idx].n_DMRS_ID[UE_Specific[cell_ue_idx].n_SCID[DCI_idx]] + 1) + UE_Specific[cell_ue_idx].n_SCID[DCI_idx];
    }
    if (NormalCP == Cell_Specific.CP_Type) {
      PNSequence_Gen(c_init, c, 24 * N_MAX_DL_RB);
      l.resize(4);
      if (S == Cell_Specific.SubframeType && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific.SpecialSubframeConfiguration)) {
        l << 2, 3, 5, 6;
      } else if (S == Cell_Specific.SubframeType && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific.SpecialSubframeConfiguration)) {
        l << 2, 3, 9, 10;
      } else {
        l << 5, 6, 12, 13;
      }
      for (p = (int)UE_Specific[cell_ue_idx].Start_Port; p < ((int)UE_Specific[cell_ue_idx].Start_Port + (int)UE_Specific[cell_ue_idx].N_L[DCI_idx]); p++) {
        for (l_prime = 0; l_prime < 4; l_prime++) {
          power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l(l_prime)]) ? row_a_v : row_b_v;
          for (i = 0; i < (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); i++) {
            n_PRB = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][i];
            subband_idx = n_PRB / Feedback[cell_ue_idx].k_Subband;
            Codebook_index = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
            for (m_prime = 0; m_prime < 3; m_prime++) {
              k_prime = ((int)Port7 == p || (int)Port8 == p || (int)Port11 == p || (int)Port13 == p) ? 1 : 0;
              k = 5 * m_prime + Cell_Specific.N_RB_sc * n_PRB + k_prime;
              w = (0 == ((m_prime + n_PRB) & 1)) ? Seq_w_NormalCP[p - (int)Port7][l_prime] : Seq_w_NormalCP[p - (int)Port7][3 - l_prime];
              m = 2 * (3 * l_prime * N_MAX_DL_RB + 3 * n_PRB + m_prime);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port7_14 == RE_Type(k, l(l_prime))) {
                temp = power_scale * w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                if (1 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
                  if (Simuctrl.With_Simultaneous_Transmission) {
                    RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](0) * temp;
                    RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](1) * temp;
                  } else {
                    RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](0) * temp;
                    RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](1) * temp;
                  }
                } else if (2 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
                  RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += Codebook_2P2v[Codebook_index](0, p - (int)Port7) * temp;
                  RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P2v[Codebook_index](1, p - (int)Port7) * temp;
                }
                RE_Type(k, l(l_prime)) = DMRS_Port7_14;
              }
            }
          }
        }
      }
    } else {
      PNSequence_Gen(c_init, c, 32 * N_MAX_DL_RB);
      if (S == Cell_Specific.SubframeType && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration)) {
        l.resize(2);
        l << 4, 5;
      } else {
        l.resize(4);
        l << 4, 5, 10, 11;
      }
      for (p = (int)UE_Specific[cell_ue_idx].Start_Port; p < ((int)UE_Specific[cell_ue_idx].Start_Port + (int)UE_Specific[cell_ue_idx].N_L[DCI_idx]); p++) {
        for (l_prime = 0; l_prime < l.size(); l_prime++) {
          power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l(l_prime)]) ? row_a_v : row_b_v;
          for (i = 0; i < (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); i++) {
            n_PRB = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][i];
            subband_idx = n_PRB / Feedback[cell_ue_idx].k_Subband;
            Codebook_index = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
            for (m_prime = 0; m_prime < 4; m_prime++) {
              k_prime = (l(l_prime) <= 5) ? 1 : 2;
              k = 3 * m_prime + Cell_Specific.N_RB_sc * n_PRB + k_prime;
              w = (0 == (m_prime & 1)) ? Seq_w_NormalCP[p - (int)Port7][l_prime] : Seq_w_NormalCP[p - (int)Port7][1 - l_prime];
              m = 2 * (4 * l_prime * N_MAX_DL_RB + 4 * n_PRB + m_prime);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port7_14 == RE_Type(k, l(l_prime))) {
                temp = power_scale * w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                if (1 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
                  if (Simuctrl.With_Simultaneous_Transmission) {
                    RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](0) * temp;
                    RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](1) * temp;
                  } else {
                    RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](0) * temp;
                    RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P1v[Codebook_index](1) * temp;
                  }
                } else if (2 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
                  RE[PortToPhyAntenna[(int)Port7]](k, l(l_prime)) += Codebook_2P2v[Codebook_index](0, p - (int)Port7) * temp;
                  RE[PortToPhyAntenna[(int)Port8]](k, l(l_prime)) += Codebook_2P2v[Codebook_index](1, p - (int)Port7) * temp;
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

//! EPDCCH DMRS产生。出自 ： 3GPP 36.211 6.10.3A节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::EPDCCH_DMRS_Gen(int cell_ue_idx) {
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
  if (UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific.SubframeType) {
    if (NormalCP == Cell_Specific.CP_Type) {
      l.resize(4);
      if (S == Cell_Specific.SubframeType && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific.SpecialSubframeConfiguration)) {
        l << 2, 3, 5, 6;
      } else if (S == Cell_Specific.SubframeType && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific.SpecialSubframeConfiguration)) {
        l << 2, 3, 9, 10;
      } else {
        l << 5, 6, 12, 13;
      }
      for (set_idx = 0; set_idx < UE_Specific[cell_ue_idx].EPDCCH_Set_Num; set_idx++) {
        c_init = 65536 * (Cell_Specific.n_subframe + 1) * (2 * UE_Specific[cell_ue_idx].n_EPDCCH_ID[set_idx] + 1) + 2;
        PNSequence_Gen(c_init, c, 24 * N_MAX_DL_RB);
        for (p = (int)Port107; p < ((int)Port107 + 4); p++) {
          for (l_prime = 0; l_prime < 4; l_prime++) {
            for (i = 0; i < (int)UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
              n_PRB = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][i];
              for (m_prime = 0; m_prime < 3; m_prime++) {
                k_prime = ((int)Port107 == p || (int)Port108 == p) ? 1 : 0;
                k = 5 * m_prime + Cell_Specific.N_RB_sc * n_PRB + k_prime;
                w = (0 == ((m_prime + n_PRB) & 1)) ? Seq_w_NormalCP[p - (int)Port107][l_prime] : Seq_w_NormalCP[p - (int)Port107][3 - l_prime];
                m = 2 * (3 * l_prime * N_MAX_DL_RB + 3 * n_PRB + m_prime);
                if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port107_110 == RE_Type(k, l(l_prime))) {
                  RE[PortToPhyAntenna[p]](k, l(l_prime)) += w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
                  RE_Type(k, l(l_prime)) = DMRS_Port107_110;
                }
              }
            }
          }
        }
      }
    } else {
      if (S == Cell_Specific.SubframeType && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration)) {
        l.resize(2);
        l << 4, 5;
      } else {
        l.resize(4);
        l << 4, 5, 10, 11;
      }
      for (set_idx = 0; set_idx < UE_Specific[cell_ue_idx].EPDCCH_Set_Num; set_idx++) {
        c_init = 65536 * (Cell_Specific.n_subframe + 1) * (2 * UE_Specific[cell_ue_idx].n_EPDCCH_ID[set_idx] + 1) + 2;
        PNSequence_Gen(c_init, c, 32 * N_MAX_DL_RB);
        for (p = (int)Port107; p < ((int)Port107 + 2); p++) {
          for (l_prime = 0; l_prime < l.size(); l_prime++) {
            for (i = 0; i < (int)UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
              n_PRB = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][i];
              for (m_prime = 0; m_prime < 4; m_prime++) {
                k_prime = (l(l_prime) <= 5) ? 1 : 2;
                k = 3 * m_prime + Cell_Specific.N_RB_sc * n_PRB + k_prime;
                w = (0 == (m_prime & 1)) ? Seq_w_NormalCP[p - (int)Port107][l_prime] : Seq_w_NormalCP[p - (int)Port107][1 - l_prime];
                m = 2 * (4 * l_prime * N_MAX_DL_RB + 4 * n_PRB + m_prime);
                if (EMPTY_RE == RE_Type(k, l(l_prime)) || DMRS_Port107_110 == RE_Type(k, l(l_prime))) {
                  RE[PortToPhyAntenna[p]](k, l(l_prime)) += w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m + 1)));
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

//! CSI RS产生。出自 ： 3GPP 36.211 6.10.5节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::CSI_RS_Gen(int cell_ue_idx) {
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
  int ZP_idx = 0;
  int T_CSI_RS = 0;
  int Delta_CSI_RS = 0;
  int n_s = 0;
  float power_scale = 0.0f;
  if ((UE_Specific[cell_ue_idx].NZP_CSI_Set_Num > 0 || UE_Specific[cell_ue_idx].ZP_CSI_Set_Num > 0) && U != Cell_Specific.SubframeType && S != Cell_Specific.SubframeType) {
    power_scale = powf(10.0f, UE_Specific[cell_ue_idx].p_C / 20.0f);
    for (NZP_idx = 0; NZP_idx < UE_Specific[cell_ue_idx].NZP_CSI_Set_Num; NZP_idx++) {
      UE_Specific[cell_ue_idx].CSI_Flag_NZP[NZP_idx] = false;
      Get_T_CSI_RS_Delta_CSI_RS(UE_Specific[cell_ue_idx].I_CSI_RS_NZP[NZP_idx], &T_CSI_RS, &Delta_CSI_RS);
      if (0 == ((10 * Cell_Specific.n_f + Cell_Specific.n_subframe - Delta_CSI_RS + T_CSI_RS) % T_CSI_RS)) {
        UE_Specific[cell_ue_idx].CSI_Flag_NZP[NZP_idx] = true;
        l.resize(2);
        if (NormalCP == Cell_Specific.CP_Type) {
          k_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][1];
          n_s = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][2];
          if (UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx] <= 19) {
            l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
            l(1) = l_prime + 1 + n_s * Cell_Specific.N_DL_symb;
          } else {
            l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
            l(1) = l_prime + 2 + n_s * Cell_Specific.N_DL_symb;
          }
        } else {
          k_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][1];
          n_s = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_NZP[NZP_idx]][2];
          l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
          l(1) = l_prime + 1 + n_s * Cell_Specific.N_DL_symb;
        }
        for (p = (int)Port15; p < ((int)Port15 + UE_Specific[cell_ue_idx].CSIPortNum_NZP[NZP_idx]); p++) {
          for (l_prime_prime = 0; l_prime_prime < 2; l_prime_prime++) {
            c_init = 1024 * (7 * (n_s + 1) + (l(l_prime_prime) % Cell_Specific.N_DL_symb) + 1) * (2 * UE_Specific[cell_ue_idx].N_CSI_ID_NZP[NZP_idx] + 1) + 2 * UE_Specific[cell_ue_idx].N_CSI_ID_NZP[NZP_idx] + Cell_Specific.N_CP;
            PNSequence_Gen(c_init, c, 2 * N_MAX_DL_RB);
            if ((int)Port15 == p || (int)Port17 == p || (int)Port19 == p || (int)Port21 == p) {
              w = 1.0f;
            } else {
              w = (0 == l_prime_prime) ? 1.0f : -1.0f;
            }
            for (m = 0; m < Cell_Specific.N_DL_RB; m++) {
              m_prime = 2 * (m + (N_MAX_DL_RB - Cell_Specific.N_DL_RB) / 2);
              k = k_prime + 12 * m + ((NormalCP == Cell_Specific.CP_Type) ? CSI_k_offset_NormalCP[p - (int)Port15] : CSI_k_offset_ExtendedCP[p - (int)Port15]);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || CSI_RS == RE_Type(k, l(l_prime))) {
                RE[PortToPhyAntenna[p]](k, l(l_prime)) += power_scale * w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
                RE_Type(k, l(l_prime)) = CSI_RS;
              }
            }
          }
        }
      }
    }
    for (ZP_idx = 0; ZP_idx < UE_Specific[cell_ue_idx].ZP_CSI_Set_Num; ZP_idx++) {
      UE_Specific[cell_ue_idx].CSI_Flag_ZP[ZP_idx] = false;
      Get_T_CSI_RS_Delta_CSI_RS(UE_Specific[cell_ue_idx].I_CSI_RS_ZP[ZP_idx], &T_CSI_RS, &Delta_CSI_RS);
      if (0 == ((10 * Cell_Specific.n_f + Cell_Specific.n_subframe - Delta_CSI_RS + T_CSI_RS) % T_CSI_RS)) {
        UE_Specific[cell_ue_idx].CSI_Flag_ZP[ZP_idx] = true;
        l.resize(2);
        if (NormalCP == Cell_Specific.CP_Type) {
          k_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][1];
          n_s = CSI_RS_Signal_Config_NormalCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][2];
          if (UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx] <= 19) {
            l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
            l(1) = l_prime + 1 + n_s * Cell_Specific.N_DL_symb;
          } else {
            l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
            l(1) = l_prime + 2 + n_s * Cell_Specific.N_DL_symb;
          }
        } else {
          k_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][0];
          l_prime = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][1];
          n_s = CSI_RS_Signal_Config_ExtendedCP[UE_Specific[cell_ue_idx].CSI_RS_Signal_Config_ZP[ZP_idx]][2];
          l(0) = l_prime + n_s * Cell_Specific.N_DL_symb;
          l(1) = l_prime + 1 + n_s * Cell_Specific.N_DL_symb;
        }
        for (p = (int)Port15; p < ((int)Port15 + UE_Specific[cell_ue_idx].CSIPortNum_ZP[ZP_idx]); p++) {
          for (l_prime_prime = 0; l_prime_prime < 2; l_prime_prime++) {
            c_init = 1024 * (7 * (n_s + 1) + (l(l_prime_prime) % Cell_Specific.N_DL_symb) + 1) * (2 * UE_Specific[cell_ue_idx].N_CSI_ID_ZP[ZP_idx] + 1) + 2 * UE_Specific[cell_ue_idx].N_CSI_ID_ZP[ZP_idx] + Cell_Specific.N_CP;
            PNSequence_Gen(c_init, c, 2 * N_MAX_DL_RB);
            if ((int)Port15 == p || (int)Port17 == p || (int)Port19 == p || (int)Port21 == p) {
              w = 1.0f;
            } else {
              w = (0 == l_prime_prime) ? 1.0f : -1.0f;
            }
            for (m = 0; m < Cell_Specific.N_DL_RB; m++) {
              m_prime = 2 * (m + (N_MAX_DL_RB - Cell_Specific.N_DL_RB) / 2);
              k = k_prime + 12 * m + ((NormalCP == Cell_Specific.CP_Type) ? CSI_k_offset_NormalCP[p - (int)Port15] : CSI_k_offset_ExtendedCP[p - (int)Port15]);
              if (EMPTY_RE == RE_Type(k, l(l_prime)) || CSI_RS == RE_Type(k, l(l_prime))) {
                RE[PortToPhyAntenna[p]](k, l(l_prime)) += power_scale * w * cpxf(SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime)), SQRT_2_RECIP * (1.0f - 2.0f * (float)c(m_prime + 1)));
                RE_Type(k, l(l_prime)) = CSI_RS;
              }
            }
          }
        }
      }
    }
  }
}
