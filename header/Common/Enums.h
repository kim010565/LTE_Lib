/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Enums.h
    \brief Enums头文件
    内容 ：一些枚举型定义
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Enums_h_
#define _Enums_h_

//! UE能力等级枚举。出自 ：3GPP 36.306 4.1节
typedef enum {
  Category_NULL,  //!< 空UE能力等级
  Category_1,     //!< UE能力等级1
  Category_2,     //!< UE能力等级2
  Category_3,     //!< UE能力等级3
  Category_4,     //!< UE能力等级4
  Category_5,     //!< UE能力等级5
  Category_6,     //!< UE能力等级6
  Category_7,     //!< UE能力等级7
  Category_8,     //!< UE能力等级8
  Category_9,     //!< UE能力等级9
  Category_10,    //!< UE能力等级10
  Category_11,    //!< UE能力等级11
  Category_12     //!< UE能力等级12
} UE_Category_E;

//! LTE无线帧结构枚举
typedef enum {
  FDD,  //!< 频分复用FDD
  TDD   //!< 时分复用TDD
} FrameStructureType_E;

//! RE类型枚举
typedef enum {
  EMPTY_RE,          //!< 空闲RE
  PSS,               //!< PSS所占RE
  SSS,               //!< SSS所占RE
  CRS,               //!< CRS所占RE
  DMRS_Port5,        //!< Port5 DMRS所占RE
  DMRS_Port7_14,     //!< Port7-14 DMRS所占RE
  DMRS_Port107_110,  //!< Port107-110 DMRS所占RE
  CSI_RS,            //!< CSI RS所占RE
  MBSFN_RS,          //!< MBSFN RS所占RE
  PRS,               //!< PRS所占RE
  PDSCH,             //!< PDSCH所占RE
  PBCH,              //!< PBCH所占RE
  PMCH,              //!< PMCH所占RE
  PCFICH,            //!< PCFICH所占RE
  PDCCH,             //!< PDCCH所占RE
  PHICH,             //!< PHICH所占RE
  EPDCCH             //!< EPDCCH所占RE
} RE_Type_E;

//! 子帧类型枚举
typedef enum {
  D,     //!< 下行子帧
  S,     //!< 特殊子帧
  U,     //!< 上行子帧
  DandU  //!< 既有下行又有上行子帧，即FDD下子帧
} SubframeType_E;

//! 带宽枚举。出自 ：3GPP 36.101 5.6节
typedef enum {
  BW1p4MHz,  //!< 1.4M带宽
  BW3MHz,    //!< 3M带宽
  BW5MHz,    //!< 5M带宽
  BW10MHz,   //!< 10M带宽
  BW15MHz,   //!< 15M带宽
  BW20MHz    //!< 20M带宽
} BandWidth_E;

//! TDD上下行配置枚举。出自 ：3GPP 36.211 4.2节
typedef enum {
  ULDL_CFG0,  //!< 上下行配置0
  ULDL_CFG1,  //!< 上下行配置1
  ULDL_CFG2,  //!< 上下行配置2
  ULDL_CFG3,  //!< 上下行配置3
  ULDL_CFG4,  //!< 上下行配置4
  ULDL_CFG5,  //!< 上下行配置5
  ULDL_CFG6   //!< 上下行配置6
} ULDLConfiguration_E;

//! TDD特殊子帧配置枚举。出自 ：3GPP 36.211 4.2节
typedef enum {
  SS_CFG0,  //!< 特殊子帧配置0
  SS_CFG1,  //!< 特殊子帧配置1
  SS_CFG2,  //!< 特殊子帧配置2
  SS_CFG3,  //!< 特殊子帧配置3
  SS_CFG4,  //!< 特殊子帧配置4
  SS_CFG5,  //!< 特殊子帧配置5
  SS_CFG6,  //!< 特殊子帧配置6
  SS_CFG7,  //!< 特殊子帧配置7
  SS_CFG8,  //!< 特殊子帧配置8
  SS_CFG9   //!< 特殊子帧配置9
} SpecialSubframeConfiguration_E;

//! CP类型枚举。出自 ：3GPP 36.211 6.2.3节
typedef enum {
  NormalCP,   //!< 正常CP
  ExtendedCP  //!< 扩展CP
} CP_Type_E;

//! 传输模式TM枚举。出自 ：3GPP 36.213 7.1节
typedef enum {
  NULL_TM,  //!< 传输模式0
  TM1,      //!< 传输模式1
  TM2,      //!< 传输模式2
  TM3,      //!< 传输模式3
  TM4,      //!< 传输模式4
  TM5,      //!< 传输模式5
  TM6,      //!< 传输模式6
  TM7,      //!< 传输模式7
  TM8,      //!< 传输模式8
  TM9,      //!< 传输模式9
  TM10      //!< 传输模式10
} TM_E;

//! 传输方式枚举。出自 ：3GPP 36.213 7.1节
typedef enum {
  Single_Antenna_Port,  //!< 单端口传输
  Transmit_Diversity,   //!< 发射分集传输
  Large_Delay_CDD,      //!< Large_Delay_CDD开环传输
  Closed_Loop_SM,       //!< Closed_Loop_SM闭环传输
  MU_MIMO,              //!< 多用户MIMO传输
  Dual_layer,           //!< 双层传输
  Up_to_8_layer         //!< Up_to_8_layer传输
} Transmission_Scheme_E;

//! 发射端口枚举。出自 ：3GPP 36.211
typedef enum {
  Port0,    //!< 端口0，用于CRS, PBCH, PCFICH, PDCCH, PHICH, PDSCH
  Port1,    //!< 端口1，用于CRS, PBCH, PCFICH, PDCCH, PHICH, PDSCH
  Port2,    //!< 端口2，用于CRS, PBCH, PCFICH, PDCCH, PHICH, PDSCH
  Port3,    //!< 端口3，用于CRS, PBCH, PCFICH, PDCCH, PHICH, PDSCH
  Port4,    //!< 端口4，用于MBSFN RS, PMCH
  Port5,    //!< 端口5，用于TM7 DMRS, PDSCH
  Port6,    //!< 端口6，用于PRS
  Port7,    //!< 端口7，用于TM8 DMRS, TM9 DMRS, TM10 DMRS, PDSCH
  Port8,    //!< 端口8，用于TM8 DMRS, TM9 DMRS, TM10 DMRS, PDSCH
  Port9,    //!< 端口9，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port10,   //!< 端口10，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port11,   //!< 端口11，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port12,   //!< 端口12，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port13,   //!< 端口13，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port14,   //!< 端口14，用于TM9 DMRS, TM10 DMRS, PDSCH
  Port15,   //!< 端口15，用于CSI RS
  Port16,   //!< 端口16，用于CSI RS
  Port17,   //!< 端口17，用于CSI RS
  Port18,   //!< 端口18，用于CSI RS
  Port19,   //!< 端口19，用于CSI RS
  Port20,   //!< 端口20，用于CSI RS
  Port21,   //!< 端口21，用于CSI RS
  Port22,   //!< 端口22，用于CSI RS
  Port107,  //!< 端口107，用于EPDCCH RS, EDPCCH
  Port108,  //!< 端口108，用于EPDCCH RS, EDPCCH
  Port109,  //!< 端口109，用于EPDCCH RS, EDPCCH
  Port110   //!< 端口110，用于EPDCCH RS, EDPCCH
} Port_Type_E;

//! DCI类型枚举。出自 ：3GPP 36.212 5.3.3.1节
typedef enum {
  NULL_DCI,       //!< 无DCI
  DCI_Format_0,   //!< DCI格式0，上行调度
  DCI_Format_1,   //!< DCI格式1，下行调度
  DCI_Format_1A,  //!< DCI格式1A，下行调度
  DCI_Format_1B,  //!< DCI格式1B，下行调度
  DCI_Format_1C,  //!< DCI格式1C，下行调度
  DCI_Format_1D,  //!< DCI格式1D，下行调度
  DCI_Format_2,   //!< DCI格式2，下行调度
  DCI_Format_2A,  //!< DCI格式2A，下行调度
  DCI_Format_2B,  //!< DCI格式2B，下行调度
  DCI_Format_2C,  //!< DCI格式2C，下行调度
  DCI_Format_2D,  //!< DCI格式2D，下行调度
  DCI_Format_3,   //!< DCI格式3，下行调度
  DCI_Format_3A,  //!< DCI格式3A，下行调度
  DCI_Format_4    //!< DCI格式1，上行调度
} DCI_Format_Type_E;

//! DCI搜索空间类型枚举。出自 ：3GPP 36.213 9.1.1节
typedef enum {
  Common_Search_Space,      //!< 公共搜索空间
  UE_Specific_Search_Space  //!< UE专用搜索空间
} DCI_Search_Space_E;

//! EPDCCH分布方式枚举。出自 ：3GPP 36.213 9.1.4节
typedef enum {
  Localized_EPDCCH,   //!< 集中式EPDCCH分布
  Distributed_EPDCCH  //!< 离散式EPDCCH分布
} TMType_EPDCCH_E;

//! RNTI类型枚举。出自 ：3GPP 36.321 7.1节
typedef enum {
  SI_RNTI_Type,           //!< SI-RNTI，FFFF
  C_RNTI_Type,            //!< C-RNTI，003D-FFF3
  P_RNTI_Type,            //!< P-RNTI，FFFE
  RA_RNTI_Type,           //!< RA-RNTI，0001-003C
  TPC_RNTI_Type,          //!< TPC-RNTI，003D-FFF3
  Temporary_C_RNTI_Type,  //!< Temporary C-RNTI，003D-FFF3
  SPS_RNTI_Type           //!< SPS-RNTI，003D-FFF3
} RNTI_Type_E;

//! PHICH Duration枚举。出自 ：3GPP 36.211 6.9.3节
typedef enum {
  Normal_Duration,   //!< PHICH正常Duration
  Extended_Duration  //!< PHICH扩展Duration
} PHICH_Duration_Type_E;

//! PHICH Resource类型枚举。出自 ：3GPP 36.211 6.9节
typedef enum {
  oneSixth,  //!< Ng = 1/6
  half,      //!< Ng = 1/2
  one,       //!< Ng = 1
  two        //!< Ng = 2
} PHICH_Resource_Type_E;

//! QCL类型枚举。出自 ：3GPP 36.213 7.1.10节
typedef enum {
  QCL_TypeA,  //!< QCL类型A
  QCL_TypeB   //!< QCL类型B
} QCL_Type_E;

//! 无线信道类型枚举
typedef enum {
  AWGN,     //!< 高斯白噪信道
  EPA,      //!< EPA信道。出自 ：3GPP 36.101 附录B.2.1
  EVA,      //!< EVA信道。出自 ：3GPP 36.101 附录B.2.1
  ETU,      //!< ETU信道。出自 ：3GPP 36.101 附录B.2.1
  HST,      //!< HST信道。出自 ：3GPP 36.101 附录B.3
  HST_SFN,  //!< HST SFN信道。出自 ：3GPP 36.101 附录？？？（尚未更新）
  B_2_4     //!< CQI测试信道。出自 ：3GPP 36.101 附录B.2.4
} ChannelType_E;

//! 天线相关性枚举。出自3GPP 36.101 附录B.2.3.2
typedef enum {
  LowCorr,  //!< 低相关
  MidCorr,  //!< 中相关
  HighCorr  //!< 高相关
} ChanCorrInd_E;

//! 无线信道多径生成插值方式枚举
typedef enum {
  InterpHold,    //!< 保持
  InterpLinear,  //!< 线性插值
  InterpCubic    //!< 三次样条插值
} InterpType_E;

//! 小区类型枚举
typedef enum {
  ServingCell,       //!< 服务小区
  InterfCellwiInfo,  //!< 带有辅助信息的干扰小区
  InterfCellwoInfo   //!< 不带辅助信息的干扰小区
} CellType_E;

//! UE类型枚举
typedef enum {
  Wanted_UE,  //!< 需要检测的用户
  Interf_UE,  //!< 干扰用户
} UEType_E;

//! CSI上报方式枚举。出自3GPP 36.213 7.2.1节和7.2.2节
typedef enum {
  RM10,  //!< ReportMode 1-0
  RM11,  //!< ReportMode 1-1
  RM12,  //!< ReportMode 1-2
  RM20,  //!< ReportMode 2-0
  RM21,  //!< ReportMode 2-1
  RM22,  //!< ReportMode 2-2
  RM30,  //!< ReportMode 3-0
  RM31,  //!< ReportMode 3-1
  RM32   //!< ReportMode 3-2
} ReportModeType_E;

//! PHICH Pattern枚举。出自3GPP 36.101 附录A.3.6
typedef enum {
  Wanted_User,      //!< 需要检测的用户
  Interfering_User  //!< 干扰用户
} PHICH_Wanted_Pattern_E;

//! PHICH_HARQ Pattern枚举
typedef enum {
  PHICH_ACK,    //!< ACK
  PHICH_NACK,   //!< NACK
  PHICH_Random  //!< ACK或NACK随机
} PHICH_HARQ_Pattern_E;

//! 参考量类型枚举
typedef enum {
  NULL_Ref,                      //!< 无参考
  Ref_PBCH_BLER,                 //!< PBCH BLER参考
  Ref_PCHICH_PDCCH_BLER,         //!< PCHICH+PDCCH BLER参考
  Ref_PHICH_BLER,                //!< PHICH BLER参考
  Ref_EPDCCH_BLER,               //!< EPDCCH BLER参考
  Ref_PDSCH_Throughput,          //!< PDSCH吞吐量参考
  Ref_CQI_BLER,                  //!< CQI参考
  Ref_Median_CQI_Reported,       //!< CQI上报中值参考
  Ref_Freq_Select_CQI_Reported,  //!< 频选CQI上报参考
  Ref_Freq_Non_Select_CQI_Reported,
  Ref_Freq_Select_Interfere_CQI_Reported,
  Ref_UE_Selected_CQI_Reported,
  Ref_PMI_Reported,
  Ref_RI_Reported
} Ref_Type_E;
#endif
