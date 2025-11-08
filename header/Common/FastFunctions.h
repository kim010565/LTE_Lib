/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file FastFunctions.h
    \brief FastFunctions头文件
    内容 ：一些快速计算函数定义
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _FastFunctions_h_
#define _FastFunctions_h_

#include "Definitions.h"
#include "Typedefs.h"

//! 自定义联合体MyFabs_U
union MyFabs_U {
  float fnum;     //!< float型成员fnum
  uint8 bnum[4];  //!< uint8型成员数组bnum[4]
};
//! 快速计算float型绝对值，仅正对小端模式
/*!
  \param x ： 小数，输入
  \return 小数，绝对值
*/
inline float MyFabs(float x)  //快速取float abs, only for litter enddian
{
  MyFabs_U tmp;
  tmp.fnum = x;
  tmp.bnum[3] &= 0x7f;
  return tmp.fnum;
}
//! 快速计算float型列向量的绝对值，仅正对小端模式
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MyFabs(fvec &x) {
  int i = 0;
  MyFabs_U tmp;
  for (i = 0; i < (int)x.size(); i++) {
    tmp.fnum = x(i);
    tmp.bnum[3] &= 0x7f;
    x(i) = tmp.fnum;
  }
}
//! 快速计算float型行向量的绝对值，仅正对小端模式
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MyFabs(frowvec &x) {
  int i = 0;
  MyFabs_U tmp;
  for (i = 0; i < (int)x.size(); i++) {
    tmp.fnum = x(i);
    tmp.bnum[3] &= 0x7f;
    x(i) = tmp.fnum;
  }
}
//! 快速计算float型矩阵的绝对值，仅正对小端模式
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MyFabs(fmat &x) {
  int i = 0;
  int j = 0;
  MyFabs_U tmp;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      tmp.fnum = x(i, j);
      tmp.bnum[3] &= 0x7f;
      x(i, j) = tmp.fnum;
    }
  }
}
//! 快速计算float型列向量的绝对值，仅正对小端模式
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MyFabs(const fvec &x, fvec &y) {
  int i = 0;
  MyFabs_U tmp;
  y.resize(x.size());
  for (i = 0; i < (int)x.size(); i++) {
    tmp.fnum = x(i);
    tmp.bnum[3] &= 0x7f;
    y(i) = tmp.fnum;
  }
}
//! 快速计算float型行向量的绝对值，仅正对小端模式
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MyFabs(const frowvec &x, frowvec &y) {
  int i = 0;
  MyFabs_U tmp;
  y.resize(x.size());
  for (i = 0; i < (int)x.size(); i++) {
    tmp.fnum = x(i);
    tmp.bnum[3] &= 0x7f;
    y(i) = tmp.fnum;
  }
}
//! 快速计算float型矩阵的绝对值，仅正对小端模式
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MyFabs(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  MyFabs_U tmp;
  y.resize(x.rows(), x.cols());
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      tmp.fnum = x(i, j);
      tmp.bnum[3] &= 0x7f;
      y(i, j) = tmp.fnum;
    }
  }
}

//! 快速计算Sin(x)函数
/*!
  \param x ： 小数，输入
  \return Sin(x)
*/
inline float MySin(float x)  //快速sin(x)计算
{
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  x = fmodf(x, 2.0f * PI);
  if (x < -PI) {
    x += 2.0f * PI;
  } else if (x > PI) {
    x -= 2.0f * PI;
  }
  float y = x * (B + C * MyFabs(x));
  y = P * y * (MyFabs(y) + Q);
  return y;
}
//! 快速计算float型列向量的Sin(x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MySin(fvec &x) {
  int i = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i), 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    x(i) = x_tmp * (B + C * MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型行向量的Sin(x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MySin(frowvec &x) {
  int i = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i), 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    x(i) = x_tmp * (B + C * MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型矩阵的Sin(x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MySin(fmat &x) {
  int i = 0;
  int j = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = fmodf(x(i, j), 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      x(i, j) = x_tmp * (B + C * MyFabs(x_tmp));
      x(i, j) = P * x(i, j) * (MyFabs(x(i, j)) + Q);
    }
  }
}
//! 快速计算float型列向量的Sin(x)值
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MySin(const fvec &x, fvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i), 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i) = x_tmp * (B + C * MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型行向量的Sin(x)值
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MySin(const frowvec &x, frowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i), 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i) = x_tmp * (B + C * MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型矩阵的Sin(x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MySin(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = fmodf(x(i, j), 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      y(i, j) = x_tmp * (B + C * MyFabs(x_tmp));
      y(i, j) = P * y(i, j) * (MyFabs(y(i, j)) + Q);
    }
  }
}

//! 快速计算Cos(x)函数
/*!
  \param x ： 小数，输入
  \return Cos(x)
*/
inline float MyCos(float x)  //快速cos(x)计算
{
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  x = fmodf(x + PI / 2.0f, 2.0f * PI);
  if (x < -PI) {
    x += 2.0f * PI;
  } else if (x > PI) {
    x -= 2.0f * PI;
  }
  float y = x * (B + C * MyFabs(x));
  y = P * y * (MyFabs(y) + Q);
  return y;
}
//! 快速计算float型列向量的Cos(x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MyCos(fvec &x) {
  int i = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    x(i) = x_tmp * (B + C * MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型行向量的Cos(x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MyCos(frowvec &x) {
  int i = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    x(i) = x_tmp * (B + C * MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型矩阵的Cos(x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MyCos(fmat &x) {
  int i = 0;
  int j = 0;
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = fmodf(x(i, j) + PI / 2.0f, 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      x(i, j) = x_tmp * (B + C * MyFabs(x_tmp));
      x(i, j) = P * x(i, j) * (MyFabs(x(i, j)) + Q);
    }
  }
}
//! 快速计算float型列向量的Cos(x)值
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MyCos(const fvec &x, fvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i) = x_tmp * (B + C * MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型行向量的Cos(x)值
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MyCos(const frowvec &x, frowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i) = x_tmp * (B + C * MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型矩阵的Cos(x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MyCos(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = fmodf(x(i, j) + PI / 2.0f, 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      y(i, j) = x_tmp * (B + C * MyFabs(x_tmp));
      y(i, j) = P * y(i, j) * (MyFabs(y(i, j)) + Q);
    }
  }
}

//! 快速计算Sin(2*pi*x)函数
/*!
  \param x ： 小数，输入
  \return Sin(2*pi*x)
*/
inline float MySin_2PI(float x)  //快速sin(2*pi*x)计算
{
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float y = 0.0f;
  x = x - floor(x);
  if (x < -0.5f) {
    x += 1.0f;
  } else if (x > 0.5f) {
    x -= 1.0f;
  }
  y = x * (0.5f - MyFabs(x));
  y = P * y * (MyFabs(y) + Q);
  return y;
}
//! 快速计算float型列向量的Sin(2*pi*x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MySin_2PI(fvec &x) {
  int i = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) - floor(x(i));
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    x(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型行向量的Sin(2*pi*x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MySin_2PI(frowvec &x) {
  int i = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) - floor(x(i));
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    x(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型矩阵的Sin(2*pi*x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MySin_2PI(fmat &x) {
  int i = 0;
  int j = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = x(i, j) - floor(x(i, j));
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      x(i, j) = x_tmp * (0.5f - MyFabs(x_tmp));
      x(i, j) = P * x(i, j) * (MyFabs(x(i, j)) + Q);
    }
  }
}
//! 快速计算float型列向量的Sin(2*pi*x)值
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MySin_2PI(const fvec &x, fvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) - floor(x(i));
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型行向量的Sin(2*pi*x)值
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MySin_2PI(const frowvec &x, frowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) - floor(x(i));
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型矩阵的Sin(2*pi*x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MySin_2PI(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = x(i, j) - floor(x(i, j));
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      y(i, j) = x_tmp * (0.5f - MyFabs(x_tmp));
      y(i, j) = P * y(i, j) * (MyFabs(y(i, j)) + Q);
    }
  }
}

//! 快速计算Cos(2*pi*x)函数
/*!
  \param x ： 小数，输入
  \return Cos(2*pi*x)
*/
inline float MyCos_2PI(float x)  //快速cos(2*pi*x)计算
{
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float y = 0.0f;
  x = x + 0.25f;
  x = x - floor(x);
  if (x < -0.5f) {
    x += 1.0f;
  } else if (x > 0.5f) {
    x -= 1.0f;
  }
  y = x * (0.5f - MyFabs(x));
  y = P * y * (MyFabs(y) + Q);
  return y;
}
//! 快速计算float型列向量的Cos(2*pi*x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MyCos_2PI(fvec &x) {
  int i = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    x(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型行向量的Cos(2*pi*x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MyCos_2PI(frowvec &x) {
  int i = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    x(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    x(i) = P * x(i) * (MyFabs(x(i)) + Q);
  }
}
//! 快速计算float型矩阵的Cos(2*pi*x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MyCos_2PI(fmat &x) {
  int i = 0;
  int j = 0;
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = x(i, j) + 0.25f;
      x_tmp = x_tmp - floor(x_tmp);
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      x(i, j) = x_tmp * (0.5f - MyFabs(x_tmp));
      x(i, j) = P * x(i, j) * (MyFabs(x(i, j)) + Q);
    }
  }
}
//! 快速计算float型列向量的Cos(2*pi*x)值
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MyCos_2PI(const fvec &x, fvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型行向量的Cos(2*pi*x)值
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MyCos_2PI(const frowvec &x, frowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.size(); i++) {
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i) = x_tmp * (0.5f - MyFabs(x_tmp));
    y(i) = P * y(i) * (MyFabs(y(i)) + Q);
  }
}
//! 快速计算float型矩阵的Cos(2*pi*x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MyCos_2PI(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x_tmp = x(i, j) + 0.25f;
      x_tmp = x_tmp - floor(x_tmp);
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      y(i, j) = x_tmp * (0.5f - MyFabs(x_tmp));
      y(i, j) = P * y(i, j) * (MyFabs(y(i, j)) + Q);
    }
  }
}

//! 快速计算Exp(j*x)函数
/*!
  \param x ： 小数，输入
  \return 复数Exp(j*x)
*/
inline cpxf MyExpCpx(float x)  //快速exp(j*x)计算
{
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  cpxf y;
  float x1;
  x1 = x;
  x = fmodf(x + PI / 2.0f, 2.0f * PI);
  if (x < -PI) {
    x += 2.0f * PI;
  } else if (x > PI) {
    x -= 2.0f * PI;
  }
  y.real(x * (B + C * MyFabs(x)));
  y.real(P * y.real() * (MyFabs(y.real()) + Q));
  x1 = fmodf(x1, 2.0f * PI);
  if (x1 < -PI) {
    x1 += 2.0f * PI;
  } else if (x1 > PI) {
    x1 -= 2.0f * PI;
  }
  y.imag(x1 * (B + C * MyFabs(x1)));
  y.imag(P * y.imag() * (MyFabs(y.imag()) + Q));
  return y;
}
//! 快速计算float型列向量的Exp(j*x)值
/*!
  \param x ： float型列向量，输入
  \param y ： 复数float型列向量，输出
  \return 无
*/
inline void MyExpCpx(const fvec &x, cfvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.size(); i++) {
    x1 = x(i);
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i).real(x_tmp * (B + C * MyFabs(x_tmp)));
    y(i).real(P * y(i).real() * (MyFabs(y(i).real()) + Q));
    x_tmp = fmodf(x1, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i).imag(x_tmp * (B + C * MyFabs(x_tmp)));
    y(i).imag(P * y(i).imag() * (MyFabs(y(i).imag()) + Q));
  }
}
//! 快速计算float型行向量的Exp(j*x)值
/*!
  \param x ： float型行向量，输入
  \param y ： 复数float型行向量，输出
  \return 无
*/
inline void MyExpCpx(const frowvec &x, cfrowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.size(); i++) {
    x1 = x(i);
    x_tmp = fmodf(x(i) + PI / 2.0f, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i).real(x_tmp * (B + C * MyFabs(x_tmp)));
    y(i).real(P * y(i).real() * (MyFabs(y(i).real()) + Q));
    x_tmp = fmodf(x1, 2.0f * PI);
    if (x_tmp < -PI) {
      x_tmp += 2.0f * PI;
    } else if (x_tmp > PI) {
      x_tmp -= 2.0f * PI;
    }
    y(i).imag(x_tmp * (B + C * MyFabs(x_tmp)));
    y(i).imag(P * y(i).imag() * (MyFabs(y(i).imag()) + Q));
  }
}
//! 快速计算float型矩阵的Exp(j*x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： 复数float型矩阵，输出
  \return 无
*/
inline void MyExpCpx(const fmat &x, cfmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float B = 4.0f / PI;
  const float C = -4.0f / PI / PI;
  const float P = 0.225f;
  const float Q = (1.0f - P) / P;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x1 = x(i, j);
      x_tmp = fmodf(x(i, j) + PI / 2.0f, 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      y(i, j).real(x_tmp * (B + C * MyFabs(x_tmp)));
      y(i, j).real(P * y(i, j).real() * (MyFabs(y(i, j).real()) + Q));
      x_tmp = fmodf(x1, 2.0f * PI);
      if (x_tmp < -PI) {
        x_tmp += 2.0f * PI;
      } else if (x_tmp > PI) {
        x_tmp -= 2.0f * PI;
      }
      y(i, j).imag(x_tmp * (B + C * MyFabs(x_tmp)));
      y(i, j).imag(P * y(i, j).imag() * (MyFabs(y(i, j).imag()) + Q));
    }
  }
}

//! 快速计算Exp(j*2*pi*x)函数
/*!
  \param x ： 小数，输入
  \return 复数Exp(j*2*pi*x)
*/
inline cpxf MyExpCpx_2PI(float x)  //快速exp(j*2*PI*x)计算
{
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  cpxf y;
  float x1;
  x1 = x;
  x = x + 0.25f;
  x = x - floor(x);
  if (x < -0.5f) {
    x += 1.0f;
  } else if (x > 0.5f) {
    x -= 1.0f;
  }
  y.real(x * (0.5f - MyFabs(x)));
  y.real(P * y.real() * (MyFabs(y.real()) + Q));
  x1 = x1 - floor(x1);
  if (x1 < -0.5f) {
    x1 += 1.0f;
  } else if (x1 > 0.5f) {
    x1 -= 1.0f;
  }
  y.imag(x1 * (0.5f - MyFabs(x1)));
  y.imag(P * y.imag() * (MyFabs(y.imag()) + Q));
  return y;
}
//! 快速计算float型列向量的Exp(j*2*pi*x)值
/*!
  \param x ： float型列向量，输入
  \param y ： 复数float型列向量，输出
  \return 无
*/
inline void MyExpCpx_2PI(const fvec &x, cfvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.size(); i++) {
    x1 = x(i);
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i).real(x_tmp * (0.5f - MyFabs(x_tmp)));
    y(i).real(P * y(i).real() * (MyFabs(y(i).real()) + Q));
    x_tmp = x1 - floor(x1);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i).imag(x_tmp * (0.5f - MyFabs(x_tmp)));
    y(i).imag(P * y(i).imag() * (MyFabs(y(i).imag()) + Q));
  }
}
//! 快速计算float型行向量的Exp(j*2*pi*x)值
/*!
  \param x ： float型行向量，输入
  \param y ： 复数float型行向量，输出
  \return 无
*/
inline void MyExpCpx_2PI(const frowvec &x, cfrowvec &y) {
  int i = 0;
  y.resize(x.size());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.size(); i++) {
    x1 = x(i);
    x_tmp = x(i) + 0.25f;
    x_tmp = x_tmp - floor(x_tmp);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i).real(x_tmp * (0.5f - MyFabs(x_tmp)));
    y(i).real(P * y(i).real() * (MyFabs(y(i).real()) + Q));
    x_tmp = x1 - floor(x1);
    if (x_tmp < -0.5f) {
      x_tmp += 1.0f;
    } else if (x_tmp > 0.5f) {
      x_tmp -= 1.0f;
    }
    y(i).imag(x_tmp * (0.5f - MyFabs(x_tmp)));
    y(i).imag(P * y(i).imag() * (MyFabs(y(i).imag()) + Q));
  }
}
//! 快速计算float型矩阵的Exp(j*2*pi*x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： 复数float型矩阵，输出
  \return 无
*/
inline void MyExpCpx_2PI(const fmat &x, cfmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  const float P = 0.225f * 16.0f * 16.0f;
  const float Q = (1.0f - 0.225f) / 0.225f / 16.0f;
  float x_tmp = 0.0f;
  float x1;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x1 = x(i, j);
      x_tmp = x(i, j) + 0.25f;
      x_tmp = x_tmp - floor(x_tmp);
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      y(i, j).real(x_tmp * (0.5f - MyFabs(x_tmp)));
      y(i, j).real(P * y(i, j).real() * (MyFabs(y(i, j).real()) + Q));
      x_tmp = x1 - floor(x1);
      if (x_tmp < -0.5f) {
        x_tmp += 1.0f;
      } else if (x_tmp > 0.5f) {
        x_tmp -= 1.0f;
      }
      y(i, j).imag(x_tmp * (0.5f - MyFabs(x_tmp)));
      y(i, j).imag(P * y(i, j).imag() * (MyFabs(y(i, j).imag()) + Q));
    }
  }
}

//! 快速计算Exp(a)函数。原理 ：exp(a) = 2^(a/ln(2))，并利用IEEE 754标准
/*!
  \param a ： 小数，输入
  \return Exp(a)
*/
inline float MyExp(float a)  //快速exp(x)计算, exp(x) = 2^(x/ln(2)) && use IEEE 754
{
  double x = (double)a;
  double y = 0;
  if (x < -700.0) {
    y = 0;
  } else {
    *(int *)&y = 0;
    *(((int *)&y) + 1) = (int)(1512775 * x + 1072632447);
  }
  return (float)y;
}
//! 快速计算float型列向量的Exp(a)值
/*!
  \param a ： float型列向量，输入/输出
  \return 无
*/
inline void MyExp(fvec &a) {
  int i = 0;
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.size(); i++) {
    x_tmp = (double)a(i);
    if (x_tmp < -700.0) {
      y_tmp = 0;
    } else {
      *(int *)&y_tmp = 0;
      *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
    }
    a(i) = (float)y_tmp;
  }
}
//! 快速计算float型行向量的Exp(a)值
/*!
  \param a ： float型行向量，输入/输出
  \return 无
*/
inline void MyExp(frowvec &a) {
  int i = 0;
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.size(); i++) {
    x_tmp = (double)a(i);
    if (x_tmp < -700.0) {
      y_tmp = 0;
    } else {
      *(int *)&y_tmp = 0;
      *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
    }
    a(i) = (float)y_tmp;
  }
}
//! 快速计算float型矩阵的Exp(a)值
/*!
  \param a ： float型矩阵，输入/输出
  \return 无
*/
inline void MyExp(fmat &a) {
  int i = 0;
  int j = 0;
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.rows(); i++) {
    for (j = 0; j < (int)a.cols(); j++) {
      x_tmp = (double)a(i, j);
      if (x_tmp < -700.0) {
        y_tmp = 0;
      } else {
        *(int *)&y_tmp = 0;
        *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
      }
      a(i, j) = (float)y_tmp;
    }
  }
}
//! 快速计算float型列向量的Exp(a)值
/*!
  \param a ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MyExp(const fvec &a, fvec &y) {
  int i = 0;
  y.resize(a.size());
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.size(); i++) {
    x_tmp = (double)a(i);
    if (x_tmp < -700.0) {
      y_tmp = 0;
    } else {
      *(int *)&y_tmp = 0;
      *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
    }
    y(i) = (float)y_tmp;
  }
}
//! 快速计算float型行向量的Exp(a)值
/*!
  \param a ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MyExp(const frowvec &a, frowvec &y) {
  int i = 0;
  y.resize(a.size());
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.size(); i++) {
    x_tmp = (double)a(i);
    if (x_tmp < -700.0) {
      y_tmp = 0;
    } else {
      *(int *)&y_tmp = 0;
      *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
    }
    y(i) = (float)y_tmp;
  }
}
//! 快速计算float型矩阵的Exp(a)值
/*!
  \param a ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MyExp(const fmat &a, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(a.rows(), a.cols());
  double x_tmp = 0.0;
  double y_tmp = 0.0;
  for (i = 0; i < (int)a.rows(); i++) {
    for (j = 0; j < (int)a.cols(); j++) {
      x_tmp = (double)a(i, j);
      if (x_tmp < -700.0) {
        y_tmp = 0;
      } else {
        *(int *)&y_tmp = 0;
        *(((int *)&y_tmp) + 1) = (int)(1512775 * x_tmp + 1072632447);
      }
      y(i, j) = (float)y_tmp;
    }
  }
}

//! 快速计算Sqrt(number)函数。出自 ：卡马克数，游戏雷神之锤3
/*!
  \param number ： 小数，输入
  \return Sqrt(number)
*/
inline float MySqrt(float number)  //快速sqrt(x)计算， 卡马克数
{
  long i;
  float x, y;
  const float f = 1.5f;
  float b = 0.0f;
  x = number * 0.5f;
  y = number;
  i = *(long *)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (f - (x * y * y));
  y = y * (f - (x * y * y));
  b = number * y;
  return b;
}
//! 快速计算float型列向量的Sqrt(number)值
/*!
  \param number ： float型列向量，输入/输出
  \return 无
*/
inline void MySqrt(fvec &number) {
  int ii = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  for (ii = 0; ii < (int)number.size(); ii++) {
    x = number(ii) * 0.5f;
    y = number(ii);
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (f - (x * y * y));
    y = y * (f - (x * y * y));
    number(ii) = number(ii) * y;
  }
}
//! 快速计算float型行向量的Sqrt(number)值
/*!
  \param number ： float型行向量，输入/输出
  \return 无
*/
inline void MySqrt(frowvec &number) {
  int ii = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  for (ii = 0; ii < (int)number.size(); ii++) {
    x = number(ii) * 0.5f;
    y = number(ii);
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (f - (x * y * y));
    y = y * (f - (x * y * y));
    number(ii) = number(ii) * y;
  }
}
//! 快速计算float型矩阵的Sqrt(number)值
/*!
  \param number ： float型矩阵，输入/输出
  \return 无
*/
inline void MySqrt(fmat &number) {
  int ii = 0;
  int jj = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  for (ii = 0; ii < (int)number.rows(); ii++) {
    for (jj = 0; jj < (int)number.cols(); jj++) {
      x = number(ii, jj) * 0.5f;
      y = number(ii, jj);
      i = *(long *)&y;
      i = 0x5f3759df - (i >> 1);
      y = *(float *)&i;
      y = y * (f - (x * y * y));
      y = y * (f - (x * y * y));
      number(ii, jj) = number(ii, jj) * y;
    }
  }
}
//! 快速计算float型列向量的Sqrt(number)值
/*!
  \param number ： float型列向量，输入
  \param b ： float型列向量，输出
  \return 无
*/
inline void MySqrt(const fvec &number, fvec &b) {
  int ii = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  b.resize(number.size());
  for (ii = 0; ii < (int)number.size(); ii++) {
    x = number(ii) * 0.5f;
    y = number(ii);
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (f - (x * y * y));
    y = y * (f - (x * y * y));
    b(ii) = number(ii) * y;
  }
}
//! 快速计算float型行向量的Sqrt(number)值
/*!
  \param number ： float型行向量，输入
  \param b ： float型行向量，输出
  \return 无
*/
inline void MySqrt(const frowvec &number, frowvec &b) {
  int ii = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  b.resize(number.size());
  for (ii = 0; ii < (int)number.size(); ii++) {
    x = number(ii) * 0.5f;
    y = number(ii);
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (f - (x * y * y));
    y = y * (f - (x * y * y));
    b(ii) = number(ii) * y;
  }
}
//! 快速计算float型矩阵的Sqrt(number)值
/*!
  \param number ： float型矩阵，输入
  \param b ： float型矩阵，输出
  \return 无
*/
inline void MySqrt(const fmat &number, fmat &b) {
  int ii = 0;
  int jj = 0;
  long i;
  float x, y;
  const float f = 1.5f;
  b.resize(number.rows(), number.cols());
  for (ii = 0; ii < (int)number.rows(); ii++) {
    for (jj = 0; jj < (int)number.cols(); jj++) {
      x = number(ii, jj) * 0.5f;
      y = number(ii, jj);
      i = *(long *)&y;
      i = 0x5f3759df - (i >> 1);
      y = *(float *)&i;
      y = y * (f - (x * y * y));
      y = y * (f - (x * y * y));
      b(ii, jj) = number(ii, jj) * y;
    }
  }
}

//! 快速计算1/Sqrt(x)函数。出自 ：卡马克数，游戏雷神之锤3
/*!
  \param x ： 小数，输入
  \return 1/Sqrt(x)
*/
inline float MySqrt_Recip(float x)  //快速1/sqrt(x)计算， 卡马克数
{
  float b = 0.0f;
  float xhalf = 0.5f * x;
  int i = *(int *)&x;
  i = 0x5f3759df - (i >> 1);
  x = *(float *)&i;
  b = x * (1.5f - xhalf * x * x);
  return b;
}
//! 快速计算float型列向量的1/Sqrt(x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MySqrt_Recip(fvec &x) {
  int ii = 0;
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.size(); ii++) {
    xhalf = 0.5f * x(ii);
    i = *(int *)&x(ii);
    i = 0x5f3759df - (i >> 1);
    x_tmp = *(float *)&i;
    x(ii) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
  }
}
//! 快速计算float型行向量的1/Sqrt(x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MySqrt_Recip(frowvec &x) {
  int ii = 0;
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.size(); ii++) {
    xhalf = 0.5f * x(ii);
    i = *(int *)&x(ii);
    i = 0x5f3759df - (i >> 1);
    x_tmp = *(float *)&i;
    x(ii) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
  }
}
//! 快速计算float型矩阵的1/Sqrt(x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MySqrt_Recip(fmat &x) {
  int ii = 0;
  int jj = 0;
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.rows(); ii++) {
    for (jj = 0; jj < (int)x.cols(); jj++) {
      xhalf = 0.5f * x(ii, jj);
      i = *(int *)&x(ii, jj);
      i = 0x5f3759df - (i >> 1);
      x_tmp = *(float *)&i;
      x(ii, jj) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
    }
  }
}
//! 快速计算float型列向量的1/Sqrt(x)值
/*!
  \param x ： float型列向量，输入
  \param b ： float型列向量，输出
  \return 无
*/
inline void MySqrt_Recip(const fvec &x, fvec &b) {
  int ii = 0;
  b.resize(x.size());
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.size(); ii++) {
    xhalf = 0.5f * x(ii);
    i = *(int *)&x(ii);
    i = 0x5f3759df - (i >> 1);
    x_tmp = *(float *)&i;
    b(ii) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
  }
}
//! 快速计算float型行向量的1/Sqrt(x)值
/*!
  \param x ： float型行向量，输入
  \param b ： float型行向量，输出
  \return 无
*/
inline void MySqrt_Recip(const frowvec &x, frowvec &b) {
  int ii = 0;
  b.resize(x.size());
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.size(); ii++) {
    xhalf = 0.5f * x(ii);
    i = *(int *)&x(ii);
    i = 0x5f3759df - (i >> 1);
    x_tmp = *(float *)&i;
    b(ii) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
  }
}
//! 快速计算float型矩阵的1/Sqrt(x)值
/*!
  \param x ： float型矩阵，输入
  \param b ： float型矩阵，输出
  \return 无
*/
inline void MySqrt_Recip(const fmat &x, fmat &b) {
  int ii = 0;
  int jj = 0;
  b.resize(x.rows(), x.cols());
  float xhalf = 0.0f;
  int i = 0;
  float x_tmp = 0.0f;
  for (ii = 0; ii < (int)x.rows(); ii++) {
    for (jj = 0; jj < (int)x.cols(); jj++) {
      xhalf = 0.5f * x(ii, jj);
      i = *(int *)&x(ii, jj);
      i = 0x5f3759df - (i >> 1);
      x_tmp = *(float *)&i;
      b(ii, jj) = x_tmp * (1.5f - xhalf * x_tmp * x_tmp);
    }
  }
}

//! 快速计算Log(x)函数。出自 ：IEEE论文"Practically Fast Multipe-Precision Evaluation of LOG(x)"
/*!
  \param x ： 小数，输入
  \return Log(x)
*/
inline float MyLog(float x)  //快速ln(x)计算, "Practically Fast Multipe-Precision Evaluation of LOG(x)"
{
  float y = 0.0f;
  y = 6.0f * (x - 1.0f) / (x + 1.0f + 4.0f * MySqrt(x));
  return y;
}
//! 快速计算float型列向量的Log(x)值
/*!
  \param x ： float型列向量，输入/输出
  \return 无
*/
inline void MyLog(fvec &x) {
  int i = 0;
  for (i = 0; i < (int)x.size(); i++) {
    x(i) = 6.0f * (x(i) - 1.0f) / (x(i) + 1.0f + 4.0f * MySqrt(x(i)));
  }
}
//! 快速计算float型行向量的Log(x)值
/*!
  \param x ： float型行向量，输入/输出
  \return 无
*/
inline void MyLog(frowvec &x) {
  int i = 0;
  for (i = 0; i < (int)x.size(); i++) {
    x(i) = 6.0f * (x(i) - 1.0f) / (x(i) + 1.0f + 4.0f * MySqrt(x(i)));
  }
}
//! 快速计算float型矩阵的Log(x)值
/*!
  \param x ： float型矩阵，输入/输出
  \return 无
*/
inline void MyLog(fmat &x) {
  int i = 0;
  int j = 0;
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      x(i, j) = 6.0f * (x(i, j) - 1.0f) / (x(i, j) + 1.0f + 4.0f * MySqrt(x(i, j)));
    }
  }
}
//! 快速计算float型列向量的Log(x)值
/*!
  \param x ： float型列向量，输入
  \param y ： float型列向量，输出
  \return 无
*/
inline void MyLog(const fvec &x, fvec &y) {
  int i = 0;
  y.resize(x.size());
  for (i = 0; i < (int)x.size(); i++) {
    y(i) = 6.0f * (x(i) - 1.0f) / (x(i) + 1.0f + 4.0f * MySqrt(x(i)));
  }
}
//! 快速计算float型行向量的Log(x)值
/*!
  \param x ： float型行向量，输入
  \param y ： float型行向量，输出
  \return 无
*/
inline void MyLog(const frowvec &x, frowvec &y) {
  int i = 0;
  y.resize(x.size());
  for (i = 0; i < (int)x.size(); i++) {
    y(i) = 6.0f * (x(i) - 1.0f) / (x(i) + 1.0f + 4.0f * MySqrt(x(i)));
  }
}
//! 快速计算float型矩阵的Log(x)值
/*!
  \param x ： float型矩阵，输入
  \param y ： float型矩阵，输出
  \return 无
*/
inline void MyLog(const fmat &x, fmat &y) {
  int i = 0;
  int j = 0;
  y.resize(x.rows(), x.cols());
  for (i = 0; i < (int)x.rows(); i++) {
    for (j = 0; j < (int)x.cols(); j++) {
      y(i, j) = 6.0f * (x(i, j) - 1.0f) / (x(i, j) + 1.0f + 4.0f * MySqrt(x(i, j)));
    }
  }
}

//! 快速对输入input的低bit_size比特做倒位序
/*!
  \param input ： 无符号整数，输入
  \param bit_size ： 整数位宽，输入
  \return 比特倒位序输出
*/
inline uint32 Reverse_Bit(uint32 input, int bit_size)  //快速对bit_size位数据倒位序
{
  uint32 a = input;
  a = ((a & 0xAAAAAAAA) >> 1) | ((a & 0x55555555) << 1);
  a = ((a & 0xCCCCCCCC) >> 2) | ((a & 0x33333333) << 2);
  a = ((a & 0xF0F0F0F0) >> 4) | ((a & 0x0F0F0F0F) << 4);
  a = ((a & 0xFF00FF00) >> 8) | ((a & 0x00FF00FF) << 8);
  a = ((a & 0xFFFF0000) >> 16) | ((a & 0x0000FFFF) << 16);
  a = a >> (32 - bit_size);
  return a;
}

//! 快速统计输入x中bit位为1的个数
/*!
  \param x ： 无符号整数
  \return x中bit位为1的个数，无符号整数
*/
inline uint32 BitCount(uint32 x)  //快速统计x中bit位为1的个数
{
  uint32 a = x;
  a = ((a & 0xAAAAAAAA) >> 1) + ((a & 0x55555555));
  a = ((a & 0xCCCCCCCC) >> 2) + ((a & 0x33333333));
  a = ((a & 0xF0F0F0F0) >> 4) + ((a & 0x0F0F0F0F));
  a = ((a & 0xFF00FF00) >> 8) + ((a & 0x00FF00FF));
  a = ((a & 0xFFFF0000) >> 16) + ((a & 0x0000FFFF));
  return a;
}

//! 快速判断输入x是否是2的幂次
/*!
  \param x ： 整数
  \return bool型
*/
inline bool Is_Pow2(int x) {
  return (0 == (x & (x - 1)));
}

//! 快速计算Log2(x)
/*!
  \param x ： 整数
  \return Log2(x)，整数
*/
inline int Log2(uint32 x) {
  int ans = 0;
  if (x & 0xFFFF0000) {
    ans += 16;
    x >>= 16;
  }
  if (x & 0xFF00) {
    ans += 8;
    x >>= 8;
  }
  if (x & 0xF0) {
    ans += 4;
    x >>= 4;
  }
  if (x & 0xC) {
    ans += 2;
    x >>= 2;
  }
  if (x & 0x2) {
    ans += 1;
  }
  return ans;
}

//! 快速计算CeilLog2(x)
/*!
  \param x ： 整数
  \return CeilLog2(x)，整数
*/
inline int CeilLog2(uint32 x) {
  int ans = Log2(x);
  uint32 a = (1 << ans);
  if (x > a) {
    ans++;
  }
  return ans;
}

//! 快速计算第一类零阶贝塞尔函数J0(2*pi*x)。原理 ：matlab曲线拟合得到
/*!
  \param x ： 小数，必须>=0
  \return J0(2*pi*x)
*/
inline float Besselj_2PI(float x)  //J0(2*PI*x), x must >= 0.0
{
  float y = 0.0f;
  if (x < 0.15f) {
    x = 2.0f * PI * x;
    y = (1.0f - x * x / 8.0f) * (1.0f - x * x / 8.0f);
  } else if (x < 0.5f) {
    x = 2.0f * PI * x;
    y = 0.0626f * x * x * x - 0.3470f * x * x + 0.0616f * x + 0.9885f;
  } else {
    x = 2.0f * PI * x;
    y = -0.0287f * x * x * x + 0.5262f * x * x - 2.7641f * x + 4.0787f;
  }
  return y;
}

//! 快速计算第一类零阶贝塞尔反函数J0_INV(x)/(2*pi)。原理 ：matlab曲线拟合得到
/*!
  \param x ： 小数，必须<=1.0
  \return J0_INV(x)/(2*pi)
*/
inline float Inverse_Besselj_2PI(float x)  //J0_INV(x) / (2*PI), x must <=1.0
{
  float y = 0;
  if (x > 0.79) {
    y = (2.2103f * MySqrt(0.9061f - 0.9048f * x) - 0.0271f) / (2.0f * PI);
  } else {
    y = (21.4762f * MySqrt(0.3426f - 0.0931f * x) - 10.1732f) / (2.0f * PI);
  }
  return y;
}

//! 快速冒泡排序，从小到大排列
/*!
  \param a ： float型列向量，输入/输出
  \return 无
*/
inline void BubbleSort(fvec &a)  //从小到大，冒泡排序
{
  int i = 0;
  int j = 0;
  int asize = (int)a.size();
  float temp = 0.0f;
  bool flag = true;  //用于标记此次循环没有交换元素，避免前面序列已经有序，还要继续比较的情况
  for (i = 0; i < asize && flag; i++) {
    flag = false;
    for (j = asize - 2; j >= i; j--) {
      if (a(j + 1) < a(j)) {
        temp = a(j + 1);
        a(j + 1) = a(j);
        a(j) = temp;
        flag = true;
      }
    }
  }
}

//! 快速冒泡排序，从小到大排列，不改变输入a的顺序，只输出排序后的序号idx
/*!
  \param a ： float型列向量，输入
  \param idx ： int型列向量，输出
  \return 无
*/
inline void BubbleSortIdx(const fvec &a, ivec &idx)  //从小到大，冒泡排序，不改变数组a的顺序，只输出序号idx
{
  int i = 0;
  int j = 0;
  int asize = (int)a.size();
  int temp = 0;
  bool flag = true;  //用于标记此次循环没有交换元素，避免前面序列已经有序，还要继续比较的情况
  idx.resize(asize);
  for (i = 0; i < asize && flag; i++) {
    idx(i) = i;
  }
  for (i = 0; i < asize && flag; i++) {
    flag = false;
    for (j = asize - 2; j >= i; j--) {
      if (a(idx(j + 1)) < a(idx(j))) {
        temp = idx(j + 1);
        idx(j + 1) = idx(j);
        idx(j) = temp;
        flag = true;
      }
    }
  }
}

#endif
