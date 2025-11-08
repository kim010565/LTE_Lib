/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Simulation.cpp
    \brief Simulation实现文件
    内容 ：Simulation_C类相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Simulation/Simulation.h"

//! Simulation_C类构造函数。初始化输出路径为当前路径；仿真长度为1 TTI；仿真迭代次数为1
/*!
  \return 无返回值
*/
Simulation_C::Simulation_C() {
  outpath = ".";
  runlength = 1;
  iter_num = 1;
}

//! Simulation_C类析构函数
/*!
  \return 无返回值
*/
Simulation_C::~Simulation_C() {
}

//! Simulation_C类参数解析函数。-i表示输入文件；-o表示输出路径；-s表示仿真长度缩放比例
/*!
  \param argc ： main函数参数个数，输入
  \param argv ： main函数参数列表，输入
  \return 无返回值
*/
void Simulation_C::ParseArgs(int argc, char **argv) {
  int ch = 0;
  string str;
  bool i_flag = false;
  short_scale_factor = 1;
  while ((ch = getopt(argc, argv, "i:o:s:")) != EOF) {
    switch (ch) {
      case 'i':
        infile = optarg;
        Parser.LoadFile(infile);  //解析配置文件
        iter_num = Parser.GetIterNum();
        i_flag = true;
        break;
      case 'o':
        outpath = optarg;
        if (-1 == access(outpath.c_str(), 0)) {
          str = "mkdir " + outpath;
          system(str.c_str());
        }
        FilesPath = outpath;
        break;
      case 's':
        short_scale_factor = MAX(atoi(optarg), 1);
        break;
      default:
        Error("Error para: %c", (char)ch);
    }
  }
  if (!i_flag) {
    Error("There is no -i opt!");
  }
}

//! 获取仿真迭代次数
/*!
  \return 仿真迭代次数
*/
int Simulation_C::GetIterNum() {
  return iter_num;
}

//! 获取第iter_idx次迭代下的仿真参数
/*!
  \param iter_idx ： 第iter_idx次迭代，输入
  \return 无返回值
*/
void Simulation_C::SetSimulationParas(int iter_idx) {
  int i = 0;
  int j = 0;
  int cw_idx = 0;
  int rb_idx = 0;
  int cell_idx = 0;
  int cell_ue_idx = 0;
  int ue_idx = 0;
  int ue_cell_idx = 0;
  int cell_num = 0;
  int ue_num = 0;
  int connect_cell_num = 0;
  int paras_num = 0;
  int CellID = 0;
  vector<vector<int>> UE_Connect_Cell_idx;
  vector<vector<int>> Cell_Connect_UE_idx;
  vector<vector<int>> UE_Connect_Cell_innerUE_idx;
  vector<vector<int>> Cell_Connect_UE_innerCell_idx;
  Cell_Specific_S Cell_Specific;
  UE_Specific_S UE_Specific;
  Channel_Specific_S Channel_Specific;
  int MAX_N_FFT = 0;
  char tmp_str[256];
  vector<int> x;
  cell_num = Parser.ReadValue("CellNum", iter_idx, 0, 0, 0, 0, 1);
  ue_num = Parser.ReadValue("UeNum", iter_idx, 0, 0, 0, 0, 1);
  Cell.resize(cell_num);
  Channel.resize(ue_num);
  UE.resize(ue_num);
  //SimuCtrl Paras
  Simuctrl.RC_FIR_Flag = (bool)Parser.ReadValue("RC_FIR_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.AGC_Flag = (bool)Parser.ReadValue("AGC_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.ADC_Len = (int)Parser.ReadValue("ADC_Len", iter_idx, 0, 0, 0, 0, 11);
  Simuctrl.ADC_AVEPower = round((float)(1 << (2 * (Simuctrl.ADC_Len - 1) + 1)) / PAPR);
  Simuctrl.ADC_MAX = (1 << (Simuctrl.ADC_Len - 1)) - 1;
  Simuctrl.ADC_MIN = -(1 << (Simuctrl.ADC_Len - 1));
  Simuctrl.AFC_Flag = (bool)Parser.ReadValue("AFC_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.ATC_Flag = (bool)Parser.ReadValue("ATC_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PSS_SSS_DTFlag = (bool)Parser.ReadValue("PSS_SSS_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PBCH_DTFlag = (bool)Parser.ReadValue("PBCH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PCFICH_DTFlag = (bool)Parser.ReadValue("PCFICH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PDCCH_DTFlag = (bool)Parser.ReadValue("PDCCH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PHICH_DTFlag = (bool)Parser.ReadValue("PHICH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.EPDCCH_DTFlag = (bool)Parser.ReadValue("EPDCCH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PDSCH_DTFlag = (bool)Parser.ReadValue("PDSCH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.PMCH_DTFlag = (bool)Parser.ReadValue("PMCH_DTFlag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.BLIND_PBCH_DT_Flag = (bool)Parser.ReadValue("BLIND_PBCH_DT_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.CQI_Feedback_Flag = (bool)Parser.ReadValue("CQI_Feedback_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.RI_Feedback_Flag = (bool)Parser.ReadValue("RI_Feedback_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.Fixed_RI = (int)Parser.ReadValue("Fixed_RI", iter_idx, 0, 0, 0, 0, 1) - 1;
  Simuctrl.PMI_Feedback_Flag = (bool)Parser.ReadValue("PMI_Feedback_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.RML_Flag = (bool)Parser.ReadValue("RML_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.TD_RML_Flag = (bool)Parser.ReadValue("TD_RML_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.CQI_Th_Adapt = (bool)Parser.ReadValue("CQI_Th_Adapt", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.Random_Selected_Subband = (bool)Parser.ReadValue("Random_Selected_Subband", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.Random_BF_Flag = (bool)Parser.ReadValue("Random_BF_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.With_Simultaneous_Transmission = (bool)Parser.ReadValue("With_Simultaneous_Transmission", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.Inject_Flag = (bool)Parser.ReadValue("Inject_Flag", iter_idx, 0, 0, 0, 0, FALSE);
  Simuctrl.start_n_subframe = (int)Parser.ReadValue("start_n_subframe", iter_idx, 0, 0, 0, 0, 0);
  Simuctrl.runlength = (int)Parser.ReadValue("runlength", iter_idx, 0, 0, 0, 0, 1) / short_scale_factor;
  for (cell_idx = 0; cell_idx < cell_num; cell_idx++) {
    Cell[cell_idx].Simuctrl = Simuctrl;
  }
  for (ue_idx = 0; ue_idx < ue_num; ue_idx++) {
    Channel[ue_idx].Simuctrl = Simuctrl;
    UE[ue_idx].Simuctrl = Simuctrl;
  }
  //Cell Paras
  for (cell_idx = 0; cell_idx < cell_num; cell_idx++) {
    //Common
    Cell_Specific.CellID = (int)Parser.ReadValue("CellID", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.FrameStructureType = FrameStructureType_Str(Parser.ReadString("FrameStructureType", iter_idx, 0, cell_idx, 0, 0, "FDD"));
    Cell_Specific.HARQ_ProcNum = (int)Parser.ReadValue("HARQ_ProcNum", iter_idx, 0, cell_idx, 0, 0, 8);
    Cell_Specific.MaxTxNum = (int)Parser.ReadValue("MaxTxNum", iter_idx, 0, cell_idx, 0, 0, 4);
    Cell_Specific.RV_QPSK.resize(4);
    Cell_Specific.RV_16QAM.resize(4);
    Cell_Specific.RV_64QAM.resize(4);
    Cell_Specific.RV_256QAM.resize(4);
    for (i = 0; i < 4; i++) {
      Cell_Specific.RV_QPSK[i] = (int)Parser.ReadValue("RV_QPSK", iter_idx, 0, cell_idx, 0, i, 0);
      Cell_Specific.RV_16QAM[i] = (int)Parser.ReadValue("RV_16QAM", iter_idx, 0, cell_idx, 0, i, 0);
      Cell_Specific.RV_64QAM[i] = (int)Parser.ReadValue("RV_64QAM", iter_idx, 0, cell_idx, 0, i, 0);
      Cell_Specific.RV_256QAM[i] = (int)Parser.ReadValue("RV_256QAM", iter_idx, 0, cell_idx, 0, i, 0);
    }
    Cell_Specific.EVM = (float)Parser.ReadValue("EVM", iter_idx, 0, cell_idx, 0, 0, 0.03);
    Cell_Specific.Ctrl_Symb_Num = (int)Parser.ReadValue("Ctrl_Symb_Num", iter_idx, 0, cell_idx, 0, 0, 1);
    Cell_Specific.Ctrl_Symb_Num_Special = (int)Parser.ReadValue("Ctrl_Symb_Num_Special", iter_idx, 0, cell_idx, 0, 0, 1);
    //MIB
    Cell_Specific.dl_BandWidth = BandWidth_Str(Parser.ReadString("dl_BandWidth", iter_idx, 0, cell_idx, 0, 0, "20M"));
    Cell_Specific.phich_Duration = PHICH_Duration_Type_Str(Parser.ReadString("phich_Duration", iter_idx, 0, cell_idx, 0, 0, "normal"));
    Cell_Specific.phich_Resource = PHICH_Resource_Type_Str(Parser.ReadString("phich_Resource", iter_idx, 0, cell_idx, 0, 0, "1"));
    Cell_Specific.SFN_High8bit = (int)Parser.ReadValue("SFN_High8bit", iter_idx, 0, cell_idx, 0, 0, 0);
    //SIB1
    Cell_Specific.ULDLConfiguration = (ULDLConfiguration_E)Parser.ReadValue("ULDLConfiguration", iter_idx, 0, cell_idx, 0, 0, 2);
    Cell_Specific.SpecialSubframeConfiguration = (SpecialSubframeConfiguration_E)Parser.ReadValue("SpecialSubframeConfiguration", iter_idx, 0, cell_idx, 0, 0, 7);
    //SIB2
    Cell_Specific.p_b = (int)Parser.ReadValue("p_b", iter_idx, 0, cell_idx, 0, 0, 0);
    //PSS_SSS_DT
    Cell_Specific.CP_Type = CP_Type_Str(Parser.ReadString("CP_Type", iter_idx, 0, cell_idx, 0, 0, "normal"));
    Cell_Specific.N_cell_ID = (int)Parser.ReadValue("N_cell_ID", iter_idx, 0, cell_idx, 0, 0, 0);
    //PBCH_DT
    Cell_Specific.CRSPortNum = (int)Parser.ReadValue("CRSPortNum", iter_idx, 0, cell_idx, 0, 0, 1);
    Cell_Specific.SFN_Low2bit = (int)Parser.ReadValue("SFN_Low2bit", iter_idx, 0, cell_idx, 0, 0, 0);
    //Other
    Cell_Specific.TxFreqOffset = (float)Parser.ReadValue("TxFreqOffset", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PBCH_RA = (float)Parser.ReadValue("PBCH_RA", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PBCH_RB = (float)Parser.ReadValue("PBCH_RB", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PCFICH_RA = (float)Parser.ReadValue("PCFICH_RA", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PCFICH_RB = (float)Parser.ReadValue("PCFICH_RB", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PHICH_RA = (float)Parser.ReadValue("PHICH_RA", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PHICH_RB = (float)Parser.ReadValue("PHICH_RB", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PDCCH_RA = (float)Parser.ReadValue("PDCCH_RA", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PDCCH_RB = (float)Parser.ReadValue("PDCCH_RB", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PDSCH_RA = (float)Parser.ReadValue("PDSCH_RA", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell_Specific.PDSCH_RB = (float)Parser.ReadValue("PDSCH_RB", iter_idx, 0, cell_idx, 0, 0, 0);
    Cell[cell_idx].Cell_Specific = Cell_Specific;
  }
  //UE Paras
  for (ue_idx = 0; ue_idx < ue_num; ue_idx++) {
    connect_cell_num = Parser.Getl2Num("Connected_CellID", iter_idx, ue_idx);
    UE[ue_idx].UE_Specific.resize(connect_cell_num);
    UE[ue_idx].Cell_Specific.resize(connect_cell_num);
    for (ue_cell_idx = 0; ue_cell_idx < connect_cell_num; ue_cell_idx++) {
      //Common
      UE_Specific.UEID = (int)Parser.ReadValue("UEID", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.UE_Category = (UE_Category_E)Parser.ReadValue("UE_Category", iter_idx, ue_idx, ue_cell_idx, 0, 0, 4);
      UE_Specific.RNTI = (int)Parser.ReadValue("C_RNTI", iter_idx, ue_idx, ue_cell_idx, 0, 0, 61);
      UE_Specific.RNTI_Type = RNTI_Type_Str(Parser.ReadString("RNTI_Type", iter_idx, ue_idx, ue_cell_idx, 0, 0, "C_RNTI"));
      //RRC
      UE_Specific.p_a = (float)Parser.ReadValue("p_a", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.TM = (TM_E)Parser.ReadValue("TM", iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
      UE_Specific.UL_DCI_Format = DCI_Format_Type_Str(Parser.ReadString("UL_DCI_Format", iter_idx, ue_idx, ue_cell_idx, 0, 0, "NULL_DCI"), NULL_TM);
      UE_Specific.alternativeCodeBookEnabledFor4TX_r12 = (bool)Parser.ReadValue("alternativeCodeBookEnabledFor4TX_r12", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.UE_Transmit_Selection = (bool)Parser.ReadValue("UE_Transmit_Selection", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.N_pd_CQI_PMI = (int)Parser.ReadValue("N_pd_CQI_PMI", iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
      UE_Specific.N_pd_RI = (int)Parser.ReadValue("N_pd_RI", iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
      UE_Specific.N_offset_CQI_PMI = (int)Parser.ReadValue("N_offset_CQI_PMI", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.N_offset_RI = (int)Parser.ReadValue("N_offset_RI", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.I_CQI_PMI = (int)Parser.ReadValue("I_CQI_PMI", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.I_RI = (int)Parser.ReadValue("I_RI", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.ReportMode = ReportModeType_Str(Parser.ReadString("ReportMode", iter_idx, ue_idx, ue_cell_idx, 0, 0, "RM10"));
      UE_Specific.ReportOnPUSCH = (bool)Parser.ReadValue("ReportOnPUSCH", iter_idx, ue_idx, ue_cell_idx, 0, 0, FALSE);
      UE_Specific.PMI_RI_Report = (bool)Parser.ReadValue("PMI_RI_Report", iter_idx, ue_idx, ue_cell_idx, 0, 0, FALSE);
      UE_Specific.n_DMRS_ID[0] = (int)Parser.ReadValue("n_DMRS_ID0", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.n_DMRS_ID[1] = (int)Parser.ReadValue("n_DMRS_ID1", iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
      UE_Specific.codebookSubsetRestriction[0] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 0, 65535);  //0~15 bit
      UE_Specific.codebookSubsetRestriction[1] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 1, 65535);  //16~31 bit
      UE_Specific.codebookSubsetRestriction[2] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 2, 65535);  //32~47 bit
      UE_Specific.codebookSubsetRestriction[3] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 3, 65535);  //48~63 bit
      UE_Specific.codebookSubsetRestriction[4] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 4, 65535);  //64~79 bit
      UE_Specific.codebookSubsetRestriction[5] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 5, 65535);  //80~95 bit
      UE_Specific.codebookSubsetRestriction[6] = (int)Parser.ReadValue("codebookSubsetRestriction", iter_idx, ue_idx, ue_cell_idx, 0, 6, 65535);  //96~112 bit
      //RRC EPDCCH
      UE_Specific.EPDCCH_Set_Num = (int)Parser.ReadValue("EPDCCH_Set_Num", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.startSymb_EPDCCH = (int)Parser.ReadValue("startSymb_EPDCCH", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.EPDCCH_Period = (int)Parser.ReadValue("EPDCCH_Period", iter_idx, ue_idx, ue_cell_idx, 0, 0, 10);
      UE_Specific.subframePattern_EPDCCH.resize(UE_Specific.EPDCCH_Period);
      UE_Specific.RB_Assignment_EPDCCH.resize(UE_Specific.EPDCCH_Set_Num);
      UE_Specific.TMType_EPDCCH.resize(UE_Specific.EPDCCH_Set_Num);
      UE_Specific.numberPRB_Pairs_EPDCCH.resize(UE_Specific.EPDCCH_Set_Num);
      UE_Specific.resourceBlockAssignment_EPDCCH.resize(UE_Specific.EPDCCH_Set_Num);
      UE_Specific.n_EPDCCH_ID.resize(UE_Specific.EPDCCH_Set_Num);
      for (i = 0; i < UE_Specific.EPDCCH_Set_Num; i++) {
        sprintf(tmp_str, "RB_Assignment_EPDCCH%d", i);
        paras_num = Parser.Getl0Num(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0);
        UE_Specific.RB_Assignment_EPDCCH[i].resize(paras_num);
        for (rb_idx = 0; rb_idx < paras_num; rb_idx++) {
          UE_Specific.RB_Assignment_EPDCCH[i][rb_idx] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, rb_idx, rb_idx);
        }
        sprintf(tmp_str, "TMType_EPDCCH%d", i);
        UE_Specific.TMType_EPDCCH[i] = TMType_EPDCCH_Str(Parser.ReadString(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, "Localized_EPDCCH"));
        sprintf(tmp_str, "numberPRB_Pairs_EPDCCH%d", i);
        UE_Specific.numberPRB_Pairs_EPDCCH[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "resourceBlockAssignment_EPDCCH%d", i);
        UE_Specific.resourceBlockAssignment_EPDCCH[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        if (-1 == UE_Specific.resourceBlockAssignment_EPDCCH[i] && UE_Specific.RB_Assignment_EPDCCH[i].size() > 0) {
          UE_Specific.numberPRB_Pairs_EPDCCH[i] = UE_Specific.RB_Assignment_EPDCCH[i].size();
          x.resize(UE_Specific.numberPRB_Pairs_EPDCCH[i]);
          for (j = 0; j < UE_Specific.numberPRB_Pairs_EPDCCH[i]; j++) {
            x[j] = Cell_Specific.N_DL_RB - UE_Specific.RB_Assignment_EPDCCH[i][j];
          }
          UE_Specific.resourceBlockAssignment_EPDCCH[i] = combinatorial_idx(x);
        }
        sprintf(tmp_str, "n_EPDCCH_ID%d", i);
        UE_Specific.n_EPDCCH_ID[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      }
      for (i = 0; i < UE_Specific.EPDCCH_Period; i++) {
        UE_Specific.subframePattern_EPDCCH[i] = (int)Parser.ReadValue("subframePattern_EPDCCH", iter_idx, ue_idx, ue_cell_idx, 0, i, 1);
      }
      //RRC CSI
      UE_Specific.NZP_CSI_Set_Num = (int)Parser.ReadValue("NZP_CSI_Set_Num", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.I_CSI_RS_NZP.resize(UE_Specific.NZP_CSI_Set_Num);
      UE_Specific.CSIPortNum_NZP.resize(UE_Specific.NZP_CSI_Set_Num);
      UE_Specific.CSI_RS_Signal_Config_NZP.resize(UE_Specific.NZP_CSI_Set_Num);
      UE_Specific.N_CSI_ID_NZP.resize(UE_Specific.NZP_CSI_Set_Num);
      UE_Specific.CSI_Flag_NZP.resize(UE_Specific.NZP_CSI_Set_Num);
      for (i = 0; i < UE_Specific.NZP_CSI_Set_Num; i++) {
        sprintf(tmp_str, "I_CSI_RS_NZP%d", i);
        UE_Specific.I_CSI_RS_NZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "CSIPortNum_NZP%d", i);
        UE_Specific.CSIPortNum_NZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 8);
        sprintf(tmp_str, "CSI_RS_Signal_Config_NZP%d", i);
        UE_Specific.CSI_RS_Signal_Config_NZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "N_CSI_ID_NZP%d", i);
        UE_Specific.N_CSI_ID_NZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE_Specific.CSI_Flag_NZP[i] = false;
      }
      UE_Specific.ZP_CSI_Set_Num = (int)Parser.ReadValue("ZP_CSI_Set_Num", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.I_CSI_RS_ZP.resize(UE_Specific.ZP_CSI_Set_Num);
      UE_Specific.CSIPortNum_ZP.resize(UE_Specific.ZP_CSI_Set_Num);
      UE_Specific.CSI_RS_Signal_Config_ZP.resize(UE_Specific.ZP_CSI_Set_Num);
      UE_Specific.N_CSI_ID_ZP.resize(UE_Specific.ZP_CSI_Set_Num);
      UE_Specific.CSI_Flag_ZP.resize(UE_Specific.ZP_CSI_Set_Num);
      for (i = 0; i < UE_Specific.ZP_CSI_Set_Num; i++) {
        sprintf(tmp_str, "I_CSI_RS_ZP%d", i);
        UE_Specific.I_CSI_RS_ZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "CSIPortNum_ZP%d", i);
        UE_Specific.CSIPortNum_ZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 8);
        sprintf(tmp_str, "CSI_RS_Signal_Config_ZP%d", i);
        UE_Specific.CSI_RS_Signal_Config_ZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "N_CSI_ID_ZP%d", i);
        UE_Specific.N_CSI_ID_ZP[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE_Specific.CSI_Flag_ZP[i] = false;
      }
      UE_Specific.p_C = (float)Parser.ReadValue("p_C", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      //RRC FEICIC
      UE_Specific.CRS_AssistanceInfo_Num = (int)Parser.ReadValue("CRS_AssistanceInfo_Num", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.CRS_AssistanceInfo_CellID.resize(UE_Specific.CRS_AssistanceInfo_Num);
      UE_Specific.CRS_AssistanceInfo_PortNum.resize(UE_Specific.CRS_AssistanceInfo_Num);
      for (i = 0; i < UE_Specific.CRS_AssistanceInfo_Num; i++) {
        UE_Specific.CRS_AssistanceInfo_CellID[i] = (int)Parser.ReadValue("CRS_AssistanceInfo_CellID", iter_idx, ue_idx, ue_cell_idx, 0, i, 0);
        UE_Specific.CRS_AssistanceInfo_PortNum[i] = (int)Parser.ReadValue("CRS_AssistanceInfo_PortNum", iter_idx, ue_idx, ue_cell_idx, 0, i, 1);
      }
      //RRC COMP
      UE_Specific.QCL_Type = QCL_Type_Str(Parser.ReadString("QCL_Type", iter_idx, ue_idx, ue_cell_idx, 0, 0, "TypeA"));
      for (i = 0; i < UE_Specific.CRS_AssistanceInfo_Num; i++) {
        UE_Specific.QCL_NZP_CSI_idx[i] = (int)Parser.ReadValue("QCL_NZP_CSI_idx", iter_idx, ue_idx, ue_cell_idx, 0, i, 0);
        UE_Specific.QCL_ZP_CSI_idx[i] = (int)Parser.ReadValue("QCL_ZP_CSI_idx", iter_idx, ue_idx, ue_cell_idx, 0, i, 0);
      }
      //DCI
      UE_Specific.DCI_Period = (int)Parser.ReadValue("DCI_Period", iter_idx, ue_idx, ue_cell_idx, 0, 0, 10);
      UE_Specific.DCI_Format_Type.resize(UE_Specific.DCI_Period);
      UE_Specific.DCI_On_EPDCCH.resize(UE_Specific.DCI_Period);
      UE_Specific.Aggregation_level.resize(UE_Specific.DCI_Period);
      UE_Specific.EPDCCH_set_idx.resize(UE_Specific.DCI_Period);
      UE_Specific.DCI_Search_Space.resize(UE_Specific.DCI_Period);
      UE_Specific.m_Candidated.resize(UE_Specific.DCI_Period);
      UE_Specific.codeword_num.resize(UE_Specific.DCI_Period);
      for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
        UE_Specific.CQI[cw_idx].resize(UE_Specific.DCI_Period);
      }
      UE_Specific.N_L.resize(UE_Specific.DCI_Period);
      UE_Specific.RB_Assignment.resize(UE_Specific.DCI_Period);
      UE_Specific.RIV.resize(UE_Specific.DCI_Period);
      for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
        UE_Specific.MCS[cw_idx].resize(UE_Specific.DCI_Period);
        UE_Specific.NDI[cw_idx].resize(UE_Specific.DCI_Period);
      }
      UE_Specific.TPC_for_PUSCH.resize(UE_Specific.DCI_Period);
      UE_Specific.Cyclic_shift_OCC.resize(UE_Specific.DCI_Period);
      UE_Specific.UL_index.resize(UE_Specific.DCI_Period);
      UE_Specific.DAI.resize(UE_Specific.DCI_Period);
      UE_Specific.CSI_request_num.resize(UE_Specific.DCI_Period);
      UE_Specific.CSI_request.resize(UE_Specific.DCI_Period);
      UE_Specific.SRS_request_num.resize(UE_Specific.DCI_Period);
      UE_Specific.SRS_request.resize(UE_Specific.DCI_Period);
      UE_Specific.Resource_allocation_type_num.resize(UE_Specific.DCI_Period);
      UE_Specific.Resource_allocation_type.resize(UE_Specific.DCI_Period);
      UE_Specific.subset_p.resize(UE_Specific.DCI_Period);
      UE_Specific.resource_allocation_span.resize(UE_Specific.DCI_Period);
      UE_Specific.HARQ_ACK_resource_offset.resize(UE_Specific.DCI_Period);
      UE_Specific.Random_Access_Flag.resize(UE_Specific.DCI_Period);
      UE_Specific.Locailized_Distributed_VRB_Flag.resize(UE_Specific.DCI_Period);
      UE_Specific.Preamble_Index.resize(UE_Specific.DCI_Period);
      UE_Specific.PRACH_Mask_Index.resize(UE_Specific.DCI_Period);
      UE_Specific.gap_index.resize(UE_Specific.DCI_Period);
      UE_Specific.TPC_for_PUCCH.resize(UE_Specific.DCI_Period);
      UE_Specific.TPMI.resize(UE_Specific.DCI_Period);
      UE_Specific.PMI_Conf.resize(UE_Specific.DCI_Period);
      UE_Specific.Very_Compact_Flag.resize(UE_Specific.DCI_Period);
      UE_Specific.N_DL_VRB_gap1.resize(UE_Specific.DCI_Period);
      UE_Specific.Info_MCCH_Change.resize(UE_Specific.DCI_Period);
      UE_Specific.Downlink_power_offset.resize(UE_Specific.DCI_Period);
      UE_Specific.TB_swap_flag.resize(UE_Specific.DCI_Period);
      UE_Specific.Precoding_Info.resize(UE_Specific.DCI_Period);
      UE_Specific.Scrambling_ID.resize(UE_Specific.DCI_Period);
      UE_Specific.Antenna_Scrambling_Layers.resize(UE_Specific.DCI_Period);
      UE_Specific.n_SCID.resize(UE_Specific.DCI_Period);
      UE_Specific.PDSCH_RE_Mapping_QCL.resize(UE_Specific.DCI_Period);
      UE_Specific.s0.resize(UE_Specific.DCI_Period);
      UE_Specific.s1.resize(UE_Specific.DCI_Period);
      UE_Specific.s2.resize(UE_Specific.DCI_Period);
      UE_Specific.s3.resize(UE_Specific.DCI_Period);
      for (i = 0; i < UE_Specific.DCI_Period; i++) {
        sprintf(tmp_str, "DCI_Format_Type_S%d", i);
        UE_Specific.DCI_Format_Type[i] = DCI_Format_Type_Str(Parser.ReadString(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, "NULL_DCI"), UE_Specific.TM);
        sprintf(tmp_str, "DCI_On_EPDCCH_S%d", i);
        UE_Specific.DCI_On_EPDCCH[i] = (bool)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, FALSE);
        sprintf(tmp_str, "Aggregation_level_S%d", i);
        UE_Specific.Aggregation_level[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
        sprintf(tmp_str, "EPDCCH_set_idx_S%d", i);
        UE_Specific.EPDCCH_set_idx[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "DCI_Search_Space_S%d", i);
        UE_Specific.DCI_Search_Space[i] = DCI_Search_Space_Str(Parser.ReadString(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, "UE_Specific"));
        sprintf(tmp_str, "m_Candidated_S%d", i);
        UE_Specific.m_Candidated[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "codeword_num_S%d", i);
        UE_Specific.codeword_num[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
          sprintf(tmp_str, "CQI%d_S%d", cw_idx, i);
          UE_Specific.CQI[cw_idx][i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        }
        sprintf(tmp_str, "N_L_S%d", i);
        UE_Specific.N_L[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
        sprintf(tmp_str, "RB_Assignment_S%d", i);
        paras_num = Parser.Getl0Num(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0);
        UE_Specific.RB_Assignment[i].resize(paras_num);
        for (rb_idx = 0; rb_idx < paras_num; rb_idx++) {
          UE_Specific.RB_Assignment[i][rb_idx] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, rb_idx, rb_idx);
        }
        sprintf(tmp_str, "RIV_S%d", i);
        UE_Specific.RIV[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
          sprintf(tmp_str, "MCS%d_S%d", cw_idx, i);
          UE_Specific.MCS[cw_idx][i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
          sprintf(tmp_str, "NDI%d_S%d", cw_idx, i);
          UE_Specific.NDI[cw_idx][i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        }
        sprintf(tmp_str, "TPC_for_PUSCH_S%d", i);
        UE_Specific.TPC_for_PUSCH[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Cyclic_shift_OCC_S%d", i);
        UE_Specific.Cyclic_shift_OCC[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "UL_index_S%d", i);
        UE_Specific.UL_index[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "DAI_S%d", i);
        UE_Specific.DAI[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "CSI_request_num_S%d", i);
        UE_Specific.CSI_request_num[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
        sprintf(tmp_str, "CSI_request_S%d", i);
        UE_Specific.CSI_request[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "SRS_request_num_S%d", i);
        UE_Specific.SRS_request_num[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "SRS_request_S%d", i);
        UE_Specific.SRS_request[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Resource_allocation_type_num_S%d", i);
        UE_Specific.Resource_allocation_type_num[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Resource_allocation_type_S%d", i);
        UE_Specific.Resource_allocation_type[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "subset_p_S%d", i);
        UE_Specific.subset_p[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "resource_allocation_span_S%d", i);
        UE_Specific.resource_allocation_span[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "HARQ_ACK_resource_offset_S%d", i);
        UE_Specific.HARQ_ACK_resource_offset[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Random_Access_Flag_S%d", i);
        UE_Specific.Random_Access_Flag[i] = (bool)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, FALSE);
        sprintf(tmp_str, "Locailized_Distributed_VRB_Flag_S%d", i);
        UE_Specific.Locailized_Distributed_VRB_Flag[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Preamble_Index_S%d", i);
        UE_Specific.Preamble_Index[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "PRACH_Mask_Index_S%d", i);
        UE_Specific.PRACH_Mask_Index[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "gap_index_S%d", i);
        UE_Specific.gap_index[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "TPC_for_PUCCH_S%d", i);
        UE_Specific.TPC_for_PUCCH[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "TPMI_S%d", i);
        UE_Specific.TPMI[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        sprintf(tmp_str, "PMI_Conf_S%d", i);
        UE_Specific.PMI_Conf[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Very_Compact_Flag_S%d", i);
        UE_Specific.Very_Compact_Flag[i] = (bool)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, TRUE);
        sprintf(tmp_str, "N_DL_VRB_gap1_S%d", i);
        UE_Specific.N_DL_VRB_gap1[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Info_MCCH_Change_S%d", i);
        UE_Specific.Info_MCCH_Change[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Downlink_power_offset_S%d", i);
        UE_Specific.Downlink_power_offset[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "TB_swap_flag_S%d", i);
        UE_Specific.TB_swap_flag[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Precoding_Info_S%d", i);
        UE_Specific.Precoding_Info[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        sprintf(tmp_str, "Scrambling_ID_S%d", i);
        UE_Specific.Scrambling_ID[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "Antenna_Scrambling_Layers_S%d", i);
        UE_Specific.Antenna_Scrambling_Layers[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, -1);
        sprintf(tmp_str, "n_SCID_S%d", i);
        UE_Specific.n_SCID[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "PDSCH_RE_Mapping_QCL_S%d", i);
        UE_Specific.PDSCH_RE_Mapping_QCL[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "s0_S%d", i);
        UE_Specific.s0[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "s1_S%d", i);
        UE_Specific.s1[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "s2_S%d", i);
        UE_Specific.s2[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        sprintf(tmp_str, "s3_S%d", i);
        UE_Specific.s3[i] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      }
      //PHICH
      UE_Specific.PHICH_Period = (int)Parser.ReadValue("PHICH_Period", iter_idx, ue_idx, ue_cell_idx, 0, 0, 10);
      UE_Specific.PHICH_Wanted_Pattern.resize(UE_Specific.PHICH_Period);
      UE_Specific.n_group_PHICH.resize(UE_Specific.PHICH_Period);
      UE_Specific.n_seq_PHICH.resize(UE_Specific.PHICH_Period);
      UE_Specific.PHICH_Power_Offset.resize(UE_Specific.PHICH_Period);
      UE_Specific.PHICH_HARQ_Pattern.resize(UE_Specific.PHICH_Period);
      for (i = 0; i < UE_Specific.PHICH_Period; i++) {
        sprintf(tmp_str, "PHICH_Wanted_Pattern_S%d", i);
        paras_num = Parser.Getl0Num(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0);
        UE_Specific.PHICH_Wanted_Pattern[i].resize(paras_num);
        UE_Specific.n_group_PHICH[i].resize(paras_num);
        UE_Specific.n_seq_PHICH[i].resize(paras_num);
        UE_Specific.PHICH_Power_Offset[i].resize(paras_num);
        UE_Specific.PHICH_HARQ_Pattern[i].resize(paras_num);
        for (j = 0; j < paras_num; j++) {
          sprintf(tmp_str, "PHICH_Wanted_Pattern_S%d", i);
          UE_Specific.PHICH_Wanted_Pattern[i][j] = PHICH_Wanted_Pattern_Str(Parser.ReadString(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, j, "Wanted_User"));
          sprintf(tmp_str, "n_group_PHICH_S%d", i);
          UE_Specific.n_group_PHICH[i][j] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, j, 0);
          sprintf(tmp_str, "n_seq_PHICH_S%d", i);
          UE_Specific.n_seq_PHICH[i][j] = (int)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, j, 0);
          sprintf(tmp_str, "PHICH_Power_Offset_S%d", i);
          UE_Specific.PHICH_Power_Offset[i][j] = (float)Parser.ReadValue(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, j, 0);
          sprintf(tmp_str, "PHICH_HARQ_Pattern_S%d", i);
          UE_Specific.PHICH_HARQ_Pattern[i][j] = PHICH_HARQ_Pattern_Str(Parser.ReadString(tmp_str, iter_idx, ue_idx, ue_cell_idx, 0, j, "ACK"));
        }
      }
      //Channel
      UE_Specific.RxFreqOffset = (float)Parser.ReadValue("RxFreqOffset", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      UE_Specific.RxNum = (int)Parser.ReadValue("RxNum", iter_idx, ue_idx, ue_cell_idx, 0, 0, 2);
      UE_Specific.ChanCorrInd = ChanCorrInd_Str(Parser.ReadString("ChanCorrInd", iter_idx, ue_idx, ue_cell_idx, 0, 0, "Low"));
      UE_Specific.CellType = CellType_Str(Parser.ReadString("CellType", iter_idx, ue_idx, ue_cell_idx, 0, 0, "ServingCell"));
      //Other
      UE_Specific.Ref_Type = Ref_Type_Str(Parser.ReadString("Ref_Type", iter_idx, ue_idx, ue_cell_idx, 0, 0, "NULL"));
      UE_Specific.I_ot_Offset.resize(N_MAX_DL_RB);
      for (rb_idx = 0; rb_idx < N_MAX_DL_RB; rb_idx++) {
        UE_Specific.I_ot_Offset[rb_idx] = (float)Parser.ReadValue("I_ot_Offset", iter_idx, ue_idx, ue_cell_idx, 0, rb_idx, 0);
      }
      UE[ue_idx].UE_Specific[ue_cell_idx] = UE_Specific;
    }
    UE[ue_idx].UEType = UEType_Str(Parser.ReadString("UEType", iter_idx, ue_idx, 0, 0, 0, "Wanted_UE"));
  }
  //Channel
  for (ue_idx = 0; ue_idx < ue_num; ue_idx++) {
    connect_cell_num = Parser.Getl2Num("Connected_CellID", iter_idx, ue_idx);
    Channel[ue_idx].Channel_Specific.resize(connect_cell_num);
    for (ue_cell_idx = 0; ue_cell_idx < connect_cell_num; ue_cell_idx++) {
      Channel_Specific.ChannelType = ChannelType_Str(Parser.ReadString("ChannelType", iter_idx, ue_idx, ue_cell_idx, 0, 0, "AWGN"));
      Channel_Specific.DopplerShift = (float)Parser.ReadValue("DopplerShift", iter_idx, ue_idx, ue_cell_idx, 0, 0, 5);
      Channel_Specific.TimeOffset = (float)Parser.ReadValue("TimeOffset", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      Channel_Specific.RxNum = (int)Parser.ReadValue("RxNum", iter_idx, ue_idx, ue_cell_idx, 0, 0, 2);
      Channel_Specific.SNR = (float)Parser.ReadValue("SNR", iter_idx, ue_idx, ue_cell_idx, 0, 0, 20);
      Channel_Specific.InterpType = InterpType_Str(Parser.ReadString("InterpType", iter_idx, ue_idx, ue_cell_idx, 0, 0, "Hold"));
      Channel_Specific.tao_d = (float)Parser.ReadValue("tao_d", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0.00000045);
      Channel_Specific.a = (float)Parser.ReadValue("a", iter_idx, ue_idx, ue_cell_idx, 0, 0, 1);
      Channel_Specific.StartTime = (float)Parser.ReadValue("StartTime", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      Channel_Specific.Ds = (float)Parser.ReadValue("Ds", iter_idx, ue_idx, ue_cell_idx, 0, 0, 300);
      Channel_Specific.Dmin = (float)Parser.ReadValue("Dmin", iter_idx, ue_idx, ue_cell_idx, 0, 0, 2);
      Channel_Specific.Speed = (float)Parser.ReadValue("Speed", iter_idx, ue_idx, ue_cell_idx, 0, 0, 300) / 3.6f;
      Channel_Specific.ChanCorrInd = ChanCorrInd_Str(Parser.ReadString("ChanCorrInd", iter_idx, ue_idx, ue_cell_idx, 0, 0, "Low"));
      Channel_Specific.InstantFlag = (bool)Parser.ReadValue("InstantFlag", iter_idx, ue_idx, ue_cell_idx, 0, 0, FALSE);
      Channel_Specific.InstantOffset = (float)Parser.ReadValue("InstantOffset", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      Channel[ue_idx].Channel_Specific[ue_cell_idx] = Channel_Specific;
    }
  }
  //UE, Channel & Cell Relationship
  UE_Connect_Cell_idx.resize(ue_num);
  UE_Connect_Cell_innerUE_idx.resize(ue_num);
  Cell_Connect_UE_idx.resize(cell_num);
  Cell_Connect_UE_innerCell_idx.resize(cell_num);
  for (ue_idx = 0; ue_idx < ue_num; ue_idx++) {
    connect_cell_num = Parser.Getl2Num("Connected_CellID", iter_idx, ue_idx);
    UE_Connect_Cell_idx[ue_idx].resize(connect_cell_num);
    for (ue_cell_idx = 0; ue_cell_idx < connect_cell_num; ue_cell_idx++) {
      CellID = (int)Parser.ReadValue("Connected_CellID", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
      for (cell_idx = 0; cell_idx < cell_num; cell_idx++) {
        if (CellID == Cell[cell_idx].Cell_Specific.CellID) {
          UE_Connect_Cell_idx[ue_idx][ue_cell_idx] = cell_idx;
          Cell_Connect_UE_idx[cell_idx].push_back(ue_idx);
          Cell_Connect_UE_innerCell_idx[cell_idx].push_back(ue_cell_idx);
          UE_Connect_Cell_innerUE_idx[ue_idx].push_back(Cell_Connect_UE_idx[cell_idx].size() - 1);
          break;
        }
      }
    }
  }
  for (cell_idx = 0; cell_idx < cell_num; cell_idx++) {
    Cell[cell_idx].Cell_Connect_UE_idx = Cell_Connect_UE_idx[cell_idx];
    Cell[cell_idx].Cell_Connect_UE_innerCell_idx = Cell_Connect_UE_innerCell_idx[cell_idx];
    Cell[cell_idx].UE_Specific.resize(Cell[cell_idx].Cell_Connect_UE_idx.size());
    Cell[cell_idx].ReTransmit.resize(Cell[cell_idx].Cell_Connect_UE_idx.size());
    Cell[cell_idx].Feedback.resize(Cell[cell_idx].Cell_Connect_UE_idx.size());
    for (cell_ue_idx = 0; cell_ue_idx < (int)Cell[cell_idx].Cell_Connect_UE_idx.size(); cell_ue_idx++) {
      Cell[cell_idx].UE_Specific[cell_ue_idx] = UE[Cell_Connect_UE_idx[cell_idx][cell_ue_idx]].UE_Specific[Cell_Connect_UE_innerCell_idx[cell_idx][cell_ue_idx]];
    }
    Cell[cell_idx].InitParas();
  }
  for (ue_idx = 0; ue_idx < ue_num; ue_idx++) {
    UE[ue_idx].UE_Connect_Cell_idx = UE_Connect_Cell_idx[ue_idx];
    UE[ue_idx].UE_Connect_Cell_innerUE_idx = UE_Connect_Cell_innerUE_idx[ue_idx];
    Channel[ue_idx].UE_Connect_Cell_idx = UE_Connect_Cell_idx[ue_idx];
    Channel[ue_idx].UE_Connect_Cell_innerUE_idx = UE_Connect_Cell_innerUE_idx[ue_idx];
    UE[ue_idx].Cell_Specific.resize(UE[ue_idx].UE_Connect_Cell_idx.size());
    UE[ue_idx].ReTransmit.resize(UE[ue_idx].UE_Connect_Cell_idx.size());
    UE[ue_idx].Feedback.resize(UE[ue_idx].UE_Connect_Cell_idx.size());
    UE[ue_idx].Result.resize(UE[ue_idx].UE_Connect_Cell_idx.size());
    if (0 == iter_idx) {
      for (ue_cell_idx = 0; ue_cell_idx < (int)UE[ue_idx].UE_Connect_Cell_idx.size(); ue_cell_idx++) {
        //Ref
        UE[ue_idx].Result[ue_cell_idx].Ref_SNR = (float)Parser.ReadValue("Ref_SNR", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE[ue_idx].Result[ue_cell_idx].Ref_Value = (float)Parser.ReadValue("Ref_Value", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE[ue_idx].Result[ue_cell_idx].Ref_Alpha = (float)Parser.ReadValue("Ref_Alpha", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE[ue_idx].Result[ue_cell_idx].Ref_Beta = (float)Parser.ReadValue("Ref_Beta", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE[ue_idx].Result[ue_cell_idx].Ref_Gamma = (float)Parser.ReadValue("Ref_Gamma", iter_idx, ue_idx, ue_cell_idx, 0, 0, 0);
        UE[ue_idx].Result[ue_cell_idx].SNR.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PBCH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PCHICH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PDCCH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PCHICH_PDCCH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PHICH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].EPDCCH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PDSCH_Throughput.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].PDSCH_BLER.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].Feedback_Test_Flag = true;
        UE[ue_idx].Result[ue_cell_idx].CQI_SNR_eff.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].CQI_SNR_eff.setZero();
        UE[ue_idx].Result[ue_cell_idx].CQI_SNR_eff_Num.resize(iter_num);
        UE[ue_idx].Result[ue_cell_idx].CQI_SNR_eff_Num.setZero();
      }
    }
    MAX_N_FFT = 0;
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE[ue_idx].UE_Connect_Cell_idx.size(); ue_cell_idx++) {
      MAX_N_FFT = MAX(MAX_N_FFT, Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.N_FFT);
    }
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE[ue_idx].UE_Connect_Cell_idx.size(); ue_cell_idx++) {
      Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.ChanCorrInd = UE[ue_idx].UE_Specific[ue_cell_idx].ChanCorrInd;
      Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_N_FFT = MAX_N_FFT;
      for (i = 0; i < MAX_SYMBNUM_TTI; i++) {
        Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_N_CP_l[i] = MAX_N_FFT * Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.N_CP_l[i] / Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.N_FFT;
      }
      Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_Num = MAX_N_FFT * Get_SamplesPerTTI(Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.dl_BandWidth) / Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.N_FFT;
      Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_unit = 0.001 / (double)Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_Num;
#if IDEAL_H_METHOD > 0
      j = Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_N_CP_l[0];
      for (i = 0; i < (2 * Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.N_DL_symb); i++) {
        Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].IdealSamplePos[i] = j;
        j += (Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_N_CP_l[i] + Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_N_FFT);
      }
#endif
      Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].BB.resize(MAX_PHY_ANTENNA_NUM, Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific.OverSample_Num);
    }
    for (ue_cell_idx = 0; ue_cell_idx < (int)UE[ue_idx].UE_Connect_Cell_idx.size(); ue_cell_idx++) {
      UE[ue_idx].Cell_Specific[ue_cell_idx] = Cell[UE_Connect_Cell_idx[ue_idx][ue_cell_idx]].Cell_Specific;
    }
    UE[ue_idx].InitParas();
    Channel[ue_idx].InitParas(Cell);
  }
}

//! 仿真过程。Cell发射->无线信道处理->UE接收；UE接收存在一个TTI延时
/*!
  \return 无返回值
*/
void Simulation_C::Process() {
  int cell_idx = 0;
  int ue_idx = 0;
  for (Simuctrl.n_subframe = Simuctrl.start_n_subframe; Simuctrl.n_subframe < (Simuctrl.start_n_subframe + Simuctrl.runlength + 1); Simuctrl.n_subframe++) {
    for (cell_idx = 0; cell_idx < (int)Cell.size(); cell_idx++) {
      Cell[cell_idx].UpdataParas(Simuctrl.n_subframe);
      Cell[cell_idx].Process();
    }
    for (ue_idx = 0; ue_idx < (int)UE.size(); ue_idx++) {
      if (Wanted_UE == UE[ue_idx].UEType) {
        Channel[ue_idx].UpdataParas(Simuctrl.n_subframe, Cell);
        Channel[ue_idx].Process(Cell);
        UE[ue_idx].UpdataParas(Simuctrl.n_subframe, Cell);
        if (Simuctrl.n_subframe > Simuctrl.start_n_subframe) {
          sprintf(c_string, "UE%d", ue_idx);
          FilesPathUE = c_string;
          UE[ue_idx].Process(Channel[ue_idx]);
          UE[ue_idx].FeedBack(ue_idx, Cell);
        }
      }
    }
  }
  for (cell_idx = 0; cell_idx < (int)Cell.size(); cell_idx++) {
    Cell[cell_idx].GetMedianCQI(cell_idx);
  }
  for (ue_idx = 0; ue_idx < (int)UE.size(); ue_idx++) {
    UE[ue_idx].GetMedianCQI(ue_idx, Cell);
  }
}

//! 仿真结果统计
/*!
  \return 无返回值
*/
void Simulation_C::GetResult() {
  int ue_idx = 0;
  for (ue_idx = 0; ue_idx < (int)UE.size(); ue_idx++) {
    UE[ue_idx].GetResult(ue_idx);
  }
}
