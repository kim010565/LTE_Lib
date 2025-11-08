/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE.cpp
    \brief UE实现文件
    内容 ：UE_C类相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! UE_C类构造函数
/*!
  \return 无返回值
*/
UE_C::UE_C() {
}

//! UE_C类析构函数
/*!
  \return 无返回值
*/
UE_C::~UE_C() {
}

//! UE_C类初始化函数
/*!
  \return 无返回值
*/
void UE_C::InitParas() {
  int ue_cell_idx = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  Simuctrl_Next = Simuctrl;
  Cell_Specific_Next = Cell_Specific;
  UE_Specific_Next = UE_Specific;
  TA_Target = -36;
  TA = -36;
  TA_Target_prime = round((float)(TA_Target * Cell_Specific[0].OverSample_N_FFT) / 2048.0f);
  Wire_Signal.resize(UE_Specific[0].RxNum, RC_FIR_LEN + Cell_Specific[0].OverSample_Num);
  Wire_Signal.setZero();
  Wire_Signal_FIR.resize(UE_Specific[0].RxNum, Cell_Specific[0].OverSample_Num);
  Wire_Signal.setZero();
  AGC.resize(UE_Specific[0].RxNum);
  Last_AGC.resize(UE_Specific[0].RxNum);
  Last_AGC_PBCH.resize(UE_Specific[0].RxNum);
  for (i = 0; i < UE_Specific[0].RxNum; i++) {
    AGC[i] = 0;
    Last_AGC[i] = 0;
    Last_AGC_PBCH[i] = 0;
  }
  Phase.resize(UE_Specific.size());
  Cell_Signal.resize(UE_Specific.size());
  if (Simuctrl.PSS_SSS_DTFlag || Simuctrl.BLIND_PBCH_DT_Flag) {
    Cell_Signal_5ms.resize(UE_Specific.size());
    Cell_Signal_1p4M_5ms.resize(UE_Specific.size());
  }
  Counter_5ms = 0;
  last_n_subframe.resize(UE_Specific.size());
#if IDEAL_H_METHOD > 0
  IdealSamplePos.resize(UE_Specific.size());
  IdealSamplePosNum.resize(UE_Specific.size());
  IdealSamplePos_Next.resize(UE_Specific.size());
  IdealSamplePosNum_Next.resize(UE_Specific.size());
#endif
  MEA.resize(UE_Specific.size());
  CHE.resize(UE_Specific.size());
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    Phase[ue_cell_idx] = 0.0;
    Cell_Signal[ue_cell_idx].resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].OverSample_Num);
    Cell_Signal[ue_cell_idx].setZero();
    if (Simuctrl.PSS_SSS_DTFlag || Simuctrl.BLIND_PBCH_DT_Flag) {
      Cell_Signal_5ms[ue_cell_idx].resize(UE_Specific[ue_cell_idx].RxNum, 6 * Cell_Specific[ue_cell_idx].OverSample_Num);
      Cell_Signal_5ms[ue_cell_idx].setZero();
      Cell_Signal_1p4M_5ms[ue_cell_idx].resize(UE_Specific[ue_cell_idx].RxNum, 6 * 1920);
      Cell_Signal_1p4M_5ms[ue_cell_idx].setZero();
    }
    last_n_subframe[ue_cell_idx] = Simuctrl.start_n_subframe - 1;
    for (i = 0; i < MAXHARQNUM; i++) {
      for (j = 0; j < MAX_TB_SIZE; j++) {
        ReTransmit[ue_cell_idx].LastNDI[j][i] = 0;
      }
    }
    Feedback[ue_cell_idx].Perfered_M = 1;
    if ((TM7 == UE_Specific[ue_cell_idx].TM || TM8 == UE_Specific[ue_cell_idx].TM) && Simuctrl.Random_BF_Flag) {
      Feedback[ue_cell_idx].k_Subband = 1;
    } else if (UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode)) {
      Get_Perfered_M_SubbandSize(Cell_Specific[ue_cell_idx].N_DL_RB, &Feedback[ue_cell_idx].k_Subband, &Feedback[ue_cell_idx].Perfered_M);
    } else if (UE_Specific[ue_cell_idx].ReportOnPUSCH) {
      Feedback[ue_cell_idx].k_Subband = Get_SubbandSize(Cell_Specific[ue_cell_idx].N_DL_RB);
    } else {
      Feedback[ue_cell_idx].k_Subband = Get_SubbandSize_PUCCH(Cell_Specific[ue_cell_idx].N_DL_RB, &Feedback[ue_cell_idx].J);
      Feedback[ue_cell_idx].PUCCH_Last_j = 0;
      Feedback[ue_cell_idx].N_j = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)Feedback[ue_cell_idx].k_Subband / (float)Feedback[ue_cell_idx].J);
    }
    Feedback[ue_cell_idx].Perfered_M_idx.resize(MAX_TB_SIZE, Feedback[ue_cell_idx].Perfered_M);
    Feedback[ue_cell_idx].N_Subband = ceil((float)Cell_Specific[ue_cell_idx].N_DL_RB / (float)Feedback[ue_cell_idx].k_Subband);
    Feedback[ue_cell_idx].k_LastSubband = Cell_Specific[ue_cell_idx].N_DL_RB - Feedback[ue_cell_idx].k_Subband * (Feedback[ue_cell_idx].N_Subband - 1);
    Feedback[ue_cell_idx].RI = Simuctrl.Fixed_RI;
    Feedback[ue_cell_idx].RI_Last_Report = Simuctrl.Fixed_RI;
    for (i = 0; i < 15; i++) {
      Feedback[ue_cell_idx].CQI_BLER_Acc[i] = 0;
      Feedback[ue_cell_idx].CQI_BLER_Counter[i] = 0;
    }
    Feedback[ue_cell_idx].AccFreqOffset = 0.0f;
    Feedback[ue_cell_idx].AccFreqOffsetLen = 0;
    Feedback[ue_cell_idx].AccIRT = 0;
    Feedback[ue_cell_idx].AccIRTLen = 0;
    Feedback[ue_cell_idx].RxFreqOffset = UE_Specific[ue_cell_idx].RxFreqOffset;
    Feedback[ue_cell_idx].Subband_PMI.resize(MIN(MAXCSIRSPORTNUM, UE_Specific[ue_cell_idx].RxNum), Feedback[ue_cell_idx].N_Subband);
    Feedback[ue_cell_idx].Subband_PMI.setZero();
    for (i = 0; i < MAX_TB_SIZE; i++) {
      Feedback[ue_cell_idx].SubbandCQI[i].resize(MAX_TB_SIZE, Feedback[ue_cell_idx].N_Subband);
      for (j = 0; j < MAX_TB_SIZE; j++) {
        Feedback[ue_cell_idx].WidebandCQI[i][j] = 1;
        for (k = 0; k < Feedback[ue_cell_idx].N_Subband; k++) {
          Feedback[ue_cell_idx].SubbandCQI[i](j, k) = 1;
        }
      }
    }
    MEA[ue_cell_idx].Subband_N.resize(UE_Specific[ue_cell_idx].RxNum, Feedback[ue_cell_idx].N_Subband);
    MEA[ue_cell_idx].Subband_SNR.resize(MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum));
    for (i = 0; i < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum); i++) {
      MEA[ue_cell_idx].Subband_SNR[i].resize(MAX_PMI, Feedback[ue_cell_idx].N_Subband * MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum));
      MEA[ue_cell_idx].Subband_SNR[i].setZero();
    }
    MEA[ue_cell_idx].CRS_PDP_Alpha_1stFlag = true;
    MEA[ue_cell_idx].CRS_PDP.resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPLENGTH);
    MEA[ue_cell_idx].CRS_PDP.setZero();
    MEA[ue_cell_idx].CRS_PDP_last.resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPLENGTH);
    MEA[ue_cell_idx].CRS_PDP_last.setZero();
    MEA[ue_cell_idx].CRS_PDP_lastAGC.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_PDP_lastAGC.setZero();
    MEA[ue_cell_idx].CRS_PDP_Alpha.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_PDP_AccSNRTh.resize(UE_Specific[ue_cell_idx].RxNum);
    for (i = 0; i < UE_Specific[ue_cell_idx].RxNum; i++) {
      MEA[ue_cell_idx].CRS_PDP_Alpha(i) = 0.25f;
      MEA[ue_cell_idx].CRS_PDP_AccSNRTh(i) = 0.0f;
    }
    MEA[ue_cell_idx].CRS_PDP_Counter = 0;
    MEA[ue_cell_idx].MaxPDPs_Value.resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPSNUM);
    MEA[ue_cell_idx].MaxPDPs_Idx.resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPSNUM);
    MEA[ue_cell_idx].CRS_N.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_S.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_SNR.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].IRT.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].R_F.resize(UE_Specific[ue_cell_idx].RxNum, R_F_Num);
    MEA[ue_cell_idx].R_T.resize(UE_Specific[ue_cell_idx].RxNum, R_T_Num);
    MEA[ue_cell_idx].fd = 5.0f;
    MEA[ue_cell_idx].fd_shift = 0.0f;
    MEA[ue_cell_idx].last_RxFreqOffset = UE_Specific[ue_cell_idx].RxFreqOffset;
    MEA[ue_cell_idx].fd_shift_1stFlag = true;
    MEA[ue_cell_idx].fd_shift_Counter = 0;
    MEA[ue_cell_idx].FilterF_Noise_Scale.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_N_2nd.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_S_2nd.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].CRS_SNR_2nd.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].HST_SFN_Flag = false;
    MEA[ue_cell_idx].HST_SFN_Counter = 0;
    MEA[ue_cell_idx].HST_SFN_Delay_Counter = 0;
    MEA[ue_cell_idx].DMRS_S_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_SNR_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_N_2nd_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_S_2nd_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_SNR_2nd_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_S_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_SNR_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_FilterF_Noise_Scale_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_N_2nd_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_S_2nd_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].DMRS_SNR_2nd_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum);
    MEA[ue_cell_idx].EPDCCH_DMRS_S.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].EPDCCH_DMS_SNR.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].EPDCCH_DEMS_FilterF_Noise_Scale.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].EPDCCH_DMRS_N_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].EPDCCH_DMRS_S_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].EPDCCH_DMRS_SNR_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    MEA[ue_cell_idx].CSI_RS_PDP.resize(UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_MaxPDPs_Value.resize(UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_MaxPDPs_Idx.resize(UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    for (i = 0; i < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; i++) {
      MEA[ue_cell_idx].CSI_RS_PDP[i].resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPLENGTH / 2);
      MEA[ue_cell_idx].CSI_MaxPDPs_Value[i].resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPSNUM);
      MEA[ue_cell_idx].CSI_MaxPDPs_Idx[i].resize(UE_Specific[ue_cell_idx].RxNum, MAXPDPSNUM);
    }
    MEA[ue_cell_idx].CSI_RS_N.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_RS_S.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_RS_SNR.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_R_F.resize(UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    for (i = 0; i < UE_Specific[ue_cell_idx].NZP_CSI_Set_Num; i++) {
      MEA[ue_cell_idx].CSI_R_F[i].resize(UE_Specific[ue_cell_idx].RxNum, 2 * R_F_Num);
    }
    MEA[ue_cell_idx].CSI_RS_N_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_RS_S_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    MEA[ue_cell_idx].CSI_RS_SNR_2nd.resize(UE_Specific[ue_cell_idx].RxNum, UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
#if IDEAL_H_METHOD > 0
    MEA[ue_cell_idx].Ideal_PDP.resize(UE_Specific[ue_cell_idx].RxNum, Cell_Specific[ue_cell_idx].OverSample_N_FFT);
#endif
    MEA[ue_cell_idx].last_IRT.resize(UE_Specific[ue_cell_idx].NZP_CSI_Set_Num);
    CHE[ue_cell_idx].CRS_LS_H.resize(UE_Specific[ue_cell_idx].RxNum * MAXCRSPORTNUM);
    CHE[ue_cell_idx].CRS_H_F.resize(UE_Specific[ue_cell_idx].RxNum * MAXCRSPORTNUM);
    CHE[ue_cell_idx].CRS_H_T.resize(UE_Specific[ue_cell_idx].RxNum * MAXCRSPORTNUM);
    CHE[ue_cell_idx].DMRS_LS_H_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    CHE[ue_cell_idx].DMRS_H_F_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    CHE[ue_cell_idx].DMRS_H_T_Port5.resize(UE_Specific[ue_cell_idx].RxNum);
    CHE[ue_cell_idx].DMRS_LS_H_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum * MAXDMRSPORTNUM);
    CHE[ue_cell_idx].DMRS_H_F_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum * MAXDMRSPORTNUM);
    CHE[ue_cell_idx].DMRS_H_T_Port7to14.resize(UE_Specific[ue_cell_idx].RxNum * MAXDMRSPORTNUM);
    CHE[ue_cell_idx].EPDCCH_DMRS_LS_H.resize(UE_Specific[ue_cell_idx].RxNum * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    CHE[ue_cell_idx].EPDCCH_DMRS_H_F.resize(UE_Specific[ue_cell_idx].RxNum * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    CHE[ue_cell_idx].EPDCCH_DMRS_H_T.resize(UE_Specific[ue_cell_idx].RxNum * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].EPDCCH_Set_Num);
    CHE[ue_cell_idx].CSI_RS_LS_H.resize(UE_Specific[ue_cell_idx].RxNum * MAXCSIRSPORTNUM);
    CHE[ue_cell_idx].CSI_RS_H_F.resize(UE_Specific[ue_cell_idx].RxNum * MAXCSIRSPORTNUM);
    CHE[ue_cell_idx].PSS_H.resize(UE_Specific[ue_cell_idx].RxNum, 62);
    CHE[ue_cell_idx].SSS_H.resize(UE_Specific[ue_cell_idx].RxNum, 62);
    for (i = 0; i < MAX_TB_SIZE; i++) {
      Result[ue_cell_idx].BLER_PDSCH_perCW[i] = 0;
      Result[ue_cell_idx].BLER_PDSCH_perCW_Num[i] = 0;
      Result[ue_cell_idx].Throughput_perCW[i] = 0.0f;
    }
    Result[ue_cell_idx].BLER_PDSCH = 0;
    Result[ue_cell_idx].BLER_PDSCH_Num = 0;
    Result[ue_cell_idx].Throughput = 0.0f;
    Result[ue_cell_idx].BLER_PHICH = 0;
    Result[ue_cell_idx].BLER_PHICH_Num = 0;
    Result[ue_cell_idx].BLER_PCFICH = 0;
    Result[ue_cell_idx].BLER_PCFICH_Num = 0;
    Result[ue_cell_idx].BLER_PDCCH = 0;
    Result[ue_cell_idx].BLER_PDCCH_Num = 0;
    Result[ue_cell_idx].BLER_PCFICH_PDCCH = 0;
    Result[ue_cell_idx].BLER_PCFICH_PDCCH_Num = 0;
    Result[ue_cell_idx].BLER_EPDCCH = 0;
    Result[ue_cell_idx].BLER_EPDCCH_Num = 0;
    Result[ue_cell_idx].BLER_PBCH = 0;
    Result[ue_cell_idx].BLER_PBCH_Num = 0;
    Result[ue_cell_idx].PSS_DT_Flag = false;
    Result[ue_cell_idx].PSS_Pos = 0;
    Result[ue_cell_idx].SSS_DT_Flag = false;
    Result[ue_cell_idx].PBCH_Blind_Flag = false;
    Result[ue_cell_idx].PBCH_Flag = false;
    if (Simuctrl.PBCH_DTFlag) {
      Result[ue_cell_idx].MIMO_symbs_out.resize((NormalCP == Cell_Specific[ue_cell_idx].CP_Type) ? 960 : 864);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * MAXCRSPORTNUM); i++) {
      CHE[ue_cell_idx].CRS_LS_H[i].resize(2 * Cell_Specific[ue_cell_idx].N_DL_RB, MAXCRSSYMB_TTI);
      CHE[ue_cell_idx].CRS_H_F[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, MAXCRSSYMB_TTI);
      CHE[ue_cell_idx].CRS_H_T[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum); i++) {
      CHE[ue_cell_idx].DMRS_LS_H_Port5[i].resize(4 * Cell_Specific[ue_cell_idx].N_DL_RB, MAXDMRSSYMB_TTI_Port5);
      CHE[ue_cell_idx].DMRS_H_F_Port5[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, MAXDMRSSYMB_TTI_Port5);
      CHE[ue_cell_idx].DMRS_H_T_Port5[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * MAXDMRSPORTNUM); i++) {
      CHE[ue_cell_idx].DMRS_LS_H_Port7to14[i].resize(4 * Cell_Specific[ue_cell_idx].N_DL_RB, MAXDMRSSYMB_TTI_Port7to14);
      CHE[ue_cell_idx].DMRS_H_F_Port7to14[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, MAXDMRSSYMB_TTI_Port7to14);
      CHE[ue_cell_idx].DMRS_H_T_Port7to14[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * MAXEPDCCHRSPORTNUM * UE_Specific[ue_cell_idx].EPDCCH_Set_Num); i++) {
      CHE[ue_cell_idx].EPDCCH_DMRS_LS_H[i].resize(4 * Cell_Specific[ue_cell_idx].N_DL_RB, MAXDMRSSYMB_TTI_Port7to14);
      CHE[ue_cell_idx].EPDCCH_DMRS_H_F[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, MAXDMRSSYMB_TTI_Port7to14);
      CHE[ue_cell_idx].EPDCCH_DMRS_H_T[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * MAXCSIRSPORTNUM); i++) {
      CHE[ue_cell_idx].CSI_RS_LS_H[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB, MAXNZPCSIRSNUM);
      CHE[ue_cell_idx].CSI_RS_H_F[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, MAXNZPCSIRSNUM);
    }
#if IDEAL_H_METHOD > 0
    CHE[ue_cell_idx].Ideal_h.resize(Cell_Specific[ue_cell_idx].OverSample_N_FFT * MAX_SYMBNUM_TTI);
    CHE[ue_cell_idx].Ideal_H_T.resize(UE_Specific[ue_cell_idx].RxNum * MAX_PHY_ANTENNA_NUM);
    for (i = 0; i < (Cell_Specific[ue_cell_idx].OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
      CHE[ue_cell_idx].Ideal_h[i].resize(UE_Specific[ue_cell_idx].RxNum, MAX_PHY_ANTENNA_NUM);
    }
    for (i = 0; i < (UE_Specific[ue_cell_idx].RxNum * MAX_PHY_ANTENNA_NUM); i++) {
      CHE[ue_cell_idx].Ideal_H_T[i].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
    }
#endif
  }
  RE.resize(UE_Specific[0].RxNum);
}

//! 由于调度等影响，每个TTI需要做参数更新
/*!
  \param n_subframe ： 子帧序号，输入
  \param Cell ： 连接的小区，输入
  \return 无返回值
*/
void UE_C::UpdataParas(int n_subframe, const vector<Cell_C> &Cell) {
  int cell_idx = 0;
  int ue_cell_idx = 0;
  Simuctrl.n_subframe = Simuctrl_Next.n_subframe;
  Simuctrl_Next.n_subframe = n_subframe;
  MaxOverSample_DataLen = 0;
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
    Cell_Specific[ue_cell_idx] = Cell_Specific_Next[ue_cell_idx];
    UE_Specific[ue_cell_idx] = UE_Specific_Next[ue_cell_idx];
    //UE_Specific[ue_cell_idx].Codebook_index0.resize(UE_Specific_Next[ue_cell_idx].Codebook_index0.size());
    UE_Specific[ue_cell_idx].Codebook_index0 = UE_Specific_Next[ue_cell_idx].Codebook_index0;
    //UE_Specific[ue_cell_idx].Codebook_index1.resize(UE_Specific_Next[ue_cell_idx].Codebook_index1.size());
    UE_Specific[ue_cell_idx].Codebook_index1 = UE_Specific_Next[ue_cell_idx].Codebook_index1;
#if IDEAL_H_METHOD > 0
    IdealSamplePos[ue_cell_idx] = IdealSamplePos_Next[ue_cell_idx];
    IdealSamplePosNum[ue_cell_idx] = IdealSamplePosNum_Next[ue_cell_idx];
#endif
    Cell_Specific_Next[ue_cell_idx] = Cell[cell_idx].Cell_Specific;
    UE_Specific_Next[ue_cell_idx] = Cell[cell_idx].UE_Specific[UE_Connect_Cell_innerUE_idx[ue_cell_idx]];
    //UE_Specific_Next[ue_cell_idx].Codebook_index0.resize(Cell[cell_idx].UE_Specific[UE_Connect_Cell_innerUE_idx[ue_cell_idx]].Codebook_index0.size());
    UE_Specific_Next[ue_cell_idx].Codebook_index0 = Cell[cell_idx].UE_Specific[UE_Connect_Cell_innerUE_idx[ue_cell_idx]].Codebook_index0;
    //UE_Specific_Next[ue_cell_idx].Codebook_index1.resize(Cell[cell_idx].UE_Specific[UE_Connect_Cell_innerUE_idx[ue_cell_idx]].Codebook_index1.size());
    UE_Specific_Next[ue_cell_idx].Codebook_index1 = Cell[cell_idx].UE_Specific[UE_Connect_Cell_innerUE_idx[ue_cell_idx]].Codebook_index1;
#if IDEAL_H_METHOD > 0
    IdealSamplePos_Next[ue_cell_idx] = Cell[cell_idx].IdealSamplePos;
    IdealSamplePosNum_Next[ue_cell_idx] = Cell[cell_idx].IdealSamplePosNum;
#endif
    MaxOverSample_DataLen = MAX(MaxOverSample_DataLen, Cell_Specific[ue_cell_idx].OverSample_DataLen);
  }
}

//! UE接收处理过程
/*!
  \param Channel ： 连接的无线信道，输入
  \return 无返回值
*/
void UE_C::Process(Channel_C &Channel) {
  int ue_cell_idx = 0;
  int r = 0;
  int DCI_idx = 0;
  //RF
  RF(Channel);
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    if (ServingCell == UE_Specific[ue_cell_idx].CellType && Cell_Specific[ue_cell_idx].OverSample_DataLen > 0) {
      sprintf(c_string, "CellIdx%d", ue_cell_idx);
      FilesPathUE_Cell = c_string;
      Result[ue_cell_idx].SNR[loop] = Channel.Channel_Specific[ue_cell_idx].SNR;
      sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
      WithFile(c_string);
      FilePrint("n_subframe=%d\n", Simuctrl.n_subframe);
      DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
      for (r = 0; r < UE_Specific[ue_cell_idx].RxNum; r++) {
        RE[r].resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
      }
      RE_Type.resize(Cell_Specific[ue_cell_idx].N_DL_RB * Cell_Specific[ue_cell_idx].N_RB_sc, 2 * Cell_Specific[ue_cell_idx].N_DL_symb);
      RE_Type.setConstant(EMPTY_RE);
      if (Simuctrl.PSS_SSS_DTFlag) {
        //PSS, SSS DT
        PSS_DT(ue_cell_idx);
        SSS_DT(ue_cell_idx);
      } else if (Simuctrl.BLIND_PBCH_DT_Flag) {
        //PBCH DT Blind
        PBCH_DMP_MIMO_DCC_Blind(ue_cell_idx);
      } else {
        //Front
        RemoveCP_FFT(ue_cell_idx);
        //PSS, SSS DMP
        PSS_Remove(ue_cell_idx);
        SSS_Remove(ue_cell_idx);
        //FEICIC
        if (UE_Specific[ue_cell_idx].CRS_AssistanceInfo_Num > 0) {
          FEICIC(ue_cell_idx);
        }
        //CRS CHE & MEA
        CRS_LSCheEst(ue_cell_idx);
        CRS_Measurement_1st(ue_cell_idx);
        CRS_FilterF(ue_cell_idx);
        CRS_FilterT(ue_cell_idx);
        CRS_Measurement_2nd(ue_cell_idx);
        if (Simuctrl.PBCH_DTFlag) {
          PBCH_DMP_MIMO_DCC(ue_cell_idx);
        } else {
          PBCH_Remove(ue_cell_idx);
        }
        if (UE_Specific[ue_cell_idx].NZP_CSI_Set_Num > 0 || UE_Specific[ue_cell_idx].ZP_CSI_Set_Num > 0) {
          //CSI_RS CHE & MEA
          CSI_RS_LSCheEst(ue_cell_idx);
          CSI_RS_Measurement_1st(ue_cell_idx);
          CSI_RS_FilterF(ue_cell_idx);
          CSI_RS_Measurement_2nd(ue_cell_idx);
        }
        //PCFICH
        if (Simuctrl.PCFICH_DTFlag) {
          PCFICH_DMP_MIMO_DCC(ue_cell_idx);
        } else if (Simuctrl.PHICH_DTFlag || Simuctrl.PDCCH_DTFlag) {
          PCFICH_Remove(ue_cell_idx);
        }
        //PHICH
        if (Simuctrl.PHICH_DTFlag) {
          PHICH_DMP_MIMO_DCC(ue_cell_idx);
        } else if (Simuctrl.PDCCH_DTFlag) {
          PHICH_Remove(ue_cell_idx);
        }
        //PDCCH
        if (Simuctrl.PDCCH_DTFlag) {
          PDCCH_DMP_MIMO_DCC(ue_cell_idx);
        }
        if (UE_Specific[ue_cell_idx].EPDCCH_Set_Num > 0) {
          //EPDCCH CHE & MEA
          EPDCCH_DMRS_LSCheEst(ue_cell_idx);
          if (Simuctrl.EPDCCH_DTFlag) {
            EPDCCH_DMRS_Measurement_1st(ue_cell_idx);
            EPDCCH_DMRS_FilterF(ue_cell_idx);
            EPDCCH_DMRS_FilterT(ue_cell_idx);
            EPDCCH_DMRS_Measurement_2nd(ue_cell_idx);
          }
          //EPDCCH
          if (Simuctrl.EPDCCH_DTFlag) {
            EPDCCH_DMP_MIMO_DCC(ue_cell_idx);
          } else {
            EPDCCH_Remove(ue_cell_idx);
          }
        }
        //DMRS CHE & MEA
        if (TM7 == UE_Specific[ue_cell_idx].TM && DCI_Format_1 == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) {
          DMRS_Port5_LSCheEst(ue_cell_idx);
          DMRS_Port5_Measurement_1st(ue_cell_idx);
          DMRS_Port5_FilterF(ue_cell_idx);
          DMRS_Port5_FilterT(ue_cell_idx);
          DMRS_Port5_Measurement_2nd(ue_cell_idx);
        } else if ((TM8 == UE_Specific[ue_cell_idx].TM && DCI_Format_2B == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM9 == UE_Specific[ue_cell_idx].TM && DCI_Format_2C == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx]) || (TM10 == UE_Specific[ue_cell_idx].TM && DCI_Format_2D == UE_Specific[ue_cell_idx].DCI_Format_Type[DCI_idx])) {
          DMRS_Port7to14_LSCheEst(ue_cell_idx);
          DMRS_Port7to14_Measurement_1st(ue_cell_idx);
          DMRS_Port7to14_FilterF(ue_cell_idx);
          DMRS_Port7to14_FilterT(ue_cell_idx);
          DMRS_Port7to14_Measurement_2nd(ue_cell_idx);
        }
        //PDSCH
        if (Simuctrl.PDSCH_DTFlag) {
          PDSCH_DMP_MIMO_DCC(ue_cell_idx);
        }
      }
    }
  }
  if (MaxOverSample_DataLen > 0) {
    if (Simuctrl.AGC_Flag) {
      for (r = 0; r < UE_Specific[0].RxNum; r++) {
        Last_AGC[r] = AGC[r];
      }
    }
  }
}

//! 仿真结果统计
/*!
  \param ue_idx ： UE序号
  \return 无返回值
*/
void UE_C::GetResult(int ue_idx) {
  int ue_cell_idx = 0;
  fvec value;
  float Margin = 0.0f;
  float nearest_value[2];
  int nearest_idx[2];
  float SNR_eff = 0.0;
  int Min_SNR_Idx = 0;
  float Min_SNR = 65535.0f;
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    if (ServingCell == UE_Specific[ue_cell_idx].CellType && NULL_Ref != UE_Specific[ue_cell_idx].Ref_Type && (int)Result[ue_cell_idx].SNR.size() >= 1) {
      WithFile("Log");
      FilePrint("ue_idx=%d, ue_cell_idx=%d\n", ue_idx, ue_cell_idx);
      if (Ref_Median_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Non_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_UE_Selected_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_PMI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_RI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
        FilePrint("Case %s\n\n", Result[ue_cell_idx].Feedback_Test_Flag ? "Pass" : "Fail");
      } else if ((int)Result[ue_cell_idx].SNR.size() >= 2) {
        if (Ref_CQI_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
          FilePrint("Ref_Value=%f\n", Result[ue_cell_idx].Ref_Value);
        } else {
          FilePrint("Ref_SNR=%f, Ref_Value=%f\n", Result[ue_cell_idx].Ref_SNR, Result[ue_cell_idx].Ref_Value);
        }
        value.resize(Result[ue_cell_idx].SNR.size());
        nearest_value[0] = -100.0f;
        nearest_idx[0] = 0;
        Min_SNR = 65535.0f;
        Min_SNR_Idx = 0;
        for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
          if (Ref_PBCH_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, BLER=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].PBCH_BLER(loop));
            value(loop) = Result[ue_cell_idx].PBCH_BLER(loop);
          } else if (Ref_PCHICH_PDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, BLER=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].PCHICH_PDCCH_BLER(loop));
            value(loop) = Result[ue_cell_idx].PCHICH_PDCCH_BLER(loop);
          } else if (Ref_PHICH_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, BLER=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].PHICH_BLER(loop));
            value(loop) = Result[ue_cell_idx].PHICH_BLER(loop);
          } else if (Ref_EPDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, BLER=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].EPDCCH_BLER(loop));
            value(loop) = Result[ue_cell_idx].EPDCCH_BLER(loop);
          } else if (Ref_PDSCH_Throughput == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, Throughput=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].PDSCH_Throughput(loop));
            value(loop) = Result[ue_cell_idx].PDSCH_Throughput(loop);
          } else if (Ref_CQI_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
            FilePrint("SNR=%f, BLER=%f\n", Result[ue_cell_idx].SNR(loop), Result[ue_cell_idx].PDSCH_BLER(loop));
            value(loop) = Result[ue_cell_idx].PDSCH_BLER(loop);
          }
          if (MyFabs(value(loop) - Result[ue_cell_idx].Ref_Value) < MyFabs(nearest_value[0] - Result[ue_cell_idx].Ref_Value)) {
            nearest_value[0] = value(loop);
            nearest_idx[0] = loop;
          }
          if (Result[ue_cell_idx].SNR(loop) < Min_SNR) {
            Min_SNR = Result[ue_cell_idx].SNR(loop);
            Min_SNR_Idx = loop;
          }
        }
        nearest_value[1] = -100.0f;  //suppose BLER or Throught >=0
        nearest_idx[1] = (0 == nearest_idx[0]) ? 1 : 0;
        if (nearest_value[0] >= Result[ue_cell_idx].Ref_Value) {
          for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
            if (MyFabs(value(loop) - Result[ue_cell_idx].Ref_Value) < MyFabs(nearest_value[1] - Result[ue_cell_idx].Ref_Value) && loop != nearest_idx[0] && value(loop) != nearest_value[0] && value(loop) <= Result[ue_cell_idx].Ref_Value) {
              nearest_value[1] = value(loop);
              nearest_idx[1] = loop;
            }
          }
        } else {
          for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
            if (MyFabs(value(loop) - Result[ue_cell_idx].Ref_Value) < MyFabs(nearest_value[1] - Result[ue_cell_idx].Ref_Value) && loop != nearest_idx[0] && value(loop) != nearest_value[0] && value(loop) >= Result[ue_cell_idx].Ref_Value) {
              nearest_value[1] = value(loop);
              nearest_idx[1] = loop;
            }
          }
        }
        if (-100.0f == nearest_value[1]) {
          for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
            if (MyFabs(value(loop) - Result[ue_cell_idx].Ref_Value) < MyFabs(nearest_value[1] - Result[ue_cell_idx].Ref_Value) && loop != nearest_idx[0] && value(loop) != nearest_value[0]) {
              nearest_value[1] = value(loop);
              nearest_idx[1] = loop;
            }
          }
        }
        if (Ref_CQI_BLER == UE_Specific[ue_cell_idx].Ref_Type) {
          for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
            Result[ue_cell_idx].CQI_SNR_eff[loop] = Result[ue_cell_idx].CQI_SNR_eff[loop] / (float)Result[ue_cell_idx].CQI_SNR_eff_Num[loop];
            Result[ue_cell_idx].CQI_SNR_eff[loop] = 10.0f * log10f(Result[ue_cell_idx].CQI_SNR_eff[loop]);
          }
          SNR_eff = ((Result[ue_cell_idx].CQI_SNR_eff(nearest_idx[1]) - Result[ue_cell_idx].CQI_SNR_eff(nearest_idx[0])) / (nearest_value[1] - nearest_value[0]) * (Result[ue_cell_idx].Ref_Value - nearest_value[0]) + Result[ue_cell_idx].CQI_SNR_eff(nearest_idx[0]));
          FilePrint("SNR_eff=%f, linear=%f\n\n", SNR_eff, powf(10.0f, SNR_eff / 10.0f));
        } else if (Ref_PDSCH_Throughput == UE_Specific[ue_cell_idx].Ref_Type && value(Min_SNR_Idx) > Result[ue_cell_idx].Ref_Value && Min_SNR < Result[ue_cell_idx].Ref_SNR) {
          FilePrint("Margin>%f, Case %s\n\n", Result[ue_cell_idx].Ref_SNR - Min_SNR, "Pass");
        } else if ((Ref_PBCH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_PCHICH_PDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_PHICH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_EPDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type) && value(Min_SNR_Idx) < Result[ue_cell_idx].Ref_Value && Min_SNR < Result[ue_cell_idx].Ref_SNR) {
          FilePrint("Margin>%f, Case %s\n\n", Result[ue_cell_idx].Ref_SNR - Min_SNR, "Pass");
        } else if (Ref_PDSCH_Throughput == UE_Specific[ue_cell_idx].Ref_Type && nearest_value[0] < Result[ue_cell_idx].Ref_Value && nearest_value[1] < Result[ue_cell_idx].Ref_Value) {
          FilePrint("No Margin, Case %s\n\n", "Fail");
        } else if ((Ref_PBCH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_PCHICH_PDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_PHICH_BLER == UE_Specific[ue_cell_idx].Ref_Type || Ref_EPDCCH_BLER == UE_Specific[ue_cell_idx].Ref_Type) && nearest_value[0] > Result[ue_cell_idx].Ref_Value && nearest_value[1] > Result[ue_cell_idx].Ref_Value) {
          FilePrint("No Margin, Case %s\n\n", "Fail");
        } else {
          Margin = Result[ue_cell_idx].Ref_SNR - ((Result[ue_cell_idx].SNR(nearest_idx[1]) - Result[ue_cell_idx].SNR(nearest_idx[0])) / (nearest_value[1] - nearest_value[0]) * (Result[ue_cell_idx].Ref_Value - nearest_value[0]) + Result[ue_cell_idx].SNR(nearest_idx[0]));
          FilePrint("Margin=%f, Case %s\n\n", Margin, (Margin >= 0.5f) ? "Pass" : "Fail");
        }
#if 1 == EESM_TEST
        WithFile("EESM_BLER");
        for (loop = 0; loop < (int)Result[ue_cell_idx].SNR.size(); loop++) {
          FilePrint("%f\n", Result[ue_cell_idx].PDSCH_BLER(loop));
        }
#endif
      }
    }
  }
}
