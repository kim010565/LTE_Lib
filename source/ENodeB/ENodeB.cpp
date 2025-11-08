/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file ENodeB.cpp
    \brief ENodeB实现文件
    内容 ：Cell_C类相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/ENodeB/ENodeB.h"

//! Cell_C类构造函数
/*!
  \return 无返回值
*/
Cell_C::Cell_C() {
}

//! Cell_C类析构函数
/*!
  \return 无返回值
*/
Cell_C::~Cell_C() {
}

//! Cell_C类初始化函数
/*!
  \return 无返回值
*/
void Cell_C::InitParas() {
  int cell_ue_idx = 0;
  int p = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int DCI_idx = 0;
  Cell_Specific.delta_f = 15000;
  Cell_Specific.N_FFT = Get_N_FFT(Cell_Specific.dl_BandWidth, Cell_Specific.delta_f);
  for (i = 0; i < MAX_SYMBNUM_TTI; i++) {
    Cell_Specific.N_CP_l[i] = Get_N_CP_l(Cell_Specific.dl_BandWidth, Cell_Specific.CP_Type, i, Cell_Specific.delta_f);
  }
  Cell_Specific.N_DL_RB = Get_N_DL_RB(Cell_Specific.dl_BandWidth);
  Cell_Specific.N_RB_sc = Get_N_RB_sc(Cell_Specific.CP_Type, Cell_Specific.delta_f);
  Cell_Specific.N_DL_symb = Get_N_DL_symb(Cell_Specific.CP_Type, Cell_Specific.delta_f);
  Cell_Specific.N_1_ID = Cell_Specific.N_cell_ID / 3;
  Cell_Specific.N_2_ID = Cell_Specific.N_cell_ID % 3;
  Cell_Specific.N_CP = (NormalCP == Cell_Specific.CP_Type) ? 1 : 0;
  Cell_Specific.M_DL_HARQ = (FDD == Cell_Specific.FrameStructureType) ? 8 : Max_HARQProc_Num[(int)Cell_Specific.ULDLConfiguration];
  Cell_Specific.HARQ_ProcNum = MIN(Cell_Specific.M_DL_HARQ, Cell_Specific.HARQ_ProcNum);
  Cell_Specific.N_UL_RB = Cell_Specific.N_DL_RB;
  Cell_Specific.MBSFN_Subframe_Flag = false;
  for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
    UE_Specific[cell_ue_idx].CA_Flag = false;
    UE_Specific[cell_ue_idx].HARQ_ID = -1;
    UE_Specific[cell_ue_idx].N_soft = N_Soft_Table[(int)UE_Specific[cell_ue_idx].UE_Category - 1];
    UE_Specific[cell_ue_idx].N_IR = Get_PDSCH_N_IR(UE_Specific[cell_ue_idx].N_soft, UE_Specific[cell_ue_idx].UE_Category, UE_Specific[cell_ue_idx].TM, Cell_Specific.M_DL_HARQ);
    if (TM3 == UE_Specific[cell_ue_idx].TM)  //from 3GPP 36.213 table7.2-1b
    {
      if (2 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetNum[1] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0x3);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[1].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[1]);
        k = 0;
        for (i = 0; i < 2; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[1](k++) = i;
          }
        }
      } else if (4 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(4);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(4);
        for (j = 1; j < 4; j++) {
          UE_Specific[cell_ue_idx].codebookSubsetNum[j] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0xF);
          UE_Specific[cell_ue_idx].codebookSubsetIdxs[j].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[j]);
          k = 0;
          for (i = 0; i < 4; i++) {
            if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
              UE_Specific[cell_ue_idx].codebookSubsetIdxs[j](k++) = i + 12;
            }
          }
        }
      }
    } else if (TM4 == UE_Specific[cell_ue_idx].TM) {
      if (2 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetNum[0] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0xF);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[0].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[0]);
        k = 0;
        for (i = 0; i < 4; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](k++) = i;
          }
        }
        UE_Specific[cell_ue_idx].codebookSubsetNum[1] = 1 + BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0x30);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[1].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[1]);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[1](0) = 0;
        k = 1;
        for (i = 0; i < 2; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> (i + 4)) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[1](k++) = i + 1;
          }
        }
      } else if (4 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(4);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(4);
        for (j = 0; j < 4; j++) {
          UE_Specific[cell_ue_idx].codebookSubsetNum[j] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[j] & 0xFFFF);
          UE_Specific[cell_ue_idx].codebookSubsetIdxs[j].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[j]);
          k = 0;
          for (i = 0; i < 16; i++) {
            if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[j] >> i) & 1)) {
              UE_Specific[cell_ue_idx].codebookSubsetIdxs[j](k++) = i;
            }
          }
        }
      }
    } else if (TM5 == UE_Specific[cell_ue_idx].TM || TM6 == UE_Specific[cell_ue_idx].TM) {
      if (2 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(1);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(1);
        UE_Specific[cell_ue_idx].codebookSubsetNum[0] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0xF);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[0].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[0]);
        k = 0;
        for (i = 0; i < 4; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](k++) = i;
          }
        }
      } else if (4 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(1);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(1);
        UE_Specific[cell_ue_idx].codebookSubsetNum[0] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0xFFFF);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[0].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[0]);
        k = 0;
        for (i = 0; i < 16; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](k++) = i;
          }
        }
      }
    } else if (TM7 == UE_Specific[cell_ue_idx].TM)  //TM7 only for test, not from 3GPP 36.213 table7.2-1b
    {
      UE_Specific[cell_ue_idx].codebookSubsetNum.resize(1);
      UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(1);
      UE_Specific[cell_ue_idx].codebookSubsetNum[0] = 4;
      UE_Specific[cell_ue_idx].codebookSubsetIdxs[0].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[0]);
      UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](0) = 0;
      UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](1) = 1;
      UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](2) = 2;
      UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](3) = 3;
    } else if (TM8 == UE_Specific[cell_ue_idx].TM) {
      if (2 == Cell_Specific.CRSPortNum) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetNum[0] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0xF);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[0].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[0]);
        k = 0;
        for (i = 0; i < 4; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> i) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[0](k++) = i;
          }
        }
        UE_Specific[cell_ue_idx].codebookSubsetNum[1] = 1 + BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] & 0x30);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[1].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[1]);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs[1](0) = 0;
        k = 1;
        for (i = 0; i < 2; i++) {
          if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[0] >> (i + 4)) & 1)) {
            UE_Specific[cell_ue_idx].codebookSubsetIdxs[1](k++) = i + 1;
          }
        }
      } else if (4 == Cell_Specific.CRSPortNum && !UE_Specific[cell_ue_idx].alternativeCodeBookEnabledFor4TX_r12) {
        UE_Specific[cell_ue_idx].codebookSubsetNum.resize(2);
        UE_Specific[cell_ue_idx].codebookSubsetIdxs.resize(2);
        for (j = 0; j < 2; j++) {
          UE_Specific[cell_ue_idx].codebookSubsetNum[j] = BitCount(UE_Specific[cell_ue_idx].codebookSubsetRestriction[j] & 0xFFFF);
          UE_Specific[cell_ue_idx].codebookSubsetIdxs[j].resize(UE_Specific[cell_ue_idx].codebookSubsetNum[j]);
          k = 0;
          for (i = 0; i < 16; i++) {
            if (1 == ((UE_Specific[cell_ue_idx].codebookSubsetRestriction[j] >> i) & 1)) {
              UE_Specific[cell_ue_idx].codebookSubsetIdxs[j](k++) = i;
            }
          }
        }
      } else if (4 == Cell_Specific.CRSPortNum) {
        //TBD
      }
    }
    if (-1 != UE_Specific[cell_ue_idx].I_CQI_PMI) {
      if (FDD == Cell_Specific.FrameStructureType) {
        if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 1) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 2;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 6) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 5;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 2;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 16) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 10;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 7;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 36) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 20;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 17;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 76) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 40;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 37;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 156) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 80;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 77;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 316) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 160;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 157;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 349) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 32;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 318;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 413) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 64;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 350;
        } else {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 128;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 414;
        }
      } else {
        if (UE_Specific[cell_ue_idx].I_CQI_PMI == 0) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 1;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 5) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 5;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 1;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 15) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 10;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 6;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 35) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 20;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 16;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 75) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 40;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 36;
        } else if (UE_Specific[cell_ue_idx].I_CQI_PMI <= 155) {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 80;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 76;
        } else {
          UE_Specific[cell_ue_idx].N_pd_CQI_PMI = 160;
          UE_Specific[cell_ue_idx].N_offset_CQI_PMI = UE_Specific[cell_ue_idx].I_CQI_PMI - 156;
        }
      }
    }
    if (-1 != UE_Specific[cell_ue_idx].I_RI) {
      if (UE_Specific[cell_ue_idx].I_RI <= 160) {
        UE_Specific[cell_ue_idx].N_pd_RI = UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - UE_Specific[cell_ue_idx].I_RI;
      } else if (UE_Specific[cell_ue_idx].I_RI <= 321) {
        UE_Specific[cell_ue_idx].N_pd_RI = 2 * UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - (UE_Specific[cell_ue_idx].I_RI - 161);
      } else if (UE_Specific[cell_ue_idx].I_RI <= 482) {
        UE_Specific[cell_ue_idx].N_pd_RI = 4 * UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - (UE_Specific[cell_ue_idx].I_RI - 322);
      } else if (UE_Specific[cell_ue_idx].I_RI <= 643) {
        UE_Specific[cell_ue_idx].N_pd_RI = 8 * UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - (UE_Specific[cell_ue_idx].I_RI - 483);
      } else if (UE_Specific[cell_ue_idx].I_RI <= 804) {
        UE_Specific[cell_ue_idx].N_pd_RI = 16 * UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - (UE_Specific[cell_ue_idx].I_RI - 644);
      } else {
        UE_Specific[cell_ue_idx].N_pd_RI = 32 * UE_Specific[cell_ue_idx].N_pd_CQI_PMI;
        UE_Specific[cell_ue_idx].N_offset_RI = UE_Specific[cell_ue_idx].N_offset_CQI_PMI - (UE_Specific[cell_ue_idx].I_RI - 805);
      }
    }
    for (i = 0; i < MAXHARQNUM; i++) {
      for (j = 0; j < MAX_TB_SIZE; j++) {
        ReTransmit[cell_ue_idx].TxNum[j][i] = Cell_Specific.MaxTxNum - 1;
        ReTransmit[cell_ue_idx].LastNDI[j][i] = 0;
      }
    }
    Feedback[cell_ue_idx].Perfered_M = 1;
    if ((TM7 == UE_Specific[cell_ue_idx].TM || TM8 == UE_Specific[cell_ue_idx].TM) && Simuctrl.Random_BF_Flag) {
      Feedback[cell_ue_idx].k_Subband = 1;
    } else if (UE_Specific[cell_ue_idx].ReportOnPUSCH && (RM20 == UE_Specific[cell_ue_idx].ReportMode || RM22 == UE_Specific[cell_ue_idx].ReportMode)) {
      Get_Perfered_M_SubbandSize(Cell_Specific.N_DL_RB, &Feedback[cell_ue_idx].k_Subband, &Feedback[cell_ue_idx].Perfered_M);
    } else if (UE_Specific[cell_ue_idx].ReportOnPUSCH) {
      Feedback[cell_ue_idx].k_Subband = Get_SubbandSize(Cell_Specific.N_DL_RB);
    } else {
      Feedback[cell_ue_idx].k_Subband = Get_SubbandSize_PUCCH(Cell_Specific.N_DL_RB, &Feedback[cell_ue_idx].J);
      Feedback[cell_ue_idx].PUCCH_Last_j = 0;
      Feedback[cell_ue_idx].N_j = ceil((float)Cell_Specific.N_DL_RB / (float)Feedback[cell_ue_idx].k_Subband / (float)Feedback[cell_ue_idx].J);
    }
    Feedback[cell_ue_idx].Perfered_M_idx.resize(MAX_TB_SIZE, Feedback[cell_ue_idx].Perfered_M);
    Feedback[cell_ue_idx].N_Subband = ceil((float)Cell_Specific.N_DL_RB / (float)Feedback[cell_ue_idx].k_Subband);
    Feedback[cell_ue_idx].k_LastSubband = Cell_Specific.N_DL_RB - Feedback[cell_ue_idx].k_Subband * (Feedback[cell_ue_idx].N_Subband - 1);
    Feedback[cell_ue_idx].Subband_PMI.resize(MIN(MAXCSIRSPORTNUM, UE_Specific[cell_ue_idx].RxNum), Feedback[cell_ue_idx].N_Subband);
    Feedback[cell_ue_idx].Subband_PMI.setZero();
    for (i = 0; i < MAX_TB_SIZE; i++) {
      Feedback[cell_ue_idx].SubbandCQI[i].resize(MAX_TB_SIZE, Feedback[cell_ue_idx].N_Subband);
      Feedback[cell_ue_idx].SubbandCQI[i].setZero();
      for (j = 0; j < MAX_TB_SIZE; j++) {
        Feedback[cell_ue_idx].WidebandCQI[i][j] = 1;
        for (k = 0; k < Feedback[cell_ue_idx].N_Subband; k++) {
          Feedback[cell_ue_idx].SubbandCQI[i](j, k) = 1;
        }
      }
    }
    for (i = 0; i < MAX_Feedback_BufferSize; i++) {
      //RI
      Feedback[cell_ue_idx].RI_Report_Flag[i] = 0;
      Feedback[cell_ue_idx].RI_Report_n_subframe[i] = 0;
      Feedback[cell_ue_idx].RI_Report_Buffer[i] = 0;
      //PMI
      Feedback[cell_ue_idx].PMI_Report_Flag[i] = 0;
      Feedback[cell_ue_idx].PMI_Report_n_subframe[i] = 0;
      Feedback[cell_ue_idx].PMI_Report_Buffer[i].resize(MIN(Cell_Specific.CRSPortNum, UE_Specific[cell_ue_idx].RxNum), Feedback[cell_ue_idx].N_Subband);
      Feedback[cell_ue_idx].PMI_Report_Buffer[i].setZero();
      //CQI
      Feedback[cell_ue_idx].WidebandCQI_Report_Flag[i] = 0;
      Feedback[cell_ue_idx].WidebandCQI_Report_n_subframe[i] = 0;
      for (j = 0; j < MAX_TB_SIZE; j++) {
        Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[j][i] = 1;
        for (k = 0; k < MAX_N_Subband; k++) {
          Feedback[cell_ue_idx].SubbandCQI_Report_Buffer[j][k][i] = 1;
        }
      }
    }
    Feedback[cell_ue_idx].RI_Report_Buffer_idx = 0;
    Feedback[cell_ue_idx].PMI_Report_Buffer_idx = 0;
    Feedback[cell_ue_idx].WidebandCQI_Report_Buffer_idx = 0;
    Feedback[cell_ue_idx].RI = Simuctrl.Fixed_RI;
    Feedback[cell_ue_idx].RI_Last_Report = Simuctrl.Fixed_RI;
    for (DCI_idx = 0; DCI_idx < UE_Specific[cell_ue_idx].DCI_Period; DCI_idx++) {
      if (DCI_Format_2 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
        UE_Specific[cell_ue_idx].N_L[DCI_idx] = Feedback[cell_ue_idx].RI + 1;
        if (1 == UE_Specific[cell_ue_idx].N_L[DCI_idx]) {
          UE_Specific[cell_ue_idx].codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific[cell_ue_idx].codeword_num[DCI_idx] = 2;
        }
      }
    }
    for (i = 0; i < MAX_TB_SIZE; i++) {
      for (j = 0; j < 15; j++) {
        Feedback[cell_ue_idx].CQI_Num[i][j] = 0;
      }
      for (j = 0; j < MAX_N_Subband; j++) {
        Feedback[cell_ue_idx].SubbandCQI_Offset_Num[i][j][0] = 0;
        Feedback[cell_ue_idx].SubbandCQI_Offset_Num[i][j][1] = 0;
        Feedback[cell_ue_idx].SubbandCQI_Offset_Num[i][j][2] = 0;
        Feedback[cell_ue_idx].SubbandCQI_Offset_Num[i][j][3] = 0;
      }
    }
  }
  PBCH_Symbs.resize(Cell_Specific.CRSPortNum, (NormalCP == Cell_Specific.CP_Type) ? 960 : 864);
  PBCH_Symbs.setZero();
  RE.resize(MAX_PHY_ANTENNA_NUM);
  for (p = 0; p < MAX_PHY_ANTENNA_NUM; p++) {
    RE[p].resize(Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc, 2 * Cell_Specific.N_DL_symb);
    RE[p].setZero();
  }
  RE_Type.resize(Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc, 2 * Cell_Specific.N_DL_symb);
  RE_Type.setConstant(EMPTY_RE);
#if IDEAL_H_METHOD > 0
  Ideal_h.resize(MAX_PHY_ANTENNA_NUM, MAX_SYMBNUM_TTI);
  IdealSamplePos.resize(2 * Cell_Specific.N_DL_symb);
#endif
  Phase = 0.0;
  last_n_subframe = Simuctrl.start_n_subframe - 1;
}

//! 由于调度等影响，每个TTI需要做参数更新
/*!
  \param n_subframe ： 子帧序号，输入
  \return 无返回值
*/
void Cell_C::UpdataParas(int n_subframe) {
  int cell_ue_idx = 0;
  int p = 0;
  Simuctrl.n_subframe = n_subframe;
  Cell_Specific.n_subframe = Simuctrl.n_subframe % 10;
  Cell_Specific.n_f = (Cell_Specific.SFN_High8bit * 4 + Cell_Specific.SFN_Low2bit + n_subframe / 10) % 1024;
  Cell_Specific.SubframeType = Get_SubframeType(Cell_Specific.n_subframe, Cell_Specific.ULDLConfiguration, Cell_Specific.FrameStructureType);
  if (Simuctrl.PSS_SSS_DTFlag) {
    Cell_Specific.DL_Symb_Num = 2 * Cell_Specific.N_DL_symb;
    Cell_Specific.OverSample_DataLen = Cell_Specific.OverSample_Num;
  } else if (S == Cell_Specific.SubframeType) {
    Cell_Specific.DL_Symb_Num = DwPTS_SymbNum[(int)Cell_Specific.CP_Type][(int)Cell_Specific.SpecialSubframeConfiguration];
    Cell_Specific.OverSample_DataLen = Cell_Specific.OverSample_Num * DwPTS_Ts[(int)Cell_Specific.CP_Type][(int)Cell_Specific.SpecialSubframeConfiguration] / 30720;
  } else if (U != Cell_Specific.SubframeType) {
    Cell_Specific.DL_Symb_Num = 2 * Cell_Specific.N_DL_symb;
    Cell_Specific.OverSample_DataLen = Cell_Specific.OverSample_Num;
  } else {
    Cell_Specific.DL_Symb_Num = 0;
    Cell_Specific.OverSample_DataLen = 0;
  }
  for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
    UE_Specific[cell_ue_idx].DCI_idx = n_subframe % UE_Specific[cell_ue_idx].DCI_Period;
    UE_Specific[cell_ue_idx].PHICH_idx = n_subframe % UE_Specific[cell_ue_idx].PHICH_Period;
    //Feedback
    //TBD
  }
  for (p = 0; p < MAX_PHY_ANTENNA_NUM; p++) {
    RE[p].setZero();
  }
  RE_Type.setConstant(EMPTY_RE);
}

//! TM7、8、9、10 Beamforming，出自 ： 3GPP 36.101 附录B.4
/*!
  \param cell_ue_idx ： 当前Cell内的UE序号，输入
  \return 无返回值
*/
void Cell_C::RandomBeamforming(int cell_ue_idx) {
  int subband_idx = 0;
  int DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
  int random_idx = 0;
  if (TM4 == UE_Specific[cell_ue_idx].TM || TM5 == UE_Specific[cell_ue_idx].TM || TM6 == UE_Specific[cell_ue_idx].TM) {
    if (!UE_Specific[cell_ue_idx].ReportOnPUSCH || (UE_Specific[cell_ue_idx].ReportOnPUSCH && (RM31 == UE_Specific[cell_ue_idx].ReportMode || RM22 == UE_Specific[cell_ue_idx].ReportMode))) {
      if (2 == Cell_Specific.CRSPortNum) {
        random_idx = (0 == Feedback[cell_ue_idx].RI) ? (rand() & 3) : (rand() % 3);
      } else {
        random_idx = rand() & 15;
      }
      for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
        Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) = random_idx;
      }
    } else {
      for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
        if (2 == Cell_Specific.CRSPortNum) {
          random_idx = (0 == Feedback[cell_ue_idx].RI) ? (rand() & 3) : (rand() % 3);
        } else {
          random_idx = rand() & 15;
        }
        Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) = random_idx;
      }
    }
  } else if ((TM7 == UE_Specific[cell_ue_idx].TM && DCI_Format_1 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (1 == UE_Specific[cell_ue_idx].N_L[DCI_idx] && ((TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])))) {
    for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
      Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) = rand() & 3;
      if (Simuctrl.With_Simultaneous_Transmission && 1 == cell_ue_idx)  //With_Simultaneous_Transmission下，假设0是另一个UE，1是当前干扰UE
      {
        if (Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) == Feedback[cell_ue_idx - 1].Subband_PMI(Feedback[cell_ue_idx - 1].RI, subband_idx)) {
          Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) = (Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) + 1) & 3;
        }
      }
    }
  } else if (TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
    for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
      Feedback[cell_ue_idx].Subband_PMI(Feedback[cell_ue_idx].RI, subband_idx) = rand() % 3;
    }
  } else if ((TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
    //TBD
  }
}

//! 基带信号产生、EVM、发端天线相关、发端频偏。出自 ： 3GPP 36.211 6.12节
/*!
  \return 无返回值
*/
void Cell_C::OFDM_BB_Signal_Gen() {
  int p = 0;
  int l = 0;
  int s = 0;
  int Acc_Sample = 0;
  int l_max = 0;
  float EVM_sigma = 0;
  Eigen::FFT<float> fft;
  cfvec fft_in(Cell_Specific.OverSample_N_FFT);
  cfvec fft_out(Cell_Specific.OverSample_N_FFT);
  float power_scale = 0.0f;
  int cell_ue_idx = 0;
  int DCI_idx = 0;
  cpxf w = 0.0f;
  int NZP_idx = 0;
  int ZP_idx = 0;
  if (U != Cell_Specific.SubframeType) {
    power_scale = MySqrt((float)Cell_Specific.OverSample_N_FFT);
    fft_in.resize(Cell_Specific.OverSample_N_FFT);
    fft_out.resize(Cell_Specific.OverSample_N_FFT);
    fft_in.setZero();
    l_max = Cell_Specific.DL_Symb_Num;
    //Calc TxNum
    Cell_Specific.TxNum = Cell_Specific.CRSPortNum;
    for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
      //DMRS
      if (TM7 == UE_Specific[cell_ue_idx].TM && DCI_Format_1 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
        Cell_Specific.TxNum = MAX(Cell_Specific.TxNum, 2);
      } else if ((TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
        Cell_Specific.TxNum = MAX(Cell_Specific.TxNum, MAX(2, UE_Specific[cell_ue_idx].N_L[DCI_idx]));
      }
      //EPDCH RS
      if (UE_Specific[cell_ue_idx].EPDCCH_Set_Num > 0) {
        if (UE_Specific[cell_ue_idx].RB_Assignment_EPDCCH[0].size() > 0) {
          Cell_Specific.TxNum = MAX(Cell_Specific.TxNum, (NormalCP == Cell_Specific.CP_Type) ? 4 : 2);
        }
      }
      //CSI RS
      if (UE_Specific[cell_ue_idx].NZP_CSI_Set_Num > 0 || UE_Specific[cell_ue_idx].ZP_CSI_Set_Num > 0) {
        for (NZP_idx = 0; NZP_idx < UE_Specific[cell_ue_idx].NZP_CSI_Set_Num; NZP_idx++) {
          if (UE_Specific[cell_ue_idx].CSI_Flag_NZP[NZP_idx]) {
            Cell_Specific.TxNum = MAX(Cell_Specific.TxNum, UE_Specific[cell_ue_idx].CSIPortNum_NZP[NZP_idx]);
          }
        }
        for (ZP_idx = 0; ZP_idx < UE_Specific[cell_ue_idx].ZP_CSI_Set_Num; ZP_idx++) {
          if (UE_Specific[cell_ue_idx].CSI_Flag_ZP[ZP_idx]) {
            Cell_Specific.TxNum = MAX(Cell_Specific.TxNum, UE_Specific[cell_ue_idx].CSIPortNum_ZP[ZP_idx]);
          }
        }
      }
    }
    //OFDM
    for (p = 0; p < Cell_Specific.TxNum; p++) {
      Acc_Sample = 0;
      for (l = 0; l < l_max; l++) {
        fft_in.segment(1, Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2) = RE[p].col(l).segment(Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2, Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2);
        fft_in.segment(Cell_Specific.OverSample_N_FFT - Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2, Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2) = RE[p].col(l).segment(0, Cell_Specific.N_DL_RB * Cell_Specific.N_RB_sc / 2);
        fft.inv(fft_out, fft_in);
        fft_out *= power_scale;
        BB.row(p).segment(Acc_Sample, Cell_Specific.OverSample_N_CP_l[l]) = fft_out.segment((Cell_Specific.OverSample_N_FFT - Cell_Specific.OverSample_N_CP_l[l]), Cell_Specific.OverSample_N_CP_l[l]);
        Acc_Sample += Cell_Specific.OverSample_N_CP_l[l];
        BB.row(p).segment(Acc_Sample, Cell_Specific.OverSample_N_FFT) = fft_out;
        Acc_Sample += Cell_Specific.OverSample_N_FFT;
      }
    }
    //EVM
    if (Cell_Specific.EVM > 0.0f) {
      EVM_sigma = Cell_Specific.EVM * Cell_Specific.EVM / (float)MySqrt(Cell_Specific.TxNum) * SQRT_2_RECIP;
      for (s = 0; s < Cell_Specific.OverSample_DataLen; s++) {
        for (p = 0; p < Cell_Specific.TxNum; p++) {
          BB(p, s) += gauss_cpx(0.0f, EVM_sigma, &seed);
        }
      }
    }
    //TxFreqOffset
    if (0 != Cell_Specific.TxFreqOffset) {
      Phase += (double)Cell_Specific.TxFreqOffset * (double)(Simuctrl.n_subframe - last_n_subframe - 1) * 0.001;
#if IDEAL_H_METHOD > 0
      for (l = 0; l < l_max; l++) {
        w = MyExpCpx_2PI((float)(Phase + (double)Cell_Specific.TxFreqOffset * (double)IdealSamplePos[l] * Cell_Specific.OverSample_unit));
        for (p = 0; p < Cell_Specific.TxNum; p++) {
          Ideal_h(p, l) = w;
        }
      }
#endif
      for (s = 0; s < Cell_Specific.OverSample_DataLen; s++) {
        Phase += (double)Cell_Specific.TxFreqOffset * Cell_Specific.OverSample_unit;
        w = MyExpCpx_2PI((float)Phase);
        for (p = 0; p < Cell_Specific.TxNum; p++) {
          BB(p, s) *= w;
        }
      }
      Phase += (double)(Cell_Specific.OverSample_Num - Cell_Specific.OverSample_DataLen) * Cell_Specific.OverSample_unit;
      last_n_subframe = Simuctrl.n_subframe;
    } else {
#if IDEAL_H_METHOD > 0
      for (l = 0; l < l_max; l++) {
        for (p = 0; p < Cell_Specific.TxNum; p++) {
          Ideal_h(p, l) = 1.0f;
        }
      }
#endif
    }
    //Tx Corr
    if (MidCorr == Cell_Specific.ChanCorrInd) {
      if (2 == Cell_Specific.TxNum) {
        BB.block(0, 0, 2, Cell_Specific.OverSample_DataLen) = Corr_MID_2.transpose() * BB.block(0, 0, 2, Cell_Specific.OverSample_DataLen);
      } else if (4 == Cell_Specific.TxNum) {
        BB.block(0, 0, 4, Cell_Specific.OverSample_DataLen) = Corr_MID_4.transpose() * BB.block(0, 0, 4, Cell_Specific.OverSample_DataLen);
      }
#if IDEAL_H_METHOD > 0
      if (2 == Cell_Specific.TxNum) {
        Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max) = Corr_MID_2.transpose() * Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max);
      } else if (4 == Cell_Specific.TxNum) {
        Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max) = Corr_MID_4.transpose() * Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max);
      }
#endif
    } else if (HighCorr == Cell_Specific.ChanCorrInd) {
      if (2 == Cell_Specific.TxNum) {
        BB.block(0, 0, 2, Cell_Specific.OverSample_DataLen) = Corr_HIGH_2.transpose() * BB.block(0, 0, 2, Cell_Specific.OverSample_DataLen);
      } else if (4 == Cell_Specific.TxNum) {
        BB.block(0, 0, 4, Cell_Specific.OverSample_DataLen) = Corr_HIGH_4.transpose() * BB.block(0, 0, 4, Cell_Specific.OverSample_DataLen);
      }
#if IDEAL_H_METHOD > 0
      if (2 == Cell_Specific.TxNum) {
        Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max) = Corr_HIGH_2.transpose() * Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max);
      } else if (4 == Cell_Specific.TxNum) {
        Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max) = Corr_HIGH_4.transpose() * Ideal_h.block(0, 0, Cell_Specific.TxNum, l_max);
      }
#endif
    }
  }
#if IDEAL_H_METHOD > 0
  IdealSamplePosNum = l_max;
#endif
}

//! Cell发射处理过程
/*!
  \return 无返回值
*/
void Cell_C::Process() {
  int cell_ue_idx = 0;
  int DCI_idx = 0;
  PSS_Gen();
  SSS_Gen();
  CRS_Gen();
  PBCH_Gen();
  PCFICH_Gen();
  PHICH_Gen();
  for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
    DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
    FeedBack(Simuctrl, Cell_Specific, UE_Specific[cell_ue_idx], Feedback[cell_ue_idx]);
    ParasFromDCI(Simuctrl, Cell_Specific, UE_Specific[cell_ue_idx], false);
    if (Simuctrl.Random_BF_Flag) {
      RandomBeamforming(cell_ue_idx);
    }
    if (TM7 == UE_Specific[cell_ue_idx].TM && DCI_Format_1 == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) {
      DMRS_Port5_Gen(cell_ue_idx);
    } else if ((TM8 == UE_Specific[cell_ue_idx].TM && DCI_Format_2B == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[cell_ue_idx].TM && DCI_Format_2C == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[cell_ue_idx].TM && DCI_Format_2D == UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
      DMRS_Port7to14_Gen(cell_ue_idx);
    }
    if (UE_Specific[cell_ue_idx].EPDCCH_Set_Num > 0) {
      ParasFromEPDCCH(Simuctrl, Cell_Specific, UE_Specific[cell_ue_idx]);
      EPDCCH_DMRS_Gen(cell_ue_idx);
      EPDCCH_DMP(cell_ue_idx);
    }
    if (UE_Specific[cell_ue_idx].NZP_CSI_Set_Num > 0 || UE_Specific[cell_ue_idx].ZP_CSI_Set_Num > 0) {
      CSI_RS_Gen(cell_ue_idx);
    }
    UE_Specific[cell_ue_idx].G_symb = Calc_PDSCH_G_symb(cell_ue_idx);
    CQI_to_MCS_Qm(Cell_Specific, UE_Specific[cell_ue_idx]);
    ParseReTransmit(Cell_Specific, UE_Specific[cell_ue_idx], ReTransmit[cell_ue_idx]);
  }
  PDCCH_Gen();
  for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
    DCI_idx = UE_Specific[cell_ue_idx].DCI_idx;
    if (UE_Specific[cell_ue_idx].EPDCCH_Set_Num > 0) {
      EPDCCH_Gen(cell_ue_idx);
    }
    if (Simuctrl.PDSCH_DTFlag && (NULL_DCI != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] && DCI_Format_0 != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx] && DCI_Format_4 != UE_Specific[cell_ue_idx].DCI_Format_Type[DCI_idx])) {
      PDSCH_Gen(cell_ue_idx);
    }
  }
  OFDM_BB_Signal_Gen();
}

void Cell_C::GetMedianCQI(int cell_idx) {
  int cell_ue_idx = 0;
  int CQI_idx = 0;
  int Sum_CQI_Num = 0;
  int Acc_CQI_Num = 0;
  int Median_CQI = 0;
  int cw_idx = 0;
  int subband_idx = 0;
  if (Simuctrl.CQI_Feedback_Flag) {
    for (cell_ue_idx = 0; cell_ue_idx < (int)UE_Specific.size(); cell_ue_idx++) {
      WithFile("Log");
      FilePrint("cell_idx=%d, cell_ue_idx=%d, loop=%d\n", cell_idx, cell_ue_idx, loop);
      for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
        Sum_CQI_Num = 0;
        for (CQI_idx = 0; CQI_idx < 15; CQI_idx++) {
          if (Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx] > 0) {
            Sum_CQI_Num += Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx];
          }
        }
        Acc_CQI_Num = 0;
        for (CQI_idx = 0; CQI_idx < 15; CQI_idx++) {
          if (Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx] > 0) {
            Acc_CQI_Num += Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx];
            if ((2 * Acc_CQI_Num) >= Sum_CQI_Num) {
              Median_CQI = CQI_idx;
              break;
            }
          }
        }
        Acc_CQI_Num = Feedback[cell_ue_idx].CQI_Num[cw_idx][Median_CQI];
        if (Median_CQI > 0) {
          Acc_CQI_Num += Feedback[cell_ue_idx].CQI_Num[cw_idx][Median_CQI - 1];
        }
        if (Median_CQI < 14) {
          Acc_CQI_Num += Feedback[cell_ue_idx].CQI_Num[cw_idx][Median_CQI + 1];
        }
        if (Sum_CQI_Num > 0) {
          Feedback[cell_ue_idx].Median_CQI[cw_idx] = Median_CQI;
          Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] = 0;
          Feedback[cell_ue_idx].Median_CQI_Rate[cw_idx] = (float)Acc_CQI_Num / (float)Sum_CQI_Num;
          for (CQI_idx = 0; CQI_idx < 15; CQI_idx++) {
            if (Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx] > 0) {
              FilePrint("  cw_idx = %d, CQI_Rate[%d] = %f\n", cw_idx, CQI_idx + 1, (float)Feedback[cell_ue_idx].CQI_Num[cw_idx][CQI_idx] / (float)Sum_CQI_Num);
            }
          }
        }
        if (Ref_Freq_Select_CQI_Reported == UE_Specific[cell_ue_idx].Ref_Type && UE_Specific[cell_ue_idx].ReportOnPUSCH && (RM30 == UE_Specific[cell_ue_idx].ReportMode || RM31 == UE_Specific[cell_ue_idx].ReportMode || RM32 == UE_Specific[cell_ue_idx].ReportMode)) {
          for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
            Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx] = (float)Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][0] / (float)(Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][0] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][1] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][2] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][3]);
          }
        } else if (Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific[cell_ue_idx].Ref_Type && UE_Specific[cell_ue_idx].ReportOnPUSCH && (RM30 == UE_Specific[cell_ue_idx].ReportMode || RM31 == UE_Specific[cell_ue_idx].ReportMode || RM32 == UE_Specific[cell_ue_idx].ReportMode)) {
          for (subband_idx = 0; subband_idx < Feedback[cell_ue_idx].N_Subband; subband_idx++) {
            Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx] = (float)Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][2] / (float)(Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][0] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][1] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][2] + Feedback[cell_ue_idx].SubbandCQI_Offset_Num[cw_idx][subband_idx][3]);
          }
        }
      }
    }
  }
}
