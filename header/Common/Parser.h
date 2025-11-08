/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Parser.h
    \brief Parser头文件
    内容 ：输入文件解析相关定义、声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Parser_h_
#define _Parser_h_

#include "Definitions.h"
#include "Enums.h"
#include "FastFunctions.h"
#include "Typedefs.h"

//! Parser类， 用来读取配置文件
class Parser_C  //Parser类， 用来读取配置文件
{
 public:
  Parser_C();
  ~Parser_C();

  void LoadFile(const string &filename);
  int GetIterNum();
  string ReadString(const string &str_key, int iter_idx, int l3, int l2, int l1, int l0, string v_default);
  float ReadValue(const string &str_key, int iter_idx, int l3, int l2, int l1, int l0, float v_default);
  int Getl3Num(const string &str_key, int iter_idx);
  int Getl2Num(const string &str_key, int iter_idx, int l3);
  int Getl1Num(const string &str_key, int iter_idx, int l3, int l2);
  int Getl0Num(const string &str_key, int iter_idx, int l3, int l2, int l1);

 private:
  string str_filename;                    //!< 文件路径及文件名
  vector<map<string, string>> m_Strings;  //!< 容器，存储变量名称关键字
  vector<map<string, string>> m_Values;   //!< 容器，存储变量值
  vector<map<string, int>> m_Lines;       //!< 容器，存储变量名称关键字所在行
  int iter_num;                           //!< iter个数
};

//! 无线帧类型识别
/*!
  \param str ： 字符串值，输入
  \return 无线帧类型
*/
inline FrameStructureType_E FrameStructureType_Str(const string &str) {
  FrameStructureType_E temp = FDD;
  if (0 == str.compare("FDD")) {
    temp = FDD;
  } else {
    temp = TDD;
  }
  return temp;
}

//! 带宽识别
/*!
  \param str ： 字符串值，输入
  \return 带宽
*/
inline BandWidth_E BandWidth_Str(const string &str) {
  BandWidth_E temp = BW20MHz;
  if (0 == str.compare("1.4M")) {
    temp = BW1p4MHz;
  } else if (0 == str.compare("3M")) {
    temp = BW3MHz;
  } else if (0 == str.compare("5M")) {
    temp = BW5MHz;
  } else if (0 == str.compare("10M")) {
    temp = BW10MHz;
  } else if (0 == str.compare("15M")) {
    temp = BW15MHz;
  } else {
    temp = BW20MHz;
  }
  return temp;
}

//! PHICH_Duration识别
/*!
  \param str ： 字符串值，输入
  \return PHICH_Duration
*/
inline PHICH_Duration_Type_E PHICH_Duration_Type_Str(const string &str) {
  PHICH_Duration_Type_E temp = Normal_Duration;
  if (0 == str.compare("normal")) {
    temp = Normal_Duration;
  } else {
    temp = Extended_Duration;
  }
  return temp;
}

//! PHICH_Resource识别
/*!
  \param str ： 字符串值，输入
  \return PHICH_Resource
*/
inline PHICH_Resource_Type_E PHICH_Resource_Type_Str(const string &str) {
  PHICH_Resource_Type_E temp = oneSixth;
  if (0 == str.compare("1/6")) {
    temp = oneSixth;
  } else if (0 == str.compare("1/2")) {
    temp = half;
  } else if (0 == str.compare("1")) {
    temp = one;
  } else {
    temp = two;
  }
  return temp;
}

//! CP类型识别
/*!
  \param str ： 字符串值，输入
  \return CP类型
*/
inline CP_Type_E CP_Type_Str(const string &str) {
  CP_Type_E temp = NormalCP;
  if (0 == str.compare("normal")) {
    temp = NormalCP;
  } else {
    temp = ExtendedCP;
  }
  return temp;
}

//! RNTI类型识别
/*!
  \param str ： 字符串值，输入
  \return RNTI类型
*/
inline RNTI_Type_E RNTI_Type_Str(const string &str) {
  RNTI_Type_E temp = C_RNTI_Type;
  if (0 == str.compare("SI_RNTI")) {
    temp = SI_RNTI_Type;
  } else if (0 == str.compare("C_RNTI")) {
    temp = C_RNTI_Type;
  } else if (0 == str.compare("P_RNTI")) {
    temp = P_RNTI_Type;
  } else if (0 == str.compare("RA_RNTI")) {
    temp = RA_RNTI_Type;
  } else if (0 == str.compare("TPC_RNTI")) {
    temp = TPC_RNTI_Type;
  } else if (0 == str.compare("Temporary_C_RNTI")) {
    temp = Temporary_C_RNTI_Type;
  } else {
    temp = SPS_RNTI_Type;
  }
  return temp;
}

//! 上报模式识别
/*!
  \param str ： 字符串值，输入
  \return 上报模式
*/
inline ReportModeType_E ReportModeType_Str(const string &str) {
  ReportModeType_E temp = RM10;
  if (0 == str.compare("RM10")) {
    temp = RM10;
  } else if (0 == str.compare("RM11")) {
    temp = RM11;
  } else if (0 == str.compare("RM12")) {
    temp = RM12;
  } else if (0 == str.compare("RM20")) {
    temp = RM20;
  } else if (0 == str.compare("RM21")) {
    temp = RM21;
  } else if (0 == str.compare("RM22")) {
    temp = RM22;
  } else if (0 == str.compare("RM30")) {
    temp = RM30;
  } else if (0 == str.compare("RM31")) {
    temp = RM31;
  } else {
    temp = RM32;
  }
  return temp;
}

//! DCI格式识别，TM_Default下使用TM对应的默认DCI格式
/*!
  \param str ： 字符串值，输入
  \param TM ： 传输模式，输入
  \return DCI格式识别
*/
inline DCI_Format_Type_E DCI_Format_Type_Str(const string &str, TM_E TM) {
  DCI_Format_Type_E temp = NULL_DCI;
  if (0 == str.compare("TM_Default")) {
    switch (TM) {
      case TM1:
      case TM2:
      case TM7:
        temp = DCI_Format_1;
        break;
      case TM6:
        temp = DCI_Format_1B;
        break;
      case TM5:
        temp = DCI_Format_1C;
        break;
      case TM4:
        temp = DCI_Format_2;
        break;
      case TM3:
        temp = DCI_Format_2A;
        break;
      case TM8:
        temp = DCI_Format_2B;
        break;
      case TM9:
        temp = DCI_Format_2C;
        break;
      case TM10:
        temp = DCI_Format_2D;
        break;
      default:
        temp = NULL_DCI;
        break;
    }
  } else if (0 == str.compare("NULL_DCI")) {
    temp = NULL_DCI;
  } else if (0 == str.compare("Format_0")) {
    temp = DCI_Format_0;
  } else if (0 == str.compare("Format_1")) {
    temp = DCI_Format_1;
  } else if (0 == str.compare("Format_1A")) {
    temp = DCI_Format_1A;
  } else if (0 == str.compare("Format_1B")) {
    temp = DCI_Format_1B;
  } else if (0 == str.compare("Format_1C")) {
    temp = DCI_Format_1C;
  } else if (0 == str.compare("Format_1D")) {
    temp = DCI_Format_1D;
  } else if (0 == str.compare("Format_2")) {
    temp = DCI_Format_2;
  } else if (0 == str.compare("Format_2A")) {
    temp = DCI_Format_2A;
  } else if (0 == str.compare("Format_2B")) {
    temp = DCI_Format_2B;
  } else if (0 == str.compare("Format_2C")) {
    temp = DCI_Format_2C;
  } else if (0 == str.compare("Format_2D")) {
    temp = DCI_Format_2D;
  } else if (0 == str.compare("Format_3")) {
    temp = DCI_Format_3;
  } else if (0 == str.compare("Format_3A")) {
    temp = DCI_Format_3A;
  } else {
    temp = DCI_Format_4;
  }
  return temp;
}

//! DCI搜索空间识别
/*!
  \param str ： 字符串值，输入
  \return DCI搜索空间
*/
inline DCI_Search_Space_E DCI_Search_Space_Str(const string &str) {
  DCI_Search_Space_E temp = Common_Search_Space;
  if (0 == str.compare("Common")) {
    temp = Common_Search_Space;
  } else {
    temp = UE_Specific_Search_Space;
  }
  return temp;
}

//! 天线相关性识别
/*!
  \param str ： 字符串值，输入
  \return 天线相关性
*/
inline ChanCorrInd_E ChanCorrInd_Str(const string &str) {
  ChanCorrInd_E temp = LowCorr;
  if (0 == str.compare("Low")) {
    temp = LowCorr;
  } else if (0 == str.compare("Mid")) {
    temp = MidCorr;
  } else {
    temp = HighCorr;
  }
  return temp;
}

//! 无线信道类型识别
/*!
  \param str ： 字符串值，输入
  \return 无线信道类型
*/
inline ChannelType_E ChannelType_Str(const string &str) {
  ChannelType_E temp = AWGN;
  if (0 == str.compare("AWGN")) {
    temp = AWGN;
  } else if (0 == str.compare("EPA")) {
    temp = EPA;
  } else if (0 == str.compare("EVA")) {
    temp = EVA;
  } else if (0 == str.compare("ETU")) {
    temp = ETU;
  } else if (0 == str.compare("HST")) {
    temp = HST;
  } else if (0 == str.compare("HST_SFN")) {
    temp = HST_SFN;
  } else if (0 == str.compare("B_2_4")) {
    temp = B_2_4;
  } else {
    temp = AWGN;
  }
  return temp;
}

//! 小区类型识别
/*!
  \param str ： 字符串值，输入
  \return 小区类型
*/
inline CellType_E CellType_Str(const string &str) {
  CellType_E temp = ServingCell;
  if (0 == str.compare("ServingCell")) {
    temp = ServingCell;
  } else if (0 == str.compare("InterfCellwiInfo")) {
    temp = InterfCellwiInfo;
  } else {
    temp = InterfCellwoInfo;
  }
  return temp;
}

inline UEType_E UEType_Str(const string &str) {
  UEType_E temp = Wanted_UE;
  if (0 == str.compare("Interf_UE")) {
    temp = Interf_UE;
  } else {
    temp = Wanted_UE;
  }
  return temp;
}

//! 无线信道多径插值类型识别
/*!
  \param str ： 字符串值，输入
  \return 无线信道多径插值类型
*/
inline InterpType_E InterpType_Str(const string &str) {
  InterpType_E temp = InterpHold;
  if (0 == str.compare("Hold")) {
    temp = InterpHold;
  } else if (0 == str.compare("Linear")) {
    temp = InterpLinear;
  } else {
    temp = InterpCubic;
  }
  return temp;
}

//! PHICH_Wanted_Pattern识别
/*!
  \param str ： 字符串值，输入
  \return PHICH_Wanted_Pattern
*/
inline PHICH_Wanted_Pattern_E PHICH_Wanted_Pattern_Str(const string &str) {
  PHICH_Wanted_Pattern_E temp = Wanted_User;
  if (0 == str.compare("Wanted_User")) {
    temp = Wanted_User;
  } else {
    temp = Interfering_User;
  }
  return temp;
}

//! PHICH_HARQ_Pattern识别
/*!
  \param str ： 字符串值，输入
  \return PHICH_HARQ_Pattern
*/
inline PHICH_HARQ_Pattern_E PHICH_HARQ_Pattern_Str(const string &str) {
  PHICH_HARQ_Pattern_E temp = PHICH_ACK;
  if (0 == str.compare("ACK")) {
    temp = PHICH_ACK;
  } else if (0 == str.compare("NACK")) {
    temp = PHICH_NACK;
  } else {
    temp = PHICH_Random;
  }
  return temp;
}

//! EPDCCH资源分布类型识别
/*!
  \param str ： 字符串值，输入
  \return EPDCCH资源分布类型
*/
inline TMType_EPDCCH_E TMType_EPDCCH_Str(const string &str) {
  TMType_EPDCCH_E temp = Localized_EPDCCH;
  if (0 == str.compare("Localized_EPDCCH")) {
    temp = Localized_EPDCCH;
  } else {
    temp = Distributed_EPDCCH;
  }
  return temp;
}

//! QCL类型识别
/*!
  \param str ： 字符串值，输入
  \return QCL类型
*/
inline QCL_Type_E QCL_Type_Str(const string &str) {
  QCL_Type_E temp = QCL_TypeA;
  if (0 == str.compare("TypeA")) {
    temp = QCL_TypeA;
  } else {
    temp = QCL_TypeB;
  }
  return temp;
}

//! 性能统计参考类型识别
/*!
  \param str ： 字符串值，输入
  \return 性能统计参考类型
*/
inline Ref_Type_E Ref_Type_Str(const string &str) {
  Ref_Type_E temp = NULL_Ref;
  if (0 == str.compare("PBCH_BLER")) {
    temp = Ref_PBCH_BLER;
  } else if (0 == str.compare("PCHICH_PDCCH_BLER")) {
    temp = Ref_PCHICH_PDCCH_BLER;
  } else if (0 == str.compare("PHICH_BLER")) {
    temp = Ref_PHICH_BLER;
  } else if (0 == str.compare("EPDCCH_BLER")) {
    temp = Ref_EPDCCH_BLER;
  } else if (0 == str.compare("PDSCH_Throughput")) {
    temp = Ref_PDSCH_Throughput;
  } else if (0 == str.compare("CQI_BLER")) {
    temp = Ref_CQI_BLER;
  } else if (0 == str.compare("Median_CQI_Reported")) {
    temp = Ref_Median_CQI_Reported;
  } else if (0 == str.compare("Freq_Select_CQI_Reported")) {
    temp = Ref_Freq_Select_CQI_Reported;
  } else if (0 == str.compare("Freq_Non_Select_CQI_Reported")) {
    temp = Ref_Freq_Non_Select_CQI_Reported;
  } else if (0 == str.compare("Freq_Select_Interfere_CQI_Reported")) {
    temp = Ref_Freq_Select_Interfere_CQI_Reported;
  } else if (0 == str.compare("UE_Selected_CQI_Reported")) {
    temp = Ref_UE_Selected_CQI_Reported;
  } else if (0 == str.compare("PMI_Reported")) {
    temp = Ref_PMI_Reported;
  } else if (0 == str.compare("RI_Reported")) {
    temp = Ref_RI_Reported;
  }
  return temp;
}

#endif
