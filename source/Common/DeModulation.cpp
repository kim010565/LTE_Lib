/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file DeModulation.cpp
    \brief DeModulation实现文件
    内容 ：一些解调制函数相关实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/DeModulation.h"

//! 简化解调宏开关，1 ：简化方法； 0 ：非简化方法
#define SIMPLE_LLR 1  // 1 : Enable; 0: Disable

//! BPSK解调函数
/*!
  \param symbs ： 复数float型列向量，输入
  \param softbits ： float型列向量，输出
  \return 无返回值
*/
void DeModulation_BPSK(const cfvec &symbs, fvec &softbits)  //BPSK解调
{
  int i = 0;
  int nsymbs = symbs.size();
  softbits.resize(nsymbs);
  for (i = 0; i < nsymbs; i++) {
#if 0 == SIMPLE_LLR
        softbits(i) = (symbs(i).real() + symbs(i).imag()) * SQRT_2_RECIP;
#else
    softbits(i) = symbs(i).real() + symbs(i).imag();
#endif
  }
}

//! QPSK解调函数
/*!
  \param symbs ： 复数float型列向量，输入
  \param softbits ： float型列向量，输出
  \return 无返回值
*/
void DeModulation_QPSK(const cfvec &symbs, fvec &softbits)  //QPSK解调
{
  int i = 0;
  int j = 0;
  int nsymbs = symbs.size();
  softbits.resize(2 * nsymbs);
  for (i = 0; i < nsymbs; i++) {
    j = 2 * i;
#if 0 == SIMPLE_LLR
        softbits(j) = symbs(i).real() * SQRT_2_RECIP;
        softbits(j + 1) = symbs(i).imag() * SQRT_2_RECIP;
#else
    softbits(j) = symbs(i).real();
    softbits(j + 1) = symbs(i).imag();
#endif
  }
}

//! 16QAM解调函数
/*!
  \param symbs ： 复数float型列向量，输入
  \param d ： 16QAM星座图区域边界划分值，输入
  \param softbits ： float型列向量，输出
  \return 无返回值
*/
void DeModulation_16QAM(const cfvec &symbs, float d, fvec &softbits)  //16QAM解调
{
  int i = 0;
  int j = 0;
  int nsymbs = symbs.size();
  softbits.resize(4 * nsymbs);
  for (i = 0; i < nsymbs; i++) {
    j = 4 * i;
#if 1 == SIMPLE_LLR
    if (symbs(i).real() >= d) {
      softbits(j) = 2.0f * D_16QAM * (2.0f * symbs(i).real() - d);
    } else if (symbs(i).real() <= (-d)) {
      softbits(j) = 2.0f * D_16QAM * (2.0f * symbs(i).real() + d);
    } else {
      softbits(j) = 2.0f * D_16QAM * symbs(i).real();
    }
    if (symbs(i).imag() >= d) {
      softbits(j + 1) = 2.0f * D_16QAM * (2.0f * symbs(i).imag() - d);
    } else if (symbs(i).imag() <= (-d)) {
      softbits(j + 1) = 2.0f * D_16QAM * (2.0f * symbs(i).imag() + d);
    } else {
      softbits(j + 1) = 2.0f * D_16QAM * symbs(i).imag();
    }
    softbits(j + 2) = 2.0f * D_16QAM * (d - MyFabs(symbs(i).real()));
    softbits(j + 3) = 2.0f * D_16QAM * (d - MyFabs(symbs(i).imag()));
#else
    softbits(j) = symbs(i).real();
    softbits(j + 1) = symbs(i).imag();
    softbits(j + 2) = d - MyFabs(symbs(i).real());
    softbits(j + 3) = d - MyFabs(symbs(i).imag());
#endif
  }
}

//! 64QAM解调函数
/*!
  \param symbs ： 复数float型列向量，输入
  \param d ： 64QAM星座图区域边界划分值，输入
  \param softbits ： float型列向量，输出
  \return 无返回值
*/
void DeModulation_64QAM(const cfvec &symbs, float d, fvec &softbits)  //64QAM解调
{
  int i = 0;
  int j = 0;
  int nsymbs = symbs.size();
  softbits.resize(6 * nsymbs);
  for (i = 0; i < nsymbs; i++) {
    j = 6 * i;
#if 0 == SIMPLE_LLR
        if (symbs(i).real() >= (3.0f * d))
        {
            softbits(j) = 2.0f * D_64QAM * (4.0f * symbs(i).real() - 6.0f * d);
        }
        else if (symbs(i).real() <= (-3.0f * d))
        {
            softbits(j) = 2.0f * D_64QAM * (4.0f * symbs(i).real() + 6.0f * d);
        }
        else if (symbs(i).real() >= (2.0f * d))
        {
            softbits(j) = 2.0f * D_64QAM * (3.0f * symbs(i).real() - 3.0f * d);
        }
        else if (symbs(i).real() <= (-2.0f * d))
        {
            softbits(j) = 2.0f * D_64QAM * (3.0f * symbs(i).real() + 3.0f * d);
        }
        else if (symbs(i).real() >= d)
        {
            softbits(j) = 2.0f * D_64QAM * (2.0f * symbs(i).real() - d);
        }
        else if (symbs(i).real() <= (-d))
        {
            softbits(j) = 2.0f * D_64QAM * (2.0f * symbs(i).real() + d);
        }
        else
        {
            softbits(j) = 2.0f * D_64QAM * symbs(i).real();
        }
        if (symbs(i).imag() >= (3.0f * d))
        {
            softbits(j + 1) = 2.0f * D_64QAM * (4.0f * symbs(i).imag() - 6.0f * d);
        }
        else if (symbs(i).imag() <= (-3.0f * d))
        {
            softbits(j + 1) = 2.0f * D_64QAM * (4.0f * symbs(i).imag() + 6.0f * d);
        }
        else if (symbs(i).imag() >= (2.0f * d))
        {
            softbits(j + 1) = 2.0f * D_64QAM * (3.0f * symbs(i).imag() - 3.0f * d);
        }
        else if (symbs(i).imag() <= (-2.0f * d))
        {
            softbits(j + 1) = 2.0f * D_64QAM * (3.0f * symbs(i).imag() + 3.0f * d);
        }
        else if (symbs(i).imag() >= d)
        {
            softbits(j + 1) = 2.0f * D_64QAM * (2.0f * symbs(i).imag() - d);
        }
        else if (symbs(i).imag() <= (-d))
        {
            softbits(j + 1) = 2.0f * D_64QAM * (2.0f * symbs(i).imag() + d);
        }
        else
        {
            softbits(j + 1) = 2.0f * D_64QAM * symbs(i).imag();
        }
        if (MyFabs(symbs(i).real()) <= d)
        {
            softbits(j + 2) = 2.0f * D_64QAM * (-2.0f * MyFabs(symbs(i).real()) + 3.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (3.0f * d))
        {
            softbits(j + 2) = 2.0f * D_64QAM * (-MyFabs(symbs(i).real()) + 2.0f * d);
        }
        else
        {
            softbits(j + 2) = 2.0f * D_64QAM * (-2.0f * MyFabs(symbs(i).real()) + 5.0f * d);
        }
        if (MyFabs(symbs(i).imag()) <= d)
        {
            softbits(j + 3) = 2.0f * D_64QAM * (-2.0f * MyFabs(symbs(i).imag()) + 3.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (3.0f * d))
        {
            softbits(j + 3) = 2.0f * D_64QAM * (-MyFabs(symbs(i).imag()) + 2.0f * d);
        }
        else
        {
            softbits(j + 3) = 2.0f * D_64QAM * (-2.0f * MyFabs(symbs(i).imag()) + 5.0f * d);
        }
        if (MyFabs(symbs(i).real()) <= (2.0f * d))
        {
            softbits(j + 4) = 2.0f * D_64QAM * (MyFabs(symbs(i).real()) - d);
        }
        else
        {
            softbits(j + 4) = 2.0f * D_64QAM * (-MyFabs(symbs(i).real()) + 3.0f * d);
        }
        if (MyFabs(symbs(i).imag()) <= (2.0f * d))
        {
            softbits(j + 5) = 2.0f * D_64QAM * (MyFabs(symbs(i).imag()) - d);
        }
        else
        {
            softbits(j + 5) = 2.0f * D_64QAM * (-MyFabs(symbs(i).imag()) + 3.0f * d);
        }
#else
    softbits(j) = symbs(i).real();
    softbits(j + 1) = symbs(i).imag();
    softbits(j + 2) = 2.0f * d - MyFabs(symbs(i).real());
    softbits(j + 3) = 2.0f * d - MyFabs(symbs(i).imag());
    softbits(j + 4) = d - MyFabs(softbits(j + 2.0f));
    softbits(j + 5) = d - MyFabs(softbits(j + 3.0f));
#endif
  }
}

//! 256QAM解调函数
/*!
  \param symbs ： 复数float型列向量，输入
  \param d ： 256QAM星座图区域边界划分值，输入
  \param softbits ： float型列向量，输出
  \return 无返回值
*/
void DeModulation_256QAM(const cfvec &symbs, float d, fvec &softbits)  //256QAM解调
{
  int i = 0;
  int j = 0;
  int nsymbs = symbs.size();
  softbits.resize(8 * nsymbs);
  for (i = 0; i < nsymbs; i++) {
    j = 8 * i;
#if 0 == SIMPLE_LLR
        if (symbs(i).real() >= (7.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (8.0f * symbs(i).real() - 28.0f * d);
        }
        else if (symbs(i).real() <= (-7.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (8.0f * symbs(i).real() + 28.0f * d);
        }
        else if (symbs(i).real() >= (6.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (7.0f * symbs(i).real() - 21.0f * d);
        }
        else if (symbs(i).real() <= (-6.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (7.0f * symbs(i).real() + 21.0f * d);
        }
        else if (symbs(i).real() >= (5.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (6.0f * symbs(i).real() - 15.0f * d);
        }
        else if (symbs(i).real() <= (-5.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (6.0f * symbs(i).real() + 15.0f * d);
        }
        else if (symbs(i).real() >= (4.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (5.0f * symbs(i).real() - 10.0f * d);
        }
        else if (symbs(i).real() <= (-4.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (5.0f * symbs(i).real() + 10.0f * d);
        }
        else if (symbs(i).real() >= (3.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (4.0f * symbs(i).real() - 6.0f * d);
        }
        else if (symbs(i).real() <= (-3.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (4.0f * symbs(i).real() + 6.0f * d);
        }
        else if (symbs(i).real() >= (2.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (3.0f * symbs(i).real() - 3.0f * d);
        }
        else if (symbs(i).real() <= (-2.0f * d))
        {
            softbits(j) = 2.0f * D_256QAM * (3.0f * symbs(i).real() + 3.0f * d);
        }
        else if (symbs(i).real() >= d)
        {
            softbits(j) = 2.0f * D_256QAM * (2.0f * symbs(i).real() - d);
        }
        else if (symbs(i).real() <= (-d))
        {
            softbits(j) = 2.0f * D_256QAM * (2.0f * symbs(i).real() + d);
        }
        else
        {
            softbits(j) = 2.0f * D_256QAM * symbs(i).real();
        }
        if (symbs(i).imag() >= (7.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (8.0f * symbs(i).imag() - 28.0f * d);
        }
        else if (symbs(i).imag() <= (-7.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (8.0f * symbs(i).imag() + 28.0f * d);
        }
        else if (symbs(i).imag() >= (6.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (7.0f * symbs(i).imag() - 21.0f * d);
        }
        else if (symbs(i).imag() <= (-6.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (7.0f * symbs(i).imag() + 21.0f * d);
        }
        else if (symbs(i).imag() >= (5.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (6.0f * symbs(i).imag() - 15.0f * d);
        }
        else if (symbs(i).imag() <= (-5.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (6.0f * symbs(i).imag() + 15.0f * d);
        }
        else if (symbs(i).imag() >= (4.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (5.0f * symbs(i).imag() - 10.0f * d);
        }
        else if (symbs(i).imag() <= (-4.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (5.0f * symbs(i).imag() + 10.0f * d);
        }
        else if (symbs(i).imag() >= (3.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (4.0f * symbs(i).imag() - 6.0f * d);
        }
        else if (symbs(i).imag() <= (-3.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (4.0f * symbs(i).imag() + 6.0f * d);
        }
        else if (symbs(i).imag() >= (2.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (3.0f * symbs(i).imag() - 3.0f * d);
        }
        else if (symbs(i).imag() <= (-2.0f * d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (3.0f * symbs(i).imag() + 3.0f * d);
        }
        else if (symbs(i).imag() >= d)
        {
            softbits(j + 1) = 2.0f * D_256QAM * (2.0f * symbs(i).imag() - d);
        }
        else if (symbs(i).imag() <= (-d))
        {
            softbits(j + 1) = 2.0f * D_256QAM * (2.0f * symbs(i).imag() + d);
        }
        else
        {
            softbits(j + 1) = 2.0f * D_256QAM * symbs(i).imag();
        }
        if (MyFabs(symbs(i).real()) <= d)
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-4.0f * MyFabs(symbs(i).real()) + 10.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (2.0f * d))
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-3.0f * MyFabs(symbs(i).real()) + 9.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (3.0f * d))
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-2.0f * MyFabs(symbs(i).real()) + 7.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (5.0f * d))
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-MyFabs(symbs(i).real()) + 4.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (6.0f * d))
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-2.0f * MyFabs(symbs(i).real()) + 9.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (7.0f * d))
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-3.0f * MyFabs(symbs(i).real()) + 15.0f * d);
        }
        else
        {
            softbits(j + 2) = 2.0f * D_256QAM * (-4.0f * MyFabs(symbs(i).real()) + 22.0f * d);
        }
        if (MyFabs(symbs(i).imag()) <= d)
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-4.0f * MyFabs(symbs(i).imag()) + 10.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (2.0f * d))
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-3.0f * MyFabs(symbs(i).imag()) + 9.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (3.0f * d))
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-2.0f * MyFabs(symbs(i).imag()) + 7.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (5.0f * d))
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-MyFabs(symbs(i).imag()) + 4.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (6.0f * d))
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-2.0f * MyFabs(symbs(i).imag()) + 9.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (7.0f * d))
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-3.0f * MyFabs(symbs(i).imag()) + 15.0f * d);
        }
        else
        {
            softbits(j + 3) = 2.0f * D_256QAM * (-4.0f * MyFabs(symbs(i).imag()) + 22.0f * d);
        }
        if (MyFabs(symbs(i).real()) <= d)
        {
            softbits(j + 4) = 2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).real()) - 3.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (3.0f * d))
        {
            softbits(j + 4) = 2.0f * D_256QAM * (MyFabs(symbs(i).real()) - 2.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (4.0f * d))
        {
            softbits(j + 4) = 2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).real()) - 5.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (5.0f * d))
        {
            softbits(j + 4) = -2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).real()) - 11.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (7.0f * d))
        {
            softbits(j + 4) = -2.0f * D_256QAM * (MyFabs(symbs(i).real()) - 6.0f * d);
        }
        else
        {
            softbits(j + 4) = -2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).real()) - 13.0f * d);
        }
        if (MyFabs(symbs(i).imag()) <= d)
        {
            softbits(j + 5) = 2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).imag()) - 3.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (3.0f * d))
        {
            softbits(j + 5) = 2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - 2.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (4.0f * d))
        {
            softbits(j + 5) = 2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).imag()) - 5.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (5.0f * d))
        {
            softbits(j + 5) = -2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).imag()) - 11.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (7.0f * d))
        {
            softbits(j + 5) = -2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - 6.0f * d);
        }
        else
        {
            softbits(j + 5) = -2.0f * D_256QAM * (2.0f * MyFabs(symbs(i).imag()) - 13.0f * d);
        }
        if (MyFabs(symbs(i).real()) <= (2.0f * d))
        {
            softbits(j + 6) = 2.0f * D_256QAM * (MyFabs(symbs(i).real()) - d);
        }
        else if (MyFabs(symbs(i).real()) <= (4.0f * d))
        {
            softbits(j + 6) = -2.0f * D_256QAM * (MyFabs(symbs(i).real()) - 3.0f * d);
        }
        else if (MyFabs(symbs(i).real()) <= (6.0f * d))
        {
            softbits(j + 6) = 2.0f * D_256QAM * (MyFabs(symbs(i).real()) - 5.0f * d);
        }
        else
        {
            softbits(j + 6) = -2.0f * D_256QAM * (MyFabs(symbs(i).real()) - 7.0f * d);
        }
        if (MyFabs(symbs(i).imag()) <= (2.0f * d))
        {
            softbits(j + 7) = 2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - d);
        }
        else if (MyFabs(symbs(i).imag()) <= (4.0f * d))
        {
            softbits(j + 7) = -2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - 3.0f * d);
        }
        else if (MyFabs(symbs(i).imag()) <= (6.0f * d))
        {
            softbits(j + 7) = 2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - 5.0f * d);
        }
        else
        {
            softbits(j + 7) = -2.0f * D_256QAM * (MyFabs(symbs(i).imag()) - 7.0f * d);
        }
#else
    softbits(j) = symbs(i).real();
    softbits(j + 1) = symbs(i).imag();
    softbits(j + 2) = 4.0f * d - MyFabs(symbs(i).real());
    softbits(j + 3) = 4.0f * d - MyFabs(symbs(i).imag());
    softbits(j + 4) = 2.0f * d - MyFabs(softbits(j + 2.0f));
    softbits(j + 5) = 2.0f * d - MyFabs(softbits(j + 3.0f));
    softbits(j + 6) = d - MyFabs(softbits(j + 4.0f));
    softbits(j + 7) = d - MyFabs(softbits(j + 5.0f));
#endif
  }
}
