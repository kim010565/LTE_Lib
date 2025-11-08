/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE_Feedback.cpp
    \brief UE反馈实现文件
    内容 ：UE_C类反馈相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/UE/UE.h"

//! 反馈处理过程
/*!
  \param ue_idx ： UE序号，输入
  \param Cell ： 连接的Cell
  \return 无返回值
*/
void UE_C::FeedBack(int ue_idx, std::vector<Cell_C> &Cell) {
  int cell_ue_idx = 0;
  int ue_cell_idx = 0;
  int cell_idx = 0;
  int cw_idx = 0;
  int DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
  int RI_idx = 0;
  int codeword_num = 0;
  int PMI_idx = 0;
  int subband_idx = 0;
  int CQI_idx = 0;
  int CQI_BLER_idx = 0;
  int CQI_Offset = 0;
  int vv = 0;
  int vv_start = 0;
  int N_L_1cw = 0;
  float SNR_eff = 0.0f;
  fvec Acc_PMI_Efficiency;  //Acc_PMI_Efficiency(PMI_idx)
  fvec Acc_RI_Efficiency;   //Acc_RI_Efficiency(PMI_idx)
  imat SubbandCQI;
  fmat SubbandSNR;
  fvec SortValue;
  ivec SortIdx;
  int max_idx = 0;
  float max_value = 0.0f;
  int buffer_idx = 0;
  int diff_value = 0;
  int nearest_value = 0;
  int nearest_buffer_idx = 0;
  int n_subframe = 10 * Cell_Specific[ue_cell_idx].n_f + Cell_Specific[ue_cell_idx].n_subframe;
  //RI
  if (Simuctrl.RI_Feedback_Flag) {
    if (Closed_Loop_SM == UE_Specific[ue_cell_idx].Transmission_Scheme || MU_MIMO == UE_Specific[ue_cell_idx].Transmission_Scheme || TM8 == UE_Specific[ue_cell_idx].TM) {
      if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
        if (!UE_Specific[ue_cell_idx].ReportOnPUSCH || (UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM31 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode))) {
          Acc_RI_Efficiency.resize(MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum));
          for (RI_idx = 0; RI_idx < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum); RI_idx++) {
            N_L_1cw = RI_idx + 1;
            Acc_PMI_Efficiency.resize(UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]);
            Acc_PMI_Efficiency.setZero();
            max_idx = 0;
            max_value = 0.0f;
            for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                CQI_BLER_idx = 0;
                for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
                  SNR_eff = 0.0f;
                  for (vv = 0; vv < N_L_1cw; vv++) {
                    SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                  }
                  SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)N_L_1cw);
                  if (SNR_eff >= CQI_Th[CQI_idx]) {
                    CQI_BLER_idx = CQI_idx;
                    break;
                  }
                }
                Acc_PMI_Efficiency(PMI_idx) += (float)N_L_1cw * CQI_Efficiency[CQI_BLER_idx];
              }
              if (max_value < Acc_PMI_Efficiency(PMI_idx)) {
                max_value = Acc_PMI_Efficiency(PMI_idx);
                max_idx = PMI_idx;
              }
            }
            Acc_RI_Efficiency(RI_idx) = Acc_PMI_Efficiency(max_idx);
          }
        } else {
          Acc_RI_Efficiency.resize(MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum));
          Acc_RI_Efficiency.setZero();
          for (RI_idx = 0; RI_idx < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum); RI_idx++) {
            N_L_1cw = RI_idx + 1;
            Acc_PMI_Efficiency.resize(UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]);
            for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
              max_idx = 0;
              max_value = 0.0f;
              for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
                CQI_BLER_idx = 0;
                for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
                  SNR_eff = 0.0f;
                  for (vv = 0; vv < N_L_1cw; vv++) {
                    SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                  }
                  SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)N_L_1cw);
                  if (SNR_eff >= CQI_Th[CQI_idx]) {
                    CQI_BLER_idx = CQI_idx;
                    break;
                  }
                }
                Acc_PMI_Efficiency(PMI_idx) = (float)N_L_1cw * CQI_Efficiency[CQI_BLER_idx];
                if (max_value < Acc_PMI_Efficiency(PMI_idx)) {
                  max_value = Acc_PMI_Efficiency(PMI_idx);
                  max_idx = PMI_idx;
                }
              }
              Acc_RI_Efficiency(RI_idx) += Acc_PMI_Efficiency(max_idx);
            }
          }
        }
        max_idx = 0;
        max_value = 0.0f;
        for (RI_idx = 0; RI_idx < MIN(Cell_Specific[ue_cell_idx].CRSPortNum, UE_Specific[ue_cell_idx].RxNum); RI_idx++) {
          if (max_value < Acc_RI_Efficiency(RI_idx)) {
            max_value = Acc_RI_Efficiency(RI_idx);
            max_idx = RI_idx;
          }
        }
        Feedback[ue_cell_idx].RI = max_idx;
        buffer_idx = Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer_idx;
        Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Flag[buffer_idx] = 1;
        Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer[buffer_idx] = Feedback[ue_cell_idx].RI;
        Cell[cell_idx].Feedback[cell_ue_idx].RI_Gen_n_subframe[buffer_idx] = Simuctrl.n_subframe;
        Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer_idx = (Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer_idx + 1) % MAX_Feedback_BufferSize;
      }
      if (-1 != UE_Specific[ue_cell_idx].N_offset_RI && 0 == ((n_subframe - UE_Specific[ue_cell_idx].N_offset_RI) % UE_Specific[ue_cell_idx].N_pd_RI) && (U == Cell_Specific[ue_cell_idx].SubframeType || DandU == Cell_Specific[ue_cell_idx].SubframeType)) {
        nearest_value = 10;
        nearest_buffer_idx = 0;
        for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
          diff_value = Simuctrl.n_subframe - Cell[cell_idx].Feedback[cell_ue_idx].RI_Gen_n_subframe[buffer_idx];
          if (1 == Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Flag[buffer_idx] && diff_value >= 4 && nearest_value > diff_value) {
            nearest_value = diff_value;
            nearest_buffer_idx = buffer_idx;
          }
        }
        //MyPrint("Debug %d %d\n", Simuctrl.n_subframe, Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer[nearest_buffer_idx]);
        if (nearest_value < 10) {
          Feedback[ue_cell_idx].RI_Last_Report = Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer[nearest_buffer_idx];
          Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Flag[nearest_buffer_idx] = 2;
          Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_n_subframe[nearest_buffer_idx] = Simuctrl.n_subframe;
          sprintf(c_string, "%s/%s_%s_RI", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].RI_Report_Buffer[nearest_buffer_idx] + 1);
        }
      }
    }
  }
  //PMI
  if (Simuctrl.PMI_Feedback_Flag) {
    if (TM4 == UE_Specific[ue_cell_idx].TM || TM5 == UE_Specific[ue_cell_idx].TM || TM6 == UE_Specific[ue_cell_idx].TM || TM7 == UE_Specific[ue_cell_idx].TM || TM8 == UE_Specific[ue_cell_idx].TM)  //miss TM3
    {
      if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
        if (!UE_Specific[ue_cell_idx].ReportOnPUSCH || (UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM31 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode))) {
          RI_idx = Feedback[ue_cell_idx].RI_Last_Report;
          N_L_1cw = RI_idx + 1;
          Acc_PMI_Efficiency.resize(UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]);
          Acc_PMI_Efficiency.setZero();
          max_idx = 0;
          max_value = 0.0f;
          for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
            for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
              CQI_BLER_idx = 0;
              for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
                SNR_eff = 0.0f;
                for (vv = 0; vv < N_L_1cw; vv++) {
                  SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                }
                SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)N_L_1cw);
                if (SNR_eff >= CQI_Th[CQI_idx]) {
                  CQI_BLER_idx = CQI_idx;
                  break;
                }
              }
              Acc_PMI_Efficiency(PMI_idx) += (float)N_L_1cw * CQI_Efficiency[CQI_BLER_idx];
            }
            if (max_value < Acc_PMI_Efficiency(PMI_idx)) {
              max_value = Acc_PMI_Efficiency(PMI_idx);
              max_idx = PMI_idx;
            }
          }
          for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
            Feedback[ue_cell_idx].Subband_PMI(RI_idx, subband_idx) = UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](max_idx);
          }
        } else {
          RI_idx = Feedback[ue_cell_idx].RI_Last_Report;
          N_L_1cw = RI_idx + 1;
          Acc_PMI_Efficiency.resize(UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]);
          for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
            max_idx = 0;
            max_value = 0.0f;
            for (PMI_idx = 0; PMI_idx < UE_Specific[ue_cell_idx].codebookSubsetNum[RI_idx]; PMI_idx++) {
              CQI_BLER_idx = 0;
              for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
                SNR_eff = 0.0f;
                for (vv = 0; vv < N_L_1cw; vv++) {
                  SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, vv * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                }
                SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)N_L_1cw);
                if (SNR_eff >= CQI_Th[CQI_idx]) {
                  CQI_BLER_idx = CQI_idx;
                  break;
                }
              }
              Acc_PMI_Efficiency(PMI_idx) = SNR_eff;  //(float)N_L_1cw * CQI_Efficiency[CQI_BLER_idx];
              if (max_value < Acc_PMI_Efficiency(PMI_idx)) {
                max_value = Acc_PMI_Efficiency(PMI_idx);
                max_idx = PMI_idx;
              }
            }
            Feedback[ue_cell_idx].Subband_PMI(RI_idx, subband_idx) = UE_Specific[ue_cell_idx].codebookSubsetIdxs[RI_idx](max_idx);
          }
        }
        buffer_idx = Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Buffer_idx;
        Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Flag[buffer_idx] = 1;
        for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
          Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Buffer[buffer_idx](RI_idx, subband_idx) = Feedback[ue_cell_idx].Subband_PMI(RI_idx, subband_idx);
        }
        Cell[cell_idx].Feedback[cell_ue_idx].PMI_Gen_n_subframe[buffer_idx] = Simuctrl.n_subframe;
        Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Buffer_idx = (Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Buffer_idx + 1) % MAX_Feedback_BufferSize;
      }
      if ((-1 != UE_Specific[ue_cell_idx].N_offset_CQI_PMI && 0 == ((n_subframe - UE_Specific[ue_cell_idx].N_offset_CQI_PMI) % UE_Specific[ue_cell_idx].N_pd_CQI_PMI)) && !(!UE_Specific[ue_cell_idx].ReportOnPUSCH && -1 != UE_Specific[ue_cell_idx].N_offset_RI && (0 == ((n_subframe - UE_Specific[ue_cell_idx].N_offset_RI) % UE_Specific[ue_cell_idx].N_pd_RI))) && (U == Cell_Specific[ue_cell_idx].SubframeType || DandU == Cell_Specific[ue_cell_idx].SubframeType)) {
        nearest_value = 10;
        nearest_buffer_idx = 0;
        for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
          diff_value = Simuctrl.n_subframe - Cell[cell_idx].Feedback[cell_ue_idx].PMI_Gen_n_subframe[buffer_idx];
          if (1 == Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Flag[buffer_idx] && diff_value >= 4 && nearest_value > diff_value) {
            nearest_value = diff_value;
            nearest_buffer_idx = buffer_idx;
          }
        }
        if (nearest_value < 10) {
          Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Flag[nearest_buffer_idx] = 2;
          Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_n_subframe[nearest_buffer_idx] = Simuctrl.n_subframe;
          sprintf(c_string, "%s/%s_%s_Subband_PMI", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
          WithFile(c_string);
          RI_idx = Feedback[ue_cell_idx].RI_Last_Report;
          for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
            FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].PMI_Report_Buffer[nearest_buffer_idx](RI_idx, subband_idx));
          }
        }
      }
    }
  }
  //CQI
  if (Simuctrl.CQI_Feedback_Flag) {
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
      if (ServingCell == UE_Specific[ue_cell_idx].CellType) {
        cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
        cell_ue_idx = UE_Connect_Cell_innerUE_idx[ue_cell_idx];
        if (15000 == Cell_Specific[ue_cell_idx].delta_f && U != Cell_Specific[ue_cell_idx].SubframeType) {
          RI_idx = Feedback[ue_cell_idx].RI_Last_Report;
          codeword_num = (0 == RI_idx) ? 1 : 2;
          //WidebandCQI
          for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
            if (2 == codeword_num) {
              vv_start = (0 == cw_idx) ? 0 : floor((float)(RI_idx + 1) / 2.0f);
              N_L_1cw = (0 == cw_idx) ? floor((float)(RI_idx + 1) / 2.0f) : ceil((float)(RI_idx + 1) / 2.0f);
            } else {
              vv_start = 0;
              N_L_1cw = (RI_idx + 1);
            }
            for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
              SNR_eff = 0.0f;
              for (vv = 0; vv < N_L_1cw; vv++) {
                for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                  PMI_idx = Feedback[ue_cell_idx].Subband_PMI(RI_idx, subband_idx);
                  SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, (vv + vv_start) * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                }
              }
              SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)Feedback[ue_cell_idx].N_Subband / (float)N_L_1cw);
              if (SNR_eff > CQI_Th[CQI_idx]) {
                Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] = CQI_idx + 1;
                break;
              }
            }
          }
          if (2 == codeword_num) {
            if (UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM30 == UE_Specific[ue_cell_idx].ReportMode)) {
              Feedback[ue_cell_idx].WidebandCQI[1][1] = Feedback[ue_cell_idx].WidebandCQI[1][0];
            } else  //from 3GPP 36.213 table 7.2-2
            {
              CQI_Offset = Feedback[ue_cell_idx].WidebandCQI[1][0] - Feedback[ue_cell_idx].WidebandCQI[1][1];
              if (CQI_Offset >= 3) {
                Feedback[ue_cell_idx].WidebandCQI[1][1] = Feedback[ue_cell_idx].WidebandCQI[1][0] - 3;
              } else if (CQI_Offset <= -4) {
                Feedback[ue_cell_idx].WidebandCQI[1][1] = Feedback[ue_cell_idx].WidebandCQI[1][0] + 4;
              }
            }
          }
          //SubbandCQI
          SubbandCQI.resize(codeword_num, Feedback[ue_cell_idx].N_Subband);
          SubbandSNR.resize(codeword_num, Feedback[ue_cell_idx].N_Subband);
          for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
            for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
              if (2 == codeword_num) {
                vv_start = (0 == cw_idx) ? 0 : floor((float)(RI_idx + 1) / 2.0f);
                N_L_1cw = (0 == cw_idx) ? floor((float)(RI_idx + 1) / 2.0f) : ceil((float)(RI_idx + 1) / 2.0f);
              } else {
                vv_start = 0;
                N_L_1cw = (RI_idx + 1);
              }
              for (CQI_idx = 14; CQI_idx >= 0; CQI_idx--) {
                SNR_eff = 0.0f;
                for (vv = 0; vv < N_L_1cw; vv++) {
                  PMI_idx = Feedback[ue_cell_idx].Subband_PMI(RI_idx, subband_idx);
                  SNR_eff += MyExp((-MEA[ue_cell_idx].Subband_SNR[RI_idx](PMI_idx, (vv + vv_start) * Feedback[ue_cell_idx].N_Subband + subband_idx) / CQI_Beta[CQI_idx]));
                }
                SNR_eff = -CQI_Beta[CQI_idx] * MyLog(SNR_eff / (float)N_L_1cw);
                if (SNR_eff > CQI_Th[CQI_idx]) {
                  SubbandCQI(cw_idx, subband_idx) = CQI_idx + 1;
                  SubbandSNR(cw_idx, subband_idx) = SNR_eff;
                  break;
                }
              }
            }
          }
          if (UE_Specific[ue_cell_idx].ReportOnPUSCH &&
              (RM30 == UE_Specific[ue_cell_idx].ReportMode || RM31 == UE_Specific[ue_cell_idx].ReportMode || RM32 == UE_Specific[ue_cell_idx].ReportMode)) {
            for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                CQI_Offset = SubbandCQI(cw_idx, subband_idx) - Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx];  //from 3GPP 36.213 table 7.2.1-2
                if (CQI_Offset >= 2) {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] + 2;
                } else if (CQI_Offset <= -1) {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] - 1;
                } else {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] + CQI_Offset;
                }
              }
            }
            if (2 == codeword_num && RM30 == UE_Specific[ue_cell_idx].ReportMode) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                Feedback[ue_cell_idx].SubbandCQI[1](1, subband_idx) = Feedback[ue_cell_idx].SubbandCQI[1](0, subband_idx);
              }
            }
            for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
              max_idx = 0;
              max_value = 0.0f;
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                if (max_value < (float)Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx)) {
                  max_value = (float)Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx);
                  max_idx = subband_idx;
                }
              }
              Feedback[ue_cell_idx].Perfered_M_idx(cw_idx, 0) = max_idx;
            }
          } else if (UE_Specific[ue_cell_idx].ReportOnPUSCH &&
                     (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode)) {
            for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
              SortValue.resize(Feedback[ue_cell_idx].N_Subband);
              SortIdx.resize(Feedback[ue_cell_idx].N_Subband);
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                SortValue(subband_idx) = SubbandSNR(cw_idx, subband_idx);
              }
              BubbleSortIdx(SortValue, SortIdx);
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].Perfered_M; subband_idx++) {
                Feedback[ue_cell_idx].Perfered_M_idx(cw_idx, subband_idx) = SortIdx(Feedback[ue_cell_idx].N_Subband - 1 - subband_idx);
              }
            }
            for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                CQI_Offset = SubbandCQI(cw_idx, subband_idx) - Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx];  //from 3GPP 36.213 table 7.2.1-4
                if (CQI_Offset <= 1) {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx];
                } else if (CQI_Offset >= 4) {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] + 4;
                } else {
                  Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx) =
                      Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx] + CQI_Offset;
                }
              }
            }
            if (2 == codeword_num && RM20 == UE_Specific[ue_cell_idx].ReportMode) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                Feedback[ue_cell_idx].SubbandCQI[1](1, subband_idx) = Feedback[ue_cell_idx].SubbandCQI[1](0, subband_idx);
              }
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].Perfered_M; subband_idx++) {
                Feedback[ue_cell_idx].Perfered_M_idx(1, subband_idx) = Feedback[ue_cell_idx].Perfered_M_idx(0, subband_idx);
              }
            }
          } else if (!UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM21 == UE_Specific[ue_cell_idx].ReportMode)) {
            for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
              max_idx = 0;
              max_value = 0.0f;
              for (subband_idx = (Feedback[ue_cell_idx].N_j * Feedback[ue_cell_idx].J); subband_idx < MIN(Feedback[ue_cell_idx].N_Subband, (Feedback[ue_cell_idx].N_j + 1) * Feedback[ue_cell_idx].J); subband_idx++) {
                if (max_value < SubbandSNR(cw_idx, subband_idx)) {
                  max_value = SubbandSNR(cw_idx, subband_idx);
                  max_idx = subband_idx;
                }
              }
              Feedback[ue_cell_idx].Perfered_M_idx(cw_idx, 0) = max_idx;
            }
          }
          buffer_idx = Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer_idx;
          Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Flag[buffer_idx] = 1;
          Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[1][buffer_idx] = -1;
          for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
            Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Report_Buffer[1][subband_idx][buffer_idx] = -1;
          }
          for (cw_idx = 0; cw_idx < codeword_num; cw_idx++) {
            Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[cw_idx][buffer_idx] = Feedback[ue_cell_idx].WidebandCQI[codeword_num - 1][cw_idx];
            for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
              Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Report_Buffer[cw_idx][subband_idx][buffer_idx] = Feedback[ue_cell_idx].SubbandCQI[codeword_num - 1](cw_idx, subband_idx);
            }
            if (UE_Specific[ue_cell_idx].ReportOnPUSCH &&
                (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode)) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].Perfered_M; subband_idx++) {
                Cell[cell_idx].Feedback[cell_ue_idx].Perfered_M_idx_Report_Buffer[cw_idx][subband_idx][buffer_idx] = Feedback[ue_cell_idx].Perfered_M_idx(cw_idx, subband_idx);
              }
            }
            if ((!UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM21 == UE_Specific[ue_cell_idx].ReportMode)) || (UE_Specific[ue_cell_idx].ReportOnPUSCH &&
                                                                                                                                                              (RM30 == UE_Specific[ue_cell_idx].ReportMode || RM31 == UE_Specific[ue_cell_idx].ReportMode || RM32 == UE_Specific[ue_cell_idx].ReportMode))) {
              Cell[cell_idx].Feedback[cell_ue_idx].Perfered_M_idx_Report_Buffer[cw_idx][0][buffer_idx] = Feedback[ue_cell_idx].Perfered_M_idx(cw_idx, 0);
            }
          }
          Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Gen_n_subframe[buffer_idx] = Simuctrl.n_subframe;
          Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer_idx = (Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer_idx + 1) % MAX_Feedback_BufferSize;
        }
        if (-1 != UE_Specific[ue_cell_idx].N_offset_CQI_PMI && 0 == ((n_subframe - UE_Specific[ue_cell_idx].N_offset_CQI_PMI) % UE_Specific[ue_cell_idx].N_pd_CQI_PMI) && (U == Cell_Specific[ue_cell_idx].SubframeType || DandU == Cell_Specific[ue_cell_idx].SubframeType)) {
          nearest_value = 10;
          nearest_buffer_idx = 0;
          for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
            diff_value = Simuctrl.n_subframe - Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Gen_n_subframe[buffer_idx];
            if (1 == Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Flag[buffer_idx] && diff_value >= 4 && nearest_value > diff_value) {
              nearest_value = diff_value;
              nearest_buffer_idx = buffer_idx;
            }
          }
          if (nearest_value < 10) {
            Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Flag[nearest_buffer_idx] = 2;
            Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Gen_n_subframe[nearest_buffer_idx] = Simuctrl.n_subframe;
            if (!UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM21 == UE_Specific[ue_cell_idx].ReportMode)) {
              Feedback[ue_cell_idx].N_j = (Feedback[ue_cell_idx].N_j + 1) % Feedback[ue_cell_idx].J;
            }
            sprintf(c_string, "%s/%s_%s_WidebandCQI%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), 0);
            WithFile(c_string);
            FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[0][nearest_buffer_idx]);
            if (-1 != Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[1][nearest_buffer_idx]) {
              sprintf(c_string, "%s/%s_%s_WidebandCQI%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), 1);
              WithFile(c_string);
              FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[1][nearest_buffer_idx]);
            }
            sprintf(c_string, "%s/%s_%s_SubbandCQI%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), 0);
            WithFile(c_string);
            for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
              FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Report_Buffer[0][subband_idx][nearest_buffer_idx]);
            }
            if (-1 != Cell[cell_idx].Feedback[cell_ue_idx].WidebandCQI_Report_Buffer[1][nearest_buffer_idx]) {
              sprintf(c_string, "%s/%s_%s_SubbandCQI%d", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str(), 1);
              WithFile(c_string);
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                FilePrint("%d\n", Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Report_Buffer[1][subband_idx][nearest_buffer_idx]);
              }
            }
          }
        }
      }
    }
  }
  //AFC
  if (Simuctrl.AFC_Flag) {
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
      if (ServingCell == UE_Specific[ue_cell_idx].CellType && Cell_Specific[ue_cell_idx].OverSample_DataLen > 0 && CHE[ue_cell_idx].CRS_Num[0] >= 3) {
        Feedback[ue_cell_idx].AccFreqOffset += MEA[ue_cell_idx].Freq;
        Feedback[ue_cell_idx].AccFreqOffsetLen++;
        if (1 == (Simuctrl.n_subframe % 10) &&
            ((MyFabs(Feedback[ue_cell_idx].AccFreqOffset) > (100.0f * (float)Feedback[ue_cell_idx].AccFreqOffsetLen) && Feedback[ue_cell_idx].AccFreqOffsetLen > 10) || (MyFabs(Feedback[ue_cell_idx].AccFreqOffset) > (50.0f * (float)Feedback[ue_cell_idx].AccFreqOffsetLen) && Feedback[ue_cell_idx].AccFreqOffsetLen > 20) || Feedback[ue_cell_idx].AccFreqOffsetLen > 50)) {
          Feedback[ue_cell_idx].AccFreqOffset /= (float)Feedback[ue_cell_idx].AccFreqOffsetLen;
          Feedback[ue_cell_idx].RxFreqOffset = (float)round(Feedback[ue_cell_idx].RxFreqOffset - Feedback[ue_cell_idx].AccFreqOffset);
          //Feedback[ue_cell_idx].RxFreqOffset = (float) (Feedback[ue_cell_idx].RxFreqOffset - Feedback[ue_cell_idx].AccFreqOffset);
          Feedback[ue_cell_idx].AccFreqOffset = 0.0f;
          Feedback[ue_cell_idx].AccFreqOffsetLen = 0;
        }
      }
      if (ServingCell == UE_Specific[ue_cell_idx].CellType && Cell_Specific[ue_cell_idx].OverSample_DataLen > 0) {
        sprintf(c_string, "CellIdx%d", ue_cell_idx);
        FilesPathUE_Cell = c_string;
        sprintf(c_string, "%s/%s_%s_AFC", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
        WithFile(c_string);
        FilePrint("n_subframe=%d, RxFreqOffset[%d]=%f\n", Simuctrl.n_subframe, ue_cell_idx, Feedback[ue_cell_idx].RxFreqOffset);
      }
    }
  }
  //ATC only on ue_cell_idx=0
  if (Simuctrl.ATC_Flag) {
    if (ServingCell == UE_Specific[0].CellType && Cell_Specific[0].OverSample_DataLen > 0) {
      Feedback[0].AccIRT += MEA[0].IRT_Final;
      Feedback[0].AccIRTLen++;
      if (1 == (Simuctrl.n_subframe % 10) &&
          ((abs(Feedback[0].AccIRT) > (10 * Feedback[0].AccIRTLen) && Feedback[0].AccIRTLen > 10) || (abs(Feedback[0].AccIRT) > (5 * Feedback[0].AccIRTLen) && Feedback[0].AccIRTLen > 20) || Feedback[0].AccIRTLen > 50)) {
        Feedback[0].AccIRT = round((float)Feedback[0].AccIRT / (float)Feedback[0].AccIRTLen);
        TA = MIN(TA + Feedback[0].AccIRT, TA_Target);
        TA = MAX(TA + Feedback[0].AccIRT, -MAXTASAMPLES + (RC_FIR_LEN >> 1));
        Feedback[0].AccIRT = 0;
        Feedback[0].AccIRTLen = 0;
      }
      sprintf(c_string, "%s/%s_ATC", FilesPathIter.c_str(), FilesPathUE.c_str());
      WithFile(c_string);
      FilePrint("n_subframe=%d, TA=%d\n", Simuctrl.n_subframe, TA);
    }
  }
  //CRC
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    sprintf(c_string, "CellIdx%d", ue_cell_idx);
    FilesPathUE_Cell = c_string;
    sprintf(c_string, "%s/%s_%s_Log", FilesPathIter.c_str(), FilesPathUE.c_str(), FilesPathUE_Cell.c_str());
    WithFile(c_string);
    DCI_idx = UE_Specific[ue_cell_idx].DCI_idx;
    cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
    cell_ue_idx = UE_Connect_Cell_innerUE_idx[ue_cell_idx];
    if (UE_Specific[ue_cell_idx].RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific[ue_cell_idx].SubframeType) {
      for (cw_idx = 0; cw_idx < UE_Specific[ue_cell_idx].codeword_num[DCI_idx]; cw_idx++) {
        Cell[cell_idx].ReTransmit[cell_ue_idx].CRC[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID] = ReTransmit[ue_cell_idx].CRC[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID];
        FilePrint("  ReTransmit: TxNum[%d] = %d\n", cw_idx, Cell[cell_idx].ReTransmit[cell_ue_idx].TxNum[cw_idx][UE_Specific[ue_cell_idx].HARQ_ID]);
      }
    }
    if (ServingCell == UE_Specific[ue_cell_idx].CellType && Cell_Specific[ue_cell_idx].OverSample_DataLen > 0) {
      FilePrint("\n");
    }
  }
}

void UE_C::GetMedianCQI(int ue_idx, std::vector<Cell_C> &Cell) {
  int cell_ue_idx = 0;
  int ue_cell_idx = 0;
  int cell_idx = 0;
  int cw_idx = 0;
  float BLER = 0.0f;
  int subband_idx = 0;
  float gamma = 0.0f;
  float max_value = 0.0f;
  for (ue_cell_idx = 0; ue_cell_idx < (int)UE_Specific.size(); ue_cell_idx++) {
    if (Ref_Median_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Non_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type || Ref_UE_Selected_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
      WithFile("Log");
      if (Simuctrl.CQI_Feedback_Flag) {
        FilePrint("ue_idx=%d, ue_cell_idx=%d, loop=%d, SNR=%f\n", ue_idx, ue_cell_idx, loop, Result[ue_cell_idx].SNR(loop));
      }
      cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
      cell_ue_idx = UE_Connect_Cell_innerUE_idx[ue_cell_idx];
      for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
        if (Result[ue_cell_idx].BLER_PDSCH_perCW[cw_idx] > 0) {
          if (Ref_Median_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
            if (Simuctrl.CQI_Feedback_Flag && Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Rate[cw_idx] <= 0.9f) {
              FilePrint("  cw_idx = %d, Median_CQI = %d, Median_CQI(+/-1) Rate = %f\n", cw_idx, Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI[cw_idx] + 1, Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Rate[cw_idx]);
              Result[ue_cell_idx].Feedback_Test_Flag = false;
            } else if (!Simuctrl.CQI_Feedback_Flag) {
              BLER = (float)Result[ue_cell_idx].BLER_PDSCH_perCW[cw_idx] / (float)Result[ue_cell_idx].BLER_PDSCH_perCW_Num[cw_idx];
              FilePrint("  cw_idx = %d, CQI = %d, BLER = %f\n", cw_idx, Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI[cw_idx] + Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] + 1, BLER);
              if (0 == Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx]) {
                if (BLER <= 0.1f) {
                  Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] = 1;
                } else {
                  Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] = -1;
                }
              } else {
                if (1 == Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] && BLER <= 0.1f) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                } else if (-1 == Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Delta[cw_idx] && BLER > 0.1f) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                }
              }
            }
          } else if (Ref_Freq_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type && UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM30 == UE_Specific[ue_cell_idx].ReportMode || RM31 == UE_Specific[ue_cell_idx].ReportMode || RM32 == UE_Specific[ue_cell_idx].ReportMode)) {
            if (Simuctrl.CQI_Feedback_Flag) {
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                FilePrint("  cw_idx = %d, subband_idx = %d, SubbandCQI_Offset_Rate = %f\n", cw_idx, subband_idx, Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx]);
                if (Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx] < Result[ue_cell_idx].Ref_Alpha || Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx] >= Result[ue_cell_idx].Ref_Beta) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                }
              }
              BLER = (float)Result[ue_cell_idx].BLER_PDSCH_perCW[cw_idx] / (float)Result[ue_cell_idx].BLER_PDSCH_perCW_Num[cw_idx];
              FilePrint("  cw_idx = %d, BLER = %f\n", cw_idx, BLER);
              if (BLER < 0.05f) {
                Result[ue_cell_idx].Feedback_Test_Flag = false;
              }
            } else {
              if (0 == cw_idx) {
                gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
                FilePrint("  gamma = %f\n", gamma);
                if (gamma < Result[ue_cell_idx].Ref_Gamma) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                }
              }
            }
          } else if (Ref_Freq_Non_Select_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type && !UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM10 == UE_Specific[ue_cell_idx].ReportMode || RM11 == UE_Specific[ue_cell_idx].ReportMode)) {
            if (Simuctrl.CQI_Feedback_Flag && Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Rate[cw_idx] < Result[ue_cell_idx].Ref_Alpha) {
              FilePrint("  cw_idx = %d, Median_CQI = %d, Median_CQI(+/-1) Rate = %f\n", cw_idx, Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI[cw_idx] + 1, Cell[cell_idx].Feedback[cell_ue_idx].Median_CQI_Rate[cw_idx]);
              Result[ue_cell_idx].Feedback_Test_Flag = false;
            } else if (!Simuctrl.CQI_Feedback_Flag) {
              if (0 == cw_idx) {
                gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
                FilePrint("  gamma = %f\n", gamma);
                if (gamma < Result[ue_cell_idx].Ref_Gamma) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                }
              }
            }
          } else if (Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type && UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM30 == UE_Specific[ue_cell_idx].ReportMode || RM31 == UE_Specific[ue_cell_idx].ReportMode || RM32 == UE_Specific[ue_cell_idx].ReportMode)) {
            if (Simuctrl.CQI_Feedback_Flag) {
              max_value = 0.0f;
              for (subband_idx = 0; subband_idx < Feedback[ue_cell_idx].N_Subband; subband_idx++) {
                if (max_value < Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx]) {
                  max_value = Cell[cell_idx].Feedback[cell_ue_idx].SubbandCQI_Offset_Rate[cw_idx][subband_idx];
                }
              }
              FilePrint("  cw_idx = %d, SubbandCQI_Offset_Rate = %f\n", cw_idx, max_value);
              if (max_value < Result[ue_cell_idx].Ref_Alpha) {
                Result[ue_cell_idx].Feedback_Test_Flag = false;
              }
            } else {
              if (0 == cw_idx) {
                gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
                FilePrint("  gamma = %f\n", gamma);
                if (gamma < Result[ue_cell_idx].Ref_Gamma) {
                  Result[ue_cell_idx].Feedback_Test_Flag = false;
                }
              }
            }
          }
        } else if (Ref_UE_Selected_CQI_Reported == UE_Specific[ue_cell_idx].Ref_Type && UE_Specific[ue_cell_idx].ReportOnPUSCH && (RM20 == UE_Specific[ue_cell_idx].ReportMode || RM22 == UE_Specific[ue_cell_idx].ReportMode)) {
          if (!Simuctrl.CQI_Feedback_Flag) {
            if (0 == cw_idx) {
              gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
              FilePrint("  gamma = %f\n", gamma);
              if (gamma < Result[ue_cell_idx].Ref_Gamma) {
                Result[ue_cell_idx].Feedback_Test_Flag = false;
              }
            }
          }
        }
      }
    } else if (Ref_PMI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
      WithFile("Log");
      if (!Simuctrl.Random_BF_Flag) {
        FilePrint("ue_idx=%d, ue_cell_idx=%d, loop=%d, SNR=%f\n", ue_idx, ue_cell_idx, loop, Result[ue_cell_idx].SNR(loop));
      } else {
        gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
        FilePrint("  gamma = %f\n", gamma);
        if (gamma < Result[ue_cell_idx].Ref_Gamma) {
          Result[ue_cell_idx].Feedback_Test_Flag = false;
        }
      }
    } else if (Ref_RI_Reported == UE_Specific[ue_cell_idx].Ref_Type) {
      WithFile("Log");
      if (!Simuctrl.RI_Feedback_Flag) {
        FilePrint("ue_idx=%d, ue_cell_idx=%d, loop=%d, SNR=%f\n", ue_idx, ue_cell_idx, loop, Result[ue_cell_idx].SNR(loop));
        gamma = Result[ue_cell_idx].PDSCH_Throughput[loop - 1] / Result[ue_cell_idx].PDSCH_Throughput[loop];
        FilePrint("  gamma = %f\n", gamma);
        if (gamma < Result[ue_cell_idx].Ref_Gamma) {
          Result[ue_cell_idx].Feedback_Test_Flag = false;
        }
      }
    }
    FilePrint("\n");
  }
}
