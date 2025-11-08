/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_DMP.cpp
    \brief UE解映射实现文件
    内容 ：UE_C类解映射相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! PSS解映射
/*!
\param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PSS_Remove(int ue_cell_idx) {
  int k = -31 + Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2;
  int l = Cell_Specific[ue_cell_idx].N_DL_symb - 1;
  if (FDD == Cell_Specific[ue_cell_idx].FrameStructureType && (0 == Cell_Specific[ue_cell_idx].n_subframe || 5 == Cell_Specific[ue_cell_idx].n_subframe)) {
    l = Cell_Specific[ue_cell_idx].N_DL_symb - 1;
    RE_Type.block(k - 5, l, 72, 1).setConstant(PSS);
  } else if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType && (1 == Cell_Specific[ue_cell_idx].n_subframe || 6 == Cell_Specific[ue_cell_idx].n_subframe)) {
    l = 2;
    RE_Type.block(k - 5, l, 72, 1).setConstant(PSS);
  }
}

//! SSS解映射
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::SSS_Remove(int ue_cell_idx) {
  int k = -31 + Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2;
  int l = Cell_Specific[ue_cell_idx].N_DL_symb - 2;
  if ((FDD == Cell_Specific[ue_cell_idx].FrameStructureType || TDD == Cell_Specific[ue_cell_idx].FrameStructureType) && (0 == Cell_Specific[ue_cell_idx].n_subframe || 5 == Cell_Specific[ue_cell_idx].n_subframe)) {
    if (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) {
      l = Cell_Specific[ue_cell_idx].N_DL_symb - 2;
    } else {
      l = Cell_Specific[ue_cell_idx].N_DL_symb + Cell_Specific[ue_cell_idx].N_DL_symb - 1;
    }
    RE_Type.block(k - 5, l, 72, 1).setConstant(SSS);
  }
}

//! PBCH解映射
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PBCH_Remove(int ue_cell_idx) {
  int k = 0;
  int l = 0;
  int k_offset = 0;
  if (0 == Cell_Specific[ue_cell_idx].n_subframe) {
    k_offset = Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc / 2 - 36;
    for (l = Cell_Specific[ue_cell_idx].N_DL_symb; l < (Cell_Specific[ue_cell_idx].N_DL_symb + 4); l++) {
      for (k = k_offset; k < (k_offset + 72); k++) {
        if (CRS != RE_Type(k, l)) {
          RE_Type(k, l) = PBCH;
        }
      }
    }
  }
}

//! PCFICH解映射
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PCFICH_Remove(int ue_cell_idx) {
  int i = 0;
  int j = 0;
  int k = 0;
  int k_bar = 0;
  int k_offset = 0;
  int vshift = 0;
  //DMP
  Result[ue_cell_idx].PCFICH_Flag = true;
  k_bar = Cell_Specific[ue_cell_idx].N_RB_sc / 2 * (Cell_Specific[ue_cell_idx].N_cell_ID % (2 * Cell_Specific[ue_cell_idx].N_DL_RB));
  vshift = Cell_Specific[ue_cell_idx].N_cell_ID % 3;
  for (i = 0; i < 4; i++) {
    k_offset = k_bar + (i * Cell_Specific[ue_cell_idx].N_DL_RB / 2) * Cell_Specific[ue_cell_idx].N_RB_sc / 2;
    j = 0;
    for (k = k_offset; k < (k_offset + 6); k++) {
      if (vshift != (k % 3)) {
        RE_Type(k, 0) = PCFICH;
        j++;
      }
    }
  }
}

//! PHICH解映射
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PHICH_Remove(int ue_cell_idx) {
  const float N_g_map[4] = {1.0f / 6.0f, 1.0f / 2.0f, 1.0f, 2.0f};
  float N_g = N_g_map[Cell_Specific[ue_cell_idx].phich_Resource];
  int N_group_PHICH = 0;
  int i = 0;
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
  if (NormalCP == Cell_Specific[ue_cell_idx].CP_Type) {
    N_group_PHICH = ceil(N_g * ((float)Cell_Specific[ue_cell_idx].N_DL_RB / 8.0f));
  } else {
    N_group_PHICH = 2 * ceil(N_g * ((float)Cell_Specific[ue_cell_idx].N_DL_RB / 8.0f));
  }
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
  }
}

//! EPDCCH解映射
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_Remove(int ue_cell_idx) {
}
