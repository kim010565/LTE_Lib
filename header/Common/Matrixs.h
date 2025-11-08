/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Matrixs.h
    \brief Matrixs头文件
    内容 ：一些矩阵相关运算
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Matrixs_h_
#define _Matrixs_h_

#include "Definitions.h"
#include "Typedefs.h"

//! HERMITIAN矩阵求逆方法，宏开关。0: 张贤达《矩阵论》中递归方法； 1 ：下标变换方法；2 ：Eigen本身方法
#define HERMITIAN_INV_METHOD 1  //0: 《矩阵论》中递归方法； 1 ：下标变换方法；2 ：Eigen方法

//! 快速哈达码变化函数
/*!
  \param temp ： 复数float型列向量，输入/输出
  \param stage_num ： FHT变换阶数，输入
  \return 无返回值
*/
inline void FHT(cfvec &temp, int stage_num)  //快速Hadamard变换
{
  int idx, block, stage;
  int block_num, block_shift, seg, block_shift_seg;
  cpxf temp1;
  for (stage = 0; stage < stage_num; stage++) {
    block_num = (1 << (stage_num - stage - 1));
    seg = (1 << stage);
    for (block = 0; block < block_num; block++) {
      block_shift = (block << (stage + 1));
      block_shift_seg = block_shift + seg;
      for (idx = 0; idx < seg; idx++) {
        temp1 = temp[block_shift] - temp[block_shift_seg];
        temp[block_shift++] += temp[block_shift_seg];
        temp[block_shift_seg++] = temp1;
      }
    }
  }
}
//! 快速哈达码变化函数
/*!
  \param a ： 复数float型列向量，输入
  \param temp ： 复数float型列向量，输出
  \param stage_num ： FHT变换阶数，输入
  \return 无返回值
*/
inline void FHT(const cfvec &a, cfvec &temp, int stage_num) {
  int idx, block, stage;
  int block_num, block_shift, seg, block_shift_seg;
  temp = a;
  cpxf temp1;
  for (stage = 0; stage < stage_num; stage++) {
    block_num = (1 << (stage_num - stage - 1));
    seg = (1 << stage);
    for (block = 0; block < block_num; block++) {
      block_shift = (block << (stage + 1));
      block_shift_seg = block_shift + seg;
      for (idx = 0; idx < seg; idx++) {
        temp1 = temp[block_shift] - temp[block_shift_seg];
        temp[block_shift++] += temp[block_shift_seg];
        temp[block_shift_seg++] = temp1;
      }
    }
  }
}

//! 哈达码矩阵产生函数
/*!
  \param a ： float型矩阵，输出
  \param n ： 哈达码矩阵大小n*n，输入
  \return 无返回值
*/
inline void Hadamard(fmat &a, int n)  //Hadamard矩阵产生
{
  int k = 0;
  int i = 0;
  int j = 0;
  a.resize(n, n);
  if (1 == n) {
    a(0, 0) = 1.0f;
  } else {
    a(0, 0) = 1.0f;
    a(0, 1) = 1.0f;
    a(1, 0) = 1.0f;
    a(1, 1) = -1.0f;
    for (k = 2; k < n; k = k * 2) {
      for (i = 0; i < k; i++) {
        for (j = 0; j < k; j++) {
          a(i + k, j) = a(i, j);
          a(i, j + k) = a(i, j);
          a(i + k, j + k) = -a(i, j);
        }
      }
    }
  }
}
//! 哈达码矩阵产生函数
/*!
  \param a ： 复数float型矩阵，输出
  \param n ： 哈达码矩阵大小n*n，输入
  \return 无返回值
*/
inline void Hadamard(cfmat &a, int n) {
  int k = 0;
  int i = 0;
  int j = 0;
  a.resize(n, n);
  if (1 == n) {
    a(0, 0) = cpxf(1.0f, 0.0f);
  } else {
    a(0, 0) = cpxf(1.0f, 0.0f);
    a(0, 1) = cpxf(1.0f, 0.0f);
    a(1, 0) = cpxf(1.0f, 0.0f);
    a(1, 1) = cpxf(-1.0f, 0.0f);
    for (k = 2; k < n; k = k * 2) {
      for (i = 0; i < k; i++) {
        for (j = 0; j < k; j++) {
          a(i + k, j) = a(i, j);
          a(i, j + k) = a(i, j);
          a(i + k, j + k) = -a(i, j);
        }
      }
    }
  }
}

//! Hermitian矩阵求逆函数
/*!
  \param M_R ： 复数float型Hermitian矩阵，输入
  \param M_INV_R ： 复数float型矩阵，输出
  \return 无返回值
*/
inline void Hermitian_Matrix_INV(const cfmat &M_R, cfmat &M_INV_R)  //Hermitian矩阵求逆
{
#if 0 == HERMITIAN_INV_METHOD
    int i = 0;
    int m = M_R.rows();
    M_INV_R.resize(m, m);
    cfvec V_r(m);
    cfvec V_b(m);
    cpxf lu = 0.0f;
    cpxf beta = 0.0f;
    M_INV_R.setZero();
    M_INV_R(0, 0) = 1.0f / M_R(0, 0);
    for (i = 1; i < m; i++)
    {
        V_r.segment(0, i) = M_R.col(i).segment(0, i);
        lu = M_R(i, i).real();
        V_b.segment(0, i) = -M_INV_R.block(0, 0, i, i) * V_r.segment(0, i);
        beta = V_r.segment(0, i).adjoint() * V_b.segment(0, i);
        beta = lu + beta;
        M_INV_R.block(0, 0, i, i) = M_INV_R.block(0, 0, i, i) + (V_b.segment(0, i) * V_b.segment(0, i).adjoint()) / beta;
        M_INV_R.col(i).segment(0, i) = V_b.segment(0, i) / beta;
        M_INV_R.row(i).segment(0, i) = M_INV_R.col(i).segment(0, i).adjoint();
        M_INV_R(i, i) = 1.0f / beta;
    }
#elif 1 == HERMITIAN_INV_METHOD
  int i = 0;
  int m = M_R.rows();
  M_INV_R = M_R;
  cfmat M_temp(m, m);
  for (i = 0; i < m; i++) {
    M_temp(m - 1, m - 1) = 1.0f / M_INV_R(0, 0);
    M_temp.row(m - 1).segment(0, m - 1) = -M_INV_R.row(0).segment(1, m - 1) * M_temp(m - 1, m - 1);
    M_temp.col(m - 1).segment(0, m - 1) = M_INV_R.col(0).segment(1, m - 1) * M_temp(m - 1, m - 1);
    M_temp.block(0, 0, m - 1, m - 1) = M_INV_R.block(1, 1, m - 1, m - 1) - M_INV_R.col(0).segment(1, m - 1) * M_INV_R.row(0).segment(1, m - 1) * M_temp(m - 1, m - 1);
    M_INV_R = M_temp;
  }
#elif 2 == HERMITIAN_INV_METHOD
  M_INV_R = M_R.inverse();
#endif
}

#endif
