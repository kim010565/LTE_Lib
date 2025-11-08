/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Structs.h
    \brief Structs头文件
    内容 ：一些结构体定义
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Structs_h_
#define _Structs_h_

#include "Definitions.h"
#include "Enums.h"
#include "Typedefs.h"

//! Cell级参数集合
typedef struct
{
  //Common
  int CellID;                               //!< Cell识别ID
  FrameStructureType_E FrameStructureType;  //!< 无线帧类型，FDD or TDD
  int HARQ_ProcNum;                         //!< 进程数
  int MaxTxNum;                             //!< 最大传输次数
  vector<int> RV_QPSK;                      //!< QPSK的RV版本
  vector<int> RV_16QAM;                     //!< 16QAM的RV版本
  vector<int> RV_64QAM;                     //!< 64QAM的RV版本
  vector<int> RV_256QAM;                    //!< 256QAM的RV版本
  float EVM;                                //!< EVM
  int Ctrl_Symb_Num;                        //!< 正常帧下的控制符号个数
  int Ctrl_Symb_Num_Special;                //!< 特殊子帧下的控制符号个数
  int delta_f;                              //!< 子载波间隔
  //MIB
  BandWidth_E dl_BandWidth;              //!< 带宽
  PHICH_Duration_Type_E phich_Duration;  //!< phich_Duration
  PHICH_Resource_Type_E phich_Resource;  //!< phich_Resource
  int SFN_High8bit;                      //!< SFN的高8bit
  //SIB1
  ULDLConfiguration_E ULDLConfiguration;                        //!< TDD上下行配置
  SpecialSubframeConfiguration_E SpecialSubframeConfiguration;  //!< TDD特殊子帧配置
  //SIB2
  int p_b;  //!< p_b，取值0/1/2/3
  //PSS_SSS_DT
  CP_Type_E CP_Type;  //!< CP类型
  int N_cell_ID;      //!< 物理Cell ID，取值0~503
  //PBCH_DT
  int CRSPortNum;   //!< CRS端口数
  int SFN_Low2bit;  //!< SFN的低2bit
  //Ohter
  float TxFreqOffset;         //!< 发端频率偏差
  ChanCorrInd_E ChanCorrInd;  //!< 天线相关性
  float PBCH_RA;              //!< PBCH的row_a，UE不可知
  float PBCH_RB;              //!< PBCH的row_b，UE不可知
  float PCFICH_RA;            //!< PCFICH的row_a，UE不可知
  float PCFICH_RB;            //!< PCFICH的row_b，UE不可知
  float PHICH_RA;             //!< PCFICH的row_a，UE不可知
  float PHICH_RB;             //!< PCFICH的row_b，UE不可知
  float PDCCH_RA;             //!< PDCCH的row_a，UE不可知
  float PDCCH_RB;             //!< PDCCH的row_b，UE不可知
  float PDSCH_RA;             //!< DMRS下PDSCH的row_a，UE不可知
  float PDSCH_RB;             //!< DMRS下PDSCH的row_b，UE不可知

  //Calc
  int n_f;                                 //!< SFN
  int N_FFT;                               //!< FFT大小
  int N_CP_l[MAX_SYMBNUM_TTI];             //!< 各符号CP长度
  int N_DL_RB;                             //!< 可用的RB个数，N_DL_RB
  int N_RB_sc;                             //!< 一个RB内子载波个数，N_RB_sc
  int N_DL_symb;                           //!< 一个时隙内符号个数，N_DL_symb
  int N_1_ID;                              //!< N_1_ID=N_cell_ID/3，取值0~167
  int N_2_ID;                              //!< N_2_ID=N_cell_ID%3，取值0~2
  int N_CP;                                //!< Normal CP: N_CP=1； Extended CP ：N_CP=0
  int DL_Symb_Num;                         //!< 下行子帧符号数
  int n_subframe;                          //!< 子帧序号
  SubframeType_E SubframeType;             //!< 子帧类型
  int N_UL_RB;                             //!< 上行可用RB个数，N_UL_RB
  int M_DL_HARQ;                           //!< M_DL_HARQ
  int OverSample_N_FFT;                    //!< 通过增大FFT来实现过采样，过采样下的FFT大小
  int OverSample_N_CP_l[MAX_SYMBNUM_TTI];  //!< 过采样下，各符号的CP长度
  double OverSample_unit;                  //!< 过采样下，一个样点的持续时间
  int OverSample_Num;                      //!< 过采样下，一个TTI内的样点数
  int OverSample_DataLen;                  //!< 过采样下，一个TTI内下行样点数
  int TxNum;                               //!< 物理发射天线数

  //TBD
  bool MBSFN_Subframe_Flag;  //!< MBSFN子帧标志
} Cell_Specific_S;

//! UE级参数集合
typedef struct
{
  //common
  int UEID;                   //!< UE识别ID
  UE_Category_E UE_Category;  //!< UE能力等级
  int RNTI;                   //!< RNTI值
  RNTI_Type_E RNTI_Type;      //!< RNTI类型
  int N_soft;                 //!< N_soft
  int N_IR;                   //!< N_IR
  //RRC
  float p_a;                                  //!< p_a，取值-6/-4.77/-3/-1.77/0/1/2/3 dB
  TM_E TM;                                    //!< 传输模式
  DCI_Format_Type_E UL_DCI_Format;            //!< DCI类型
  bool alternativeCodeBookEnabledFor4TX_r12;  //!< alternativeCodeBookEnabledFor4TX_r12
  bool UE_Transmit_Selection;                 //!< UE_Transmit_Selection，发射天线选择
  int N_pd_CQI_PMI;                           //!< CQI/PMI上报周期
  int N_pd_RI;                                //!< RI上报周期
  int N_offset_CQI_PMI;                       //!< CQI/PMI上报子帧偏移
  int N_offset_RI;                            //!< RI上报子帧偏移
  int I_CQI_PMI;
  int I_RI;
  ReportModeType_E ReportMode;       //!< CSI上报模式
  bool ReportOnPUSCH;                //!< 在PUSCH或PUCCH上报CSI，true: on PUSCH(Aperiod); false: on PUCCH(Period)
  bool PMI_RI_Report;                //!< true: PMI/RI reporting is configured; false: PMI/RI reporting is not configured
  int n_DMRS_ID[2];                  //!< n_DMRS_ID_0和n_DMRS_ID_1
  int codebookSubsetRestriction[7];  //!< 码本子集限制
  ivec codebookSubsetNum;            //!< codebookSubsetNum[v] 码本子集大小
  ivecs codebookSubsetIdxs;          //!< codebookSubsetIdxs[v](idx) 码本子集
  //RRC EPDCCH
  int EPDCCH_Set_Num;                          //!< EPDCCH_Set_Num
  vector<vector<int>> RB_Assignment_EPDCCH;    //!< EPDCCH的RB分配
  vector<TMType_EPDCCH_E> TMType_EPDCCH;       //!< EPDCCH资源分布类型
  vector<int> numberPRB_Pairs_EPDCCH;          //!< EPDCCH numberPRB Pairs
  vector<int> resourceBlockAssignment_EPDCCH;  //!< EPDCCH resourceBlockAssignment
  int startSymb_EPDCCH;                        //!< EPDCCH起始符号
  int EPDCCH_Period;                           //!< EPDCCH周期
  vector<int> subframePattern_EPDCCH;          //!< EPDCCH子帧图案
  vector<int> n_EPDCCH_ID;                     //!< n_EPDCCH_ID
  //RRC CSI
  int NZP_CSI_Set_Num;                   //!< NZP_CSI_Set个数
  int ZP_CSI_Set_Num;                    //!< ZP_CSI_Set个数
  vector<int> I_CSI_RS_NZP;              //!< NZP的I_CSI_RS
  vector<int> CSIPortNum_NZP;            //!< NZP的CSI端口数
  vector<int> CSI_RS_Signal_Config_NZP;  //!< NZP的CSI_RS_Signal_Config
  vector<int> N_CSI_ID_NZP;              //!< NZP的N_CSI_ID
  vector<bool> CSI_Flag_NZP;             //!< NZP的CSI_Flag，用来判断当前子帧是否有CSI
  vector<int> I_CSI_RS_ZP;               //!< ZP的I_CSI_RS
  vector<int> CSIPortNum_ZP;             //!< ZP的CSI端口数
  vector<int> CSI_RS_Signal_Config_ZP;   //!< ZP的CSI_RS_Signal_Config
  vector<int> N_CSI_ID_ZP;               //!< ZP的N_CSI_ID
  vector<bool> CSI_Flag_ZP;              //!< ZP的CSI_Flag，用来判断当前子帧是否有CSI
  float p_C;                             //!< ratio of PDSCH EPRE to CSI-RS EPRE，取值-8~15
  //RRC FEICIC
  int CRS_AssistanceInfo_Num;              //!< 辅助CRS信息个数
  vector<int> CRS_AssistanceInfo_CellID;   //!< 辅助CRS的小区ID
  vector<int> CRS_AssistanceInfo_PortNum;  //!< 辅助CRS的CRS端口数
  //RRC COMP
  QCL_Type_E QCL_Type;     //!< QCL类型
  int QCL_NZP_CSI_idx[4];  //!< QCL_NZP_CSI_idx
  int QCL_ZP_CSI_idx[4];   //!< QCL_ZP_CSI_idx
  //DCI
  int DCI_Period;                               //!< 仿真时，DCI的设置周期
  vector<DCI_Format_Type_E> DCI_Format_Type;    //!< DCI格式
  vector<bool> DCI_On_EPDCCH;                   //!< DCI在PDCCH or EPDCCH上，true ：EPDCCH；false ：PDCCH
  vector<int> Aggregation_level;                //!< 聚合等级
  vector<int> EPDCCH_set_idx;                   //!< EPDCCH_set_idx
  vector<DCI_Search_Space_E> DCI_Search_Space;  //!< DCI搜索空间
  vector<int> m_Candidated;                     //!< DCI候选序号m_Candidated
  vector<int> codeword_num;                     //!< 码字数
  vector<int> CQI[MAX_TB_SIZE];                 //!< CQI
  vector<int> N_L;                              //!< 总层数
  //Format 0
  vector<vector<int>> RB_Assignment;         //!< RB分配
  vector<int> RIV;                           //!< RIV
  vector<int> MCS[MAX_TB_SIZE];              //!< MCS
  vector<int> NDI[MAX_TB_SIZE];              //!< 新数据指示NDI，翻转表示新数据，未翻转表示重传
  vector<int> TPC_for_PUSCH;                 //!< TPC_for_PUSCH
  vector<int> Cyclic_shift_OCC;              //!< Cyclic_shift_OCC
  vector<int> UL_index;                      //!< UL_index
  vector<int> DAI;                           //!< DAI
  vector<int> CSI_request_num;               //!< CSI_request比特数
  vector<int> CSI_request;                   //!< CSI_request
  vector<int> SRS_request_num;               //!< SRS_request比特数
  vector<int> SRS_request;                   //!< SRS_request
  vector<int> Resource_allocation_type_num;  //!< Resource_allocation_type比特数
  vector<int> Resource_allocation_type;      //!< Resource_allocation_type
  //Format 1
  vector<int> subset_p;                  //!< subset_p
  vector<int> resource_allocation_span;  //!< resource_allocation_span
  int HARQ_ID;                           //!< HARQ进程ID
  int RV[MAX_TB_SIZE];                   //!< 各码字的RV版本
  vector<int> HARQ_ACK_resource_offset;  //!< HARQ_ACK_resource_offset
  //Format 1A
  vector<bool> Random_Access_Flag;              //!< Random_Access_Flag
  vector<int> Locailized_Distributed_VRB_Flag;  //!< Locailized_Distributed_VRB_Flag
  vector<int> Preamble_Index;                   //!< Preamble_Index
  vector<int> PRACH_Mask_Index;                 //!< PRACH_Mask_Index
  vector<int> gap_index;                        //!< gap_index
  vector<int> TPC_for_PUCCH;                    //!< TPC_for_PUCCH
  //Format 1B
  vector<int> TPMI;      //!< TPMI
  vector<int> PMI_Conf;  //!< PMI_Conf
  //Format 1C
  vector<bool> Very_Compact_Flag;  //!< Very_Compact_Flag
  vector<int> N_DL_VRB_gap1;       //!< N_DL_VRB_gap1
  vector<int> Info_MCCH_Change;    //!< Info_MCCH_Change
  //Format 1D
  vector<int> Downlink_power_offset;  //!< Downlink_power_offset
  //Format 2
  vector<int> TB_swap_flag;    //!< TB_swap_flag
  vector<int> Precoding_Info;  //!< 预编码信息Precoding_Info
  //Format 2B
  vector<int> Scrambling_ID;  //!< Scrambling_ID
  //Format 2C
  vector<int> Antenna_Scrambling_Layers;  //!< Antenna_Scrambling_Layers
  vector<int> n_SCID;                     //!< n_SCID
  //Format 2D
  vector<int> PDSCH_RE_Mapping_QCL;  //!< QCL指示
  //Format 4
  vector<int> s0;  //!< s0
  vector<int> s1;  //!< s1
  vector<int> s2;  //!< s2
  vector<int> s3;  //!< s3
  //PHICH
  int PHICH_Period;                                             //!<
  vector<vector<PHICH_Wanted_Pattern_E>> PHICH_Wanted_Pattern;  //!< PHICH_Wanted_Pattern
  vector<vector<int>> n_group_PHICH;                            //!<  n_group_PHICH Calc from 36.213 9.1.2
  vector<vector<int>> n_seq_PHICH;                              //!<  n_seq_PHICH Calc from 36.213 9.1.2
  vector<vector<float>> PHICH_Power_Offset;                     //!< PHICH_Power_Offset
  vector<vector<PHICH_HARQ_Pattern_E>> PHICH_HARQ_Pattern;      //!< PHICH_HARQ_Pattern
  //Channel
  float RxFreqOffset;         //!< 接收端频率偏差
  int RxNum;                  //!< 接收天线数
  ChanCorrInd_E ChanCorrInd;  //!< 天线相关性
  CellType_E CellType;        //!< 小区类型

  //Calc
  int DCI_idx;                                //!< 当前DCI_idx
  int PHICH_idx;                              //!< 当前PHICH_idx
  int TBS[MAX_TB_SIZE];                       //!< 每个码字的传输块大小
  int Qm[MAX_TB_SIZE];                        //!< 每个码字的调制方式
  int v[MAX_TB_SIZE];                         //!< 每个码字的层数
  ivec Codebook_index0;                       //!< 码本0
  ivec Codebook_index1;                       //!< 码本1
  Port_Type_E Start_Port;                     //!< 传输起始端口
  Transmission_Scheme_E Transmission_Scheme;  //!< 传输方式
  int G_symb;                                 //!<

  //Other
  Ref_Type_E Ref_Type;
  vector<float> I_ot_Offset;

  //TBD
  bool CA_Flag;  //!< CA使能标志
} UE_Specific_S;

//! 重传相关结构体
typedef struct
{
  int LastCWNum[MAXHARQNUM];                         //!< 各进程历史码字数
  int LastNL[MAXHARQNUM];                            //!< 各进程历史总层数
  int CRC[MAX_TB_SIZE][MAXHARQNUM];                  //!< 各进程、各码字的CRC
  int TxNum[MAX_TB_SIZE][MAXHARQNUM];                //!< 各进程、各码字的传输次数
  int LastNDI[MAX_TB_SIZE][MAXHARQNUM];              //!< 各进程、各码字的历史NDI
  int LastTBS[MAX_TB_SIZE][MAXHARQNUM];              //!< 各进程、各码字的历史TBS
  int LastQm[MAX_TB_SIZE][MAXHARQNUM];               //!< 各进程、各码字的历史调制方式
  bmats Turbo_bits[MAX_TB_SIZE][MAXHARQNUM];         //!< 模拟发端循环buffer，用于重传发射
  fmats Turbo_bits_Buffer[MAX_TB_SIZE][MAXHARQNUM];  //!< 模拟收端循环buffer，用于重传合并
} ReTransmit_S;

//! 反馈相关结构体
typedef struct
{
  float AccFreqOffset;   //!< 频偏累计值
  int AccFreqOffsetLen;  //!< 频偏累计长度
  int AccIRT;            //!< 时偏累计值
  int AccIRTLen;         //!< 时偏累计长度
  float RxFreqOffset;    //!< AFC控制后的频偏

  int k_Subband;        //!< 子带大小
  int k_LastSubband;    //!< 最后一个子带大小
  int N_Subband;        //!< 子带个数
  int Perfered_M;       //!< Perfered_M个子带
  int J;                //!< BP个数
  imat Perfered_M_idx;  //!< Perfered_M个子带的RB位置
  int RI_Last_Report;   //!< 上次上报的RI
  int PUCCH_Last_j;     //!< 上次上报的BP序号
  int N_j;              //!< BP含有的子带个数

  int RI;                                     //!< RI
  imat Subband_PMI;                           //!< Subband_PMI(RI, Subband)
  int WidebandCQI[MAX_TB_SIZE][MAX_TB_SIZE];  //!< WidebandCQI[码字数][cw_idx]
  imat SubbandCQI[MAX_TB_SIZE];               //!< SubbandCQI[码字数](cw_idx, Subband_idx)
  int CQI_BLER_Acc[15];                       //!< 各CQI门限自适应的错误个数统计
  int CQI_BLER_Counter[15];                   //!< 各CQI门限自适应的总个数统计

  int RI_Report_Flag[MAX_Feedback_BufferSize];        //!< RI上报标志，用来模拟D/U，接收/发射产生的不同时刻。0: None; 1:Gen RI; 2: Report RI;
  int RI_Gen_n_subframe[MAX_Feedback_BufferSize];     //!< RI生成的子帧序号
  int RI_Report_n_subframe[MAX_Feedback_BufferSize];  //!< RI上报的子帧序号
  int RI_Report_Buffer[MAX_Feedback_BufferSize];      //!< 需要上报的RI Buffer
  int RI_Report_Buffer_idx;                           //!< 当前上报的RI Buffer序号

  int PMI_Report_Flag[MAX_Feedback_BufferSize];        //!< PMI上报标志，用来模拟D/U，接收/发射产生的不同时刻。0: None; 1:Gen PMI; 2: Report PMI;
  int PMI_Gen_n_subframe[MAX_Feedback_BufferSize];     //!< PMI生成的子帧序号
  int PMI_Report_n_subframe[MAX_Feedback_BufferSize];  //!< PMI上报的子帧序号
  imat PMI_Report_Buffer[MAX_Feedback_BufferSize];     //!< 需要上报的PMI Buffer
  int PMI_Report_Buffer_idx;                           //!< 当前上报的PMI Buffer序号

  int WidebandCQI_Report_Flag[MAX_Feedback_BufferSize];                                   //!< CQI上报标志，用来模拟D/U，接收/发射产生的不同时刻。0: None; 1:Gen CQI; 2: Report CQI;
  int WidebandCQI_Gen_n_subframe[MAX_Feedback_BufferSize];                                //!< CQI生成的子帧序号
  int WidebandCQI_Report_n_subframe[MAX_Feedback_BufferSize];                             //!< CQI上报的子帧序号
  int WidebandCQI_Report_Buffer[MAX_TB_SIZE][MAX_Feedback_BufferSize];                    //!< 需要上报的宽带CQI Buffer
  int SubbandCQI_Report_Buffer[MAX_TB_SIZE][MAX_N_Subband][MAX_Feedback_BufferSize];      //!< 需要上报的子带CQI Buffer
  int Perfered_M_idx_Report_Buffer[MAX_TB_SIZE][MAX_N_Subband][MAX_Feedback_BufferSize];  //!< Perfered_M子带序号Buffer
  int WidebandCQI_Report_Buffer_idx;                                                      //!< 当前上报的CQI Buffer序号

  int CQI_Num[MAX_TB_SIZE][15];
  int Median_CQI[MAX_TB_SIZE];
  float Median_CQI_Rate[MAX_TB_SIZE];
  int Median_CQI_Delta[MAX_TB_SIZE];
  int SubbandCQI_Offset_Num[MAX_TB_SIZE][MAX_N_Subband][4];
  float SubbandCQI_Offset_Rate[MAX_TB_SIZE][MAX_N_Subband];
} Feedback_S;

//! 无线信道相关结构体
typedef struct
{
  ChannelType_E ChannelType;  //!< 无线信道类型
  float DopplerShift;         //!< 最大多普勒频移
  float TimeOffset;           //!< 时间偏差
  int RxNum;                  //!< 接收天线数
  float SNR;                  //!< 信噪比
  InterpType_E InterpType;    //!< 多径插值类型
  float tao_d;                //!< B_2_4信道参数
  float a;                    //!< B_2_4信道参数
  float StartTime;            //!< 起始时间偏移，For HST, HST SFN
  float Ds;                   //!< 站间距离，For HST, HST SFN
  float Dmin;                 //!< 基站高度，For HST, HST SFN
  float Speed;                //!< 列车移动速度，For HST, HST SFN
  ChanCorrInd_E ChanCorrInd;  //!< 天线相关性
  bool InstantFlag;           //!< true ：每个TTI间的Fading都成固定一样的（主要为了CQI EESM参数测试）；false ：正常情况，不固定
  bool InstantFirst;          //!< 标志，初始第一个TTI true，后续都是false，为了实现“每个TTI间的Fading都成固定一样的”
  float InstantOffset;        //!< 实现“每个TTI间的Fading都成固定一样的”时，时间偏移设置

  //Calc
  int DataLen;                                //!< 下行发射数据长度
  int Path_Num;                               //!< 多径数
  vector<float> Delay_t;                      //!< 多径延时，单位s
  vector<int> InterDelay;                     //!< 整数倍多径延时，单位样点
  vector<vector<int>> InterDelay_Dyn;         //!< 整数倍多径延时，单位样点（主要用于HST_SFN，多径延时存在动态变化的情况）
  vector<float> FractionalDelay;              //!< 小数数倍多径延时
  vector<vector<float>> FractionalDelayCoef;  //!< 基于小数倍多径延时得到的多径插值系数
  vector<float> Relative_V;                   //!< 由多径相关功率计算得到的相对幅度
  int TxNum;                                  //!< 物理发射天线数
  cfmat InitPhase;                            //!< 初始相位
  int MaxDelay;                               //!< 一个TTI内最大延时
  int MinDelay;                               //!< 一个TTI内最小延时
  fvec SNR_Linear_Relative_V;                 //!< 由SNR和相对功率计算得到的幅度
  int MaxPathDataLen;                         //!< 一个TTI内最大下行发射数据长度

  //Buffer
  cfmats h_buffer;       //!< 一个TTI内各天线、各径信道估计生成Buffer
  vector<double> Phase;  //!< HST/HST_SFN等需要记录历史相位值
  int last_n_subframe;   //!< 历史子帧号
  cfmats Ideal_h;        //!< 各天线、各径的时域理想信道估计
} Channel_Specific_S;

//! 测量相关结构体
typedef struct
{
  //CRS
  bool CRS_PDP_Alpha_1stFlag;  //!< PDP是否做Alpha滤波
  fmat CRS_PDP;                //!< CRS估计的PDP
  fmat CRS_PDP_last;           //!< 历史CRS估计的PDP
  ivec CRS_PDP_lastAGC;        //!< 历史CRS估计PDP的AGC
  int CRS_PDP_lastTA;          //!< 历史CRS估计PDP的TA
  fvec CRS_PDP_Alpha;          //!< PDP Alpha滤波的系数
  int CRS_PDP_Counter;         //!< PDP Alpha滤波的计数器
  fvec CRS_PDP_AccSNRTh;       //!< PDP Alpha滤波 SNR累计值，用于自适应变换Alpha滤波系数
  fmat MaxPDPs_Value;          //!< 所有径中，功率最大N条径的PDP值
  imat MaxPDPs_Idx;            //!< 所有径中，功率最大N条径的序号
  fvec CRS_N;                  //!< 各天线，第一次CRS估计噪声功率，频域滤波前估计
  fvec CRS_S;                  //!< 各天线，第一次CRS估计信号功率，频域滤波前估计
  fvec CRS_SNR;                //!< 各天线，CRS估计SINR
  ivec IRT;                    //!< 各天线，估计的IRT
  int IRT_Final;               //!< 最终的IRT估计
  cfmat R_F;                   //!< 频域相关值
  cfmat R_T;                   //!< 时域相关值
  float fd;                    //!< 估计的多普勒扩展
  float fd_shift;              //!< 估计的jake's谱可能发生的平移量
  float last_RxFreqOffset;     //!< 历史频偏值
  bool fd_shift_1stFlag;       //!< 估计的jake's谱可能发生的平移量，使用时的判断标志
  int fd_shift_Counter;        //!< 估计的jake's谱可能发生的平移量，估计过程中的计数器
  float Freq;                  //!< 残留频偏估计值
  fvec FilterF_Noise_Scale;    //!< 频域滤波导致的信道估计SINR增益，用于时域滤波
  fvec CRS_N_2nd;              //!< 各天线，第二次CRS估计噪声功率，时域滤波后估计
  fvec CRS_S_2nd;              //!< 各天线，第二次CRS估计信号功率，时域滤波后估计
  fvec CRS_SNR_2nd;            //!< 各天线，第二次CRS估计SINR，时域滤波后估计
  bool HST_SFN_Flag;           //!< 是否是HST_SFN场景判断标志
  int HST_SFN_Counter;         //!< HST_SFN场景判断中的计数器，防止场景判断抖动
  int HST_SFN_Delay_Counter;   //!< HST_SFN场景判断中的迟滞计数器，防止两条径重叠的情况下的估计误差
  fmat Subband_N;              //!< Subband_N(r, Subband);
  fmats Subband_SNR;           //!< 子带SINR，Subband_SNR[RI](PMI, v*N_Subband+Subband);
  //DMRS Port5
  fvec DMRS_S_Port5;                    //!< 各天线，第一次Port5 DMRS估计信号功率，频域滤波前估计
  fvec DMRS_SNR_Port5;                  //!< 各天线，第一次Port5 DMRS估计SINR，频域滤波前估计
  fvec DMRS_FilterF_Noise_Scale_Port5;  //!< 频域滤波导致的信道估计SINR增益，用于时域滤波
  fvec DMRS_N_2nd_Port5;                //!< 各天线，第二次Port5 DMRS估计噪声功率，时域滤波后估计
  fvec DMRS_S_2nd_Port5;                //!< 各天线，第二次Port5 DMRS估计信号功率，时域滤波后估计
  fvec DMRS_SNR_2nd_Port5;              //!< 各天线，第二次Port5 DMRS估计SINR，时域滤波后估计
  //DMRS Port7to14
  fvec DMRS_S_Port7to14;                    //!< 各天线，第一次Port7to14 DMRS估计信号功率，频域滤波前估计
  fvec DMRS_SNR_Port7to14;                  //!< 各天线，第一次Port7to14 DMRS估计SINR，频域滤波前估计
  fvec DMRS_FilterF_Noise_Scale_Port7to14;  //!< 频域滤波导致的信道估计SINR增益，用于时域滤波
  fvec DMRS_N_2nd_Port7to14;                //!< 各天线，第二次Port7to14 DMRS估计噪声功率，时域滤波后估计
  fvec DMRS_S_2nd_Port7to14;                //!< 各天线，第二次Port7to14 DMRS估计信号功率，时域滤波后估计
  fvec DMRS_SNR_2nd_Port7to14;              //!< 各天线，第二次Port7to14 DMRS估计SINR，时域滤波后估计
  //EPDCCH DMRS
  fmat EPDCCH_DMRS_S;                    //!< 各天线，第一次EPDCCH DMRS估计信号功率，频域滤波前估计
  fmat EPDCCH_DMS_SNR;                   //!< 各天线，第一次EPDCCH DMRS估计SINR，频域滤波前估计
  fmat EPDCCH_DEMS_FilterF_Noise_Scale;  //!< 频域滤波导致的信道估计SINR增益，用于时域滤波
  fmat EPDCCH_DMRS_N_2nd;                //!< 各天线，第二次EPDCCH DMRS估计噪声功率，时域滤波后估计
  fmat EPDCCH_DMRS_S_2nd;                //!< 各天线，第二次EPDCCH DMRS估计信号功率，时域滤波后估计
  fmat EPDCCH_DMRS_SNR_2nd;              //!< 各天线，第二次EPDCCH DMRS估计SINR，时域滤波后估计
  //CSI RS
  fmats CSI_RS_PDP;         //!< CSI RS估计的PDP
  fmats CSI_MaxPDPs_Value;  //!< 所有径中，功率最大N条径的PDP值
  imats CSI_MaxPDPs_Idx;    //!< 所有径中，功率最大N条径的序号
  fmat CSI_RS_N;            //!< 各天线，第一次CSI RS估计噪声功率，频域滤波前估计
  fmat CSI_RS_S;            //!< 各天线，第一次CSI RS估计信号功率，频域滤波前估计
  fmat CSI_RS_SNR;          //!< 各天线，第一次CSI RS估计SINR，频域滤波前估计
  cfmats CSI_R_F;           //!< CSI RS估计的频域相关值
  fmat CSI_RS_N_2nd;        //!< 各天线，第二次CSI RS估计噪声功率，时域滤波后估计
  fmat CSI_RS_S_2nd;        //!< 各天线，第二次CSI RS估计信号功率，时域滤波后估计
  fmat CSI_RS_SNR_2nd;      //!< 各天线，第二次CSI RS估计SINR，时域滤波后估计
  ivec last_IRT;            //!< 历史IRT值

  fmat Ideal_PDP;  //!< 理想PDP
} MEA_S;

//! 信道估计相关结构体
typedef struct
{
  int CRS_Num[MAXCRSPORTNUM];  //!< 各端口，一个TTI内CRS符号个数
  cfmats CRS_LS_H;             //!< CRS LS信道估计结果，CRS_LS_H[r*MAXCRSPORTNUM + p](f, t)
  cfmats CRS_H_F;              //!< CRS频域滤波结果，CRS_H_F[r*MAXCRSPORTNUM + p](f, t)
  cfmats CRS_H_T;              //!< CRS时域滤波结果，CRS_H_T[r*MAXCRSPORTNUM + p](f, t)

  int DMRS_Num_Port5;      //!< Port5，一个TTI内DMRS符号个数
  cfmats DMRS_LS_H_Port5;  //!< Port5 DMRS LS信道估计结果，DMRS_LS_H_Port5[r](f, t)
  cfmats DMRS_H_F_Port5;   //!< Port5 DMRS 频域滤波结果，DMRS_H_F_Port5[r](f, t)
  cfmats DMRS_H_T_Port5;   //!< Port5 DMRS 时域滤波结果，DMRS_H_T_Port5[r](f, t)

  cfmats DMRS_LS_H_Port7to14;  //!< Port7to14 DMRS LS信道估计结果，DMRS_LS_H_Port7to14[r*MAXDMRSPORTNUM + p](f, t)
  cfmats DMRS_H_F_Port7to14;   //!< Port7to14 DMRS 频域滤波结果，DMRS_H_F_Port7to14[r*MAXDMRSPORTNUM + p](f, t)
  cfmats DMRS_H_T_Port7to14;   //!< Port7to14 DMRS 时域滤波结果，DMRS_H_T_Port7to14[r*MAXDMRSPORTNUM + p](f, t)

  cfmats EPDCCH_DMRS_LS_H;  //!< EPDCCH DMRS LS信道估计结果，EPDCCH_DMRS_LS_H[r*MAXEPDCCHRSPORTNUM + p](f, t)
  cfmats EPDCCH_DMRS_H_F;   //!< EPDCCH DMRS 频域滤波结果，EPDCCH_DMRS_H_F[r*MAXEPDCCHRSPORTNUM + p](f, t)
  cfmats EPDCCH_DMRS_H_T;   //!< EPDCCH DMRS 时域滤波结果，EPDCCH_DMRS_H_T[r*MAXEPDCCHRSPORTNUM + p](f, t)

  cfmats CSI_RS_LS_H;  //!< CSI_RS LS信道估计结果，CSI_RS_LS_H[r*MAXCSIRSPORTNUM + p](f, NZP_CSI_idx)
  cfmats CSI_RS_H_F;   //!< CSI_RS 频域滤波结果，CSI_RS_H_F[r*MAXCSIRSPORTNUM + p](f, NZP_CSI_idx)

  cfmat PSS_H;  //!< PSS信道估计结果
  cfmat SSS_H;  //!< SSS信道估计结果

  cfmats Ideal_h;    //!< 理想时域信道估计
  cfmats Ideal_H_T;  //!< 理想频域信道估计，Ideal_CRS_H_T[r*MAXCRSPORTNUM + p](f, t)
} CHE_S;

//! 仿真控制参数集合
typedef struct
{
  bool RC_FIR_Flag;         //!< RF是否使用RC滤波
  bool AGC_Flag;            //!< RF是否使用AGC
  int ADC_Len;              //!< ADC字长
  int ADC_AVEPower;         //!< ADC平均功率
  int ADC_MAX;              //!< ADC最大值
  int ADC_MIN;              //!< ADC最小值
  bool AFC_Flag;            //!< 是否使用AFC
  bool ATC_Flag;            //!< 是否使用ATC
  bool PSS_SSS_DTFlag;      //!< 是否检测PSS/SSS
  bool PBCH_DTFlag;         //!< 是否检测PBCH
  bool PCFICH_DTFlag;       //!< 是否检测PCFICH
  bool PDCCH_DTFlag;        //!< 是否检测PDCCH
  bool PHICH_DTFlag;        //!< 是否检测PHICH
  bool EPDCCH_DTFlag;       //!< 是否检测PEPDCCH
  bool PDSCH_DTFlag;        //!< 是否检测PDSCH
  bool PMCH_DTFlag;         //!< 是否检测PMCH
  bool BLIND_PBCH_DT_Flag;  //!< 是否忙检测PBCH
  bool CQI_Feedback_Flag;   //!< 是否反馈CQI
  bool RI_Feedback_Flag;    //!< 是否反馈RI
  int Fixed_RI;             //!< 不反馈RI时，固定RI值设置，与总层数N_L，码字数codeword_num相关
  bool PMI_Feedback_Flag;   //!< 是否反馈PMI
  bool RML_Flag;            //!< 空分复用下，是否使用RML解调
  bool TD_RML_Flag;         //!< 发射分集下，是否使用RML解调
  bool CQI_Th_Adapt;        //!< 是否CQI自适应门限调制
  bool Random_Selected_Subband;
  bool Random_BF_Flag;                  //!< 是否使用随机波束成型
  bool With_Simultaneous_Transmission;  //!< 是否同时有其他用户干扰，出自：3GPP 36.101 附录 B.4.1
  bool Inject_Flag;                     //!< 是否灌入数据
  int start_n_subframe;                 //!< 仿真起始子帧序号
  int n_subframe;                       //!< 当前子帧序号
  int runlength;                        //!< 仿真长度，单位TTI
} SimuCtrl_S;

//! 仿真结果相关结构体
typedef struct
{
  int BLER_PDSCH_perCW[MAX_TB_SIZE];
  int BLER_PDSCH_perCW_Num[MAX_TB_SIZE];
  int BLER_PDSCH;
  int BLER_PDSCH_Num;
  float Throughput_perCW[MAX_TB_SIZE];
  float Throughput;

  int BLER_PHICH;
  int BLER_PHICH_Num;

  int BLER_PCFICH;
  int BLER_PCFICH_Num;

  int BLER_PDCCH;
  int BLER_PDCCH_Num;

  int BLER_PCFICH_PDCCH;
  int BLER_PCFICH_PDCCH_Num;

  int BLER_EPDCCH;
  int BLER_EPDCCH_Num;

  int BLER_PBCH;
  int BLER_PBCH_Num;

  bool PSS_DT_Flag;
  int PSS_Pos;
  bool SSS_DT_Flag;
  int N_1_ID;
  int N_2_ID;
  int N_cell_ID;
  CP_Type_E CP_Type;

  bool PBCH_Blind_Flag;
  BandWidth_E dl_BandWidth;
  PHICH_Duration_Type_E phich_Duration;
  PHICH_Resource_Type_E phich_Resource;
  int SFN_High8bit;
  int SFN_Low2bit;
  int SFN;
  bool PBCH_Flag;
  cfvec MIMO_symbs_out;

  bool PCFICH_Flag;
  int Ctrl_Symb_Num;
  int Ctrl_Symb_Num_Special;

  //Ref
  float Ref_SNR;
  float Ref_Value;
  float Ref_Alpha;
  float Ref_Beta;
  float Ref_Gamma;
  fvec SNR;
  fvec PBCH_BLER;
  fvec PCHICH_BLER;
  fvec PDCCH_BLER;
  fvec PCHICH_PDCCH_BLER;
  fvec PHICH_BLER;
  fvec EPDCCH_BLER;
  fvec PDSCH_Throughput;
  fvec PDSCH_BLER;
  bool Feedback_Test_Flag;

  fvec CQI_SNR_eff;
  ivec CQI_SNR_eff_Num;
} Result_S;

#endif
