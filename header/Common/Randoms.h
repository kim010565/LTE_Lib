/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Randoms.h
    \brief Randoms头文件
    内容 ：一些随机数产生相关运算
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Randoms_h_
#define _Randoms_h_

#include "Definitions.h"
#include "Typedefs.h"

//! (a, b)内均匀分布随机数产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \return float型，随机数
*/
inline float uniform(float a, float b, long int *seed)  //产生(a, b)内均匀分布的随机数， 种子seed
{
  float t;
  const float c = 1.0f / 1048576.0f;
  *seed = 2045 * (*seed) + 1;
  *seed = (*seed) - (*seed / 1048576) * 1048576;
  t = ((float)(*seed) * c);
  t = a + (b - a) * t;
  return t;
}
//! (a, b)内均匀分布的随机数列向量产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数列向量，输出
  \param N ： 随机数列向量长度，输入
  \return 无
*/
inline void uniform(float a, float b, long int *seed, fvec &t, int N) {
  int i = 0;
  t.resize(N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < N; i++) {
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i) = ((float)(*seed) * c);
    t(i) = a + (b - a) * t(i);
  }
}
//! (a, b)内均匀分布的随机数行向量产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数行向量，输出
  \param N ： 随机数行向量长度，输入
  \return 无
*/
inline void uniform(float a, float b, long int *seed, frowvec &t, int N) {
  int i = 0;
  t.resize(N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < N; i++) {
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i) = ((float)(*seed) * c);
    t(i) = a + (b - a) * t(i);
  }
}
//! (a, b)内均匀分布的随机数矩阵产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数矩阵，输出
  \param M ： 随机数矩阵行数，输入
  \param N ： 随机数矩阵列数，输入
  \return 无
*/
inline void uniform(float a, float b, long int *seed, fmat &t, int M, int N) {
  int i = 0;
  int j = 0;
  t.resize(M, N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      *seed = 2045 * (*seed) + 1;
      *seed = (*seed) - (*seed / 1048576) * 1048576;
      t(i, j) = ((float)(*seed) * c);
      t(i, j) = a + (b - a) * t(i, j);
    }
  }
}
//! (a, b)内均匀分布的随机数复数列向量产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数复数列向量，输出
  \param N ： 随机数复数列向量长度，输入
  \return 无
*/
inline void uniform_cpx(float a, float b, long int *seed, cfvec &t, int N) {
  int i = 0;
  t.resize(N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < N; i++) {
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i).real((float)(*seed) * c);
    t(i).real(a + (b - a) * t(i).real());
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i).imag((float)(*seed) * c);
    t(i).imag(a + (b - a) * t(i).imag());
  }
}
//! (a, b)内均匀分布的随机数复数行向量产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数复数行向量，输出
  \param N ： 随机数复数行向量长度，输入
  \return 无
*/
inline void uniform_cpx(float a, float b, long int *seed, cfrowvec &t, int N) {
  int i = 0;
  t.resize(N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < N; i++) {
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i).real((float)(*seed) * c);
    t(i).real(a + (b - a) * t(i).real());
    *seed = 2045 * (*seed) + 1;
    *seed = (*seed) - (*seed / 1048576) * 1048576;
    t(i).imag((float)(*seed) * c);
    t(i).imag(a + (b - a) * t(i).imag());
  }
}
//! (a, b)内均匀分布的随机数复数矩阵产生函数
/*!
  \param a ： float型，分布下限，输入
  \param b ： float型，分布上限，输入
  \param seed ： 随机数种子，输入
  \param t ： 随机数复数矩阵，输出
  \param M ： 随机数复数矩阵行数，输入
  \param N ： 随机数复数矩阵列数，输入
  \return 无
*/
inline void uniform_cpx(float a, float b, long int *seed, cfmat &t, int M, int N) {
  int i = 0;
  int j = 0;
  t.resize(M, N);
  const float c = 1.0f / 1048576.0f;
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      *seed = 2045 * (*seed) + 1;
      *seed = (*seed) - (*seed / 1048576) * 1048576;
      t(i, j).real((float)(*seed) * c);
      t(i, j).real(a + (b - a) * t(i, j).real());
      *seed = 2045 * (*seed) + 1;
      *seed = (*seed) - (*seed / 1048576) * 1048576;
      t(i, j).imag((float)(*seed) * c);
      t(i, j).imag(a + (b - a) * t(i, j).imag());
    }
  }
}

//! 满足(mean, sigma^2)高斯分布的随机数产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \return float型，随机数
*/
inline float gauss(float mean, float sigma, long int *seed)  //产生均值为mean，标准差为sigma的高斯分布随机数， 种子seed
{
  int i;
  float x, y;
  for (x = 0, i = 0; i < 12; i++) {
    x += uniform(0.0f, 1.0f, seed);
  }
  x = x - 6.0f;
  y = mean + x * sigma;
  return y;
}
//! 满足(mean, sigma^2)高斯分布的随机数列向量产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \param y ： 随机数列向量，输出
  \param N ： 随机数列向量长度，输入
  \return 无
*/
inline void gauss(float mean, float sigma, long int *seed, fvec &y, int N) {
  int ii = 0;
  int i;
  float x;
  y.resize(N);
  for (ii = 0; ii < N; ii++) {
    for (x = 0, i = 0; i < 12; i++) {
      x += uniform(0.0f, 1.0f, seed);
    }
    x = x - 6.0f;
    y(ii) = mean + x * sigma;
  }
}
//! 满足(mean, sigma^2)高斯分布的随机数行向量产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \param y ： 随机数行向量，输出
  \param N ： 随机数行向量长度，输入
  \return 无
*/
inline void gauss(float mean, float sigma, long int *seed, frowvec &y, int N) {
  int ii = 0;
  int i;
  float x;
  y.resize(N);
  for (ii = 0; ii < N; ii++) {
    for (x = 0, i = 0; i < 12; i++) {
      x += uniform(0.0f, 1.0f, seed);
    }
    x = x - 6.0f;
    y(ii) = mean + x * sigma;
  }
}
//! 满足(mean, sigma^2)高斯分布的随机数矩阵产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \param y ： 随机数矩阵，输出
  \param M ： 随机数矩阵行数，输入
  \param N ： 随机数矩阵列数，输入
  \return 无
*/
inline void gauss(float mean, float sigma, long int *seed, fmat &y, int M, int N) {
  int ii = 0;
  int jj = 0;
  int i;
  float x;
  y.resize(M, N);
  for (ii = 0; ii < M; ii++) {
    for (jj = 0; jj < N; jj++) {
      for (x = 0, i = 0; i < 12; i++) {
        x += uniform(0.0f, 1.0f, seed);
      }
      x = x - 6.0f;
      y(ii, jj) = mean + x * sigma;
    }
  }
}
//! 满足(mean, sigma^2)高斯分布的随机数复数列向量产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \return 无
*/
inline cpxf gauss_cpx(float mean, float sigma, long int *seed) {
  int i;
  float x;
  cpxf tmp = 0.0f;
  for (x = 0, i = 0; i < 12; i++) {
    x += uniform(0.0f, 1.0f, seed);
  }
  x = x - 6.0f;
  tmp.real(mean + x * sigma);
  for (x = 0, i = 0; i < 12; i++) {
    x += uniform(0.0f, 1.0f, seed);
  }
  x = x - 6.0f;
  tmp.imag(mean + x * sigma);
  return tmp;
}
//! 满足(mean, sigma^2)高斯分布的随机数复数行向量产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \param y ： 随机数复数行向量，输出
  \param N ： 随机数复数行向量长度，输入
  \return 无
*/
inline void gauss_cpx(float mean, float sigma, long int *seed, cfvec &y, int N) {
  int ii = 0;
  int i;
  float x;
  y.resize(N);
  for (ii = 0; ii < N; ii++) {
    for (x = 0, i = 0; i < 12; i++) {
      x += uniform(0.0f, 1.0f, seed);
    }
    x = x - 6.0f;
    y(ii).real(mean + x * sigma);
    for (x = 0, i = 0; i < 12; i++) {
      x += uniform(0.0f, 1.0f, seed);
    }
    x = x - 6.0f;
    y(ii).imag(mean + x * sigma);
  }
}
//! 满足(mean, sigma^2)高斯分布的随机数复数矩阵产生函数
/*!
  \param mean ： float型，均值
  \param sigma ： float型，标准差
  \param seed ： 随机数种子，输入
  \param y ： 随机数复数矩阵，输出
  \param M ： 随机数复数矩阵行数，输入
  \param N ： 随机数复数矩阵列数，输入
  \return 无
*/
inline void gauss_cpx(float mean, float sigma, long int *seed, cfmat &y, int M, int N) {
  int ii = 0;
  int jj = 0;
  int i;
  float x;
  y.resize(M, N);
  for (ii = 0; ii < M; ii++) {
    for (jj = 0; jj < N; jj++) {
      for (x = 0, i = 0; i < 12; i++) {
        x += uniform(0.0f, 1.0f, seed);
      }
      x = x - 6.0f;
      y(ii, jj).real(mean + x * sigma);
      for (x = 0, i = 0; i < 12; i++) {
        x += uniform(0.0f, 1.0f, seed);
      }
      x = x - 6.0f;
      y(ii, jj).imag(mean + x * sigma);
    }
  }
}

//! 产生随机二进制列向量函数
/*!
  \param a ： 二进制随机数列向量，输出
  \param N ： 二进制随机数列向量长度，输入
  \return 无
*/
inline void random_bin_vec(bvec &a, int N)  //产生随机0/1
{
  int i = 0;
  a.resize(N);
  for (i = 0; i < N; i++) {
    a(i) = rand() & 1;
  }
}
//! 产生随机二进制行向量函数
/*!
  \param a ： 二进制随机数行向量，输出
  \param N ： 二进制随机数行向量长度，输入
  \return 无
*/
inline void random_bin_vec(browvec &a, int N) {
  int i = 0;
  a.resize(N);
  for (i = 0; i < N; i++) {
    a(i) = rand() & 1;
  }
}
//! 产生随机二进制矩阵函数
/*!
  \param a ： 二进制随机数矩阵，输出
  \param M ： 二进制随机数矩阵行数，输入
  \param N ： 二进制随机数矩阵列数，输入
  \return 无
*/
inline void random_bin_mat(bmat &a, int M, int N) {
  int i = 0;
  int j = 0;
  a.resize(M, N);
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      a(i, j) = rand() & 1;
    }
  }
}

//! order n 1st Bessel Func, Jn(x)
/*!
  \param n ：Order，输入
  \param x ：input，输入
  \return Jn(x)
*/
inline float Besselj(int n, float x) {
  int i, m;
  float t, y, z, p, q, s, b0, b1;
  static float a[6] = {57568490574.0, -13362590354.0,
                       651619640.7, -11214424.18, 77392.33017,
                       -184.9052456};
  static float b[6] = {57568490411.0, 1029532985.0,
                       9494680.718, 59272.64853, 267.8532712, 1.0};
  static float c[6] = {72362614232.0, -7895059235.0,
                       242396853.1, -2972611.439, 15704.4826,
                       -30.16036606};
  static float d[6] = {144725228443.0, 2300535178.0,
                       18583304.74, 99447.43394, 376.9991397, 1.0};
  static float e[5] = {1.0, -0.1098628627e-02,
                       0.2734510407e-04, -0.2073370639e-05,
                       0.2093887211e-06};
  static float f[5] = {-0.1562499995e-01,
                       0.1430488765e-03, -0.6911147651e-05,
                       0.7621095161e-06, -0.934935152e-07};
  static float g[5] = {1.0, 0.183105e-02,
                       -0.3516396496e-04, 0.2457520174e-05,
                       -0.240337019e-06};
  static float h[5] = {0.4687499995e-01,
                       -0.2002690873e-03, 0.8449199096e-05,
                       -0.88228987e-06, 0.105787412e-06};
  p = 0.0f;
  t = fabs(x);
  if (n < 0) {
    n = -n;
  }
  if (n != 1) {
    if (t < 8.0) {
      y = t * t;
      p = a[5];
      q = b[5];
      for (i = 4; i >= 0; i--) {
        p = p * y + a[i];
        q = q * y + b[i];
      }
      p = p / q;
    } else {
      z = 8.0 / t;
      y = z * z;
      p = e[4];
      q = f[4];
      for (i = 3; i >= 0; i--) {
        p = p * y + e[i];
        q = q * y + f[i];
      }
      s = t - 0.785398164;
      p = p * cos(s) - z * q * sin(s);
      p = p * sqrt(0.636619772 / t);
    }
  }
  if (n == 0) {
    return (p);
  }
  b0 = p;
  if (t < 8.0) {
    y = t * t;
    p = c[5];
    q = d[5];
    for (i = 4; i >= 0; i--) {
      p = p * y + c[i];
      q = q * y + d[i];
    }
    p = x * p / q;
  } else {
    z = 8.0 / t;
    y = z * z;
    p = g[4];
    q = h[4];
    for (i = 3; i >= 0; i--) {
      p = p * y + g[i];
      q = q * y + h[i];
    }
    s = t - 2.356194491;
    p = p * cos(s) - z * q * sin(s);
    p = p * x * sqrt(0.636619772 / t) / t;
  }
  if (n == 1) {
    return (p);
  }
  b1 = p;
  if (x == 0.0) {
    return (0.0);
  }
  s = 2.0 / t;
  if (t > 1.0 * n) {
    if (x < 0.0) {
      b1 = -b1;
    }
    for (i = 1; i <= n - 1; i++) {
      p = s * i * b1 - b0;
      b0 = b1;
      b1 = p;
    }
  } else {
    m = (n + (int)sqrt(40.0 * n)) / 2;
    m = 2 * m;
    p = 0.0;
    q = 0.0;
    b0 = 1.0;
    b1 = 0.0;
    for (i = m - 1; i >= 0; i--) {
      t = s * (i + 1) * b0 - b1;
      b1 = b0;
      b0 = t;
      if (fabs(b0) > 1.0e+10) {
        b0 = b0 * 1.0e-10;
        b1 = b1 * 1.0e-10;
        p = p * 1.0e-10;
        q = q * 1.0e-10;
      }
      if ((i + 2) % 2 == 0) {
        q = q + b0;
      }
      if ((i + 1) == n) {
        p = b1;
      }
    }
    q = 2.0 * q - b0;
    p = p / q;
  }
  if ((x < 0.0) && (n % 2 == 1)) {
    p = -p;
  }
  return (p);
}
#endif
