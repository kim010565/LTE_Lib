/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file ENodeB_Phy_Channels.cpp
    \brief ENodeB_Phy_Channels实现文件
    内容 ：Cell_C类物理信道相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/ENodeB/ENodeB.h"

//! PBCH产生。出自 ： 3GPP 36.331 6.2.2节（MasterInformationBlock）； 36.212 5.3.1节； 36.211 6.6节
/*!
  \return 无返回值
*/
void Cell_C::PBCH_Gen() {
  int i = 0;
  int Context = 0;
  int k = 0;
  int l = 0;
  int p = 0;
  int vshift = 0;
  int k_offset = 0;
  int E = 0;
  uint32 cinit = 0;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  bvec SourceBits;
  bvec PBCH_CRC_mask;
  bmat TBCC_bits;
  bvec RM_bits;
  cfvecs symbs;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  row_a_v = powf(10.0f, Cell_Specific.PBCH_RA / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
  row_b_v = powf(10.0f, Cell_Specific.PBCH_RB / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
  if (0 == (Cell_Specific.n_f & 3) && 0 == Cell_Specific.n_subframe) {
    SourceBits.resize(24);
    Context = ((int)Cell_Specific.dl_BandWidth << 21) + ((int)Cell_Specific.phich_Duration << 20) + ((int)Cell_Specific.phich_Resource << 18) + ((Cell_Specific.n_f >> 2) << 10);
    for (i = 0; i < 24; i++) {
      SourceBits(i) = ((Context >> (23 - i)) & 1);
    }
    AddCRC(CRC16, SourceBits);
    PBCH_CRC_mask.resize(16);
    if (1 == Cell_Specific.CRSPortNum) {
      PBCH_CRC_mask << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    } else if (2 == Cell_Specific.CRSPortNum) {
      PBCH_CRC_mask << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;
    } else {
      PBCH_CRC_mask << 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1;
    }
    for (i = 0; i < 16; i++) {
      SourceBits(24 + i) = SourceBits(24 + i) ^ PBCH_CRC_mask(i);
    }
    TBCC_Encoder(SourceBits, TBCC_bits);
    E = (NormalCP == Cell_Specific.CP_Type) ? 1920 : 1728;
    Rate_Matching_TBCC(TBCC_bits, E, RM_bits);
    cinit = Cell_Specific.N_cell_ID;
    Scrambling(cinit, RM_bits);
    symbs.resize(1);
    Modulation_QPSK(RM_bits, symbs[0]);
    Layer_Mapping(symbs,
                  Cell_Specific.CRSPortNum,
                  (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                  layer_mapping_symbs);
    Precoding(layer_mapping_symbs,
              Cell_Specific.CRSPortNum,
              (int)symbs[0].size(),
              (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
              UE_Specific[0].Codebook_index0,
              UE_Specific[0].Codebook_index1,
              false,
              precoding_symbs);
    PBCH_Symbs = precoding_symbs;
    k_offset = Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2 - 36;
    vshift = Cell_Specific.N_cell_ID % 3;
    i = 0;
    for (l = Cell_Specific.N_DL_symb; l < (Cell_Specific.N_DL_symb + 4); l++) {
      power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l]) ? row_a_v : row_b_v;
      for (k = k_offset; k < (k_offset + 72); k++) {
        if (vshift != (k % 3) || (l >= (Cell_Specific.N_DL_symb + 2) && NormalCP == Cell_Specific.CP_Type) || ((Cell_Specific.N_DL_symb + 2) == l && ExtendedCP == Cell_Specific.CP_Type)) {
          for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
            RE[PortToPhyAntenna[p]](k, l) += power_scale * PBCH_Symbs(p, i);
          }
          i++;
        }
        if (CRS != RE_Type(k, l)) {
          RE_Type(k, l) = PBCH;
        }
      }
    }
  } else if (0 == Cell_Specific.n_subframe) {
    k_offset = Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2 - 36;
    vshift = Cell_Specific.N_cell_ID % 3;
    i = (Cell_Specific.n_f & 3) * ((NormalCP == Cell_Specific.CP_Type) ? 240 : 216);
    for (l = Cell_Specific.N_DL_symb; l < (Cell_Specific.N_DL_symb + 4); l++) {
      power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l]) ? row_a_v : row_b_v;
      for (k = k_offset; k < (k_offset + 72); k++) {
        if (vshift != (k % 3) || (l >= (Cell_Specific.N_DL_symb + 2) && NormalCP == Cell_Specific.CP_Type) || ((Cell_Specific.N_DL_symb + 2) == l && ExtendedCP == Cell_Specific.CP_Type)) {
          for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
            RE[PortToPhyAntenna[p]](k, l) += power_scale * PBCH_Symbs(p, i);
          }
          i++;
        }
        if (CRS != RE_Type(k, l)) {
          RE_Type(k, l) = PBCH;
        }
      }
    }
  }
}

//! PCFICH产生。出自 ： 3GPP 36.212 5.3.4节； 36.211 6.7节
/*!
  \return 无返回值
*/
void Cell_C::PCFICH_Gen() {
  int CFI = 0;
  uint32 cinit = 0;
  int i = 0;
  int j = 0;
  int p = 0;
  int k = 0;
  int k_bar = 0;
  int k_offset = 0;
  int vshift = 0;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  int Ctrl_Symb_Num = 0;
  bvec CFI_bits;
  cfvecs symbs;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  if (U != Cell_Specific.SubframeType) {
    row_a_v = powf(10.0f, Cell_Specific.PCFICH_RA / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    row_b_v = powf(10.0f, Cell_Specific.PCFICH_RB / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    Ctrl_Symb_Num = (S == Cell_Specific.SubframeType) ? Cell_Specific.Ctrl_Symb_Num_Special : Cell_Specific.Ctrl_Symb_Num;
    if (Cell_Specific.N_DL_RB > 10) {
      CFI = Ctrl_Symb_Num;
    } else {
      CFI = Ctrl_Symb_Num - 1;
#if 1 == ERROR_REPORT
      if (Ctrl_Symb_Num < 2) {
        Error("N_DL_RB < 10, Ctrl_Symb_Num Error: shall >= 2 !");
      }
#endif
    }
    CFI_bits = CFI_code_word.row(CFI - 1);
    cinit = (((Cell_Specific.n_subframe + 1) * (2 * Cell_Specific.N_cell_ID + 1)) << 9) + Cell_Specific.N_cell_ID;
    Scrambling(cinit, CFI_bits);
    symbs.resize(1);
    Modulation_QPSK(CFI_bits, symbs[0]);
    Layer_Mapping(symbs,
                  Cell_Specific.CRSPortNum,
                  (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                  layer_mapping_symbs);
    Precoding(layer_mapping_symbs,
              Cell_Specific.CRSPortNum,
              (int)symbs[0].size(),
              (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
              UE_Specific[0].Codebook_index0,
              UE_Specific[0].Codebook_index1,
              false,
              precoding_symbs);
    k_bar = Cell_Specific.N_RB_sc / 2 * (Cell_Specific.N_cell_ID % (2 * Cell_Specific.N_DL_RB));
    vshift = Cell_Specific.N_cell_ID % 3;
    power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][0]) ? row_a_v : row_b_v;
    for (i = 0; i < 4; i++) {
      k_offset = k_bar + (i * Cell_Specific.N_DL_RB / 2) * Cell_Specific.N_RB_sc / 2;
      j = 0;
      for (k = k_offset; k < (k_offset + 6); k++) {
        if (vshift != (k % 3)) {
          for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
            RE[PortToPhyAntenna[p]](k, 0) += power_scale * precoding_symbs(p, 4 * i + j);
          }
          RE_Type(k, 0) = PCFICH;
          j++;
        }
      }
    }
  }
}

//! PHICH产生。出自 ： 3GPP 36.212 5.3.5节； 36.211 6.9节
/*!
  \return 无返回值
*/
void Cell_C::PHICH_Gen() {
  const float N_g_map[4] = {1.0f / 6.0f, 1.0f / 2.0f, 1.0f, 2.0};
  int cell_ue_idx = 0;
  float N_g = N_g_map[(int)Cell_Specific.phich_Resource];
  int N_group_PHICH = 0;
  int resource_idx = 0;
  int N_PHICH_SF = 0;
  int M_symb = 0;
  int M_symb0 = 0;
  int i = 0;
  int j = 0;
  int p = 0;
  uint32 cinit = 0;
  int m_prime = 0;
  int l_prime = 0;
  int n[3] = {0, 0, 0};
  ivec n_REG[3];
  int k_prime = 0;
  int k_prime_offset = 0;
  int n_bar_i = 0;
  int k_bar = 0;
  int vshift = 0;
  int REG_Idx0 = 0;
  int REG_Idx1 = 0;
  int m_prime_num = 0;
  int PHICH_idx;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  bvec HARQ_bits(3);
  cfvec z;
  cfvec d;
  cfvecs d0;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  bvec c;
  cfmats y_wave;
  fvec Power_Offset_V;
  float AccPower = 0.0f;
  if (U != Cell_Specific.SubframeType) {
    row_a_v = powf(10.0f, Cell_Specific.PHICH_RA / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    row_b_v = powf(10.0f, Cell_Specific.PHICH_RB / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    if (NormalCP == Cell_Specific.CP_Type) {
      N_group_PHICH = ceil(N_g * ((float)Cell_Specific.N_DL_RB / 8.0f));
      N_PHICH_SF = 4;
    } else {
      N_group_PHICH = 2 * ceil(N_g * ((float)Cell_Specific.N_DL_RB / 8.0f));
      N_PHICH_SF = 2;
    }
    M_symb = N_PHICH_SF * 3;
    if (TDD == Cell_Specific.FrameStructureType) {
      N_group_PHICH = N_group_PHICH * PHICH_mi[(int)Cell_Specific.ULDLConfiguration][Cell_Specific.n_subframe];
    }
    if (N_group_PHICH > 0) {
      n[0] = Cell_Specific.N_DL_RB * 2 - 4;
      n[1] = (4 == Cell_Specific.CRSPortNum) ? (Cell_Specific.N_DL_RB * 2) : (Cell_Specific.N_DL_RB * 3);
      n[2] = Cell_Specific.N_DL_RB * 3;
      n_REG[0].resize(n[0]);
      n_REG[1].resize(n[1]);
      n_REG[2].resize(n[2]);
      vshift = Cell_Specific.N_cell_ID % 3;
      k_bar = Cell_Specific.N_RB_sc / 2 * (Cell_Specific.N_cell_ID % (2 * Cell_Specific.N_DL_RB));
      REG_Idx0 = 0;
      REG_Idx1 = k_bar / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (Cell_Specific.N_DL_RB / 2) * Cell_Specific.N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 1] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (2 * Cell_Specific.N_DL_RB / 2) * Cell_Specific.N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 2] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = (k_bar + (3 * Cell_Specific.N_DL_RB / 2) * Cell_Specific.N_RB_sc / 2) / 6;
      for (i = REG_Idx0; i < REG_Idx1; i++) {
        n_REG[0][i - 3] = i;
      }
      REG_Idx0 = REG_Idx1 + 1;
      REG_Idx1 = Cell_Specific.N_DL_RB * 2;
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
      d0.resize(1);
      if (NormalCP == Cell_Specific.CP_Type) {
        d0[0].resize(M_symb);
        M_symb0 = M_symb;
      } else {
        d0[0].resize(2 * M_symb);
        M_symb0 = 2 * M_symb;
      }
      m_prime_num = (NormalCP == Cell_Specific.CP_Type) ? N_group_PHICH : (N_group_PHICH / 2);
      y_wave.resize(m_prime_num);
      for (i = 0; i < m_prime_num; i++) {
        y_wave[i].resize(Cell_Specific.CRSPortNum, M_symb0);
        y_wave[i].setZero();
      }
      PHICH_idx = UE_Specific[cell_ue_idx].PHICH_idx;
      for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
        Power_Offset_V.resize(UE_Specific[cell_ue_idx].PHICH_Wanted_Pattern[PHICH_idx].size());
        AccPower = 0.0f;
        for (resource_idx = 0; resource_idx < (int)UE_Specific[cell_ue_idx].PHICH_Wanted_Pattern[PHICH_idx].size(); resource_idx++) {
          AccPower += powf(10.0f, UE_Specific[cell_ue_idx].PHICH_Power_Offset[PHICH_idx][resource_idx] / 10.0f);
        }
        for (resource_idx = 0; resource_idx < (int)UE_Specific[cell_ue_idx].PHICH_Wanted_Pattern[PHICH_idx].size(); resource_idx++) {
          Power_Offset_V[resource_idx] = MySqrt(powf(10.0f, UE_Specific[cell_ue_idx].PHICH_Power_Offset[PHICH_idx][resource_idx] / 10.0f) / AccPower);
          if (PHICH_ACK == UE_Specific[cell_ue_idx].PHICH_HARQ_Pattern[PHICH_idx][resource_idx]) {
            HARQ_bits.setConstant(1);
          } else if (PHICH_NACK == UE_Specific[cell_ue_idx].PHICH_HARQ_Pattern[PHICH_idx][resource_idx]) {
            HARQ_bits.setConstant(0);
          } else {
            HARQ_bits.setConstant(rand() & 1);
          }
          Modulation_BPSK(HARQ_bits, z);
          cinit = (((Cell_Specific.n_subframe + 1) * (2 * Cell_Specific.N_cell_ID + 1)) << 9) + Cell_Specific.N_cell_ID;
          PNSequence_Gen(cinit, c, M_symb);
          if (NormalCP == Cell_Specific.CP_Type) {
            for (i = 0; i < M_symb; i++) {
              d(i) = PHICH_Orth_Seq_NormalCP(UE_Specific[cell_ue_idx].n_seq_PHICH[PHICH_idx][resource_idx], i & (N_PHICH_SF - 1)) * (1.0f - 2.0f * (float)c(i)) * z(i / N_PHICH_SF);
            }
          } else {
            for (i = 0; i < M_symb; i++) {
              d(i) = PHICH_Orth_Seq_ExtendedCP(UE_Specific[cell_ue_idx].n_seq_PHICH[PHICH_idx][resource_idx], i & (N_PHICH_SF - 1)) * (1.0f - 2.0f * (float)c(i)) * z(i / N_PHICH_SF);
            }
          }
          if (NormalCP == Cell_Specific.CP_Type) {
            d0[0] = d;
          } else if (0 == (UE_Specific[cell_ue_idx].n_group_PHICH[PHICH_idx][resource_idx] & 1)) {
            for (i = 0; i < M_symb / 2; i++) {
              d0[0](4 * i) = d(2 * i);
              d0[0](4 * i + 1) = d(2 * i + 1);
              d0[0](4 * i + 2) = 0;
              d0[0](4 * i + 3) = 0;
            }
          } else {
            for (i = 0; i < M_symb / 2; i++) {
              d0[0](4 * i) = 0;
              d0[0](4 * i + 1) = 0;
              d0[0](4 * i + 2) = d(2 * i);
              d0[0](4 * i + 3) = d(2 * i + 1);
            }
          }
          Layer_Mapping(d0,
                        Cell_Specific.CRSPortNum,
                        (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                        layer_mapping_symbs);
          if (4 == Cell_Specific.CRSPortNum) {
            precoding_symbs.resize(Cell_Specific.CRSPortNum, M_symb0);
            for (i = 0; i < 3; i++) {
              if ((0 == ((i + UE_Specific[cell_ue_idx].n_group_PHICH[PHICH_idx][resource_idx]) & 1) && NormalCP == Cell_Specific.CP_Type) || (0 == ((i + (UE_Specific[cell_ue_idx].n_group_PHICH[PHICH_idx][resource_idx] / 2)) & 1) && ExtendedCP == Cell_Specific.CP_Type)) {
                precoding_symbs(0, 4 * i) = SQRT_2_RECIP * layer_mapping_symbs(0, i);
                precoding_symbs(1, 4 * i) = 0;
                precoding_symbs(2, 4 * i) = -SQRT_2_RECIP * conj(layer_mapping_symbs(1, i));
                precoding_symbs(3, 4 * i) = 0;
                precoding_symbs(0, 4 * i + 1) = SQRT_2_RECIP * layer_mapping_symbs(1, i);
                precoding_symbs(1, 4 * i + 1) = 0;
                precoding_symbs(2, 4 * i + 1) = SQRT_2_RECIP * conj(layer_mapping_symbs(0, i));
                precoding_symbs(3, 4 * i + 1) = 0;
                precoding_symbs(0, 4 * i + 2) = SQRT_2_RECIP * layer_mapping_symbs(2, i);
                precoding_symbs(1, 4 * i + 2) = 0;
                precoding_symbs(2, 4 * i + 2) = -SQRT_2_RECIP * conj(layer_mapping_symbs(3, i));
                precoding_symbs(3, 4 * i + 2) = 0;
                precoding_symbs(0, 4 * i + 3) = SQRT_2_RECIP * layer_mapping_symbs(3, i);
                precoding_symbs(1, 4 * i + 3) = 0;
                precoding_symbs(2, 4 * i + 3) = SQRT_2_RECIP * conj(layer_mapping_symbs(2, i));
                precoding_symbs(3, 4 * i + 3) = 0;
              } else {
                precoding_symbs(0, 4 * i) = 0;
                precoding_symbs(1, 4 * i) = SQRT_2_RECIP * layer_mapping_symbs(0, i);
                precoding_symbs(2, 4 * i) = 0;
                precoding_symbs(3, 4 * i) = -SQRT_2_RECIP * conj(layer_mapping_symbs(1, i));
                precoding_symbs(0, 4 * i + 1) = 0;
                precoding_symbs(1, 4 * i + 1) = SQRT_2_RECIP * layer_mapping_symbs(1, i);
                precoding_symbs(2, 4 * i + 1) = 0;
                precoding_symbs(3, 4 * i + 1) = SQRT_2_RECIP * conj(layer_mapping_symbs(0, i));
                precoding_symbs(0, 4 * i + 2) = 0;
                precoding_symbs(1, 4 * i + 2) = SQRT_2_RECIP * layer_mapping_symbs(2, i);
                precoding_symbs(2, 4 * i + 2) = 0;
                precoding_symbs(3, 4 * i + 2) = -SQRT_2_RECIP * conj(layer_mapping_symbs(3, i));
                precoding_symbs(0, 4 * i + 3) = 0;
                precoding_symbs(1, 4 * i + 3) = SQRT_2_RECIP * layer_mapping_symbs(3, i);
                precoding_symbs(2, 4 * i + 3) = 0;
                precoding_symbs(3, 4 * i + 3) = SQRT_2_RECIP * conj(layer_mapping_symbs(2, i));
              }
            }
          } else {
            Precoding(layer_mapping_symbs,
                      Cell_Specific.CRSPortNum,
                      (int)d0[0].size(),
                      (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                      UE_Specific[0].Codebook_index0,
                      UE_Specific[0].Codebook_index1,
                      false,
                      precoding_symbs);
          }
          if (NormalCP == Cell_Specific.CP_Type) {
            for (i = 0; i < M_symb0; i++) {
              for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                y_wave[UE_Specific[cell_ue_idx].n_group_PHICH[PHICH_idx][resource_idx]](p, i) += (Power_Offset_V[resource_idx] * precoding_symbs(p, i));
              }
            }
          } else {
            for (i = 0; i < M_symb0; i++) {
              for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                y_wave[UE_Specific[cell_ue_idx].n_group_PHICH[PHICH_idx][resource_idx]](p, i >> 1) += (Power_Offset_V[resource_idx] * precoding_symbs(p, i));
              }
            }
          }
        }
      }
      for (m_prime = 0; m_prime < m_prime_num; m_prime++) {
        for (i = 0; i < 3; i++) {
          if (Normal_Duration == Cell_Specific.phich_Duration) {
            l_prime = 0;
            n_bar_i = ((Cell_Specific.N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else if (Cell_Specific.MBSFN_Subframe_Flag) {
            l_prime = (m_prime / 2 + i + 1) & 1;
            n_bar_i = ((Cell_Specific.N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else if (TDD == Cell_Specific.FrameStructureType && (1 == Cell_Specific.n_subframe || 6 == Cell_Specific.n_subframe)) {
            l_prime = (m_prime / 2 + i + 1) & 1;
            n_bar_i = ((Cell_Specific.N_cell_ID * n[l_prime] / n[1]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          } else {
            l_prime = i;
            n_bar_i = ((Cell_Specific.N_cell_ID * n[l_prime] / n[0]) + m_prime + (i * n[l_prime] / 3)) % n[l_prime];
          }
          power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l_prime]) ? row_a_v : row_b_v;
          if (0 == l_prime || (1 == l_prime && 4 == Cell_Specific.CRSPortNum)) {
            k_prime_offset = n_REG[l_prime][n_bar_i] * 6;
            j = 0;
            for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 6); k_prime++) {
              if (vshift != (k_prime % 3)) {
                for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                  RE[PortToPhyAntenna[p]](k_prime, l_prime) += power_scale * y_wave[m_prime](p, 4 * i + j);
                }
                RE_Type(k_prime, l_prime) = PHICH;
                j++;
              }
            }
          } else {
            k_prime_offset = n_REG[l_prime][n_bar_i] * 4;
            for (k_prime = k_prime_offset; k_prime < (k_prime_offset + 4); k_prime++) {
              j = k_prime - k_prime_offset;
              for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                RE[PortToPhyAntenna[p]](k_prime, l_prime) += power_scale * y_wave[m_prime](p, 4 * i + j);
              }
              RE_Type(k_prime, l_prime) = PHICH;
            }
          }
        }
      }
    }
  }
}

//! DCI产生。出自 ： 3GPP 36.212 5.3.3.1节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \param DCI_bits ： 生成的DCI序列，输出
  \return 无返回值
*/
void Cell_C::DCI_Gen(int cell_ue_idx, bvec &DCI_bits) {
  int bit_counter = 0;
  int P = 0;
  int n_RBG_subset_PRB = 0;
  int delta_shift = 0;
  int N_RBG_subset_RB = 0;
  int N_TYPE1_RB = 0;
  int i = 0;
  int RB_start = 0;
  int L_CRBs = 0;
  int RB_start_prime = 0;
  int L_CRBs_prime = 0;
  int N_DL_VRB_prime = 0;
  int N_step_RB = 0;
  int N_DL_VRB = 0;
  int RBG_Mapping = 0;
  int Payload_size = 0;
  int DCI_idx = 0;
  vector<int> s;
  int N = 0;
  //36.213 table 7.1.6.1-1, RBG size
  if (Cell_Specific.N_DL_RB <= 10) {
    P = 1;
  } else if (Cell_Specific.N_DL_RB <= 26) {
    P = 2;
  } else if (Cell_Specific.N_DL_RB <= 63) {
    P = 3;
  } else {
    P = 4;
  }
  DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  Payload_size = Get_PDCCH_Payload_size(UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx],
                                        Cell_Specific.FrameStructureType,
                                        Cell_Specific.dl_BandWidth,
                                        UE_Specific[cell_ue_idx].CA_Flag,
                                        Cell_Specific.CRSPortNum,
                                        UE_Specific[cell_ue_idx].CSI_request_num[DCI_idx],
                                        UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx],
                                        UE_Specific[cell_ue_idx].Resource_allocation_type_num[DCI_idx],
                                        UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]);
  DCI_bits.resize(Payload_size);
  switch (UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
    case NULL_DCI:
      //Empty
      break;
    case DCI_Format_0:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, 0);
      if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
        //Only non-hopping PUSCH with resource_allocation type0 36.213 8.1.1, others TBD
        RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
        L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
        UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start, L_CRBs, Cell_Specific.N_UL_RB);
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_UL_RB * (Cell_Specific.N_UL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUSCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].Cyclic_shift_OCC[DCI_idx]);
      if (TDD == Cell_Specific.FrameStructureType) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, (0 == Cell_Specific.ULDLConfiguration) ? UE_Specific[cell_ue_idx].UL_index[DCI_idx] : UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CSI_request_num[DCI_idx], UE_Specific[cell_ue_idx].CSI_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].Resource_allocation_type_num[DCI_idx], UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_1:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_1A:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, 1);
      if (UE_Specific[cell_ue_idx].Random_Access_Flag[DCI_idx]) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), 0x7FFFFFFF);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 6, UE_Specific[cell_ue_idx].Preamble_Index[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 4, UE_Specific[cell_ue_idx].PRACH_Mask_Index[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      } else {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
        if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
          //Only Locailized; Distributed TBD
          RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
          L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
          UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start, L_CRBs, Cell_Specific.N_DL_RB);
        }
        if (0 == UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        } else {
          if (Cell_Specific.N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || P_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type) {
            bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
          } else {
            bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].gap_index[DCI_idx]);
            bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2) - 1, UE_Specific[cell_ue_idx].RIV[DCI_idx]);
          }
        }
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
        if (Cell_Specific.N_DL_RB >= 50 && 1 == UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx] && (RA_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || P_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type)) {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].gap_index[DCI_idx]);
        } else {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
        }
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      }
      break;
    case DCI_Format_1B:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
      if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
        //Only Locailized; Distributed TBD
        RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
        L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
        UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start, L_CRBs, Cell_Specific.N_DL_RB);
      }
      if (0 == UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
      } else {
        if (Cell_Specific.N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || P_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type) {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        } else {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].gap_index[DCI_idx]);
          bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2) - 1, UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        }
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Cell_Specific.CRSPortNum, UE_Specific[cell_ue_idx].TPMI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].PMI_Conf[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_1C:
      if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
        RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
        L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
        N_step_RB = (Cell_Specific.N_DL_RB < 50) ? 2 : 4;
        L_CRBs_prime = L_CRBs / N_step_RB;
        RB_start_prime = RB_start / N_step_RB;
        N_DL_VRB = Cell_Specific.N_DL_RB;
        N_DL_VRB_prime = N_DL_VRB / N_step_RB;
        UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start_prime, L_CRBs_prime, N_DL_VRB_prime);
      }
      if (UE_Specific[cell_ue_idx].Very_Compact_Flag[DCI_idx]) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].gap_index[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2((UE_Specific[cell_ue_idx].N_DL_VRB_gap1[DCI_idx] / N_step_RB) * (UE_Specific[cell_ue_idx].N_DL_VRB_gap1[DCI_idx] / N_step_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      } else {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 8, UE_Specific[cell_ue_idx].Info_MCCH_Change[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      }
      break;
    case DCI_Format_1D:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
      if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
        //Only Locailized; Distributed TBD
        RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
        L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
        UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start, L_CRBs, Cell_Specific.N_DL_RB);
      }
      if (0 == UE_Specific[cell_ue_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
      } else {
        if (Cell_Specific.N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || P_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[cell_ue_idx].RNTI_Type) {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        } else {
          bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].gap_index[DCI_idx]);
          bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_DL_RB * (Cell_Specific.N_DL_RB + 1) / 2) - 1, UE_Specific[cell_ue_idx].RIV[DCI_idx]);
        }
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Cell_Specific.CRSPortNum, UE_Specific[cell_ue_idx].TPMI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Downlink_power_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_2:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].TB_swap_flag[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[1]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (2 == Cell_Specific.CRSPortNum) ? 3 : 6, UE_Specific[cell_ue_idx].Precoding_Info[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_2A:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].TB_swap_flag[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[1]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (2 == Cell_Specific.CRSPortNum) ? 0 : 2, UE_Specific[cell_ue_idx].Precoding_Info[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_2B:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Scrambling_ID[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[1]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_2C:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 3, UE_Specific[cell_ue_idx].Antenna_Scrambling_Layers[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[1]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_2D:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      RBG_Mapping = GenRBG(UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx], Cell_Specific.N_DL_RB, P);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx] || Cell_Specific.N_DL_RB <= 10) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, ceil((float)Cell_Specific.N_DL_RB / (float)P), RBG_Mapping);
      } else {
        //36.213 7.1.6.2
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(P), UE_Specific[cell_ue_idx].subset_p[DCI_idx]);
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific.N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[cell_ue_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] < (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[cell_ue_idx].subset_p[DCI_idx] == (((Cell_Specific.N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P + ((Cell_Specific.N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific.N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[cell_ue_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == ((RBG_Mapping >> (n_RBG_subset_PRB / P)) & 1)) {
            DCI_bits(bit_counter + i) = 1;
          }
        }
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 0 : 2, UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (FDD == Cell_Specific.FrameStructureType) ? 3 : 4, UE_Specific[cell_ue_idx].HARQ_ID);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 3, UE_Specific[cell_ue_idx].Antenna_Scrambling_Layers[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[0]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].RV[1]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].PDSCH_RE_Mapping_QCL[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, UE_Specific[cell_ue_idx].HARQ_ACK_resource_offset[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
    case DCI_Format_3:
      //TBD
      break;
    case DCI_Format_3A:
      //TBD
      break;
    case DCI_Format_4:
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CA_Flag ? 3 : 0, Cell_Specific.CellID);
      if (0 == UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]) {
        if (-1 == UE_Specific[cell_ue_idx].RIV[DCI_idx]) {
          //Only non-hopping PUSCH with resource_allocation type0 36.213 8.1.1, others TBD
          RB_start = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][0];
          L_CRBs = (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size();
          UE_Specific[cell_ue_idx].RIV[DCI_idx] = GenRIV(RB_start, L_CRBs, Cell_Specific.N_UL_RB);
        }
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(Cell_Specific.N_UL_RB * (Cell_Specific.N_UL_RB + 1) / 2), UE_Specific[cell_ue_idx].RIV[DCI_idx]);
      } else {
        s.resize(4);
        N = ceil((float)Cell_Specific.N_UL_RB / (float)P) + 1;
        s[0] = N - UE_Specific[cell_ue_idx].s0[DCI_idx];
        s[1] = N - UE_Specific[cell_ue_idx].s1[DCI_idx];
        s[2] = N - UE_Specific[cell_ue_idx].s2[DCI_idx];
        s[3] = N - UE_Specific[cell_ue_idx].s3[DCI_idx];
        bit_counter += SetFieldBits(DCI_bits, bit_counter, CeilLog2(extended_binomial_coef(ceil((float)Cell_Specific.N_UL_RB / (float)P), 4)), combinatorial_idx(s));
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].TPC_for_PUSCH[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, UE_Specific[cell_ue_idx].Cyclic_shift_OCC[DCI_idx]);
      if (TDD == Cell_Specific.FrameStructureType) {
        bit_counter += SetFieldBits(DCI_bits, bit_counter, 2, (0 == Cell_Specific.ULDLConfiguration) ? UE_Specific[cell_ue_idx].UL_index[DCI_idx] : UE_Specific[cell_ue_idx].DAI[DCI_idx]);
      }
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].CSI_request_num[DCI_idx], UE_Specific[cell_ue_idx].CSI_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].SRS_request_num[DCI_idx], UE_Specific[cell_ue_idx].SRS_request[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, UE_Specific[cell_ue_idx].Resource_allocation_type_num[DCI_idx], UE_Specific[cell_ue_idx].Resource_allocation_type[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[0][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 5, UE_Specific[cell_ue_idx].MCS[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, 1, UE_Specific[cell_ue_idx].NDI[1][DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, (2 == Cell_Specific.CRSPortNum) ? 3 : 6, UE_Specific[cell_ue_idx].Precoding_Info[DCI_idx]);
      bit_counter += SetFieldBits(DCI_bits, bit_counter, Payload_size - bit_counter, 0);
      break;
  }
}

//! PDCCH产生。出自 ： 3GPP 36.213 9.1.1节； 36.211 6.8节
/*!
  \return 无返回值
*/
void Cell_C::PDCCH_Gen() {
  int cell_ue_idx = 0;
  bvec DCI_bits;
  int N_REG = 0;
  int M_bit = 0;
  int L = 0;
  int Y = 0;
  int n_RNTI = 0;
  int m_prime = 0;
  int i = 0;
  int j = 0;
  int m = 0;
  int M_L = 0;
  int n_CCE = 0;
  int l = 0;
  int k = 0;
  bool DCI_Flag = false;
  uint32 cinit = 0;
  int M_tot = 0;
  int M_quad = 0;
  int p = 0;
  int vshift = 0;
  int row = 0;
  int col = 0;
  const int C_TC_subblock = 32;
  int R_TC_subblock = 0;
  int N_D = 0;
  const int P_pattern[] = {1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30};
  int DCI_idx = 0;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  int Ctrl_Symb_Num = 0;
  bmat TBCC_bits;
  bvec RM_bits;
  bvec b;
  cfvecs symbs;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  cfmat w;
  cfmat w_bar;
  if (U != Cell_Specific.SubframeType) {
    row_a_v = powf(10.0f, Cell_Specific.PDCCH_RA / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    row_b_v = powf(10.0f, Cell_Specific.PDCCH_RB / 20.0f) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    N_REG = 0;
    Ctrl_Symb_Num = (S == Cell_Specific.SubframeType) ? Cell_Specific.Ctrl_Symb_Num_Special : Cell_Specific.Ctrl_Symb_Num;
    for (l = 0; l < Ctrl_Symb_Num; l++) {
      for (k = 0; k < (Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc); k++) {
        if (PCFICH == RE_Type(k, l) || PHICH == RE_Type(k, l)) {
          N_REG++;
        }
      }
    }
    N_REG = (Cell_Specific.N_DL_RB * 2) - N_REG / 4;
    if (Ctrl_Symb_Num > 1) {
      N_REG += ((4 == Cell_Specific.CRSPortNum) ? (2 * Cell_Specific.N_DL_RB) : (3 * Cell_Specific.N_DL_RB));
    }
    if (Ctrl_Symb_Num > 2) {
      N_REG += (Cell_Specific.N_DL_RB * 3);
    }
    if (Ctrl_Symb_Num > 3) {
      N_REG += ((4 == Cell_Specific.CRSPortNum) ? (2 * Cell_Specific.N_DL_RB) : (3 * Cell_Specific.N_DL_RB));
    }
    M_tot = N_REG * 8;
    b.resize(M_tot);
    b.setZero();
    DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
    for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
      if (NULL_DCI != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] && (!UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx])) {
        DCI_Flag = true;
        DCI_Gen(cell_ue_idx, DCI_bits);
        AddCRC(CRC16, DCI_bits);
        n_RNTI = UE_Specific[cell_ue_idx].RNTI;
        for (k = (int)DCI_bits.size() - 1, i = 0; k > (int)DCI_bits.size() - 16; k--, i++) {
          DCI_bits(k) = DCI_bits(k) ^ ((n_RNTI >> i) & 1);
        }
        if (UE_Specific[cell_ue_idx].UE_Transmit_Selection) {
          DCI_bits((int)DCI_bits.size() - 1) = DCI_bits((int)DCI_bits.size() - 1) ^ 1;
        }
        TBCC_Encoder(DCI_bits, TBCC_bits);
        L = UE_Specific[cell_ue_idx].Aggregation_level[DCI_idx];
        M_bit = L * 72;
        Rate_Matching_TBCC(TBCC_bits, M_bit, RM_bits);
        if (Common_Search_Space == UE_Specific[cell_ue_idx].DCI_Search_Space[DCI_idx]) {
          Y = 0;
          M_L = (4 == L) ? 4 : 2;
        } else {
          Y = Calc_Yk(n_RNTI, Cell_Specific.n_subframe);
          M_L = (1 == L || 2 == L) ? 6 : 2;
        }
        m = UE_Specific[cell_ue_idx].m_Candidated[DCI_idx];
        m_prime = UE_Specific[cell_ue_idx].CA_Flag ? (m + M_L * Cell_Specific.CellID) : m;
        for (i = 0; i < L; i++) {
#if 1 == ERROR_REPORT
          if ((N_REG / 9) < L) {
            Error("Aggregation_level Error: N_CCE < L !");
          }
#endif
          n_CCE = L * ((Y + m_prime) % ((N_REG / 9) / L)) + i;
          for (k = 0; k < 72; k++) {
            b(n_CCE * 72 + k) = RM_bits(i * 72 + k);
          }
        }
      }
    }
    if (DCI_Flag) {
      cinit = (Cell_Specific.n_subframe << 9) + Cell_Specific.N_cell_ID;
      Scrambling(cinit, b);
      symbs.resize(1);
      Modulation_QPSK(b, symbs[0]);
      Layer_Mapping(symbs,
                    Cell_Specific.CRSPortNum,
                    (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                    layer_mapping_symbs);
      Precoding(layer_mapping_symbs,
                Cell_Specific.CRSPortNum,
                (int)symbs[0].size(),
                (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity,
                UE_Specific[0].Codebook_index0,
                UE_Specific[0].Codebook_index1,
                false,
                precoding_symbs);
      M_quad = precoding_symbs.cols() / 4;
      R_TC_subblock = ceil((float)M_quad / (float)C_TC_subblock);
      N_D = R_TC_subblock * C_TC_subblock - M_quad;
      row = 0;
      col = 0;
      k = 0;
      i = 0;
      w.resize(precoding_symbs.rows(), precoding_symbs.cols());
      while (k < M_quad) {
        i = P_pattern[col];
        for (row = 0; row < R_TC_subblock; row++) {
          if (i >= N_D) {
            for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
              w(p, 4 * k) = precoding_symbs(p, 4 * (i - N_D));
              w(p, 4 * k + 1) = precoding_symbs(p, 4 * (i - N_D) + 1);
              w(p, 4 * k + 2) = precoding_symbs(p, 4 * (i - N_D) + 2);
              w(p, 4 * k + 3) = precoding_symbs(p, 4 * (i - N_D) + 3);
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
      w_bar.resize(precoding_symbs.rows(), precoding_symbs.cols());
      for (i = 0; i < M_quad; i++) {
        j = ((i + Cell_Specific.N_cell_ID) % M_quad);
        for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
          w_bar(p, 4 * i) = w(p, 4 * j);
          w_bar(p, 4 * i + 1) = w(p, 4 * j + 1);
          w_bar(p, 4 * i + 2) = w(p, 4 * j + 2);
          w_bar(p, 4 * i + 3) = w(p, 4 * j + 3);
        }
      }
      vshift = Cell_Specific.N_cell_ID % 3;
      i = 0;
      for (k = 0; k < (Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc); k++) {
        for (l = 0; l < Ctrl_Symb_Num; l++) {
          power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l]) ? row_a_v : row_b_v;
          if (0 == l || (1 == l && 4 == Cell_Specific.CRSPortNum) || (3 == l && 4 == Cell_Specific.CRSPortNum)) {
            if (PCFICH != RE_Type(k, l) && PHICH != RE_Type(k, l) && vshift != (k % 3)) {
              for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                RE[PortToPhyAntenna[p]](k, l) += power_scale * w_bar(p, i);
              }
              RE_Type(k, l) = PDCCH;
              i++;
            }
          } else {
            if (PCFICH != RE_Type(k, l) && PHICH != RE_Type(k, l)) {
              for (p = 0; p < Cell_Specific.CRSPortNum; p++) {
                RE[PortToPhyAntenna[p]](k, l) += power_scale * w_bar(p, i);
              }
              RE_Type(k, l) = PDCCH;
              i++;
            }
          }
        }
      }
    }
  }
}

//! EPDCCH映射（需要在扣除EPDCCH映射位置后计算PDSCH一个TTI内可用的bit数）
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::EPDCCH_DMP(int cell_ue_idx) {
  int i = 0;
  int l = 0;
  int k = 0;
  int sc = 0;
  int set_idx = 0;
  if (UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific.SubframeType) {
    for (l = UE_Specific[cell_ue_idx].startSymb_EPDCCH; l < Cell_Specific.DL_Symb_Num; l++) {
      for (set_idx = 0; set_idx < UE_Specific[cell_ue_idx].EPDCCH_Set_Num; set_idx++) {
        for (i = 0; i < (int)UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx].size(); i++) {
          k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][i] * Cell_Specific.N_RB_sc;
          for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
            if (EMPTY_RE == RE_Type(k + sc, l) || EPDCCH == RE_Type(k + sc, l)) {
              RE_Type(k + sc, l) = EPDCCH;
            }
          }
        }
      }
    }
  }
}

//! EPDCCH产生。出自 ： 3GPP 36.213 9.1.4节； 36.211 6.8A节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::EPDCCH_Gen(int cell_ue_idx) {
  int i = 0;
  int j = 0;
  int p = 0;
  int n_EREG = 0;
  int k = 0;
  int l = 0;
  int n_prime = 0;
  int rb_idx = 0;
  int sc = 0;
  int PRB_idx = 0;
  int EREGs_idx = 0;
  int DCI_idx = 0;
  int n_RNTI = 0;
  bool DCI_Flag = false;
  int N_ECCE_EREG = 0;
  int N_RB_ECCE = 0;
  int N_ECCE = 0;
  int n_EPDCCH = 0;
  int L = 0;
  int M_bit = 0;
  int Y = 0;
  int m = 0;
  int n_ECCE = 0;
  int n_ECCE_low = 0;
  int set_idx = 0;
  int M_L[2] = {0};
  uint32 cinit = 0;
  bvec DCI_bits;
  bmat TBCC_bits;
  bvec RM_bits;
  cfvecs symbs;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  if (UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0].size() > 0 && U != Cell_Specific.SubframeType) {
    DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
    if (NULL_DCI != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] && UE_Specific[cell_ue_idx].DCI_On_EPDCCH[DCI_idx]) {
      set_idx = UE_Specific[cell_ue_idx].EPDCCH_set_idx[DCI_idx];
      if (NormalCP == Cell_Specific.CP_Type && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration)) {
        N_ECCE_EREG = 4;
      } else {
        N_ECCE_EREG = 8;
      }
      N_RB_ECCE = 16 / N_ECCE_EREG;
      N_ECCE = N_RB_ECCE * UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx];
      n_EPDCCH = 0;
      for (l = UE_Specific[cell_ue_idx].startSymb_EPDCCH; l < Cell_Specific.DL_Symb_Num; l++) {
        for (rb_idx = 0; rb_idx < (int)UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0].size(); rb_idx++) {
          for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
            k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0][rb_idx] * Cell_Specific.N_RB_sc + sc;
            if (EMPTY_RE == RE_Type(k, l)) {
              n_EPDCCH = n_EPDCCH + 1;
            }
          }
        }
      }
      DCI_Flag = true;
      DCI_Gen(cell_ue_idx, DCI_bits);
      AddCRC(CRC16, DCI_bits);
      n_RNTI = UE_Specific[cell_ue_idx].RNTI;
      for (k = (int)DCI_bits.size() - 1, i = 0; k > (int)DCI_bits.size() - 16; k--, i++) {
        DCI_bits(k) = DCI_bits(k) ^ ((n_RNTI >> i) & 1);
      }
      if (UE_Specific[cell_ue_idx].UE_Transmit_Selection) {
        DCI_bits((int)DCI_bits.size() - 1) = DCI_bits((int)DCI_bits.size() - 1) ^ 1;
      }
      TBCC_Encoder(DCI_bits, TBCC_bits);
      L = UE_Specific[cell_ue_idx].Aggregation_level[DCI_idx];
      if (0 == set_idx) {
        Y = Calc_Yk(n_RNTI, Cell_Specific.n_subframe);
      } else {
        Y = Calc_Yk_EPDCCH(n_RNTI, Cell_Specific.n_subframe);
      }
      Get_EPDCCH_M_L(L, n_EPDCCH, Cell_Specific, UE_Specific[cell_ue_idx], M_L);
      m = UE_Specific[cell_ue_idx].m_Candidated[DCI_idx];
      M_bit = 0;
      for (i = 0; i < L; i++) {
        n_ECCE = L * ((Y + (int)floor((float)(m * N_ECCE) / (float)(L * M_L[set_idx])) + (UE_Specific[cell_ue_idx].CA_Flag ? Cell_Specific.CellID : 0)) % (N_ECCE / L)) + i;
        if (0 == i) {
          n_ECCE_low = n_ECCE;
        }
        if (Localized_EPDCCH == UE_Specific[cell_ue_idx].TMType_EPDCCH[set_idx]) {
          PRB_idx = n_ECCE / N_RB_ECCE;
          EREGs_idx = n_ECCE % N_RB_ECCE;
          n_EREG = 0;
          for (l = 0; l < Cell_Specific.DL_Symb_Num; l++) {
            for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
              k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][PRB_idx] * Cell_Specific.N_RB_sc + sc;
              if ((n_EREG % N_RB_ECCE) == EREGs_idx && l >= UE_Specific[cell_ue_idx].startSymb_EPDCCH && EMPTY_RE == RE_Type(k, l)) {
                M_bit++;
              }
              if (DMRS_Port107_110 != RE_Type(k, l)) {
                n_EREG++;
                n_EREG = n_EREG & 15;
              }
            }
          }
        } else {
          EREGs_idx = n_ECCE / UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx];
          j = 0;
          for (j = 0; j < N_ECCE_EREG; j++) {
            PRB_idx = (n_ECCE + j * MAX(1, UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx] / N_ECCE_EREG)) % UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx];
            n_EREG = 0;
            for (l = 0; l < Cell_Specific.DL_Symb_Num; l++) {
              for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
                k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][PRB_idx] * Cell_Specific.N_RB_sc + sc;
                if ((n_EREG % N_RB_ECCE) == EREGs_idx && l >= UE_Specific[cell_ue_idx].startSymb_EPDCCH && EMPTY_RE == RE_Type(k, l)) {
                  M_bit++;
                }
                if (DMRS_Port107_110 != RE_Type(k, l)) {
                  n_EREG++;
                  n_EREG = n_EREG & 15;
                }
              }
            }
          }
        }
      }
      M_bit = M_bit * 2;
      Rate_Matching_TBCC(TBCC_bits, M_bit, RM_bits);
    }
    if (DCI_Flag) {
      cinit = (Cell_Specific.n_subframe << 9) + UE_Specific[cell_ue_idx].n_EPDCCH_ID[set_idx];
      Scrambling(cinit, RM_bits);
      symbs.resize(1);
      Modulation_QPSK(RM_bits, symbs[0]);
      Layer_Mapping(symbs,
                    1,
                    Single_Antenna_Port,
                    layer_mapping_symbs);
      Precoding(layer_mapping_symbs,
                1,
                (int)symbs[0].size(),
                Single_Antenna_Port,
                UE_Specific[0].Codebook_index0,
                UE_Specific[0].Codebook_index1,
                false,
                precoding_symbs);
      n_prime = n_ECCE_low % N_RB_ECCE + UE_Specific[cell_ue_idx].RNTI % MIN(L, N_RB_ECCE);
      if (NormalCP == Cell_Specific.CP_Type && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration)) {
        p = (int)Port107 + n_prime;
      } else if (NormalCP == Cell_Specific.CP_Type) {
        p = (int)Port107 + 2 * n_prime;
      } else if (ExtendedCP == Cell_Specific.CP_Type) {
        p = (int)Port107 + n_prime;
      }
      M_bit = 0;
      for (i = 0; i < L; i++) {
        n_ECCE = L * ((Y + (int)floor((float)(m * N_ECCE) / (float)(L * M_L[set_idx])) + (UE_Specific[cell_ue_idx].CA_Flag ? Cell_Specific.CellID : 0)) % (N_ECCE / L)) + i;
        if (0 == i) {
          n_ECCE_low = n_ECCE;
        }
        if (Localized_EPDCCH == UE_Specific[cell_ue_idx].TMType_EPDCCH[set_idx]) {
          PRB_idx = n_ECCE / N_RB_ECCE;
          EREGs_idx = n_ECCE % N_RB_ECCE;
          n_EREG = 0;
          for (l = 0; l < Cell_Specific.DL_Symb_Num; l++) {
            for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
              k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][PRB_idx] * Cell_Specific.N_RB_sc + sc;
              if ((n_EREG % N_RB_ECCE) == EREGs_idx && l >= UE_Specific[cell_ue_idx].startSymb_EPDCCH && EMPTY_RE == RE_Type(k, l)) {
                RE[PortToPhyAntenna[p]](k, l) += precoding_symbs(0, M_bit);
                M_bit++;
              }
              if (DMRS_Port107_110 != RE_Type(k, l)) {
                n_EREG++;
                n_EREG = n_EREG & 15;
              }
            }
          }
        } else {
          EREGs_idx = n_ECCE / UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx];
          j = 0;
          for (j = 0; j < N_ECCE_EREG; j++) {
            PRB_idx = (n_ECCE + j * MAX(1, UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx] / N_ECCE_EREG)) % UE_Specific[cell_ue_idx].numberPRB_Pairs_EPDCCH[set_idx];
            n_EREG = 0;
            for (l = 0; l < Cell_Specific.DL_Symb_Num; l++) {
              for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
                k = UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[set_idx][PRB_idx] * Cell_Specific.N_RB_sc + sc;
                if ((n_EREG % N_RB_ECCE) == EREGs_idx && l >= UE_Specific[cell_ue_idx].startSymb_EPDCCH && EMPTY_RE == RE_Type(k, l)) {
                  RE[PortToPhyAntenna[p]](k, l) += precoding_symbs(0, M_bit);
                  M_bit++;
                }
                if (DMRS_Port107_110 != RE_Type(k, l)) {
                  n_EREG++;
                  n_EREG = n_EREG & 15;
                }
              }
            }
          }
        }
      }
    }
  }
}

//! 一个TTI内PDSCH可用符号数计算
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
int Cell_C::Calc_PDSCH_G_symb(int cell_ue_idx) {
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int sc = 0;
  int G_symb = 0;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  int Ctrl_Symb_Num = (S == Cell_Specific.SubframeType) ? Cell_Specific.Ctrl_Symb_Num_Special : Cell_Specific.Ctrl_Symb_Num;
  for (l = Ctrl_Symb_Num; l < Cell_Specific.DL_Symb_Num; l++) {
    for (rb_idx = 0; rb_idx < (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
#if 1 == ERROR_REPORT
      if (UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] < 0 || UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] >= Cell_Specific.N_DL_RB) {
        Error("RB_Assignment Error: RB_Assignment < 0 or RB_Assignment >= N_DL_RB !");
      }
#endif
      for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
        k = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] * Cell_Specific.N_RB_sc + sc;
        if (EMPTY_RE == RE_Type(k, l) || PDSCH == RE_Type(k, l)) {
          G_symb = G_symb + 1;
        }
      }
    }
  }
  return G_symb;
}

//! 码本获取
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \param M_layer_symb ： 一层中的符号个数，输入
  \return 无返回值
*/
void Cell_C::Calc_Codebook_index(int cell_ue_idx, int M_layer_symb) {
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int sc = 0;
  int G_symb = 0;
  int subband_idx = 0;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  int Ctrl_Symb_Num = (S == Cell_Specific.SubframeType) ? Cell_Specific.Ctrl_Symb_Num_Special : Cell_Specific.Ctrl_Symb_Num;
  if (Closed_Loop_SM == UE_Specific[cell_ue_idx].Transmission_Scheme || MU_MIMO == UE_Specific[cell_ue_idx].Transmission_Scheme) {
    UE_Specific[cell_ue_idx].Codebook_index0.resize(M_layer_symb);
    if (UE_Specific[cell_ue_idx].alternativeCodeBookEnabledFor4TX_r12) {
      UE_Specific[cell_ue_idx].Codebook_index1.resize(M_layer_symb);
    }
    for (l = Ctrl_Symb_Num; l < Cell_Specific.DL_Symb_Num; l++) {
      for (rb_idx = 0; rb_idx < (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
        subband_idx = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] / Feedback[cell_ue_idx].k_Subband;
        for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
          k = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] * Cell_Specific.N_RB_sc + sc;
          if (EMPTY_RE == RE_Type(k, l) || PDSCH == RE_Type(k, l)) {
            if (!UE_Specific[cell_ue_idx].alternativeCodeBookEnabledFor4TX_r12) {
              UE_Specific[cell_ue_idx].Codebook_index0(G_symb) = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
            } else {
              //TBD
            }
            G_symb = G_symb + 1;
          }
        }
      }
    }
  }
}

//! 把PDSCH映射到RE时频资源上
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \param symbs_in ： 复数float型矩阵，矩阵行数=天线端口数，输入
  \return 无返回值
*/
void Cell_C::PDSCH_MappingToRE(int cell_ue_idx, const cfmat &symbs_in) {
  int k = 0;
  int l = 0;
  int rb_idx = 0;
  int sc = 0;
  int G_symb = 0;
  int P = symbs_in.rows();
  int p = 0;
  Port_Type_E p_start = UE_Specific[cell_ue_idx].Start_Port;
  int P_idx = (4 == Cell_Specific.CRSPortNum) ? 1 : 0;
  int CP_idx = (NormalCP == Cell_Specific.CP_Type) ? 0 : 1;
  float row_a = 0.0f;
  float row_b = 0.0f;
  float row_a_v = 0.0f;
  float row_b_v = 0.0f;
  float power_scale = 0.0f;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  int Ctrl_Symb_Num = (S == Cell_Specific.SubframeType) ? Cell_Specific.Ctrl_Symb_Num_Special : Cell_Specific.Ctrl_Symb_Num;
  int subband_idx = 0;
  int Codebook_index = 0;
  cpxf temp = 0.0f;
  if ((TM7 == UE_Specific[cell_ue_idx].TM && DCI_Format_1 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
    row_a_v = powf(10.0f, Cell_Specific.PDSCH_RA / 20.0f);
    row_b_v = powf(10.0f, Cell_Specific.PDSCH_RB / 20.0f);
  } else {
    row_a = powf(10.0f, UE_Specific[cell_ue_idx].p_a / 10.0f);
    if (MU_MIMO == UE_Specific[cell_ue_idx].Transmission_Scheme) {
      row_a = row_a * ((0 == UE_Specific[cell_ue_idx].Downlink_power_offset[DCI_idx]) ? 0.5f : 1.0f);
    }
    row_b = row_a * row_b_div_a[(1 == Cell_Specific.CRSPortNum) ? 0 : 1][Cell_Specific.p_b];
    if (Transmit_Diversity == UE_Specific[cell_ue_idx].Transmission_Scheme) {
      row_a_v = MySqrt(row_a) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
      row_b_v = MySqrt(row_b) * MySqrt((float)MIN(2, Cell_Specific.CRSPortNum));
    } else {
      row_a_v = MySqrt(row_a) * MySqrt((float)Cell_Specific.CRSPortNum);
      row_b_v = MySqrt(row_b) * MySqrt((float)Cell_Specific.CRSPortNum);
    }
  }
  for (l = Ctrl_Symb_Num; l < Cell_Specific.DL_Symb_Num; l++) {
    power_scale = (0 == row_a_b_flag_nonMBSFN[P_idx][CP_idx][l]) ? row_a_v : row_b_v;
    //power_scale for MBSFN TBD
    for (rb_idx = 0; rb_idx < (int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size(); rb_idx++) {
      subband_idx = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] / Feedback[cell_ue_idx].k_Subband;
      Codebook_index = Feedback[cell_ue_idx].Subband_PMI(UE_Specific[cell_ue_idx].N_L[DCI_idx] - 1, subband_idx);
      for (sc = 0; sc < Cell_Specific.N_RB_sc; sc++) {
        k = UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx][rb_idx] * Cell_Specific.N_RB_sc + sc;
        if (EMPTY_RE == RE_Type(k, l) || PDSCH == RE_Type(k, l)) {
          if (TM7 == UE_Specific[cell_ue_idx].TM && DCI_Format_1 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
            temp = symbs_in(0, G_symb) * power_scale;
            RE[PortToPhyAntenna[(int)Port5]](k, l) += Codebook_2P1v[Codebook_index](0) * temp;
            RE[PortToPhyAntenna[(int)Port8]](k, l) += Codebook_2P1v[Codebook_index](1) * temp;
          } else if ((TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
            if (1 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
              if (Simuctrl.With_Simultaneous_Transmission) {
                temp = symbs_in(0, G_symb) * power_scale;
                RE[PortToPhyAntenna[(int)Port7]](k, l) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](0) * temp;
                RE[PortToPhyAntenna[(int)Port8]](k, l) += SQRT_2_RECIP * Codebook_2P1v[Codebook_index](1) * temp;
              } else {
                temp = symbs_in(0, G_symb) * power_scale;
                RE[PortToPhyAntenna[(int)Port7]](k, l) += Codebook_2P1v[Codebook_index](0) * temp;
                RE[PortToPhyAntenna[(int)Port8]](k, l) += Codebook_2P1v[Codebook_index](1) * temp;
              }
            } else if (2 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
              for (p = (int)p_start; p < ((int)p_start + P); p++) {
                temp = symbs_in(p - (int)p_start, G_symb) * power_scale;
                RE[PortToPhyAntenna[(int)Port7]](k, l) += Codebook_2P2v[Codebook_index](0, p - (int)Port7) * temp;
                RE[PortToPhyAntenna[(int)Port8]](k, l) += Codebook_2P2v[Codebook_index](1, p - (int)Port7) * temp;
              }
            }
          } else {
            for (p = (int)p_start; p < ((int)p_start + P); p++) {
              RE[PortToPhyAntenna[p]](k, l) += symbs_in(p - (int)p_start, G_symb) * power_scale;
            }
          }
          G_symb = G_symb + 1;
        }
      }
    }
  }
}

//! PDSCH产生。出自 ： 3GPP 36.212 5.3.2节； 36.211 6.3节
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::PDSCH_Gen(int cell_ue_idx) {
  int cw_idx = 0;
  int cb_idx = 0;
  bvec SourceBits;
  bvecs CB_bits;
  bvec RM_bits;
  bvec CB_Concatenation_bits;
  int E_r = 0;
  uint32 cinit = 0;
  cfvecs symbs;
  cfmat layer_mapping_symbs;
  cfmat precoding_symbs;
  int G_prime = 0;  //G_prime = G / Q_m / N_L , G = total number of bits available for the transmission of one transport block
  int N_L_1cw = 0;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  int F = 0;
  if (UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific.SubframeType) {
#if 1 == ERROR_REPORT
    if ((int)UE_Specific[cell_ue_idx].RB_Assignment[DCI_idx].size() > Cell_Specific.N_DL_RB) {
      Error("RB_Assignment Error: RB_Num > N_DL_RB !");
    }
#endif
    symbs.resize(UE_Specific[cell_ue_idx].codeword_num[DCI_idx]);
    G_prime = UE_Specific[cell_ue_idx].G_symb;
    if (Transmit_Diversity == UE_Specific[cell_ue_idx].Transmission_Scheme) {
      G_prime = G_prime / 2;
    }
    for (cw_idx = 0; cw_idx < UE_Specific[cell_ue_idx].codeword_num[DCI_idx]; cw_idx++) {
      N_L_1cw = (Transmit_Diversity == UE_Specific[cell_ue_idx].Transmission_Scheme) ? 2 : UE_Specific[cell_ue_idx].v[cw_idx];
      CB_Concatenation_bits.resize(N_L_1cw * G_prime * UE_Specific[cell_ue_idx].Qm[cw_idx]);
      CB_Concatenation_bits.setZero();
      //36.212
      if (0 == ReTransmit[cell_ue_idx].TxNum[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID]) {
        random_bin_vec(SourceBits, UE_Specific[cell_ue_idx].TBS[cw_idx]);  //伪随机源
        AddCRC(CRC24A, SourceBits);                                        //Add CRC for TB
        F = CB_Segmentation_CB_CRC_Attachment(SourceBits, CB_bits);        //码块分割、加CB CRC
        ReTransmit[cell_ue_idx].Turbo_bits[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID].resize(CB_bits.size());
        for (cb_idx = 0; cb_idx < (int)CB_bits.size(); cb_idx++) {
          Turbo_Encoder(CB_bits[cb_idx], ReTransmit[cell_ue_idx].Turbo_bits[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID][cb_idx]);  //Turbo编码
        }
      }
      E_r = 0;
      for (cb_idx = 0; cb_idx < (int)ReTransmit[cell_ue_idx].Turbo_bits[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID].size(); cb_idx++) {
        Rate_Matching_Turbo(ReTransmit[cell_ue_idx].Turbo_bits[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID][cb_idx],
                            cb_idx,
                            UE_Specific[cell_ue_idx].N_IR,
                            true,
                            ReTransmit[cell_ue_idx].Turbo_bits[cw_idx][UE_Specific[cell_ue_idx].HARQ_ID].size(),
                            G_prime,
                            N_L_1cw,
                            UE_Specific[cell_ue_idx].Qm[cw_idx],
                            UE_Specific[cell_ue_idx].RV[cw_idx],
                            (0 == cb_idx) ? F : 0,
                            RM_bits);                                  //速率匹配
        CB_Concatenation_bits.segment(E_r, RM_bits.size()) = RM_bits;  //码块级联
        E_r = E_r + RM_bits.size();
      }
      //36.211
      cinit = (UE_Specific[cell_ue_idx].RNTI << 14) + (cw_idx << 13) + (Cell_Specific.n_subframe << 9) + Cell_Specific.N_cell_ID;
      Scrambling(cinit, CB_Concatenation_bits);
      /*WithFile("CB_Concatenation_bits");
            for (int i = 0; i < (int)CB_Concatenation_bits.size(); i++)
            {
                FilePrint("%d\n", CB_Concatenation_bits(i));
            }*/
      if (8 == UE_Specific[cell_ue_idx].Qm[cw_idx]) {
        Modulation_256QAM(CB_Concatenation_bits, symbs[cw_idx]);
      } else if (6 == UE_Specific[cell_ue_idx].Qm[cw_idx]) {
        Modulation_64QAM(CB_Concatenation_bits, symbs[cw_idx]);
      } else if (4 == UE_Specific[cell_ue_idx].Qm[cw_idx]) {
        Modulation_16QAM(CB_Concatenation_bits, symbs[cw_idx]);
      } else {
        Modulation_QPSK(CB_Concatenation_bits, symbs[cw_idx]);
      }
      /*WithFile("Modulation");
            for (int i = 0; i < (int)symbs[cw_idx].size(); i++)
            {
                FilePrint("%f %f\n", symbs[cw_idx](i).real(), symbs[cw_idx](i).imag());
            }*/
    }
    Layer_Mapping(symbs,
                  UE_Specific[cell_ue_idx].N_L[DCI_idx],
                  UE_Specific[cell_ue_idx].Transmission_Scheme,
                  layer_mapping_symbs);
    Calc_Codebook_index(cell_ue_idx, layer_mapping_symbs.cols());
    Precoding(layer_mapping_symbs,
              (Up_to_8_layer == UE_Specific[cell_ue_idx].Transmission_Scheme || Dual_layer == UE_Specific[cell_ue_idx].Transmission_Scheme || Single_Antenna_Port == UE_Specific[cell_ue_idx].Transmission_Scheme) ? UE_Specific[cell_ue_idx].v[0] : Cell_Specific.CRSPortNum,
              (int)symbs[0].size(),
              UE_Specific[cell_ue_idx].Transmission_Scheme,
              UE_Specific[cell_ue_idx].Codebook_index0,
              UE_Specific[cell_ue_idx].Codebook_index1,
              UE_Specific[cell_ue_idx].alternativeCodeBookEnabledFor4TX_r12,
              precoding_symbs);
    PDSCH_MappingToRE(cell_ue_idx, precoding_symbs);
  }
}
