/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Definitions.h
    \brief Definitions头文件
    内容 ：常量定义；报错函数；自定义打屏函数；包含自定义Files.h
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Definitions_h_
#define _Definitions_h_

#include "../Common/Files.h"

//! 错误检测宏开关，0 ： 不检测错误； 1 ：检测错误
#define ERROR_REPORT 0
//! 屏幕打印宏开关，0 ： 不打印； 1 ：打印
#define MYPRINT 1
//! 理想信道估计宏开关，0 ： 不时能； 1 ：理想PDP； 2 ：理想LS估计结果； 3 ：理想频域滤波估计结果； 4 ： 理想时域滤波估计结果
#define IDEAL_H_METHOD 0  //0: Disable; 1:Ideal PDP; 2: Ideal H LS; 3: Ideal H FilterF; 4: Ideal H FilterT
//! EESM测试宏开关，0 ：关闭； 1 ：打开
#define EESM_TEST 0
//! CQI测试宏开关，0 ：关闭； 1 ：打开
#define CQI_TEST 0

//宏定义
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//宏函数
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define SWAP(a, b) \
  (a) = (a) ^ (b); \
  (b) = (b) ^ (a); \
  (a) = (a) ^ (b);

//常数
//! 常数pi，圆周率
const float PI = 3.1415926535897932384626433832795f;
//! 常数sqrt(2)
const float SQRT_2 = 1.4142135623730950488016887242097f;
//! 常数1/sqrt(2)
const float SQRT_2_RECIP = 0.70710678118654752440084436210485f;
//LTE常数
//! LTE最大传输块数量
const int MAX_TB_SIZE = 2;  // MAX TB size
//! LTE最大端口数量
const int MAX_PORT_NUM = 27;
//! LTE最大物理天线数
const int MAX_PHY_ANTENNA_NUM = 8;
//! LTE最大下行RB数
const int N_MAX_DL_RB = 110;
//! LTE最大HARQ进程数
const int MAXHARQNUM = 15;
//! LTE一个TTI内最大符号数
const int MAX_SYMBNUM_TTI = 14;  // MAX SYMB NUM per TTI
//! PAPR峰均比
const float PAPR = 16.0f;
//! LTE最大CRS端口数
const int MAXCRSPORTNUM = 4;
//! LTE最大DMRS端口数
const int MAXDMRSPORTNUM = 8;
//! LTE最大EPDCCH端口数
const int MAXEPDCCHRSPORTNUM = 4;
//! LTE最大CSI-RS端口数
const int MAXCSIRSPORTNUM = 8;
//! LTE一个TTI内最大CRS符号数
const int MAXCRSSYMB_TTI = 4;
//! LTE一个TTI内最大Port5的符号数
const int MAXDMRSSYMB_TTI_Port5 = 4;
//! LTE一个TTI内最大Port7to14的符号数
const int MAXDMRSSYMB_TTI_Port7to14 = 2;
//! LTE最大ZP CSI-RS数
const int MAXNZPCSIRSNUM = 3;
//! LTE PDP DFT最大尺寸
const int MAXPDPLENGTH = 341;
//! LTE PDP最大径数
const int MAXPDPSNUM = 48;
//! LTE RC滤波器长度
const int RC_FIR_LEN = 33;
//! LTE时域相关值计算个数
const int R_T_Num = 15;
//! LTE频域相关值计算个数
const int R_F_Num = 75;
//! LTE频域滤波系数长度
const int N_FilterF = 12;
//! LTE最大RI值
const int MAX_RI = 4;
//! LTE最大PMI值
const int MAX_PMI = 16;
//! LTE反馈Buffer大小
const int MAX_Feedback_BufferSize = 16;
//! LTE最大子带个数
const int MAX_N_Subband = 100;

//! 报错函数，可由宏ERROR_REPORT控制
/*!
  \param format ：输入参考printf函数的输入
  \return 无返回值
*/
inline void Error(const char *format, ...) {
#if 1 == ERROR_REPORT
  va_list args;
  printf("\n ERROR: ");
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
  exit(0);
#endif
}

//! 自定义打屏函数，可由宏MYPRINT控制
/*!
  \param format ：输入参考printf函数的输入
  \return 无返回值
*/
inline void MyPrint(const char *format, ...) {
#if 1 == MYPRINT
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
#endif
}

#endif
