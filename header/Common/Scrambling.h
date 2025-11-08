/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Scrambling.h
    \brief Scrambling头文件
    内容 ：一些加扰、解扰函数相关声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Scrambling_h_
#define _Scrambling_h_

#include "Definitions.h"
#include "FastFunctions.h"
#include "Typedefs.h"

void PNSequence_Gen(uint32 c_init, bvec &PNBits, int N);
void Scrambling(uint32 c_init, bvec &bits);
void DeScrambling(uint32 c_init, fvec &softbits);

#endif
