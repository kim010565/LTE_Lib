/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Scrambling.cpp
    \brief Scrambling实现文件
    内容 ：一些加扰、解扰函数相关实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Scrambling.h"

//! 伪随机序列产生函数。出自 ：3GPP 36.211 7.2节
/*!
  \param c_init ： 无符号整数，序列x2移位寄存器的初始状态，输入
  \param PNBits ： 二进制伪随机列向量，输出
  \param N ： 二进制伪随机列向量长度，输入
  \return 无
*/
void PNSequence_Gen(uint32 c_init, bvec &PNBits, int N)  //伪随机序列产生 36.211 7.2
{
  uint32 x2 = c_init;
  uint32 x2_r1 = 0;
  uint32 x2_r2 = 0;
  uint32 x2_r3 = 0;
  uint32 x1 = 0x1;
  uint32 x1_r3 = 0;
  uint32 c = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  PNBits.resize(N);
  //N_C = 1600 = 32 * 50
  for (i = 0; i < 49; i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
  }
  for (i = 0; i < floor((float)N / 32.0f); i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < 32; j++) {
      PNBits((i << 5) + j) = ((c >> j) & 0x1);
    }
  }
  k = N - i * 32;
  if (k > 0) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < k; j++) {
      PNBits((i << 5) + j) = ((c >> j) & 0x1);
    }
  }
}

//! 用伪随机序列对原序列加扰函数。出自 ：3GPP 36.211 7.2节
/*!
  \param c_init ： 无符号整数，序列x2移位寄存器的初始状态，输入
  \param bits ： 二进制列向量，输入/输出
  \return 无
*/
void Scrambling(uint32 c_init, bvec &bits)  //加扰
{
  uint32 x2 = c_init;
  uint32 x2_r1 = 0;
  uint32 x2_r2 = 0;
  uint32 x2_r3 = 0;
  uint32 x1 = 0x1;
  uint32 x1_r3 = 0;
  uint32 c = 0;
  int i = 0;
  int j = 0;
  int N = bits.size();
  //N_C = 1600 = 32 * 50
  for (i = 0; i < 49; i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
  }
  for (i = 0; i < floor((float)N / 32.0f); i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < 32; j++) {
      bits(i * 32 + j) ^= ((c >> j) & 0x1);
    }
  }
  i = ceil((float)N / 32.0f) - floor((float)N / 32.0f);
  if (i > 0) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < i; j++) {
      bits(i * 32 + j) ^= ((c >> j) & 0x1);
    }
  }
}

//! 用伪随机序列对原序列解扰函数。出自 ：3GPP 36.211 7.2节
/*!
  \param c_init ： 无符号整数，序列x2移位寄存器的初始状态，输入
  \param softbits ： float型列向量，输入/输出
  \return 无
*/
void DeScrambling(uint32 c_init, fvec &softbits)  //解扰
{
  uint32 x2 = c_init;
  uint32 x2_r1 = 0;
  uint32 x2_r2 = 0;
  uint32 x2_r3 = 0;
  uint32 x1 = 0x1;
  uint32 x1_r3 = 0;
  uint32 c = 0;
  int i = 0;
  int j = 0;
  int N = softbits.size();
  //N_C = 1600 = 32 * 50
  for (i = 0; i < 49; i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
  }
  for (i = 0; i < floor((float)N / 32.0f); i++) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < 32; j++) {
      softbits((i << 5) + j) *= ((0 == ((c >> j) & 0x1)) ? 1.0f : -1.0f);
    }
  }
  i = ceil((float)N / 32.0f) - floor((float)N / 32.0f);
  if (i > 0) {
    //x2 set bit 31 to 0
    x2 = x2 & 0x7FFFFFFF;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x2_r1 = x2 >> 1;
    x2_r2 = x2 >> 2;
    x2_r3 = x2 >> 3;
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x2 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x2_r1 = (x2 << 29);
    x2_r2 = (x2 << 28);
    x2_r3 = (x2 << 27);
    x2 = ((x2 >> 1) | (x2 << 30));
    x2 = x2 ^ x2_r1 ^ x2_r2 ^ x2_r3;
    //x1 set bit 31 to 0
    x1 = x1 & 0x7FFFFFFF;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32-1 ~ (i+1)*32+26
    x1_r3 = x1 >> 3;
    x1 = x1 ^ x1_r3;
    //x1(n+31) = (x1(n+3) + x1(n)) mod 2, for n = (i+1)*32+27 ~ (i+1)*32+31 And x1 shift 1 for n = (i+1)*32 ~ (i+1)*32+31
    x1_r3 = (x1 << 27);
    x1 = ((x1 >> 1) | (x1 << 30));
    x1 = x1 ^ x1_r3;
    //c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
    c = x2 ^ x1;
    for (j = 0; j < i; j++) {
      softbits((i << 5) + j) *= ((0 == ((c >> j) & 0x1)) ? 1.0f : -1.0f);
    }
  }
}
