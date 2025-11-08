/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_DCC.cpp
    \brief UE译码实现文件
    内容 ：UE_C类译码相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! PBCH盲检测译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \param CRSPortNum ： CRS端口数
  \param SFN_Low2bit ： SFN低2bit信息
  \return 无返回值
*/
void UE_C::PBCH_DCC_Blind(int ue_cell_idx, int CRSPortNum, int SFN_Low2bit) {
  int i = 0;
  fmat d;
  bvec b;
  bvec PBCH_CRC_mask;
  int Context = 0;
  PBCH_CRC_mask.resize(16);
  if (1 == CRSPortNum) {
    PBCH_CRC_mask << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  } else if (2 == CRSPortNum) {
    PBCH_CRC_mask << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;
  } else {
    PBCH_CRC_mask << 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1;
  }
  DeRate_Matching_TBCC(V_softbits,
                       24 + 16,
                       d);
  WAVA_Viterbi(d, b);
  for (i = 0; i < 16; i++) {
    b(24 + i) = b(24 + i) ^ PBCH_CRC_mask(i);
  }
  Result[ue_cell_idx].PBCH_Blind_Flag = CheckCRC(CRC16, b);
  if (Result[ue_cell_idx].PBCH_Blind_Flag) {
    for (i = 0; i < 24; i++) {
      Context += ((b(i) & 1) << (23 - i));
    }
    Result[ue_cell_idx].dl_BandWidth = (BandWidth_E)((Context >> 21) & 0x7);
    Result[ue_cell_idx].phich_Duration = (PHICH_Duration_Type_E)((Context >> 20) & 0x1);
    Result[ue_cell_idx].phich_Resource = (PHICH_Resource_Type_E)((Context >> 18) & 0x3);
    Result[ue_cell_idx].SFN_High8bit = ((Context >> 10) & 0xFF);
    Result[ue_cell_idx].SFN_Low2bit = SFN_Low2bit;
    Result[ue_cell_idx].SFN = (Result[ue_cell_idx].SFN_High8bit << 2) + Result[ue_cell_idx].SFN_Low2bit;
    if ((Result[ue_cell_idx].dl_BandWidth != Cell_Specific[ue_cell_idx].dl_BandWidth || Result[ue_cell_idx].phich_Duration != Cell_Specific[ue_cell_idx].phich_Duration || Result[ue_cell_idx].phich_Resource != Cell_Specific[ue_cell_idx].phich_Resource) && !Simuctrl.Inject_Flag) {
      Result[ue_cell_idx].PBCH_Blind_Flag = false;
    } else {
      Counter_5ms = SFN_Low2bit - 3;
    }
    sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("PBCH_DCC_Blind: dl_BandWidth=%d, phich_Duration=%d, phich_Resource=%d, SFN=%d,  CRSPortNum=%d\n",
              Result[ue_cell_idx].dl_BandWidth, Result[ue_cell_idx].phich_Duration, Result[ue_cell_idx].phich_Resource, Result[ue_cell_idx].SFN, CRSPortNum);
  }
}

//! PBCH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \param CRSPortNum ： CRS端口数
  \return 无返回值
*/
void UE_C::PBCH_DCC(int ue_cell_idx, int CRSPortNum) {
  int i = 0;
  fmat d;
  bvec b;
  bvec PBCH_CRC_mask;
  int Context = 0;
  PBCH_CRC_mask.resize(16);
  if (1 == CRSPortNum) {
    PBCH_CRC_mask << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  } else if (2 == CRSPortNum) {
    PBCH_CRC_mask << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;
  } else {
    PBCH_CRC_mask << 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1;
  }
  DeRate_Matching_TBCC(V_softbits,
                       24 + 16,
                       d);
  WAVA_Viterbi(d, b);
  for (i = 0; i < 16; i++) {
    b(24 + i) = b(24 + i) ^ PBCH_CRC_mask(i);
  }
  Result[ue_cell_idx].PBCH_Flag = CheckCRC(CRC16, b);
  if (Result[ue_cell_idx].PBCH_Flag) {
    for (i = 0; i < 24; i++) {
      Context += ((b(i) & 1) << (23 - i));
    }
    Result[ue_cell_idx].dl_BandWidth = (BandWidth_E)((Context >> 21) & 0x7);
    Result[ue_cell_idx].phich_Duration = (PHICH_Duration_Type_E)((Context >> 20) & 0x1);
    Result[ue_cell_idx].phich_Resource = (PHICH_Resource_Type_E)((Context >> 18) & 0x3);
    Result[ue_cell_idx].SFN_High8bit = ((Context >> 10) & 0xFF);
    Result[ue_cell_idx].SFN_Low2bit = 3;
    Result[ue_cell_idx].SFN = (Result[ue_cell_idx].SFN_High8bit << 2) + Result[ue_cell_idx].SFN_Low2bit;
    if ((Result[ue_cell_idx].dl_BandWidth != Cell_Specific[ue_cell_idx].dl_BandWidth || Result[ue_cell_idx].phich_Duration != Cell_Specific[ue_cell_idx].phich_Duration || Result[ue_cell_idx].phich_Resource != Cell_Specific[ue_cell_idx].phich_Resource) && !Simuctrl.Inject_Flag) {
      Result[ue_cell_idx].PBCH_Flag = false;
    }
    sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("PBCH_DCC: dl_BandWidth=%d, phich_Duration=%d, phich_Resource=%d, SFN=%d,  CRSPortNum=%d\n",
              Result[ue_cell_idx].dl_BandWidth, Result[ue_cell_idx].phich_Duration, Result[ue_cell_idx].phich_Resource, Result[ue_cell_idx].SFN, CRSPortNum);
  } else {
    Result[ue_cell_idx].BLER_PBCH += 1;
  }
  Result[ue_cell_idx].BLER_PBCH_Num += 1;
  Result[ue_cell_idx].PBCH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PBCH / (float)Result[ue_cell_idx].BLER_PBCH_Num;
  sprintf(c_string, "%s/%s_%s_BLER_PBCH", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("%f\n", Result[ue_cell_idx].PBCH_BLER[loop]);
}

//! PCFICH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PCFICH_DCC(int ue_cell_idx) {
  bvec CFI_bits;
  int i = 0;
  int j = 0;
  float acc_value = 0.0f;
  float max_value = 0.0f;
  int CFI = 0;
  for (i = 0; i < 3; i++) {
    CFI_bits = CFI_code_word.row(i);
    acc_value = 0.0f;
    for (j = 0; j < 32; j++) {
      acc_value += ((float)(1 - 2 * CFI_bits(j)) * V_softbits(j));
    }
    if (max_value < acc_value) {
      max_value = acc_value;
      CFI = i + 1;
    }
  }
  if (Cell_Specific[ue_cell_idx].N_DL_RB > 10) {
    if (S == Cell_Specific[ue_cell_idx].SubframeType) {
      Result[ue_cell_idx].Ctrl_Symb_Num_Special = CFI;
    } else {
      Result[ue_cell_idx].Ctrl_Symb_Num = CFI;
    }
  } else {
    if (S == Cell_Specific[ue_cell_idx].SubframeType) {
      Result[ue_cell_idx].Ctrl_Symb_Num_Special = CFI + 1;
    } else {
      Result[ue_cell_idx].Ctrl_Symb_Num = CFI + 1;
    }
  }
  sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("PCFICH DCC: CFI = %d, Ctrl_Symb_Num = %d\n", CFI, (S == Cell_Specific[ue_cell_idx].SubframeType) ? Result[ue_cell_idx].Ctrl_Symb_Num_Special : Result[ue_cell_idx].Ctrl_Symb_Num);
  if ((S == Cell_Specific[ue_cell_idx].SubframeType && Result[ue_cell_idx].Ctrl_Symb_Num_Special != Cell_Specific[ue_cell_idx].Ctrl_Symb_Num_Special) || (S != Cell_Specific[ue_cell_idx].SubframeType && Result[ue_cell_idx].Ctrl_Symb_Num != Cell_Specific[ue_cell_idx].Ctrl_Symb_Num)) {
    Result[ue_cell_idx].BLER_PCFICH += 1;
    Result[ue_cell_idx].PCFICH_Flag = false;
  } else {
    Result[ue_cell_idx].PCFICH_Flag = true;
  }
  Result[ue_cell_idx].BLER_PCFICH_Num += 1;
  Result[ue_cell_idx].PCHICH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PCFICH / (float)Result[ue_cell_idx].BLER_PCFICH_Num;
  sprintf(c_string, "%s/%s_%s_BLER_PCFICH", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("%f\n", Result[ue_cell_idx].PCHICH_BLER[loop]);
  if (Simuctrl.Inject_Flag) {
    if (S == Cell_Specific[ue_cell_idx].SubframeType) {
      Cell_Specific[ue_cell_idx].Ctrl_Symb_Num_Special = Result[ue_cell_idx].Ctrl_Symb_Num_Special;
    } else {
      Cell_Specific[ue_cell_idx].Ctrl_Symb_Num = Result[ue_cell_idx].Ctrl_Symb_Num;
    }
  }
}

//! PHICH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PHICH_DCC(int ue_cell_idx) {
  int i = 0;
  int HI = 0;
  float HI_softbit = 0.0f;
  int PHICH_idx = UE_Specific[ue_cell_idx].PHICH_idx;
  for (i = 0; i < 3; i++) {
    HI_softbit += V_softbits(i);
  }
  HI = (HI_softbit > 0.0f) ? 0 : 1;
  sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("PHICH DCC: HI = %d\n", HI);
  if ((PHICH_ACK == UE_Specific[ue_cell_idx].PHICH_HARQ_Pattern[PHICH_idx][0] && 0 == HI) || (PHICH_NACK == UE_Specific[ue_cell_idx].PHICH_HARQ_Pattern[PHICH_idx][0] && 1 == HI)) {
    Result[ue_cell_idx].BLER_PHICH += 1;
  }
  Result[ue_cell_idx].BLER_PHICH_Num += 1;
  Result[ue_cell_idx].PHICH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PHICH / (float)Result[ue_cell_idx].BLER_PHICH_Num;
  sprintf(c_string, "%s/%s_%s_BLER_PHICH", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("%f\n", Result[ue_cell_idx].PHICH_BLER[loop]);
}

//! DCI解析
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \param DCI_bits ： DCI bit流，输入
  \return 无返回值
*/
void UE_C::DCI_Parse(int ue_cell_idx, const bvec &DCI_bits) {
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
  int DCI_idx = 0;
  vector<int> s;
  int N = 0;
  int CellID = 0;
  int r = 0;
  //36.213 table 7.1.6.1-1, RBG size
  if (Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
    P = 1;
  } else if (Cell_Specific[ue_cell_idx].N_DL_RB <= 26) {
    P = 2;
  } else if (Cell_Specific[ue_cell_idx].N_DL_RB <= 63) {
    P = 3;
  } else {
    P = 4;
  }
  DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  switch (UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
    case NULL_DCI:
      //Empty
      break;
    case DCI_Format_0:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += 1;
      bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_UL_RB * (Cell_Specific[ue_cell_idx].N_UL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUSCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].Cyclic_shift_OCC[DCI_idx]);
      if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, (0 == Cell_Specific[ue_cell_idx].ULDLConfiguration) ? &UE_Specific[ue_cell_idx].UL_index[DCI_idx] : &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CSI_request_num[DCI_idx], &UE_Specific[ue_cell_idx].CSI_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].Resource_allocation_type_num[DCI_idx], &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      break;
    case DCI_Format_1:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_1A:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += 1;
      if (UE_Specific[ue_cell_idx].Random_Access_Flag[DCI_idx]) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
        bit_counter += CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 6, &UE_Specific[ue_cell_idx].Preamble_Index[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 4, &UE_Specific[ue_cell_idx].PRACH_Mask_Index[DCI_idx]);
      } else {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
        if (0 == UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        } else {
          if (Cell_Specific[ue_cell_idx].N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || P_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
            bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
          } else {
            bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].gap_index[DCI_idx]);
            bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2) - 1, &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
          }
        }
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
        if (Cell_Specific[ue_cell_idx].N_DL_RB >= 50 && 1 == UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx] && (RA_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || P_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type)) {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].gap_index[DCI_idx]);
        } else {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
        }
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      }
      break;
    case DCI_Format_1B:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
      if (0 == UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
      } else {
        if (Cell_Specific[ue_cell_idx].N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || P_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        } else {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].gap_index[DCI_idx]);
          bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2) - 1, &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        }
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, Cell_Specific[ue_cell_idx].CRSPortNum, &UE_Specific[ue_cell_idx].TPMI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].PMI_Conf[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_1C:
      if (UE_Specific[ue_cell_idx].Very_Compact_Flag[DCI_idx]) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].gap_index[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2((UE_Specific[ue_cell_idx].N_DL_VRB_gap1[DCI_idx] / N_step_RB) * (UE_Specific[ue_cell_idx].N_DL_VRB_gap1[DCI_idx] / N_step_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        N_DL_VRB = Cell_Specific[ue_cell_idx].N_DL_RB;
        N_DL_VRB_prime = N_DL_VRB / N_step_RB;
        N_step_RB = (Cell_Specific[ue_cell_idx].N_DL_RB < 50) ? 2 : 4;
        ParseRIV(UE_Specific[ue_cell_idx].RIV[DCI_idx], N_DL_VRB_prime, &RB_start_prime, &L_CRBs_prime);
        L_CRBs = L_CRBs_prime * N_step_RB;
        RB_start = RB_start_prime * N_step_RB;
        UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i] = RB_start + i;
        }
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      } else {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 8, &UE_Specific[ue_cell_idx].Info_MCCH_Change[DCI_idx]);
      }
      break;
    case DCI_Format_1D:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]);
      if (0 == UE_Specific[ue_cell_idx].Locailized_Distributed_VRB_Flag[DCI_idx]) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
      } else {
        if (Cell_Specific[ue_cell_idx].N_DL_RB < 50 || RA_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || P_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || SI_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        } else {
          bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].gap_index[DCI_idx]);
          bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_DL_RB * (Cell_Specific[ue_cell_idx].N_DL_RB + 1) / 2) - 1, &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        }
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, Cell_Specific[ue_cell_idx].CRSPortNum, &UE_Specific[ue_cell_idx].TPMI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Downlink_power_offset[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_2:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].TB_swap_flag[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[1]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (2 == Cell_Specific[ue_cell_idx].CRSPortNum) ? 3 : 6, &UE_Specific[ue_cell_idx].Precoding_Info[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_2A:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].TB_swap_flag[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[1]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (2 == Cell_Specific[ue_cell_idx].CRSPortNum) ? 0 : 2, &UE_Specific[ue_cell_idx].Precoding_Info[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_2B:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Scrambling_ID[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[1]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_2C:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 3, &UE_Specific[ue_cell_idx].Antenna_Scrambling_Layers[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[1]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_2D:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      //36.213 7.1.6.1
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx] || Cell_Specific[ue_cell_idx].N_DL_RB <= 10) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P), &RBG_Mapping);
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
      } else {
        //36.213 7.1.6.2
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(P), &UE_Specific[ue_cell_idx].subset_p[DCI_idx]);
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]);
        N_TYPE1_RB = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)P) - CeilLog2(P) - 1;
        if (0 == UE_Specific[ue_cell_idx].resource_allocation_span[DCI_idx]) {
          delta_shift = 0;
        } else {
          if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] < (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + P;
          } else if (UE_Specific[ue_cell_idx].subset_p[DCI_idx] == (((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P) % P)) {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P + ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) % P) + 1;
          } else {
            N_RBG_subset_RB = ((Cell_Specific[ue_cell_idx].N_DL_RB - 1) / P / P) * P;
          }
          delta_shift = N_RBG_subset_RB - N_TYPE1_RB;
        }
        RBG_Mapping = 0;
        for (i = 0; i < N_TYPE1_RB; i++) {
          n_RBG_subset_PRB = ((i + delta_shift) / P) * P * P + UE_Specific[ue_cell_idx].subset_p[DCI_idx] * P + ((i + delta_shift) % P);
          if (1 == DCI_bits(bit_counter + i)) {
            RBG_Mapping |= (1 << (n_RBG_subset_PRB / P));
          }
        }
        ParseRBG(RBG_Mapping, UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx], Cell_Specific[ue_cell_idx].N_DL_RB, P);
        bit_counter += N_TYPE1_RB;
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUCCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 0 : 2, &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (FDD == Cell_Specific[ue_cell_idx].FrameStructureType) ? 3 : 4, &UE_Specific[ue_cell_idx].HARQ_ID);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 3, &UE_Specific[ue_cell_idx].Antenna_Scrambling_Layers[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[0]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].RV[1]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].PDSCH_RE_Mapping_QCL[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (!UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx]) ? 0 : 2, &UE_Specific[ue_cell_idx].HARQ_ACK_resource_offset[DCI_idx]);
      break;
    case DCI_Format_3:
      //TBD
      break;
    case DCI_Format_3A:
      //TBD
      break;
    case DCI_Format_4:
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, &CellID);
      if (0 == UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(Cell_Specific[ue_cell_idx].N_UL_RB * (Cell_Specific[ue_cell_idx].N_UL_RB + 1) / 2), &UE_Specific[ue_cell_idx].RIV[DCI_idx]);
        ParseRIV(UE_Specific[ue_cell_idx].RIV[DCI_idx], Cell_Specific[ue_cell_idx].N_UL_RB, &RB_start, &L_CRBs);
      } else {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, CeilLog2(extended_binomial_coef(ceil((float)Cell_Specific[ue_cell_idx].N_UL_RB / (float)P), 4)), &r);
        s.resize(4);
        Parse_combinatorial_idx(r, s);
        N = ceil((float)Cell_Specific[ue_cell_idx].N_UL_RB / (float)P) + 1;
        UE_Specific[ue_cell_idx].s0[DCI_idx] = N - s[0];
        UE_Specific[ue_cell_idx].s1[DCI_idx] = N - s[1];
        UE_Specific[ue_cell_idx].s2[DCI_idx] = N - s[2];
        UE_Specific[ue_cell_idx].s3[DCI_idx] = N - s[3];
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].TPC_for_PUSCH[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, &UE_Specific[ue_cell_idx].Cyclic_shift_OCC[DCI_idx]);
      if (TDD == Cell_Specific[ue_cell_idx].FrameStructureType) {
        bit_counter += GetFieldValue(DCI_bits, bit_counter, 2, (0 == Cell_Specific[ue_cell_idx].ULDLConfiguration) ? &UE_Specific[ue_cell_idx].UL_index[DCI_idx] : &UE_Specific[ue_cell_idx].DAI[DCI_idx]);
      }
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].CSI_request_num[DCI_idx], &UE_Specific[ue_cell_idx].CSI_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx], &UE_Specific[ue_cell_idx].SRS_request[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, UE_Specific[ue_cell_idx].Resource_allocation_type_num[DCI_idx], &UE_Specific[ue_cell_idx].Resource_allocation_type[DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[0][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 5, &UE_Specific[ue_cell_idx].MCS[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, 1, &UE_Specific[ue_cell_idx].NDI[1][DCI_idx]);
      bit_counter += GetFieldValue(DCI_bits, bit_counter, (2 == Cell_Specific[ue_cell_idx].CRSPortNum) ? 3 : 6, &UE_Specific[ue_cell_idx].Precoding_Info[DCI_idx]);
      break;
  }
}

//! PDCCH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::PDCCH_DCC(int ue_cell_idx) {
  int type_idx = 0;
  const int L[6] = {1, 2, 4, 8, 4, 8};
  const int M_L[6] = {6, 6, 2, 2, 4, 2};
  int Y = 0;
  int n_CCE = 0;
  int N_REG = 0;
  int l = 0;
  int k = 0;
  int i = 0;
  int m = 0;
  int m_prime = 0;
  int Payload_size = 0;
  int DL_DCI_Candidated_Num = 0;
  int cw_idx = 0;
  fvec e;
  fmat d;
  bvec c;
  bvec UL_c;
  bool DL_DCI_CRC = false;
  bool UL_DCI_CRC = false;
  DCI_Format_Type_E DCI_Format = NULL_DCI;
  DCI_Format_Type_E UL_DCI_Format = NULL_DCI;
  int DCI_Type_idx = 0;
  int Flag_for_Format0_1 = 0;
  const DCI_Format_Type_E *TM_DCI_Type = NULL;
  const DCI_Format_Type_E TM_DCI_Type_SI_P_RA_RNTI[2] = {DCI_Format_1C, DCI_Format_1A};  //Common
  const DCI_Format_Type_E TM_DCI_Type_C_RNTI[10][2] =
      {
          {DCI_Format_1A, DCI_Format_1},   //TM1: Common, UE Specific
          {DCI_Format_1A, DCI_Format_1},   //TM2: Common, UE Specific
          {DCI_Format_1A, DCI_Format_2A},  //TM3: Common, UE Specific
          {DCI_Format_1A, DCI_Format_2},   //TM4: Common, UE Specific
          {DCI_Format_1A, DCI_Format_1D},  //TM5: Common, UE Specific
          {DCI_Format_1A, DCI_Format_1B},  //TM6: Common, UE Specific
          {DCI_Format_1A, DCI_Format_1},   //TM7: Common, UE Specific
          {DCI_Format_1A, DCI_Format_2B},  //TM8: Common, UE Specific
          {DCI_Format_1A, DCI_Format_2C},  //TM9: Common, UE Specific
          {DCI_Format_1A, DCI_Format_2D}   //TM10: Common, UE Specific
      };
  const DCI_Format_Type_E TM_DCI_Type_Temp_C_RNTI[2] = {DCI_Format_1A, DCI_Format_1};  //Common, UE Specific
  int start_type_idx[2] = {0};
  int end_type_idx[2] = {0};
  int DCI_idx = 0;
  int N_PRB = 0;
  const string DCI_Type_Str[] =
      {
          "Format_0", "Format_1", "Format_1A", "Format_1B", "Format_1C", "Format_1D",
          "Format_2", "Format_2A", "Format_2B", "Format_2C", "Format_2D",
          "Format_3", "Format_3A",
          "Format_4"};
  int Ctrl_Symb_Num = (S == Cell_Specific[ue_cell_idx].SubframeType) ? Cell_Specific[ue_cell_idx].Ctrl_Symb_Num_Special : Cell_Specific[ue_cell_idx].Ctrl_Symb_Num;
  if (SI_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || P_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || RA_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
    TM_DCI_Type = TM_DCI_Type_SI_P_RA_RNTI;
    DL_DCI_Candidated_Num = 2;
    start_type_idx[0] = start_type_idx[1] = 4;
    end_type_idx[0] = end_type_idx[1] = 5;
  } else if (Temporary_C_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
    TM_DCI_Type = TM_DCI_Type_Temp_C_RNTI;
    DL_DCI_Candidated_Num = 2;
    start_type_idx[0] = start_type_idx[1] = 4;
    end_type_idx[0] = end_type_idx[1] = 5;
  } else if (C_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type || SPS_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type) {
    TM_DCI_Type = TM_DCI_Type_C_RNTI[(int)(UE_Specific[ue_cell_idx].TM - TM1)];
    DL_DCI_Candidated_Num = 2;
    if (SPS_RNTI_Type == UE_Specific[ue_cell_idx].RNTI_Type && (TM5 == UE_Specific[ue_cell_idx].TM || TM6 == UE_Specific[ue_cell_idx].TM)) {
      DL_DCI_Candidated_Num = 1;
    }
    start_type_idx[0] = 0;
    end_type_idx[0] = 5;
    start_type_idx[1] = 0;
    end_type_idx[1] = 3;
  }
  N_REG = 0;
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
  //DL DCI
  DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  for (DCI_Type_idx = 0; DCI_Type_idx < DL_DCI_Candidated_Num; DCI_Type_idx++) {
    for (type_idx = start_type_idx[DCI_Type_idx]; type_idx <= end_type_idx[DCI_Type_idx]; type_idx++) {
      if (type_idx > 3) {
        Y = 0;
      } else {
        Y = Calc_Yk(UE_Specific[ue_cell_idx].RNTI, Cell_Specific[ue_cell_idx].n_subframe);
      }
      e.resize(L[type_idx] * 72);
      for (m = 0; m < M_L[type_idx]; m++) {
        if (DL_DCI_CRC) {
          break;
        }
        m_prime = UE_Specific[ue_cell_idx].CA_Flag ? (m + M_L[type_idx] * Cell_Specific[ue_cell_idx].CellID) : m;
        for (i = 0; i < L[type_idx]; i++) {
          if ((N_REG / 9) < L[type_idx]) {
            continue;
          }
          n_CCE = L[type_idx] * ((Y + m_prime) % ((N_REG / 9) / L[type_idx])) + i;
          for (k = 0; k < 72; k++) {
            e(i * 72 + k) = V_softbits(n_CCE * 72 + k);
          }
        }
        Payload_size = Get_PDCCH_Payload_size(TM_DCI_Type[DCI_Type_idx],
                                              Cell_Specific[ue_cell_idx].FrameStructureType,
                                              Cell_Specific[ue_cell_idx].dl_BandWidth,
                                              UE_Specific[ue_cell_idx].CA_Flag,
                                              Cell_Specific[ue_cell_idx].CRSPortNum,
                                              UE_Specific[ue_cell_idx].CSI_request_num[DCI_idx],
                                              UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx],
                                              UE_Specific[ue_cell_idx].Resource_allocation_type_num[DCI_idx],
                                              false);
        DeRate_Matching_TBCC(e,
                             Payload_size + 16,
                             d);
        WAVA_Viterbi(d, c);
        for (k = (int)c.size() - 1, i = 0; k >= (int)c.size() - 16; k--, i++) {
          c(k) = c(k) ^ ((UE_Specific[ue_cell_idx].RNTI >> i) & 1);
        }
        if (UE_Specific[ue_cell_idx].UE_Transmit_Selection) {
          c((int)c.size() - 1) = c((int)c.size() - 1) ^ 1;
        }
        DL_DCI_CRC = CheckCRC(CRC16, c);
        if (DCI_Format_1A == TM_DCI_Type[DCI_Type_idx]) {
          GetFieldValue(c, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, 1, &Flag_for_Format0_1);
          if (0 == Flag_for_Format0_1) {
            DL_DCI_CRC = false;
          }
        }
        if (DL_DCI_CRC) {
          DCI_Format = TM_DCI_Type[DCI_Type_idx];
          sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          FilePrint("PDCCH_DCC: TM=%d, DL DCI=%s\n", (int)(UE_Specific[ue_cell_idx].TM - TM1 + 1), DCI_Type_Str[(int)(DCI_Format - DCI_Format_0)].c_str());
        }
      }
    }
  }
  //UL DCI
  if (NULL_DCI != UE_Specific[ue_cell_idx].UL_DCI_Format) {
    if (DCI_Format_4 == UE_Specific[ue_cell_idx].UL_DCI_Format) {
      UL_DCI_Format = DCI_Format_4;
      start_type_idx[0] = 0;
      end_type_idx[0] = 3;
    } else {
      UL_DCI_Format = DCI_Format_0;
      start_type_idx[0] = 0;
      end_type_idx[0] = 5;
    }
    for (type_idx = start_type_idx[0]; type_idx <= end_type_idx[0]; type_idx++) {
      if (type_idx > 3) {
        Y = 0;
      } else {
        Y = Calc_Yk(UE_Specific[ue_cell_idx].RNTI, Cell_Specific[ue_cell_idx].n_subframe);
      }
      e.resize(L[type_idx] * 72);
      for (m = 0; m < M_L[type_idx]; m++) {
        if (UL_DCI_CRC) {
          break;
        }
        m_prime = UE_Specific[ue_cell_idx].CA_Flag ? (m + M_L[type_idx] * Cell_Specific[ue_cell_idx].CellID) : m;
        for (i = 0; i < L[type_idx]; i++) {
          if ((N_REG / 9) < L[type_idx]) {
            continue;
          }
          n_CCE = L[type_idx] * ((Y + m_prime) % ((N_REG / 9) / L[type_idx])) + i;
          for (k = 0; k < 72; k++) {
            e(i * 72 + k) = V_softbits(n_CCE * 72 + k);
          }
        }
        Payload_size = Get_PDCCH_Payload_size(UL_DCI_Format,
                                              Cell_Specific[ue_cell_idx].FrameStructureType,
                                              Cell_Specific[ue_cell_idx].dl_BandWidth,
                                              UE_Specific[ue_cell_idx].CA_Flag,
                                              Cell_Specific[ue_cell_idx].CRSPortNum,
                                              UE_Specific[ue_cell_idx].CSI_request_num[DCI_idx],
                                              UE_Specific[ue_cell_idx].SRS_request_num[DCI_idx],
                                              UE_Specific[ue_cell_idx].Resource_allocation_type_num[DCI_idx],
                                              false);
        DeRate_Matching_TBCC(e,
                             Payload_size + 16,
                             d);
        WAVA_Viterbi(d, UL_c);
        for (k = (int)UL_c.size() - 1, i = 0; k >= (int)UL_c.size() - 16; k--, i++) {
          UL_c(k) = UL_c(k) ^ ((UE_Specific[ue_cell_idx].RNTI >> i) & 1);
        }
        if (UE_Specific[ue_cell_idx].UE_Transmit_Selection) {
          UL_c((int)UL_c.size() - 1) = UL_c((int)UL_c.size() - 1) ^ 1;
        }
        UL_DCI_CRC = CheckCRC(CRC16, UL_c);
        if (DCI_Format_0 == TM_DCI_Type[DCI_Type_idx]) {
          GetFieldValue(UL_c, UE_Specific[ue_cell_idx].CA_Flag ? 3 : 0, 1, &Flag_for_Format0_1);
          if (1 == Flag_for_Format0_1) {
            UL_DCI_CRC = false;
          }
        }
        if (UL_DCI_CRC) {
          sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          FilePrint("PDCCH_DCC: UL DCI=%s\n\n", DCI_Type_Str[(int)(UL_DCI_Format - DCI_Format_0)].c_str());
        }
      }
    }
  }
  if (NULL_DCI != UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
    if (!Result[ue_cell_idx].PCFICH_Flag && Simuctrl.PCFICH_DTFlag) {
      Result[ue_cell_idx].BLER_PCFICH_PDCCH += 1;
    } else if (!DL_DCI_CRC) {
      Result[ue_cell_idx].BLER_PDCCH += 1;
      Result[ue_cell_idx].BLER_PCFICH_PDCCH += 1;
    }
    Result[ue_cell_idx].BLER_PCFICH_PDCCH_Num += 1;
    Result[ue_cell_idx].PCHICH_PDCCH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PCFICH_PDCCH / (float)Result[ue_cell_idx].BLER_PCFICH_PDCCH_Num;
    sprintf(c_string, "%s/%s_%s_BLER_PCFICH_PDCCH", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("%f\n", Result[ue_cell_idx].PCHICH_PDCCH_BLER[loop]);
    if (!(!Result[ue_cell_idx].PCFICH_Flag && Simuctrl.PCFICH_DTFlag)) {
      Result[ue_cell_idx].BLER_PDCCH_Num += 1;
      Result[ue_cell_idx].PDCCH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PDCCH / (float)Result[ue_cell_idx].BLER_PDCCH_Num;
      sprintf(c_string, "%s/%s_%s_BLER_PDCCH", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("%f\n", Result[ue_cell_idx].PDCCH_BLER[loop]);
    }
  }
  if (Simuctrl.Inject_Flag) {
    if (DL_DCI_CRC) {
      UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx] = DCI_Format;
      UE_Specific[ue_cell_idx].DCI_On_EPDCCH[DCI_idx] = false;
      DCI_Parse(ue_cell_idx, c);
      ParasFromDCI(Simuctrl, Cell_Specific[ue_cell_idx], UE_Specific[ue_cell_idx], true);
      if (S == Cell_Specific[ue_cell_idx].SubframeType && ((SS_CFG9 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration && NormalCP == Cell_Specific[ue_cell_idx].CP_Type) || (SS_CFG7 == Cell_Specific[ue_cell_idx].SpecialSubframeConfiguration && ExtendedCP == Cell_Specific[ue_cell_idx].CP_Type))) {
        N_PRB = MAX(floor((float)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() * 0.375), 1);
      } else if (S == Cell_Specific[ue_cell_idx].SubframeType) {
        N_PRB = MAX(floor((float)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() * 0.75), 1);
      } else {
        N_PRB = UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size();
      }
      for (cw_idx = 0; cw_idx < UE_Specific[ue_cell_idx].codeword_num[DCI_idx]; cw_idx++) {
        UE_Specific[ue_cell_idx].TBS[cw_idx] = MCS_PRB_toTBS(UE_Specific[ue_cell_idx].MCS[cw_idx][DCI_idx], N_PRB, UE_Specific[ue_cell_idx].v[cw_idx]);
        UE_Specific[ue_cell_idx].Qm[cw_idx] = MCS_Table[UE_Specific[ue_cell_idx].MCS[cw_idx][DCI_idx]][0];
      }
    } else {
      UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx] = NULL_DCI;
      UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].resize(0);
      UE_Specific[ue_cell_idx].codeword_num[DCI_idx] = 0;
    }
  }
}

//! EPDCCH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \return 无返回值
*/
void UE_C::EPDCCH_DCC(int ue_cell_idx) {
}

//! PDSCH译码
/*!
  \param ue_cell_idx ： 该UE内的Cell序号，输入
  \param cw_idx ： 码字序号，输入
  \param G_prime ： G_prime，输入
  \return 无返回值
*/
void UE_C::PDSCH_DCC(int ue_cell_idx, int cw_idx, int G_prime) {
  int i = 0;
  int cb_idx = 0;
  int C = 0;
  ivec K_r;
  int E_r = 0;
  int gamma = 0;
  int E = 0;
  int N_L_1cw = 0;
  bool CRC = true;
  int CB_CRC_idx = 0;
  int DCI_idx = 0;
  int F = 0;
  fvec e;
  fmat d;
  bvecs c;
  bvec b;
  int CQI_idx = 0;
  float CQI_Th_BLER = 0.0f;
  DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  N_L_1cw = (Transmit_Diversity == UE_Specific[ue_cell_idx].Transmission_Scheme) ? 2 : UE_Specific[ue_cell_idx].v[cw_idx];
  if (ReTransmit[ue_cell_idx].LastNDI[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] != UE_Specific[ue_cell_idx].NDI[cw_idx][DCI_idx]) {
    C = Calc_CBSize(UE_Specific[ue_cell_idx].TBS[cw_idx] + 24, K_r, &F);
    ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID].resize(C);
    for (cb_idx = 0; cb_idx < C; cb_idx++) {
      ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID][cb_idx].resize(3, K_r[cb_idx] + 4);
      ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID][cb_idx].setZero();
    }
    ReTransmit[ue_cell_idx].LastNDI[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = UE_Specific[ue_cell_idx].NDI[cw_idx][DCI_idx];
    ReTransmit[ue_cell_idx].LastTBS[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = UE_Specific[ue_cell_idx].TBS[cw_idx];
    //ReTransmit[ue_cell_idx].LastQm[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = UE_Specific[ue_cell_idx].Qm[cw_idx];
  } else {
    UE_Specific[ue_cell_idx].TBS[cw_idx] = ReTransmit[ue_cell_idx].LastTBS[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID];
    //UE_Specific[ue_cell_idx].Qm[cw_idx] = ReTransmit[ue_cell_idx].LastQm[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID];
    C = Calc_CBSize(UE_Specific[ue_cell_idx].TBS[cw_idx] + 24, K_r, &F);
  }
  c.resize(C);
  for (cb_idx = 0; cb_idx < C; cb_idx++) {
    gamma = G_prime % C;
    if (cb_idx <= (C - gamma - 1)) {
      E = N_L_1cw * UE_Specific[ue_cell_idx].Qm[cw_idx] * floor((float)G_prime / (float)C);
    } else {
      E = N_L_1cw * UE_Specific[ue_cell_idx].Qm[cw_idx] * ceil((float)G_prime / (float)C);
    }
    e.resize(E);
    e = V_softbits.segment(E_r, E);
    E_r = E_r + E;
    DeRate_Matching_Turbo(e,
                          K_r[cb_idx] + 4,
                          UE_Specific[ue_cell_idx].N_IR,
                          true,
                          C,
                          UE_Specific[ue_cell_idx].RV[cw_idx],
                          (0 == cb_idx) ? F : 0,
                          d);
    ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID][cb_idx] =
        ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID][cb_idx] + d;
  }
  for (cb_idx = 0; cb_idx < C; cb_idx++) {
    if (!Turbo_Decoder(ReTransmit[ue_cell_idx].Turbo_bits_Buffer[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID][cb_idx], c[cb_idx], (C > 1) ? true : false)) {
      CRC = false;
      CB_CRC_idx = cb_idx;
      break;
    }
    //MyPrint("%d %d\n", cb_idx, CRC);
  }
  if (CRC) {
    DeCB_Segmentation_CB_CRC_Attachment(c, b, UE_Specific[ue_cell_idx].TBS[cw_idx] + 24);
    CRC = CheckCRC(CRC24A, b);
    if (!CRC) {
      CB_CRC_idx = C;
    }
  }
  //CRC = true;
  ReTransmit[ue_cell_idx].CRC[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = CRC ? 0 : 1;
  sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
  WithFile(c_string);
  FilePrint("PDSCH CW%d DCC: CQI = %d, MCS = %d, HARQ_ID = %d, NDI = %d, RV = %d\n", cw_idx, UE_Specific[ue_cell_idx].CQI[cw_idx][DCI_idx], UE_Specific[ue_cell_idx].MCS[cw_idx][DCI_idx],
            UE_Specific[ue_cell_idx].HARQ_ID, UE_Specific[ue_cell_idx].NDI[cw_idx][DCI_idx], UE_Specific[ue_cell_idx].RV[cw_idx]);
  FilePrint("  RB_Num = %d, RB_Assignment = ", (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size());
  for (i = 0; i < (int)UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size(); i++) {
    FilePrint("%d ", UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx][i]);
  }
  FilePrint("\n");
  FilePrint("  TBS = %d, Qm = %d, CB_Num = %d, CodeRate = %.3f\n", UE_Specific[ue_cell_idx].TBS[cw_idx], UE_Specific[ue_cell_idx].Qm[cw_idx],
            C, (float)(UE_Specific[ue_cell_idx].TBS[cw_idx] + 24 * (((1 == C) ? 0 : C) + 1)) / (float)(G_prime * UE_Specific[ue_cell_idx].Qm[cw_idx] * N_L_1cw));
  if (!CRC) {
    if (C == cb_idx || 1 == C) {
      FilePrint("  CRC = %d, TB CRC Fail\n", CRC ? 0 : 1);
    } else {
      FilePrint("  CRC = %d, CB[%d] CRC Fail\n", CRC ? 0 : 1, CB_CRC_idx);
    }
  } else {
    FilePrint("  CRC = %d\n", CRC ? 0 : 1);
  }
  //Result
  if (!CRC) {
    Result[ue_cell_idx].BLER_PDSCH_perCW[cw_idx] += 1;
    Result[ue_cell_idx].BLER_PDSCH += 1;
  } else {
    Result[ue_cell_idx].Throughput_perCW[cw_idx] += (float)UE_Specific[ue_cell_idx].TBS[cw_idx];
    Result[ue_cell_idx].Throughput += (float)UE_Specific[ue_cell_idx].TBS[cw_idx];
  }
  Result[ue_cell_idx].BLER_PDSCH_perCW_Num[cw_idx] += 1;
  Result[ue_cell_idx].BLER_PDSCH_Num += 1;
  if (UE_Specific[ue_cell_idx].codeword_num[DCI_idx] > 1) {
    sprintf(c_string, "%s/%s_%s_BLER%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), cw_idx);
    WithFile(c_string);
    FilePrint("%f\n", (float)Result[ue_cell_idx].BLER_PDSCH_perCW[cw_idx] / (float)Result[ue_cell_idx].BLER_PDSCH_perCW_Num[cw_idx]);
    sprintf(c_string, "%s/%s_%s_Throughput%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), cw_idx);
    WithFile(c_string);
    FilePrint("%f\n", (float)Result[ue_cell_idx].Throughput_perCW[cw_idx] * 0.001f / (float)(Simuctrl.n_subframe - Simuctrl.start_n_subframe + 1));
  }
  if (cw_idx == (UE_Specific[ue_cell_idx].codeword_num[DCI_idx] - 1)) {
    Result[ue_cell_idx].PDSCH_BLER[loop] = (float)Result[ue_cell_idx].BLER_PDSCH / (float)Result[ue_cell_idx].BLER_PDSCH_Num;
    sprintf(c_string, "%s/%s_%s_BLER", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("%f\n", Result[ue_cell_idx].PDSCH_BLER[loop]);
    Result[ue_cell_idx].PDSCH_Throughput[loop] = Result[ue_cell_idx].Throughput * 0.001f / (float)(Simuctrl.n_subframe - Simuctrl.start_n_subframe + 1);
    sprintf(c_string, "%s/%s_%s_Throughput", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    FilePrint("%f\n", Result[ue_cell_idx].PDSCH_Throughput[loop]);  //Unit Mbps
  }
  //CQI Th Adapt
  if (Simuctrl.CQI_Th_Adapt && Simuctrl.CQI_Feedback_Flag && Simuctrl.n_subframe > (Simuctrl.start_n_subframe + 20)) {
    CQI_idx = UE_Specific[ue_cell_idx].CQI[cw_idx][DCI_idx] - 1;
    if (!CRC) {
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx]++;
    }
    Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx]++;
    CQI_Th_BLER = (float)Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] / (float)Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx];
    if ((CQI_Th_BLER > 0.6f && Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 30) || (CQI_Th_BLER > 0.8f && Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 20)) {
      CQI_Th[CQI_idx] = CQI_Th[CQI_idx] * 1.259f;
      for (i = (CQI_idx + 1); i < 15; i++) {
        if (CQI_Th[i] <= CQI_Th[i - 1]) {
          CQI_Th[i] = CQI_Th[i - 1] * 1.412f;
          Feedback[ue_cell_idx].CQI_BLER_Acc[i] = 0;
          Feedback[ue_cell_idx].CQI_BLER_Counter[i] = 0;
        } else {
          break;
        }
      }
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] = 0;
    } else if (CQI_Th_BLER > 0.3f && Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 30) {
      CQI_Th[CQI_idx] = CQI_Th[CQI_idx] * 1.122f;
      for (i = (CQI_idx + 1); i < 15; i++) {
        if (CQI_Th[i] <= CQI_Th[i - 1]) {
          CQI_Th[i] = CQI_Th[i - 1] * 1.412f;
          Feedback[ue_cell_idx].CQI_BLER_Acc[i] = 0;
          Feedback[ue_cell_idx].CQI_BLER_Counter[i] = 0;
        } else {
          break;
        }
      }
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] = 0;
    } else if (CQI_Th_BLER > 0.15f && Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 50) {
      CQI_Th[CQI_idx] = CQI_Th[CQI_idx] * 1.059f;
      for (i = (CQI_idx + 1); i < 15; i++) {
        if (CQI_Th[i] <= CQI_Th[i - 1]) {
          CQI_Th[i] = CQI_Th[i - 1] * 1.412f;
          Feedback[ue_cell_idx].CQI_BLER_Acc[i] = 0;
          Feedback[ue_cell_idx].CQI_BLER_Counter[i] = 0;
        } else {
          break;
        }
      }
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] = 0;
    } else if (CQI_Th_BLER < 0.03f && CQI_idx < 14 && Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 50) {
      CQI_Th[CQI_idx + 1] = CQI_Th[CQI_idx + 1] * 0.977f;
      for (i = CQI_idx; i > 0; i--) {
        if (CQI_Th[i] >= CQI_Th[i + 1]) {
          CQI_Th[i] = CQI_Th[i + 1] * 0.707f;
          Feedback[ue_cell_idx].CQI_BLER_Acc[i] = 0;
          Feedback[ue_cell_idx].CQI_BLER_Counter[i] = 0;
        } else {
          break;
        }
      }
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] = 0;
    } else if (Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] > 200) {
      Feedback[ue_cell_idx].CQI_BLER_Acc[CQI_idx] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[CQI_idx] = 0;
    }
  }
}
