/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Common.h
    \brief Common头文件
    内容 ：公共函数定义
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Common_h_
#define _Common_h_

#include "../Common/DeModulation.h"
#include "../Common/Decoder.h"
#include "../Common/Definitions.h"
#include "../Common/Encoder.h"
#include "../Common/FastFunctions.h"
#include "../Common/Globals.h"
#include "../Common/Matrixs.h"
#include "../Common/Modulation.h"
#include "../Common/Randoms.h"
#include "../Common/Scrambling.h"
#include "../Common/Structs.h"
#include "../Common/Tables.h"
#include "../Common/Typedefs.h"

//! 不同带宽下的FFT长度获取。出自 ：3GPP 36.211 6.12节
/*!
  \param BandWidth ： 带宽，输入
  \param delta_f ： 子载波间隔，默认15kHz，输入
  \return FFT长度
*/
inline int Get_N_FFT(BandWidth_E BandWidth, int delta_f = 15000) {
  int N_FFT = 2048;
  switch (BandWidth) {
    case BW1p4MHz:
      N_FFT = 128;
      break;
    case BW3MHz:
      N_FFT = 256;
      break;
    case BW5MHz:
      N_FFT = 512;
      break;
    case BW10MHz:
      N_FFT = 1024;
      break;
    case BW15MHz:
      N_FFT = 1536;
      break;
    default:
      N_FFT = 2048;
      break;
  }
  if (7500 == delta_f) {
    N_FFT = N_FFT * 2;
  }
  return N_FFT;
}

//! 不同带宽+符号下的CP长度获取。出自 ：3GPP 36.211 6.12节
/*!
  \param BandWidth ： 带宽，输入
  \param CP_Type ： CP类型，输入
  \param l ： 符号序号，输入
  \param delta_f ： 子载波间隔，默认15kHz，输入
  \return 该符号的CP长度N_CP_l
*/
inline int Get_N_CP_l(BandWidth_E BandWidth, CP_Type_E CP_Type, int l, int delta_f = 15000) {
  int N_CP = 160;
  if (NormalCP == CP_Type) {
    if (15000 == delta_f) {
      if (0 == l || 7 == l) {
        N_CP = 160;
      } else {
        N_CP = 144;
      }
    }
  } else {
    if (15000 == delta_f) {
      N_CP = 512;
    } else {
      N_CP = 1024;
    }
  }
  if (BW1p4MHz == BandWidth) {
    N_CP = N_CP / 16;
  } else if (BW3MHz == BandWidth) {
    N_CP = N_CP / 8;
  } else if (BW5MHz == BandWidth) {
    N_CP = N_CP / 4;
  } else if (BW10MHz == BandWidth) {
    N_CP = N_CP / 2;
  } else if (BW15MHz == BandWidth) {
    N_CP = N_CP * 3 / 4;
  } else {
    N_CP = N_CP * 1;
  }
  return N_CP;
}

//! 不同带宽下，一个TTI内样点个数获取。出自 ：3GPP 36.211 4节
/*!
  \param BandWidth ： 带宽，输入
  \return 一个TTI内样点个数
*/
inline int Get_SamplesPerTTI(BandWidth_E BandWidth) {
  int Samples = 30720;
  if (BW1p4MHz == BandWidth) {
    Samples = 1920;
  } else if (BW3MHz == BandWidth) {
    Samples = 3840;
  } else if (BW5MHz == BandWidth) {
    Samples = 7680;
  } else if (BW10MHz == BandWidth) {
    Samples = 15360;
  } else if (BW15MHz == BandWidth) {
    Samples = 23040;
  } else {
    Samples = 30720;
  }
  return Samples;
}

//! 不同带宽下，下行可用RB个数（N_DL_RB）获取。出自 ：3GPP 36.101 5.6节
/*!
  \param BandWidth ： 带宽，输入
  \return 下行可用RB个数（N_DL_RB）
*/
inline int Get_N_DL_RB(BandWidth_E BandWidth) {
  int N_DL_RB = 100;
  switch (BandWidth) {
    case BW1p4MHz:
      N_DL_RB = 6;
      break;
    case BW3MHz:
      N_DL_RB = 15;
      break;
    case BW5MHz:
      N_DL_RB = 25;
      break;
    case BW10MHz:
      N_DL_RB = 50;
      break;
    case BW15MHz:
      N_DL_RB = 75;
      break;
    default:
      N_DL_RB = 100;
      break;
  }
  return N_DL_RB;
}

//! 不同带宽下，一个RB内子载波个数（N_RB_sc）获取。出自 ：3GPP 36.211 6.2.3节
/*!
  \param CP_Type ： CP类型，输入
  \param delta_f ： 子载波间隔，默认15kHz，输入
  \return 一个RB内子载波个数（N_RB_sc）
*/
inline int Get_N_RB_sc(CP_Type_E CP_Type, int delta_f = 15000) {
  int N_RB_sc = 12;
  if (NormalCP == CP_Type) {
    N_RB_sc = 12;
  } else {
    if (15000 == delta_f) {
      N_RB_sc = 12;
    } else {
      N_RB_sc = 24;
    }
  }
  return N_RB_sc;
}

//! 不同带宽下，一个时隙内符号个数（N_DL_symb）获取。出自 ：3GPP 36.211 6.2.3节
/*!
  \param CP_Type ： CP类型，输入
  \param delta_f ： 子载波间隔，默认15kHz，输入
  \return 一个时隙内符号个数（N_DL_symb）
*/
inline int Get_N_DL_symb(CP_Type_E CP_Type, int delta_f = 15000) {
  int N_DL_symb = 7;
  if (NormalCP == CP_Type) {
    N_DL_symb = 7;
  } else {
    if (15000 == delta_f) {
      N_DL_symb = 6;
    } else {
      N_DL_symb = 3;
    }
  }
  return N_DL_symb;
}

//! 不同带宽下，子带大小获取。出自 ：3GPP 36.213 7.2.1节 表7.2.1-3
/*!
  \param N_DL_RB ： N_DL_RB，输入
  \return 子带大小
*/
inline int Get_SubbandSize(int N_DL_RB) {
  int k_Subband = N_DL_RB;
  if (N_DL_RB <= 7) {
    k_Subband = N_DL_RB;
  } else if (N_DL_RB <= 10) {
    k_Subband = 4;
  } else if (N_DL_RB <= 26) {
    k_Subband = 4;
  } else if (N_DL_RB <= 63) {
    k_Subband = 6;
  } else {
    k_Subband = 8;
  }
  return k_Subband;
}

//! 不同带宽下，子带大小获取。出自 ：3GPP 36.213 7.2.2节 表7.2.2-2
/*!
  \param N_DL_RB ： N_DL_RB，输入
  \param J ： BP个数，输入/输出
  \return 子带大小
*/
inline int Get_SubbandSize_PUCCH(int N_DL_RB, int *J) {
  int k_Subband = N_DL_RB;
  if (N_DL_RB <= 10) {
    k_Subband = 4;
    *J = 1;
  } else if (N_DL_RB <= 26) {
    k_Subband = 4;
    *J = 2;
  } else if (N_DL_RB <= 63) {
    k_Subband = 6;
    *J = 3;
  } else {
    k_Subband = 8;
    *J = 4;
  }
  return k_Subband;
}
//! 不同带宽下，Perfered M个子带大小获取。出自 ：3GPP 36.213 7.2.1节 表7.2.1-5
/*!
  \param N_DL_RB ： N_DL_RB，输入
  \param k_Subband ： 子带大小，输出
  \param M ： 子带大小，Perfered子带个数
  \return 无
*/
inline void Get_Perfered_M_SubbandSize(int N_DL_RB, int *k_Subband, int *M) {
  *k_Subband = N_DL_RB;
  *M = 1;
  if (N_DL_RB <= 7) {
    *k_Subband = N_DL_RB;
    *M = 1;
  } else if (N_DL_RB <= 10) {
    *k_Subband = 2;
    *M = 1;
  } else if (N_DL_RB <= 26) {
    *k_Subband = 2;
    *M = 3;
  } else if (N_DL_RB <= 63) {
    *k_Subband = 3;
    *M = 5;
  } else {
    *k_Subband = 4;
    *M = 6;
  }
}

//! 子帧类型获取。出自 ：3GPP 36.211 4.2节 表4.2-2
/*!
  \param n_subframe ： 子帧序号，输入
  \param ULDLConfiguration ： 上下行配置，输入
  \param FrameStructureType ： 无线帧类型，输入
  \return 子帧类型
*/
inline SubframeType_E Get_SubframeType(int n_subframe, ULDLConfiguration_E ULDLConfiguration, FrameStructureType_E FrameStructureType) {
  if (FDD == FrameStructureType) {
    return DandU;
  } else {
    return ULDL_CFG_Table[(int)ULDLConfiguration][n_subframe];
  }
}

//! DCI搜索空间相关的Yk计算。出自 ：3GPP 36.213 9.1.1节
/*!
  \param n_RNTI ： n_RNTI，输入
  \param k ： k，输入
  \return Yk
*/
inline int Calc_Yk(int n_RNTI, int k) {
  int i = 0;
  const uint32 A = 39827;
  const uint32 D = 65537;
  uint32 Y = n_RNTI;
  for (i = 0; i <= k; i++) {
    Y = (A * Y) % D;
  }
  return (int)Y;
}

//! EPDCCH DCI搜索空间相关的Yk计算。出自 ：3GPP 36.213 9.1.4节
/*!
  \param n_RNTI ： n_RNTI，输入
  \param k ： k，输入
  \return Yk
*/
inline int Calc_Yk_EPDCCH(int n_RNTI, int k) {
  int i = 0;
  const uint32 A = 39829;
  const uint32 D = 65537;
  uint32 Y = n_RNTI;
  for (i = 0; i <= k; i++) {
    Y = (A * Y) % D;
  }
  return (int)Y;
}

//! EPDCCH候选个数M_L计算。出自 ：3GPP 36.213 9.1.4节
/*!
  \param L ： L，输入
  \param n_EPDCCH ： n_EPDCCH，输入
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入
  \param M_L ： 候选个数Mp_L/Mp1_L、Mp2_L，输出
  \return 无
*/
inline void Get_EPDCCH_M_L(int L, int n_EPDCCH, const Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific, int M_L[2]) {
  int DCI_idx = UE_Specific.DCI_idx;
  int Case = 1;
  if (S != Cell_Specific.SubframeType && NormalCP == Cell_Specific.CP_Type && (DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx]) && Cell_Specific.N_DL_RB >= 25) {
    Case = 1;
  } else if (S == Cell_Specific.SubframeType && NormalCP == Cell_Specific.CP_Type && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration) && (DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx]) && Cell_Specific.N_DL_RB >= 25) {
    Case = 1;
  } else if (S != Cell_Specific.SubframeType && NormalCP == Cell_Specific.CP_Type && (DCI_Format_1 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_0 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_4 == UE_Specific.DCI_Format_Type[DCI_idx]) && n_EPDCCH < 104) {
    Case = 1;
  } else if (S == Cell_Specific.SubframeType && NormalCP == Cell_Specific.CP_Type && (SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG4 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG8 == Cell_Specific.SpecialSubframeConfiguration) && (DCI_Format_1 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_0 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_4 == UE_Specific.DCI_Format_Type[DCI_idx]) && n_EPDCCH < 104) {
    Case = 1;
  } else if (S != Cell_Specific.SubframeType && ExtendedCP == Cell_Specific.CP_Type && (DCI_Format_1 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_0 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_4 == UE_Specific.DCI_Format_Type[DCI_idx]) && n_EPDCCH < 104) {
    Case = 2;
  } else if (S == Cell_Specific.SubframeType && NormalCP == Cell_Specific.CP_Type && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG7 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG9 == Cell_Specific.SpecialSubframeConfiguration) && (DCI_Format_1 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_0 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_4 == UE_Specific.DCI_Format_Type[DCI_idx]) && n_EPDCCH < 104) {
    Case = 2;
  } else if (S == Cell_Specific.SubframeType && ExtendedCP == Cell_Specific.CP_Type && (SS_CFG1 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG2 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG3 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG5 == Cell_Specific.SpecialSubframeConfiguration || SS_CFG6 == Cell_Specific.SpecialSubframeConfiguration) && (DCI_Format_1 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_1D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2A == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2B == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2C == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_2D == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_0 == UE_Specific.DCI_Format_Type[DCI_idx] || DCI_Format_4 == UE_Specific.DCI_Format_Type[DCI_idx]) && n_EPDCCH < 104) {
    Case = 2;
  } else {
    Case = 3;
  }
  if (1 == UE_Specific.EPDCCH_Set_Num) {
    if (1 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (1 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 8;
      } else if (4 == L) {
        M_L[0] = 4;
      } else if (8 == L) {
        M_L[0] = 2;
      } else if (16 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (1 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 6;
      } else if (4 == L) {
        M_L[0] = 4;
      } else if (8 == L) {
        M_L[0] = 3;
      } else if (16 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (2 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 4;
      } else if (2 == L) {
        M_L[0] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (2 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 8;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (2 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 6;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 3;
      } else if (8 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (3 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 8;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (3 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 4;
      } else if (2 == L) {
        M_L[0] = 5;
      } else if (4 == L) {
        M_L[0] = 4;
      } else if (8 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (3 == Case && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 4;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 4;
      } else if (8 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 2;
      }
    } else if (1 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (1 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 8;
      } else if (4 == L) {
        M_L[0] = 4;
      } else if (8 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (1 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (2 == L) {
        M_L[0] = 6;
      } else if (4 == L) {
        M_L[0] = 6;
      } else if (8 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 2;
      }
    } else if (2 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 4;
      } else if (2 == L) {
        M_L[0] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
      } else {
        M_L[0] = 0;
      }
    } else if (2 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 8;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (2 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 6;
      } else if (2 == L) {
        M_L[0] = 6;
      } else if (4 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 2;
      }
    } else if (3 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 8;
      } else if (2 == L) {
        M_L[0] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 1;
      }
    } else if (3 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 6;
      } else if (2 == L) {
        M_L[0] = 6;
      } else if (4 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 2;
      }
    } else if (3 == Case && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0]) {
      if (1 == L) {
        M_L[0] = 6;
      } else if (2 == L) {
        M_L[0] = 6;
      } else if (4 == L) {
        M_L[0] = 2;
      } else {
        M_L[0] = 2;
      }
    }
  } else if (Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[0] && Distributed_EPDCCH == UE_Specific.TMType_EPDCCH[1]) {
    if (1 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 5;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 2;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (2 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 5;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (3 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    }
  } else if (Localized_EPDCCH == UE_Specific.TMType_EPDCCH[0] && Localized_EPDCCH == UE_Specific.TMType_EPDCCH[1]) {
    if (1 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 5;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (2 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 5;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 3;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 1;
        M_L[1] = 1;
      }
    }
  } else {
    if (1 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (1 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 4;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 4;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 5;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 5;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 2;
        M_L[1] = 1;
      } else if (16 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (1 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (2 == L) {
        M_L[0] = 6;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (16 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 4;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (2 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 3;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 4;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 4;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 5;
        M_L[1] = 2;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 5;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 0;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (2 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 6;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 0;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (3 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 2;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (3 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (3 == Case && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 1;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (3 == Case && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 2 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 0;
      }
    } else if (3 == Case && 8 == UE_Specific.numberPRB_Pairs_EPDCCH[0] && 4 == UE_Specific.numberPRB_Pairs_EPDCCH[1]) {
      if (1 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (2 == L) {
        M_L[0] = 4;
        M_L[1] = 1;
      } else if (4 == L) {
        M_L[0] = 2;
        M_L[1] = 2;
      } else if (8 == L) {
        M_L[0] = 0;
        M_L[1] = 1;
      } else {
        M_L[0] = 0;
        M_L[1] = 1;
      }
    }
  }
}

//! PDCCH DCI Payload大小获取。出自 ：3GPP 36.212 5.3.3.1节
/*!
  \param DCI_Format_Type ： DCI Format，输入
  \param FrameStructureType ： 无线帧类型，输入
  \param BandWidth ： 带宽，输入
  \param CA_Flag ： CA使能标志，输入
  \param CRSPortNum ： CRS端口数，输入
  \param CSI_request_num ： CSI_request域比特数，输入
  \param SRS_request_num ： SRS_request域比特数，输入
  \param Resource_allocation_type_num ： Resource_allocation_type域比特数，输入
  \param DCI_On_EPDCCH ： DCI承载在PDCCH或EPDCCH上，true ：EPDCCH； false ：PDCCH，输入
  \return PDCCH DCI Payload大小
*/
inline int Get_PDCCH_Payload_size(DCI_Format_Type_E DCI_Format_Type,
                                  FrameStructureType_E FrameStructureType,
                                  BandWidth_E BandWidth,
                                  bool CA_Flag,
                                  int CRSPortNum,
                                  int CSI_request_num,               //1,2,3
                                  int SRS_request_num,               //0,1
                                  int Resource_allocation_type_num,  //0,1
                                  bool DCI_On_EPDCCH) {
  int DCI_Format_0_size = 0;
  int DCI_Format_1_size = 0;
  int DCI_Format_1A_size = 0;
  int DCI_Format_2_size = 0;
  int DCI_Format_2A_size = 0;
  int DCI_Format_2B_size = 0;
  int DCI_Format_2C_size = 0;
  int DCI_Format_2D_size = 0;
  int Payload_size = 0;
  const int DCI_Ambiguous_Sizes[10] = {12, 14, 16, 20, 24, 26, 32, 40, 44, 56};
  const int DCI_Payload_size[2][14][6] =
      {
          //FDD
          {
              {19, 21, 23, 25, 26, 27},  //Format 0, Suppose Carrier indicator 0bit, CSI_request 1bit, UL_index 0bit(DAI 0bit), SRS_request 0bit, Resource_allocation_type 0bit
              {20, 22, 27, 31, 33, 39},  //Format 1, Suppose Carrier indicator 0bit, DAI 0bit, HAQR_ACK resource offset 0bit
              {20, 22, 24, 26, 27, 28},  //Format 1A, Suppose Carrier indicator 0bit, DAI 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {22, 24, 26, 28, 29, 30},  //Format 1B, Suppose Carrier indicator 0bit, DAI 0bit, TPMI 2bit, HAQR_ACK resource offset 0bit
              {9, 11, 13, 12, 14, 15},   //Format 1C
              {22, 24, 26, 28, 29, 30},  //Format 1D, Suppose Carrier indicator 0bit, DAI 0bit, TPMI 2bit, HAQR_ACK resource offset 0bit
              {32, 34, 39, 43, 45, 51},  //Format 2, Suppose Carrier indicator 0bit, DAI 0bit, Precoding Info 3bit, HAQR_ACK resource offset 0bit
              {29, 31, 36, 40, 42, 48},  //Format 2A, Suppose Carrier indicator 0bit, DAI 0bit, Precoding Info 0bit, HAQR_ACK resource offset 0bit
              {29, 31, 36, 40, 42, 48},  //Format 2B, Suppose Carrier indicator 0bit, DAI 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {31, 33, 38, 42, 44, 50},  //Format 2C, Suppose Carrier indicator 0bit, DAI 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {33, 35, 40, 44, 46, 52},  //Format 2D, Suppose Carrier indicator 0bit, DAI 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {19, 21, 23, 25, 26, 27},  //Format 3, Eq to Format 0
              {19, 21, 23, 25, 26, 27},  //Format 3A, Eq to Format 0
              {28, 31, 34, 36, 36, 38}   //Format 4, Suppose Carrier indicator 0bit, UL_index 0bit(DAI 0bit), CSI_request 1bit, Precoding Info 3bit
          },
          //TDD
          {
              {22, 23, 25, 27, 28, 29},  //Format 0, Suppose Carrier indicator 0bit, CSI_request 1bit, SRS_request 0bit, Resource_allocation_type 0bit
              {23, 25, 30, 34, 36, 42},  //Format 1, Suppose Carrier indicator 0bit, HAQR_ACK resource offset 0bit
              {23, 25, 27, 29, 30, 31},  //Format 1A, Suppose Carrier indicator 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {25, 27, 29, 31, 32, 33},  //Format 1B, Suppose Carrier indicator 0bit, TPMI 2bit, HAQR_ACK resource offset 0bit
              {9, 11, 13, 12, 14, 15},   //Format 1C
              {25, 27, 29, 31, 32, 33},  //Format 1D, Suppose Carrier indicator 0bit, TPMI 2bit, HAQR_ACK resource offset 0bit
              {35, 37, 42, 46, 48, 54},  //Format 2, Suppose Carrier indicator 0bit, Precoding Info 3bit, HAQR_ACK resource offset 0bit
              {32, 34, 39, 43, 45, 51},  //Format 2A, Suppose Carrier indicator 0bit, Precoding Info 0bit, HAQR_ACK resource offset 0bit
              {32, 34, 39, 43, 45, 51},  //Format 2B, Suppose Carrier indicator 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {34, 36, 41, 45, 47, 53},  //Format 2C, Suppose Carrier indicator 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {36, 38, 43, 47, 49, 55},  //Format 2D, Suppose Carrier indicator 0bit, SRS_request 0bit, HAQR_ACK resource offset 0bit
              {22, 23, 25, 27, 28, 29},  //Format 3, Eq to Format 0
              {22, 23, 25, 27, 28, 29},  //Format 3A, Eq to Format 0
              {31, 33, 36, 38, 38, 40}   //Format 4, Suppose Carrier indicator 0bit, CSI_request 1bit, Precoding Info 3bit
          }};
  switch (DCI_Format_Type) {
    case NULL_DCI:
      Payload_size = 0;
      break;
    case DCI_Format_0:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      Payload_size = MAX(Payload_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_1:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      DCI_Format_1A_size = MAX(DCI_Format_0_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        DCI_Format_1A_size++;
      }
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (Payload_size == DCI_Format_1A_size || (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size))) {
        Payload_size++;
      }
      break;
    case DCI_Format_1A:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      Payload_size = MAX(Payload_size, DCI_Format_0_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_1B:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      DCI_Format_1A_size = MAX(DCI_Format_0_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        DCI_Format_1A_size++;
      }
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1B - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CRSPortNum - 2) + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (Payload_size == DCI_Format_1A_size || (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size))) {
        Payload_size++;
      }
      break;
    case DCI_Format_1C:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1C - 1][(int)BandWidth];
      break;
    case DCI_Format_1D:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      DCI_Format_1A_size = MAX(DCI_Format_0_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        DCI_Format_1A_size++;
      }
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1D - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CRSPortNum - 2) + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (Payload_size == DCI_Format_1A_size || (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size))) {
        Payload_size++;
      }
      break;
    case DCI_Format_2:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_2 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + ((4 == CRSPortNum) ? 3 : 0) + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_2A:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_2A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + ((4 == CRSPortNum) ? 2 : 0) + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_2B:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_2B - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_2C:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_2C - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_2D:
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_2D - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        Payload_size++;
      }
      break;
    case DCI_Format_3:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      DCI_Format_1A_size = MAX(DCI_Format_0_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        DCI_Format_1A_size++;
      }
      Payload_size = (DCI_Format_1A_size / 2) * 2;
      break;
    case DCI_Format_3A:
      DCI_Format_0_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_0 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + SRS_request_num + Resource_allocation_type_num;
      DCI_Format_1A_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_1A - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + SRS_request_num + ((!DCI_On_EPDCCH) ? 0 : 2);
      DCI_Format_1A_size = MAX(DCI_Format_0_size, DCI_Format_1A_size);
      while (!DCI_On_EPDCCH && (DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size)) {
        DCI_Format_1A_size++;
      }
      Payload_size = DCI_Format_1A_size;
      break;
    case DCI_Format_4:
      DCI_Format_1_size = Get_PDCCH_Payload_size(DCI_Format_1, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      DCI_Format_2_size = Get_PDCCH_Payload_size(DCI_Format_2, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      DCI_Format_2A_size = Get_PDCCH_Payload_size(DCI_Format_2A, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      DCI_Format_2B_size = Get_PDCCH_Payload_size(DCI_Format_2B, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      DCI_Format_2C_size = Get_PDCCH_Payload_size(DCI_Format_2C, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      DCI_Format_2D_size = Get_PDCCH_Payload_size(DCI_Format_2D, FrameStructureType, BandWidth, CA_Flag, CRSPortNum, CSI_request_num, SRS_request_num, Resource_allocation_type_num, DCI_On_EPDCCH);
      Payload_size = DCI_Payload_size[(int)FrameStructureType][(int)DCI_Format_4 - 1][(int)BandWidth] + (CA_Flag ? 3 : 0) + (CSI_request_num - 1) + ((4 == CRSPortNum) ? 3 : 0);
      while (DCI_Format_1_size == Payload_size || DCI_Format_2_size == Payload_size || DCI_Format_2A_size == Payload_size || DCI_Format_2B_size == Payload_size || DCI_Format_2C_size == Payload_size || DCI_Format_2D_size == Payload_size ||
             DCI_Ambiguous_Sizes[0] == Payload_size || DCI_Ambiguous_Sizes[1] == Payload_size || DCI_Ambiguous_Sizes[2] == Payload_size || DCI_Ambiguous_Sizes[3] == Payload_size || DCI_Ambiguous_Sizes[4] == Payload_size || DCI_Ambiguous_Sizes[5] == Payload_size || DCI_Ambiguous_Sizes[6] == Payload_size || DCI_Ambiguous_Sizes[7] == Payload_size || DCI_Ambiguous_Sizes[8] == Payload_size || DCI_Ambiguous_Sizes[9] == Payload_size) {
        Payload_size++;
      }
      break;
  }
  return Payload_size;
}

//! 填充域bit值
/*!
  \param bits ： 二进制列向量，输出
  \param start_pos ： 起始bit位置，输入
  \param len ： 填充bit长度，输入
  \param value ： 填充值，输入
  \return 填充bit长度
*/
inline int SetFieldBits(bvec &bits, int start_pos, int len, int value) {
  int i = 0;
  int j = 0;
  for (i = start_pos, j = len - 1; i < (start_pos + len); i++, j--) {
    bits(i) = ((value >> j) & 1);
  }
  return len;
}

//! 读取域bit值
/*!
  \param bits ： 二进制列向量，输入
  \param start_pos ： 起始bit位置，输入
  \param len ： 读取bit长度，输入
  \param value ： 读取值，输出
  \return 读取bit长度
*/
inline int GetFieldValue(const bvec &bits, int start_pos, int len, int *value) {
  int i = 0;
  int j = 0;
  *value = 0;
  for (i = start_pos, j = len - 1; i < (start_pos + len); i++, j--) {
    *value = *value | ((bits(i) & 1) << j);
  }
  return len;
}

//! 二项系数(n ,k)计算
/*!
  \param n ： n，输入
  \param k ： k，输入
  \return 二项系数(n ,k)
*/
inline int binomial_coef(int n, int k)  //二项系数(n ,k)
{
  int i = 0;
  int c = 1;
  if (k > (n - k)) {
    k = n - k;
  }
  c = 1;
  for (i = 0; i < k; i++) {
    c = c * (n - i) / (i + 1);
  }
  return c;
}

//! 扩展二项系数(x, y)计算
/*!
  \param x ： x，输入
  \param y ： y，输入
  \return 扩展二项系数(x, y)
*/
inline int extended_binomial_coef(int x, int y)  //扩展二项系数(x, y)
{
  if (x >= y) {
    return binomial_coef(x, y);
  } else {
    return 0;
  }
}

//! 组合数combinatorial index计算。出自 ：3GPP 36.213 9.1.4.4节
/*!
  \param x ： int型向量，输入
  \return 组合数combinatorial index
*/
inline int combinatorial_idx(const vector<int> &x)  //组合数计算
{
  int r = 0;
  int i = 0;
  int M = (int)x.size();
  for (i = 0; i < M; i++) {
    r += extended_binomial_coef(x[i], M - i);
  }
  return r;
}

//! 解析组合数combinatorial index。出自 ：wiki百科“combinatorial number system”
/*!
  \param r ： 组合数combinatorial index，输入
  \param x ： int型向量，输出
  \return 无
*/
inline void Parse_combinatorial_idx(int r, vector<int> &x)  //from wiki combinatorial number system
{
  int i = 0;
  int j = 0;
  int c = 0;
  int c_last = 0;
  int M = (int)x.size();
  for (i = 0; i < M; i++) {
    if (0 == r) {
      c_last = 0;
      x[i] = M - i - 1;
    } else if (1 == r) {
      c_last = 1;
      x[i] = M - i;
    } else {
      j = M - i + 1;
      c = (j) / (j - M + i);
      while (c <= r) {
        j++;
        c_last = c;
        c = c * (j) / (j - M + i);
      };
      x[i] = j - 1;
    }
    r = r - c_last;
  }
}

//! RIV计算。出自 ：3GPP 36.213 7.1.6.3节
/*!
  \param RB_start ： RB_start，输入
  \param L_CRBs ： L_CRBs，输入
  \param N_RB ： N_RB，输入
  \return RIV
*/
inline int GenRIV(int RB_start, int L_CRBs, int N_RB) {
  int RIV = 0;
  if ((L_CRBs - 1) <= (N_RB / 2)) {
    RIV = N_RB * (L_CRBs - 1) + RB_start;
  } else {
    RIV = N_RB * (N_RB - L_CRBs + 1) + (N_RB - 1 - RB_start);
  }
  return RIV;
}

//! 解析RIV
/*!
  \param RIV ： RIV，输入
  \param N_RB ： N_RB，输入
  \param RB_start ： RB_start，输出
  \param L_CRBs ： L_CRBs，输出
  \return 无
*/
inline void ParseRIV(int RIV, int N_RB, int *RB_start, int *L_CRBs) {
  int a = 0;
  int b = 0;
  a = (RIV / N_RB) + 1;
  b = RIV % N_RB;
  if ((a + b) > N_RB) {
    *L_CRBs = N_RB + 2 - a;
    *RB_start = N_RB - 1 - b;
  } else {
    *L_CRBs = a;
    *RB_start = b;
  }
}

//! RGB计算。出自 ：3GPP 36.213 7.1.6.1节
/*!
  \param RB_Assignment ： int型向量，RB分配，输入
  \param N_RB ： N_RB，输入
  \param P ： RBG大小，输入
  \return RBG
*/
inline int GenRBG(const vector<int> &RB_Assignment, int N_RB, int P) {
  int i = 0;
  int RBG_Mapping = 0;
  int M = ceil((float)N_RB / (float)P);
  for (i = 0; i < (int)RB_Assignment.size(); i += P) {
    RBG_Mapping |= (1 << (M - 1 - RB_Assignment[i] / P));
  }
  return RBG_Mapping;
}

//! 解析RGB
/*!
  \param RBG_Mapping ： RBG，输入
  \param RB_Assignment ： int型向量，RB分配，输出
  \param N_RB ： N_RB，输入
  \param P ： RBG大小，输入
  \return 无
*/
inline void ParseRBG(int RBG_Mapping, vector<int> &RB_Assignment, int N_RB, int P) {
  int i = 0;
  int j = 0;
  int M = ceil((float)N_RB / (float)P);
  RB_Assignment.resize(0);
  for (i = 0; i < M; i++) {
    for (j = 0; j < P; j++) {
      if (1 == ((RBG_Mapping >> (M - 1 - i)) & 1) && (P * i + j) < N_RB) {
        RB_Assignment.push_back(P * i + j);
      }
    }
  }
}

//! MCS/PRB to TBS的映射。出自 ：3GPP 36.213 7.1.7.2节
/*!
  \param MCS ： MCS，输入
  \param N_PRB ： N_PRB，输入
  \param v ： 层数，输入
  \return 码块大小TBS
*/
inline int MCS_PRB_toTBS(int MCS, int N_PRB, int v) {
  int i = 0;
  int I_TBS = MCS_Table[MCS][1];
  int A = 0;
  if (1 == v) {
    A = TBS_Table[I_TBS][N_PRB - 1];
  } else if (2 == v) {
    if (N_PRB <= 55) {
      A = TBS_Table[I_TBS][2 * N_PRB - 1];
    } else {
      A = TBS_Table[I_TBS][N_PRB - 1];
      for (i = 0; i < 120; i++) {
        if (A == TBS_Translation_Table_1vTo2v[i][0]) {
          A = TBS_Translation_Table_1vTo2v[i][1];
          break;
        }
      }
    }
  } else if (3 == v) {
    if (N_PRB <= 36) {
      A = TBS_Table[I_TBS][3 * N_PRB - 1];
    } else {
      A = TBS_Table[I_TBS][N_PRB - 1];
      for (i = 0; i < 134; i++) {
        if (A == TBS_Translation_Table_1vTo3v[i][0]) {
          A = TBS_Translation_Table_1vTo3v[i][1];
          break;
        }
      }
    }
  } else {
    if (N_PRB <= 27) {
      A = TBS_Table[I_TBS][3 * N_PRB - 1];
    } else {
      A = TBS_Table[I_TBS][N_PRB - 1];
      for (i = 0; i < 142; i++) {
        if (A == TBS_Translation_Table_1vTo4v[i][0]) {
          A = TBS_Translation_Table_1vTo4v[i][1];
          break;
        }
      }
    }
  }
  return A;
}

//! 寻找CQI对应的MCS。出自 ：3GPP 36.101 附录A.3.1
/*!
  \param CQI ： CQI，输入
  \param N_PRB ： N_PRB，输入
  \param v ： 层数，输入
  \param N_ch ： N_ch，输入
  \return MCS
*/
inline int Find_CQI_to_MCS(int CQI, int N_PRB, int v, int N_ch) {
  int A = 0;
  int C = 0;
  int N_CB = 0;
  int MCS = 0;
  int Qm = 0;
  int Start_MCS = 0;
  int End_MCS = 0;
  float rate = 0.0f;
  float rate_diff = 0.0f;
  float rate_diff_tmp = 0.0f;
  int MCS_MinDiff = 0;
  Qm = CQI_Target_Qm[CQI - 1];
  if (2 == Qm) {
    Start_MCS = 0;
    End_MCS = 9;
  } else if (4 == Qm) {
    Start_MCS = 10;
    End_MCS = 16;
  } else if (6 == Qm) {
    Start_MCS = 17;
    End_MCS = 28;
  }
  A = MCS_PRB_toTBS(Start_MCS, N_PRB, v);
  if ((A + 24) < 6144) {
    C = 1;
  } else {
    C = ceil((float)(A + 24) / (float)(6144 - 24));
  }
  N_CB = (1 == C) ? 0 : C;
  rate_diff = MyFabs(CQI_Target_Rate[CQI - 1] - (float)(A + 24 * (N_CB + 1)) / (float)N_ch);
  MCS_MinDiff = Start_MCS;
  for (MCS = (Start_MCS + 1); MCS <= End_MCS; MCS++) {
    A = MCS_PRB_toTBS(MCS, N_PRB, v);
    if ((A + 24) < 6144) {
      C = 1;
    } else {
      C = ceil((float)(A + 24) / (float)(6144 - 24));
    }
    N_CB = (1 == C) ? 0 : C;
    rate = (float)(A + 24 * (N_CB + 1)) / (float)N_ch;
    rate_diff_tmp = MyFabs(CQI_Target_Rate[CQI - 1] - rate);
    if ((rate_diff_tmp - 0.001f) < rate_diff && rate <= 0.93f) {
      rate_diff = rate_diff_tmp;
      MCS_MinDiff = MCS;
    }
  }
  return MCS_MinDiff;
}

//! EPDCCH解析得到RB分配。出自 ：3GPP 36.213 9.1.4.4节
/*!
  \param Simuctrl ： 仿真控制参数集合，输入
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入/输出
  \return 无
*/
inline void ParasFromEPDCCH(const SimuCtrl_S &Simuctrl, const Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific) {
  int i = 0;
  int j = 0;
  int EPDCCH_idx = 0;
  vector<int> x;
  EPDCCH_idx = Simuctrl.n_subframe % UE_Specific.EPDCCH_Period;
  if (1 == UE_Specific.subframePattern_EPDCCH[EPDCCH_idx]) {
    for (i = 0; i < UE_Specific.EPDCCH_Set_Num; i++) {
      if (-1 != UE_Specific.resourceBlockAssignment_EPDCCH[i]) {
        x.resize(UE_Specific.numberPRB_Pairs_EPDCCH[i]);
        Parse_combinatorial_idx(UE_Specific.resourceBlockAssignment_EPDCCH[i], x);
        UE_Specific.RB_Assignment_EPDCCH[i].resize(UE_Specific.numberPRB_Pairs_EPDCCH[i]);
        for (j = 0; j < UE_Specific.numberPRB_Pairs_EPDCCH[i]; j++) {
          UE_Specific.RB_Assignment_EPDCCH[i][j] = Cell_Specific.N_DL_RB - x[j];
        }
      }
    }
  } else {
    for (i = 0; i < UE_Specific.EPDCCH_Set_Num; i++) {
      UE_Specific.RB_Assignment_EPDCCH[i].resize(0);
    }
  }
}

//! 反馈控制函数，CQI，RI，PMI反馈
/*!
  \param Simuctrl ： 仿真控制参数集合，输入
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入/输出
  \param Feedback ： 反馈Feedback集合，输入/输出
  \return 无
*/
inline void FeedBack(const SimuCtrl_S &Simuctrl, const Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific, Feedback_S &Feedback) {
  int DCI_idx = 0;
  int buffer_idx = 0;
  int cw_idx = 0;
  int max_n_subframe = 0;
  int RI_idx = 0;
  int subband_idx = 0;
  int RB_Num = 0;
  int Start_RB = 0;
  int RB_idx = 0;
  int CQI_Offset = 0;
  int CQI_Offset_Level = 0;
  int random_idx = 0;
  //CQI
  if (Simuctrl.CQI_Feedback_Flag) {
    for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
      if (2 == Feedback.WidebandCQI_Report_Flag[buffer_idx] && (Simuctrl.n_subframe - Feedback.WidebandCQI_Report_n_subframe[buffer_idx]) >= 4 && U != Cell_Specific.SubframeType) {
        Feedback.WidebandCQI_Report_Flag[buffer_idx] = 0;
        Feedback.WidebandCQI_Report_n_subframe[buffer_idx] = 0;
        if (Feedback.WidebandCQI_Gen_n_subframe[buffer_idx] > max_n_subframe) {
          max_n_subframe = Feedback.WidebandCQI_Gen_n_subframe[buffer_idx];
          if (-1 == Feedback.WidebandCQI_Report_Buffer[1][buffer_idx]) {
            Feedback.WidebandCQI[0][cw_idx] = Feedback.WidebandCQI_Report_Buffer[0][buffer_idx];
            if (!UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM21 == UE_Specific.ReportMode)) {
              Feedback.Perfered_M_idx(cw_idx, 0) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][0][buffer_idx];
              Feedback.SubbandCQI[0](cw_idx, Feedback.Perfered_M_idx(cw_idx, 0)) = Feedback.SubbandCQI_Report_Buffer[cw_idx][Feedback.Perfered_M_idx(cw_idx, 0)][buffer_idx];
            } else if (UE_Specific.ReportOnPUSCH &&
                       (RM20 == UE_Specific.ReportMode || RM22 == UE_Specific.ReportMode)) {
              for (subband_idx = 0; subband_idx < Feedback.Perfered_M; subband_idx++) {
                Feedback.Perfered_M_idx(cw_idx, subband_idx) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][subband_idx][buffer_idx];
                Feedback.SubbandCQI[0](cw_idx, Feedback.Perfered_M_idx(cw_idx, subband_idx)) = Feedback.SubbandCQI_Report_Buffer[cw_idx][Feedback.Perfered_M_idx(cw_idx, subband_idx)][buffer_idx];
              }
            } else {
              Feedback.Perfered_M_idx(cw_idx, 0) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][0][buffer_idx];
              for (subband_idx = 0; subband_idx < Feedback.N_Subband; subband_idx++) {
                Feedback.SubbandCQI[0](cw_idx, subband_idx) = Feedback.SubbandCQI_Report_Buffer[cw_idx][subband_idx][buffer_idx];
              }
            }
          } else {
            for (cw_idx = 0; cw_idx < MAX_TB_SIZE; cw_idx++) {
              Feedback.WidebandCQI[1][cw_idx] = Feedback.WidebandCQI_Report_Buffer[cw_idx][buffer_idx];
              if (!UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM21 == UE_Specific.ReportMode)) {
                Feedback.Perfered_M_idx(cw_idx, 0) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][0][buffer_idx];
                Feedback.SubbandCQI[1](cw_idx, Feedback.Perfered_M_idx(cw_idx, 0)) = Feedback.SubbandCQI_Report_Buffer[cw_idx][Feedback.Perfered_M_idx(cw_idx, 0)][buffer_idx];
              } else if (UE_Specific.ReportOnPUSCH &&
                         (RM20 == UE_Specific.ReportMode || RM22 == UE_Specific.ReportMode)) {
                for (subband_idx = 0; subband_idx < Feedback.Perfered_M; subband_idx++) {
                  Feedback.Perfered_M_idx(cw_idx, subband_idx) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][subband_idx][buffer_idx];
                  Feedback.SubbandCQI[1](cw_idx, Feedback.Perfered_M_idx(cw_idx, subband_idx)) = Feedback.SubbandCQI_Report_Buffer[cw_idx][Feedback.Perfered_M_idx(cw_idx, subband_idx)][buffer_idx];
                }
              } else {
                Feedback.Perfered_M_idx(cw_idx, 0) = Feedback.Perfered_M_idx_Report_Buffer[cw_idx][0][buffer_idx];
                for (subband_idx = 0; subband_idx < Feedback.N_Subband; subband_idx++) {
                  Feedback.SubbandCQI[1](cw_idx, subband_idx) = Feedback.SubbandCQI_Report_Buffer[cw_idx][subband_idx][buffer_idx];
                }
              }
            }
          }
        }
        for (DCI_idx = 0; DCI_idx < UE_Specific.DCI_Period; DCI_idx++) {
          RB_Num = (int)UE_Specific.RB_Assignment[DCI_idx].size();
          if (((!UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM21 == UE_Specific.ReportMode)) || (UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM22 == UE_Specific.ReportMode || RM30 == UE_Specific.ReportMode || RM31 == UE_Specific.ReportMode || RM32 == UE_Specific.ReportMode))) && RB_Num <= MAX(Feedback.k_Subband, Cell_Specific.N_DL_RB >> 1)) {
            if (UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM22 == UE_Specific.ReportMode)) {
              random_idx = rand() % Feedback.Perfered_M;
              Start_RB = Feedback.Perfered_M_idx(0, random_idx) * Feedback.k_Subband;
            } else {
              random_idx = 0;
              Start_RB = Feedback.Perfered_M_idx(0, random_idx) * Feedback.k_Subband;
            }
            if ((Start_RB + RB_Num) > Cell_Specific.N_DL_RB) {
              Start_RB = Cell_Specific.N_DL_RB - RB_Num;
            }
            for (RB_idx = 0; RB_idx < RB_Num; RB_idx++) {
              UE_Specific.RB_Assignment[DCI_idx][RB_idx] = Start_RB + RB_idx;
            }
          }
          for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
            if (((!UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM21 == UE_Specific.ReportMode)) || (UE_Specific.ReportOnPUSCH && (RM20 == UE_Specific.ReportMode || RM22 == UE_Specific.ReportMode || RM30 == UE_Specific.ReportMode || RM31 == UE_Specific.ReportMode || RM32 == UE_Specific.ReportMode))) && RB_Num <= MAX(Feedback.k_Subband, Cell_Specific.N_DL_RB >> 1)) {
              UE_Specific.CQI[cw_idx][DCI_idx] = Feedback.SubbandCQI[UE_Specific.codeword_num[DCI_idx] - 1](cw_idx, Feedback.Perfered_M_idx(0, random_idx));
              if (Simuctrl.n_subframe > (Simuctrl.start_n_subframe + 20) && UE_Specific.ReportOnPUSCH && (RM30 == UE_Specific.ReportMode || RM31 == UE_Specific.ReportMode || RM32 == UE_Specific.ReportMode)) {
                for (subband_idx = 0; subband_idx < Feedback.N_Subband; subband_idx++) {
                  CQI_Offset = Feedback.SubbandCQI[UE_Specific.codeword_num[DCI_idx] - 1](cw_idx, subband_idx) - Feedback.WidebandCQI[UE_Specific.codeword_num[DCI_idx] - 1][cw_idx];
                  if (0 == CQI_Offset) {
                    CQI_Offset_Level = 0;
                  } else if (1 == CQI_Offset) {
                    CQI_Offset_Level = 1;
                  } else if (CQI_Offset >= 2) {
                    CQI_Offset_Level = 2;
                  } else {
                    CQI_Offset_Level = 3;
                  }
                  Feedback.SubbandCQI_Offset_Num[cw_idx][subband_idx][CQI_Offset_Level]++;
                }
              }
            } else {
              UE_Specific.CQI[cw_idx][DCI_idx] = Feedback.WidebandCQI[UE_Specific.codeword_num[DCI_idx] - 1][cw_idx];
            }
            if (Simuctrl.n_subframe > (Simuctrl.start_n_subframe + 20)) {
              Feedback.CQI_Num[cw_idx][Feedback.WidebandCQI[UE_Specific.codeword_num[DCI_idx] - 1][cw_idx] - 1]++;
            }
          }
        }
        Feedback.WidebandCQI_Gen_n_subframe[buffer_idx] = 0;
      }
    }
  } else if (Ref_Median_CQI_Reported == UE_Specific.Ref_Type || Ref_Freq_Select_CQI_Reported == UE_Specific.Ref_Type || Ref_Freq_Non_Select_CQI_Reported == UE_Specific.Ref_Type || Ref_Freq_Select_Interfere_CQI_Reported == UE_Specific.Ref_Type || Ref_UE_Selected_CQI_Reported == UE_Specific.Ref_Type) {
    for (DCI_idx = 0; DCI_idx < UE_Specific.DCI_Period; DCI_idx++) {
      if (Simuctrl.Random_Selected_Subband) {
        RB_Num = (int)UE_Specific.RB_Assignment[DCI_idx].size();
        Start_RB = rand() % Feedback.N_Subband * Feedback.k_Subband;
        if ((Start_RB + RB_Num) >= Cell_Specific.N_DL_RB) {
          Start_RB = Cell_Specific.N_DL_RB - 1 - RB_Num;
        }
        for (RB_idx = 0; RB_idx < RB_Num; RB_idx++) {
          UE_Specific.RB_Assignment[DCI_idx][RB_idx] = Start_RB + RB_idx;
        }
      }
      for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
        UE_Specific.CQI[cw_idx][DCI_idx] = Feedback.Median_CQI[cw_idx] + Feedback.Median_CQI_Delta[cw_idx] + 1;
      }
    }
  }
  //RI
  if (Simuctrl.RI_Feedback_Flag) {
    for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
      if (2 == Feedback.RI_Report_Flag[buffer_idx] && (Simuctrl.n_subframe - Feedback.RI_Report_n_subframe[buffer_idx]) >= 4 && U != Cell_Specific.SubframeType) {
        Feedback.RI_Report_Flag[buffer_idx] = 0;
        Feedback.RI_Report_n_subframe[buffer_idx] = 0;
        if (Feedback.RI_Gen_n_subframe[buffer_idx] > max_n_subframe) {
          max_n_subframe = Feedback.RI_Gen_n_subframe[buffer_idx];
          Feedback.RI = Feedback.RI_Report_Buffer[buffer_idx];
        }
        for (DCI_idx = 0; DCI_idx < UE_Specific.DCI_Period; DCI_idx++) {
          if (UE_Specific.RB_Assignment[DCI_idx].size() > 0) {
            UE_Specific.N_L[DCI_idx] = Feedback.RI + 1;
            if (1 == UE_Specific.N_L[DCI_idx]) {
              UE_Specific.codeword_num[DCI_idx] = 1;
            } else {
              UE_Specific.codeword_num[DCI_idx] = 2;
            }
          }
        }
        Feedback.RI_Gen_n_subframe[buffer_idx] = 0;
      }
    }
  }
  //PMI
  if (Simuctrl.PMI_Feedback_Flag) {
    for (buffer_idx = 0; buffer_idx < MAX_Feedback_BufferSize; buffer_idx++) {
      if (2 == Feedback.PMI_Report_Flag[buffer_idx] && (Simuctrl.n_subframe - Feedback.PMI_Report_n_subframe[buffer_idx]) >= 4 && U != Cell_Specific.SubframeType) {
        RI_idx = Feedback.RI;
        Feedback.PMI_Report_Flag[buffer_idx] = 0;
        Feedback.PMI_Report_n_subframe[buffer_idx] = 0;
        if (Feedback.PMI_Gen_n_subframe[buffer_idx] > max_n_subframe) {
          max_n_subframe = Feedback.PMI_Gen_n_subframe[buffer_idx];
          for (subband_idx = 0; subband_idx < Feedback.N_Subband; subband_idx++) {
            Feedback.Subband_PMI(RI_idx, subband_idx) = Feedback.PMI_Report_Buffer[buffer_idx](RI_idx, subband_idx);
          }
        }
        for (DCI_idx = 0; DCI_idx < UE_Specific.DCI_Period; DCI_idx++) {
          if (DCI_Format_2 == UE_Specific.DCI_Format_Type[DCI_idx]) {
            if (!UE_Specific.ReportOnPUSCH) {
              if (2 == Cell_Specific.CRSPortNum) {
                UE_Specific.Precoding_Info[DCI_idx] = (1 == UE_Specific.codeword_num[DCI_idx]) ? (Feedback.Subband_PMI(RI_idx, 0) + 1) : Feedback.Subband_PMI(RI_idx, 0);
              } else {
                if (1 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = Feedback.Subband_PMI(RI_idx, 0) + 1;
                } else if (2 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = (1 == UE_Specific.codeword_num[DCI_idx]) ? (Feedback.Subband_PMI(RI_idx, 0) + 18) : Feedback.Subband_PMI(RI_idx, 0);
                } else if (3 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = Feedback.Subband_PMI(RI_idx, 0) + 17;
                } else {
                  UE_Specific.Precoding_Info[DCI_idx] = Feedback.Subband_PMI(RI_idx, 0) + 34;
                }
              }
            } else {
              if (2 == Cell_Specific.CRSPortNum) {
                UE_Specific.Precoding_Info[DCI_idx] = (1 == UE_Specific.codeword_num[DCI_idx]) ? 5 : 2;
              } else {
                if (1 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = 17;
                } else if (2 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = (1 == UE_Specific.codeword_num[DCI_idx]) ? 34 : 16;
                } else if (3 == UE_Specific.N_L[DCI_idx]) {
                  UE_Specific.Precoding_Info[DCI_idx] = 33;
                } else {
                  UE_Specific.Precoding_Info[DCI_idx] = 50;
                }
              }
            }
          }
        }
        Feedback.PMI_Gen_n_subframe[buffer_idx] = 0;
      }
    }
  }
}

//! DCI解析函数。出自 ：3GPP 36.212 5.3.3.1节
/*!
  \param Simuctrl ： 仿真控制参数集合，输入
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入/输出
  \param UE_Parse_Flag ： 是否用于UE解析DCI，true ：用于UE解析DCI；false ：，用于计算参数，输入
  \return 无
*/
inline void ParasFromDCI(const SimuCtrl_S &Simuctrl, const Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific, bool UE_Parse_Flag) {
  int DCI_idx = UE_Specific.DCI_idx;
  int i = 0;
  int RB_start = 0;
  int L_CRBs = 0;
  switch (UE_Specific.DCI_Format_Type[DCI_idx]) {
    case DCI_Format_0:
      if (-1 != UE_Specific.RIV[DCI_idx]) {
        ParseRIV(UE_Specific.RIV[DCI_idx], Cell_Specific.N_UL_RB, &RB_start, &L_CRBs);
        UE_Specific.RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific.RB_Assignment[DCI_idx][i] = RB_start + i;
        }
      }
      break;
    case DCI_Format_1:
      UE_Specific.codeword_num[DCI_idx] = 1;
      UE_Specific.N_L[DCI_idx] = 1;
      UE_Specific.v[0] = 1;
      UE_Specific.Transmission_Scheme = Single_Antenna_Port;
      UE_Specific.Start_Port = Port0;
      if (TM2 == UE_Specific.TM) {
        UE_Specific.N_L[DCI_idx] = Cell_Specific.CRSPortNum;
        UE_Specific.v[0] = Cell_Specific.CRSPortNum;
        UE_Specific.Transmission_Scheme = Transmit_Diversity;
      } else if (TM7 == UE_Specific.TM) {
        UE_Specific.Start_Port = Port5;
      }
      break;
    case DCI_Format_1A:
      if (-1 != UE_Specific.RIV[DCI_idx]) {
        ParseRIV(UE_Specific.RIV[DCI_idx], Cell_Specific.N_DL_RB, &RB_start, &L_CRBs);
        UE_Specific.RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific.RB_Assignment[DCI_idx][i] = RB_start + i;
        }
      }
      UE_Specific.codeword_num[DCI_idx] = 1;
      UE_Specific.N_L[DCI_idx] = Cell_Specific.CRSPortNum;
      UE_Specific.v[0] = Cell_Specific.CRSPortNum;
      UE_Specific.Transmission_Scheme = (1 == Cell_Specific.CRSPortNum) ? Single_Antenna_Port : Transmit_Diversity;
      UE_Specific.Start_Port = Port0;
      break;
    case DCI_Format_1B:
      if (-1 != UE_Specific.RIV[DCI_idx]) {
        ParseRIV(UE_Specific.RIV[DCI_idx], Cell_Specific.N_DL_RB, &RB_start, &L_CRBs);
        UE_Specific.RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific.RB_Assignment[DCI_idx][i] = RB_start + i;
        }
      }
      UE_Specific.codeword_num[DCI_idx] = 1;
      UE_Specific.N_L[DCI_idx] = 1;
      UE_Specific.v[0] = 1;
      if (-1 == UE_Specific.TPMI[DCI_idx]) {
        if (Simuctrl.PMI_Feedback_Flag) {
          //TBD
        } else {
          UE_Specific.TPMI[DCI_idx] = rand() & (Cell_Specific.CRSPortNum - 1);
        }
      }
      UE_Specific.Transmission_Scheme = Closed_Loop_SM;
      UE_Specific.Start_Port = Port0;
      break;
    case DCI_Format_1C:
      //TBD
      break;
    case DCI_Format_1D:
      if (-1 != UE_Specific.RIV[DCI_idx]) {
        ParseRIV(UE_Specific.RIV[DCI_idx], Cell_Specific.N_DL_RB, &RB_start, &L_CRBs);
        UE_Specific.RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific.RB_Assignment[DCI_idx][i] = RB_start + i;
        }
      }
      UE_Specific.codeword_num[DCI_idx] = 1;
      UE_Specific.N_L[DCI_idx] = 1;
      UE_Specific.v[0] = 1;
      if (-1 == UE_Specific.TPMI[DCI_idx]) {
        if (Simuctrl.PMI_Feedback_Flag) {
          //TBD
        } else {
          UE_Specific.TPMI[DCI_idx] = rand() & (Cell_Specific.CRSPortNum - 1);
        }
      }
      UE_Specific.Transmission_Scheme = MU_MIMO;
      UE_Specific.Start_Port = Port0;
      break;
    case DCI_Format_2:
      if (UE_Parse_Flag) {
        if (0 == UE_Specific.MCS[1][DCI_idx] && 1 == UE_Specific.RV[1]) {
          UE_Specific.codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific.codeword_num[DCI_idx] = 2;
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.MCS[1][DCI_idx] = 0;
        UE_Specific.RV[1] = 1;
      }
      if (1 == UE_Specific.TB_swap_flag[DCI_idx] && 2 == UE_Specific.codeword_num[DCI_idx]) {
        SWAP(UE_Specific.MCS[0][DCI_idx], UE_Specific.MCS[1][DCI_idx]);
        SWAP(UE_Specific.NDI[0][DCI_idx], UE_Specific.NDI[1][DCI_idx]);
        SWAP(UE_Specific.RV[0], UE_Specific.RV[1]);
      }
      if (-1 == UE_Specific.Precoding_Info[DCI_idx]) {
        if (2 == Cell_Specific.CRSPortNum) {
          if (1 == UE_Specific.codeword_num[DCI_idx] && 2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Precoding_Info[DCI_idx] = 0;
          } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
            UE_Specific.Precoding_Info[DCI_idx] = (rand() & 3) + 1;
          } else {
            UE_Specific.Precoding_Info[DCI_idx] = (rand() & 1);
          }
        } else {
          if (1 == UE_Specific.codeword_num[DCI_idx] && 4 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Precoding_Info[DCI_idx] = 0;
          } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
            if (1 == UE_Specific.N_L[DCI_idx]) {
              UE_Specific.Precoding_Info[DCI_idx] = (rand() & 15) + 1;
            } else {
              UE_Specific.Precoding_Info[DCI_idx] = (rand() & 15) + 18;
            }
          } else {
            if (2 == UE_Specific.N_L[DCI_idx]) {
              UE_Specific.Precoding_Info[DCI_idx] = (rand() & 15);
            } else if (3 == UE_Specific.N_L[DCI_idx]) {
              UE_Specific.Precoding_Info[DCI_idx] = (rand() & 15) + 17;
            } else {
              UE_Specific.Precoding_Info[DCI_idx] = (rand() & 15) + 34;
            }
          }
        }
      }
      if (2 == Cell_Specific.CRSPortNum) {
        if (1 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
          UE_Specific.v[0] = 1;
          UE_Specific.N_L[DCI_idx] = 1;
        } else {
          UE_Specific.v[0] = 1;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 2;
        }
      } else {
        if (1 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
          UE_Specific.v[0] = 4;
          UE_Specific.N_L[DCI_idx] = 4;
        } else if (1 == UE_Specific.codeword_num[DCI_idx] && UE_Specific.Precoding_Info[DCI_idx] < 18) {
          UE_Specific.v[0] = 1;
          UE_Specific.N_L[DCI_idx] = 1;
        } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.codeword_num[DCI_idx] && UE_Specific.Precoding_Info[DCI_idx] < 17) {
          UE_Specific.v[0] = 1;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.codeword_num[DCI_idx] && UE_Specific.Precoding_Info[DCI_idx] < 34) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 3;
        } else {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 4;
        }
      }
      UE_Specific.Transmission_Scheme = Closed_Loop_SM;
      if (1 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
        UE_Specific.Transmission_Scheme = Transmit_Diversity;
      }
      UE_Specific.Start_Port = Port0;
      break;
    case DCI_Format_2A:
      if (UE_Parse_Flag) {
        if (0 == UE_Specific.MCS[1][DCI_idx] && 1 == UE_Specific.RV[1]) {
          UE_Specific.codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific.codeword_num[DCI_idx] = 2;
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.MCS[1][DCI_idx] = 0;
        UE_Specific.RV[1] = 1;
      }
      if (1 == UE_Specific.TB_swap_flag[DCI_idx] && 2 == UE_Specific.codeword_num[DCI_idx]) {
        SWAP(UE_Specific.MCS[0][DCI_idx], UE_Specific.MCS[1][DCI_idx]);
        SWAP(UE_Specific.NDI[0][DCI_idx], UE_Specific.NDI[1][DCI_idx]);
        SWAP(UE_Specific.RV[0], UE_Specific.RV[1]);
      }
      if (-1 == UE_Specific.Precoding_Info[DCI_idx] && 4 == Cell_Specific.CRSPortNum) {
        if (1 == UE_Specific.codeword_num[DCI_idx] && 4 == UE_Specific.N_L[DCI_idx]) {
          UE_Specific.Precoding_Info[DCI_idx] = 0;
        } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
          UE_Specific.Precoding_Info[DCI_idx] = 1;
        } else {
          if (2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Precoding_Info[DCI_idx] = 0;
          } else if (3 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Precoding_Info[DCI_idx] = 1;
          } else {
            UE_Specific.Precoding_Info[DCI_idx] = 2;
          }
        }
      }
      if (2 == Cell_Specific.CRSPortNum) {
        UE_Specific.v[0] = (1 == UE_Specific.codeword_num[DCI_idx]) ? 2 : 1;
        UE_Specific.v[1] = (1 == UE_Specific.codeword_num[DCI_idx]) ? 0 : 1;
        UE_Specific.N_L[DCI_idx] = 2;
      } else {
        if (1 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
          UE_Specific.v[0] = 4;
          UE_Specific.N_L[DCI_idx] = 4;
        } else if (1 == UE_Specific.codeword_num[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
          UE_Specific.v[0] = 1;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.codeword_num[DCI_idx] && 1 == UE_Specific.Precoding_Info[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 3;
        } else {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 4;
        }
      }
      UE_Specific.Transmission_Scheme = Large_Delay_CDD;
      if (1 == UE_Specific.codeword_num[DCI_idx] && 0 == UE_Specific.Precoding_Info[DCI_idx]) {
        UE_Specific.Transmission_Scheme = Transmit_Diversity;
      }
      UE_Specific.Start_Port = Port0;
      break;
    case DCI_Format_2B:
      if (UE_Parse_Flag) {
        if (0 == UE_Specific.MCS[1][DCI_idx] && 1 == UE_Specific.RV[1]) {
          UE_Specific.codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific.codeword_num[DCI_idx] = 2;
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.MCS[1][DCI_idx] = 0;
        UE_Specific.RV[1] = 1;
      }
      if (1 == UE_Specific.TB_swap_flag[DCI_idx] && 2 == UE_Specific.codeword_num[DCI_idx]) {
        SWAP(UE_Specific.MCS[0][DCI_idx], UE_Specific.MCS[1][DCI_idx]);
        SWAP(UE_Specific.NDI[0][DCI_idx], UE_Specific.NDI[1][DCI_idx]);
        SWAP(UE_Specific.RV[0], UE_Specific.RV[1]);
      }
      UE_Specific.v[0] = 1;
      UE_Specific.v[1] = 1;
      UE_Specific.N_L[DCI_idx] = UE_Specific.codeword_num[DCI_idx];
      UE_Specific.Transmission_Scheme = Up_to_8_layer;
      UE_Specific.Start_Port = Port7;
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.Transmission_Scheme = Single_Antenna_Port;
        UE_Specific.Start_Port = (0 == UE_Specific.NDI[1][DCI_idx]) ? Port7 : Port8;
      }
      break;
    case DCI_Format_2C:
      if (UE_Parse_Flag) {
        if (0 == UE_Specific.MCS[1][DCI_idx] && 1 == UE_Specific.RV[1]) {
          UE_Specific.codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific.codeword_num[DCI_idx] = 2;
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.MCS[1][DCI_idx] = 0;
        UE_Specific.RV[1] = 1;
      }
      if (1 == UE_Specific.TB_swap_flag[DCI_idx] && 2 == UE_Specific.codeword_num[DCI_idx]) {
        SWAP(UE_Specific.MCS[0][DCI_idx], UE_Specific.MCS[1][DCI_idx]);
        SWAP(UE_Specific.NDI[0][DCI_idx], UE_Specific.NDI[1][DCI_idx]);
        SWAP(UE_Specific.RV[0], UE_Specific.RV[1]);
      }
      if (-1 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
        if (1 == UE_Specific.codeword_num[DCI_idx]) {
          if (1 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = (0 == UE_Specific.NDI[1][DCI_idx]) ? UE_Specific.n_SCID[DCI_idx] : (UE_Specific.n_SCID[DCI_idx] + 2);
          } else if (2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 4;
          } else if (3 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 5;
          } else {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 6;
          }
        } else {
          if (2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = UE_Specific.n_SCID[DCI_idx];
          } else if (3 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 2;
          } else if (4 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 3;
          } else if (5 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 4;
          } else if (6 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 5;
          } else if (7 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 6;
          } else {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 7;
          }
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        if (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 4) {
          UE_Specific.v[0] = 1;
          UE_Specific.N_L[DCI_idx] = 1;
        } else if (4 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (5 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.N_L[DCI_idx] = 3;
        } else {
          UE_Specific.v[0] = 4;
          UE_Specific.N_L[DCI_idx] = 4;
        }
      } else {
        if (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 2) {
          UE_Specific.v[0] = 1;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 3;
        } else if (3 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 4;
        } else if (4 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 5;
        } else if (5 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.v[1] = 3;
          UE_Specific.N_L[DCI_idx] = 6;
        } else if (6 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 4;
          UE_Specific.v[1] = 3;
          UE_Specific.N_L[DCI_idx] = 7;
        } else {
          UE_Specific.v[0] = 4;
          UE_Specific.v[1] = 4;
          UE_Specific.N_L[DCI_idx] = 8;
        }
      }
      UE_Specific.Transmission_Scheme = Up_to_8_layer;
      UE_Specific.Start_Port = Port7;
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.Transmission_Scheme = Single_Antenna_Port;
        UE_Specific.Start_Port = (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 2) ? Port7 : Port8;
      }
      break;
    case DCI_Format_2D:
      if (UE_Parse_Flag) {
        if (0 == UE_Specific.MCS[1][DCI_idx] && 1 == UE_Specific.RV[1]) {
          UE_Specific.codeword_num[DCI_idx] = 1;
        } else {
          UE_Specific.codeword_num[DCI_idx] = 2;
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.MCS[1][DCI_idx] = 0;
        UE_Specific.RV[1] = 1;
      }
      if (1 == UE_Specific.TB_swap_flag[DCI_idx] && 2 == UE_Specific.codeword_num[DCI_idx]) {
        SWAP(UE_Specific.MCS[0][DCI_idx], UE_Specific.MCS[1][DCI_idx]);
        SWAP(UE_Specific.NDI[0][DCI_idx], UE_Specific.NDI[1][DCI_idx]);
        SWAP(UE_Specific.RV[0], UE_Specific.RV[1]);
      }
      if (-1 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
        if (1 == UE_Specific.codeword_num[DCI_idx]) {
          if (1 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = (0 == UE_Specific.NDI[1][DCI_idx]) ? UE_Specific.n_SCID[DCI_idx] : (UE_Specific.n_SCID[DCI_idx] + 2);
          } else if (2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 4;
          } else if (3 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 5;
          } else {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 6;
          }
        } else {
          if (2 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = UE_Specific.n_SCID[DCI_idx];
          } else if (3 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 2;
          } else if (4 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 3;
          } else if (5 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 4;
          } else if (6 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 5;
          } else if (7 == UE_Specific.N_L[DCI_idx]) {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 6;
          } else {
            UE_Specific.Antenna_Scrambling_Layers[DCI_idx] = 7;
          }
        }
      }
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        if (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 4) {
          UE_Specific.v[0] = 1;
          UE_Specific.N_L[DCI_idx] = 1;
        } else if (4 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (5 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.N_L[DCI_idx] = 3;
        } else {
          UE_Specific.v[0] = 4;
          UE_Specific.N_L[DCI_idx] = 4;
        }
      } else {
        if (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 2) {
          UE_Specific.v[0] = 1;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 2;
        } else if (2 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 1;
          UE_Specific.N_L[DCI_idx] = 3;
        } else if (3 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 2;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 4;
        } else if (4 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.v[1] = 2;
          UE_Specific.N_L[DCI_idx] = 5;
        } else if (5 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 3;
          UE_Specific.v[1] = 3;
          UE_Specific.N_L[DCI_idx] = 6;
        } else if (6 == UE_Specific.Antenna_Scrambling_Layers[DCI_idx]) {
          UE_Specific.v[0] = 4;
          UE_Specific.v[1] = 3;
          UE_Specific.N_L[DCI_idx] = 7;
        } else {
          UE_Specific.v[0] = 4;
          UE_Specific.v[1] = 4;
          UE_Specific.N_L[DCI_idx] = 8;
        }
      }
      UE_Specific.Transmission_Scheme = Up_to_8_layer;
      UE_Specific.Start_Port = Port7;
      if (1 == UE_Specific.codeword_num[DCI_idx]) {
        UE_Specific.Transmission_Scheme = Single_Antenna_Port;
        UE_Specific.Start_Port = (UE_Specific.Antenna_Scrambling_Layers[DCI_idx] < 2) ? Port7 : Port8;
      }
      break;
    case DCI_Format_4:
      if (-1 != UE_Specific.RIV[DCI_idx] && 0 == UE_Specific.Resource_allocation_type[DCI_idx]) {
        ParseRIV(UE_Specific.RIV[DCI_idx], Cell_Specific.N_UL_RB, &RB_start, &L_CRBs);
        UE_Specific.RB_Assignment[DCI_idx].resize(L_CRBs);
        for (i = 0; i < L_CRBs; i++) {
          UE_Specific.RB_Assignment[DCI_idx][i] = RB_start + i;
        }
      }
      break;
    default:
      break;
  }
}

//! 计算N_PRB，并由CQI查找对应的MCS和Qm。出自 ：3GPP 36.213 7.1.7.2节
/*!
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入/输出
  \return 无
*/
inline void CQI_to_MCS_Qm(Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific) {
  int DCI_idx = UE_Specific.DCI_idx;
  int i = 0;
  int N_PRB = 0;
  if (NULL_DCI != UE_Specific.DCI_Format_Type[DCI_idx] && DCI_Format_0 != UE_Specific.DCI_Format_Type[DCI_idx] && DCI_Format_4 != UE_Specific.DCI_Format_Type[DCI_idx]) {
    if (S == Cell_Specific.SubframeType && ((SS_CFG9 == Cell_Specific.SpecialSubframeConfiguration && NormalCP == Cell_Specific.CP_Type) || (SS_CFG7 == Cell_Specific.SpecialSubframeConfiguration && ExtendedCP == Cell_Specific.CP_Type))) {
      N_PRB = MAX(floor((float)UE_Specific.RB_Assignment[DCI_idx].size() * 0.375), 1);
    } else if (S == Cell_Specific.SubframeType) {
      N_PRB = MAX(floor((float)UE_Specific.RB_Assignment[DCI_idx].size() * 0.75), 1);
    } else {
      N_PRB = UE_Specific.RB_Assignment[DCI_idx].size();
    }
    for (i = 0; i < UE_Specific.codeword_num[DCI_idx]; i++) {
      if (-1 != UE_Specific.CQI[i][DCI_idx]) {
        UE_Specific.Qm[i] = CQI_Target_Qm[UE_Specific.CQI[i][DCI_idx] - 1];
        if (Transmit_Diversity == UE_Specific.Transmission_Scheme) {
          UE_Specific.MCS[i][DCI_idx] = Find_CQI_to_MCS(UE_Specific.CQI[i][DCI_idx], N_PRB, 1, UE_Specific.G_symb * UE_Specific.Qm[i]);
        } else {
          UE_Specific.MCS[i][DCI_idx] = Find_CQI_to_MCS(UE_Specific.CQI[i][DCI_idx], N_PRB, UE_Specific.v[i], UE_Specific.G_symb * UE_Specific.Qm[i]);
        }
      }
    }
  }
}

//! 重传处理函数
/*!
  \param Cell_Specific ： Cell级参数集合，输入
  \param UE_Specific ： UE级参数集合，输入/输出
  \param ReTransmit ： 重传ReTransmit集合，输入/输出
  \return 无
*/
inline void ParseReTransmit(const Cell_Specific_S &Cell_Specific, UE_Specific_S &UE_Specific, ReTransmit_S &ReTransmit) {
  int DCI_idx = UE_Specific.DCI_idx;
  int cw_idx = 0;
  bool ReTransmit_Flag = false;
  int N_PRB = 0;
  if (NULL_DCI != UE_Specific.DCI_Format_Type[DCI_idx] && DCI_Format_0 != UE_Specific.DCI_Format_Type[DCI_idx] && DCI_Format_4 != UE_Specific.DCI_Format_Type[DCI_idx] && UE_Specific.RB_Assignment[DCI_idx].size() > 0 && U != Cell_Specific.SubframeType) {
    UE_Specific.HARQ_ID = (UE_Specific.HARQ_ID + 1) % Cell_Specific.HARQ_ProcNum;
    for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
      if (0 == ReTransmit.CRC[cw_idx][UE_Specific.HARQ_ID] || (Cell_Specific.MaxTxNum - 1) == ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]) {
        UE_Specific.NDI[cw_idx][DCI_idx] = 1 - ReTransmit.LastNDI[cw_idx][UE_Specific.HARQ_ID];
        ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID] = 0;
      } else {
        UE_Specific.NDI[cw_idx][DCI_idx] = ReTransmit.LastNDI[cw_idx][UE_Specific.HARQ_ID];
        ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]++;
        ReTransmit_Flag = true;
      }
      if (2 == UE_Specific.Qm[cw_idx]) {
        UE_Specific.RV[cw_idx] = Cell_Specific.RV_QPSK[ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]];
      } else if (4 == UE_Specific.Qm[cw_idx]) {
        UE_Specific.RV[cw_idx] = Cell_Specific.RV_16QAM[ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]];
      } else if (6 == UE_Specific.Qm[cw_idx]) {
        UE_Specific.RV[cw_idx] = Cell_Specific.RV_64QAM[ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]];
      } else {
        UE_Specific.RV[cw_idx] = Cell_Specific.RV_256QAM[ReTransmit.TxNum[cw_idx][UE_Specific.HARQ_ID]];
      }
    }
    if (ReTransmit_Flag) {
      UE_Specific.codeword_num[DCI_idx] = ReTransmit.LastCWNum[UE_Specific.HARQ_ID];
      UE_Specific.N_L[DCI_idx] = ReTransmit.LastNL[UE_Specific.HARQ_ID];
      for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
        UE_Specific.TBS[cw_idx] = ReTransmit.LastTBS[cw_idx][UE_Specific.HARQ_ID];
        UE_Specific.Qm[cw_idx] = ReTransmit.LastQm[cw_idx][UE_Specific.HARQ_ID];
      }
    } else {
      if (S == Cell_Specific.SubframeType && ((SS_CFG9 == Cell_Specific.SpecialSubframeConfiguration && NormalCP == Cell_Specific.CP_Type) || (SS_CFG7 == Cell_Specific.SpecialSubframeConfiguration && ExtendedCP == Cell_Specific.CP_Type))) {
        N_PRB = MAX(floor((float)UE_Specific.RB_Assignment[DCI_idx].size() * 0.375), 1);
      } else if (S == Cell_Specific.SubframeType) {
        N_PRB = MAX(floor((float)UE_Specific.RB_Assignment[DCI_idx].size() * 0.75), 1);
      } else {
        N_PRB = UE_Specific.RB_Assignment[DCI_idx].size();
      }
      for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
        if (Transmit_Diversity == UE_Specific.Transmission_Scheme) {
          UE_Specific.TBS[cw_idx] = MCS_PRB_toTBS(UE_Specific.MCS[cw_idx][DCI_idx], N_PRB, 1);
        } else {
          UE_Specific.TBS[cw_idx] = MCS_PRB_toTBS(UE_Specific.MCS[cw_idx][DCI_idx], N_PRB, UE_Specific.v[cw_idx]);
        }
        UE_Specific.Qm[cw_idx] = MCS_Table[UE_Specific.MCS[cw_idx][DCI_idx]][0];
      }
    }
    ReTransmit.LastCWNum[UE_Specific.HARQ_ID] = UE_Specific.codeword_num[DCI_idx];
    ReTransmit.LastNL[UE_Specific.HARQ_ID] = UE_Specific.N_L[DCI_idx];
    for (cw_idx = 0; cw_idx < UE_Specific.codeword_num[DCI_idx]; cw_idx++) {
      ReTransmit.LastNDI[cw_idx][UE_Specific.HARQ_ID] = UE_Specific.NDI[cw_idx][DCI_idx];
      ReTransmit.LastTBS[cw_idx][UE_Specific.HARQ_ID] = UE_Specific.TBS[cw_idx];
      ReTransmit.LastQm[cw_idx][UE_Specific.HARQ_ID] = UE_Specific.Qm[cw_idx];
    }
  }
}

//! N_IR计算。出自 ：3GPP 36.212 5.1.4.1.2节
/*!
  \param N_soft ： N_soft，输入
  \param UE_Category ： UE能力等级，输入
  \param TM ： 传输模式，输入
  \param M_DL_HARQ ： M_DL_HARQ，输入
  \return N_IR
*/
inline int Get_PDSCH_N_IR(int N_soft, UE_Category_E UE_Category, TM_E TM, int M_DL_HARQ) {
  int K_C = 0;
  int K_MIMO = 0;
  const int M_limit = 8;
  int N_IR = 0;
  if (35982720 == N_soft) {
    K_C = 5;
  } else if (3654144 == N_soft && Category_6 == UE_Category) {
    K_C = 2;
  } else {
    K_C = 1;
  }
  if (TM3 == TM || TM4 == TM || TM8 == TM || TM9 == TM || TM10 == TM) {
    K_MIMO = 2;
  } else {
    K_MIMO = 1;
  }
  N_IR = floor((float)N_soft / (float)(K_C * K_MIMO * MIN(M_DL_HARQ, M_limit)));
  return N_IR;
}

//! CSI RS周期，子帧偏移计算。出自 ：3GPP 36.211 6.10.5.3节
/*!
  \param I_CSI_RS ： I_CSI_RS，输入
  \param T_CSI_RS ： T_CSI_RS周期，输出
  \param Delta_CSI_RS ： Delta_CSI_RS子帧偏移，输出
  \return 无
*/
inline void Get_T_CSI_RS_Delta_CSI_RS(int I_CSI_RS, int *T_CSI_RS, int *Delta_CSI_RS) {
  if (I_CSI_RS < 5) {
    *T_CSI_RS = 5;
    *Delta_CSI_RS = I_CSI_RS;
  } else if (I_CSI_RS < 15) {
    *T_CSI_RS = 10;
    *Delta_CSI_RS = I_CSI_RS - 5;
  } else if (I_CSI_RS < 35) {
    *T_CSI_RS = 20;
    *Delta_CSI_RS = I_CSI_RS - 15;
  } else if (I_CSI_RS < 75) {
    *T_CSI_RS = 40;
    *Delta_CSI_RS = I_CSI_RS - 35;
  } else {
    *T_CSI_RS = 80;
    *Delta_CSI_RS = I_CSI_RS - 75;
  }
}

//! MLD算法中，后续路径计算
/*!
  \param Qm ： 调制方式，输入
  \param Z ： 复数float值，接收信号，输入
  \param R ： 复数float值，信道估计，输入
  \param candidated_idxs ： 后续路径集合，输出
  \param Constellation ： 后续路径对应的调制方式集合，输出
  \return 后续路径数量
*/
inline int Get_MLD_candidated_idxs(int Qm, cpxf Z, cpxf R, ivec &candidated_idxs, const cpxf **Constellation) {
  int MinDistant_Idx = 0;
  int L = 0;
  Z = Z / R;
  if (2 == Qm) {
    *Constellation = Constellation_QPSK;
    MinDistant_Idx = (Z.real() >= 0) ? 0 : 2;
    MinDistant_Idx += (Z.imag() >= 0) ? 0 : 1;
    L = 4;
    candidated_idxs.segment(0, L) = MLD_Candidated_idxs_QPSK.col(MinDistant_Idx).segment(0, L);
  } else if (4 == Qm) {
    *Constellation = Constellation_16QAM;
    MinDistant_Idx = (Z.real() >= 0) ? 0 : 8;
    MinDistant_Idx += (Z.imag() >= 0) ? 0 : 4;
    MinDistant_Idx += (MyFabs(Z.real()) <= D_16QAM) ? 0 : 2;
    MinDistant_Idx += (MyFabs(Z.imag()) <= D_16QAM) ? 0 : 1;
    L = 16;
    candidated_idxs.segment(0, L) = MLD_Candidated_idxs_16QAM.col(MinDistant_Idx).segment(0, L);
  } else if (6 == Qm) {
    *Constellation = Constellation_64QAM;
    MinDistant_Idx = (Z.real() >= 0) ? 0 : 32;
    MinDistant_Idx += (Z.imag() >= 0) ? 0 : 16;
    MinDistant_Idx += (MyFabs(Z.real()) <= 2.0f * D_64QAM) ? 0 : 8;
    MinDistant_Idx += (MyFabs(Z.imag()) <= 2.0f * D_64QAM) ? 0 : 4;
    MinDistant_Idx += (MyFabs(MyFabs(Z.real()) - 2.0f * D_64QAM) <= D_64QAM) ? 0 : 2;
    MinDistant_Idx += (MyFabs(MyFabs(Z.imag()) - 2.0f * D_64QAM) <= D_64QAM) ? 0 : 1;
    L = 64;
    candidated_idxs.segment(0, L) = MLD_Candidated_idxs_64QAM.col(MinDistant_Idx).segment(0, L);
  } else {
    *Constellation = Constellation_256QAM;
    L = 256;
  }
  return L;
}

//! MLD算法中，寻找M条累计度量值最小的路径
/*!
  \param acc_metrics_temp ： 所有路径的累计度量值，float型列向量，输入
  \param ML ： M*L，输入
  \param M_idxs ： M条路径的序号，int型列向量，输出
  \param acc_metrics ： M条路径的累计度量值，float型列向量，输出
  \return 无
*/
inline void Find_MDL_M_metrics(fvec &acc_metrics_temp, int ML, ivec &M_idxs, fvec &acc_metrics) {
  int M = (int)M_idxs.size();
  int i = 0;
  int j = 0;
  float min_values[M] = {0.0f};
  int min_idxs[M] = {0};
  for (j = 0; j < M; j += 2) {
    if (acc_metrics_temp[0] <= acc_metrics_temp[1]) {
      min_values[j] = acc_metrics_temp[0];
      min_values[j + 1] = acc_metrics_temp[1];
      min_idxs[j] = 0;
      min_idxs[j + 1] = 1;
    } else {
      min_values[j] = acc_metrics_temp[1];
      min_values[j + 1] = acc_metrics_temp[0];
      min_idxs[j] = 1;
      min_idxs[j + 1] = 0;
    }
    for (i = 2; i < ML; i++) {
      if (min_values[j] > acc_metrics_temp[i]) {
        min_values[j + 1] = min_values[j];
        min_idxs[j + 1] = min_idxs[j];
        min_values[j] = acc_metrics_temp[i];
        min_idxs[j] = i;
      } else if (min_values[j + 1] > acc_metrics_temp[i]) {
        min_values[j + 1] = acc_metrics_temp[i];
        min_idxs[j + 1] = i;
      }
    }
    acc_metrics_temp(min_idxs[j]) = acc_metrics_temp(min_idxs[j + 1]) = FLT_MAX;
  }
  for (i = 0; i < M; i++) {
    acc_metrics[i] = min_values[i];
    M_idxs[i] = min_idxs[i];
  }
}

#endif
