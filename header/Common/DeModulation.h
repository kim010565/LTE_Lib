/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file DeModulation.h
    \brief DeModulation头文件
内容 ：一些解调制函数相关声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _DeModulation_h_
#define _DeModulation_h_

#include "Definitions.h"
#include "FastFunctions.h"
#include "Typedefs.h"

//! 16QAM标准星座图区域边界划分值
const float D_16QAM = 2.0f / MySqrt(10.0f);
//! 64QAM标准星座图区域边界划分值
const float D_64QAM = 2.0f / MySqrt(42.0f);
//! 256QAM标准星座图区域边界划分值
const float D_256QAM = 2.0f / MySqrt(170.0f);

void DeModulation_BPSK(const cfvec &symbs, fvec &softbits);
void DeModulation_QPSK(const cfvec &symbs, fvec &softbits);
void DeModulation_16QAM(const cfvec &symbs, float d, fvec &softbits);
void DeModulation_64QAM(const cfvec &symbs, float d, fvec &softbits);
void DeModulation_256QAM(const cfvec &symbs, float d, fvec &softbits);

#endif
