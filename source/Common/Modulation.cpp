/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Modulation.cpp
    \brief Modulation实现文件
    内容 ：一些调制函数相关实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Modulation.h"

//! BPSK调制函数
/*!
  \param bits ： 二进制列向量，输入
  \param symbs ： 复数float型列向量，输出
  \return 无返回值
*/
void Modulation_BPSK(const bvec &bits, cfvec &symbs)  //BPSK调制
{
  int i = 0;
  int nbits = bits.size();
  symbs.resize(nbits);
  for (i = 0; i < nbits; i++) {
    symbs(i) = Constellation_BPSK[bits(i)];
  }
}

//! QPSK调制函数
/*!
  \param bits ： 二进制列向量，输入
  \param symbs ： 复数float型列向量，输出
  \return 无返回值
*/
void Modulation_QPSK(const bvec &bits, cfvec &symbs)  //QPSK调制
{
  int i = 0;
  int j = 0;
  int nbits = bits.size();
  symbs.resize(nbits / 2);
  for (i = 0; i < (nbits / 2); i++) {
    j = 2 * i;
    symbs(i) = Constellation_QPSK[(bits(j) << 1) + bits(j + 1)];
  }
}

//! 16QAM调制函数
/*!
  \param bits ： 二进制列向量，输入
  \param symbs ： 复数float型列向量，输出
  \return 无返回值
*/
void Modulation_16QAM(const bvec &bits, cfvec &symbs)  //16QAM调制
{
  int i = 0;
  int j = 0;
  int nbits = bits.size();
  symbs.resize(nbits / 4);
  for (i = 0; i < (nbits / 4); i++) {
    j = 4 * i;
    symbs(i) = Constellation_16QAM[(bits(j) << 3) + (bits(j + 1) << 2) + (bits(j + 2) << 1) + bits(j + 3)];
  }
}

//! 64QAM调制函数
/*!
  \param bits ： 二进制列向量，输入
  \param symbs ： 复数float型列向量，输出
  \return 无返回值
*/
void Modulation_64QAM(const bvec &bits, cfvec &symbs)  //64QAM调制
{
  int i = 0;
  int j = 0;
  int nbits = bits.size();
  symbs.resize(nbits / 6);
  for (i = 0; i < (nbits / 6); i++) {
    j = 6 * i;
    symbs(i) = Constellation_64QAM[(bits(j) << 5) + (bits(j + 1) << 4) + (bits(j + 2) << 3) + (bits(j + 3) << 2) + (bits(j + 4) << 1) + bits(j + 5)];
  }
}

//! 256QAM调制函数
/*!
  \param bits ： 二进制列向量，输入
  \param symbs ： 复数float型列向量，输出
  \return 无返回值
*/
void Modulation_256QAM(const bvec &bits, cfvec &symbs)  //256QAM调制
{
  int i = 0;
  int j = 0;
  int nbits = bits.size();
  symbs.resize(nbits / 8);
  for (i = 0; i < (nbits / 8); i++) {
    j = 8 * i;
    symbs(i) = Constellation_256QAM[(bits(j) << 7) + (bits(j + 1) << 6) + (bits(j + 2) << 5) + (bits(j + 3) << 4) + (bits(j + 4) << 3) + (bits(j + 5) << 2) + (bits(j + 6) << 1) + bits(j + 7)];
  }
}

//! 层映射函数。出自 ：3GPP 36.211 6.3.3节
/*!
  \param d ： 复数float型，层映射输入列向量数组，比如d[cw_idx](i)，输入
  \param v ： 层数，输入
  \param Transmission_Scheme ： 传输方式，输入
  \param x ： 复数float型，层映射输出矩阵，比如x(v, i)，输出
  \return 无返回值
*/
void Layer_Mapping(const cfvecs &d,  //d[cw_idx](i)
                   int v,
                   Transmission_Scheme_E Transmission_Scheme,
                   cfmat &x)  //x(v, i)
{
  int i = 0;
  int j = 0;
  int M_layer_symb = 0;
  int codeword_num = d.size();
  if (Single_Antenna_Port == Transmission_Scheme) {
    M_layer_symb = d[0].size();
    x.resize(1, M_layer_symb);
    x = d[0].transpose();
  } else if (Transmit_Diversity == Transmission_Scheme) {
    if (2 == v) {
      M_layer_symb = d[0].size() / 2;
      x.resize(2, M_layer_symb);
      for (i = 0; i < M_layer_symb; i++) {
        x(0, i) = d[0](2 * i);
        x(1, i) = d[0](2 * i + 1);
      }
    } else {
      if (0 == (d[0].size() & 3)) {
        M_layer_symb = d[0].size() / 4;
      } else {
        M_layer_symb = (d[0].size() + 2) / 4;
      }
      x.resize(4, M_layer_symb);
      for (i = 0; i < M_layer_symb - 1; i++) {
        x(0, i) = d[0](4 * i);
        x(1, i) = d[0](4 * i + 1);
        x(2, i) = d[0](4 * i + 2);
        x(3, i) = d[0](4 * i + 3);
      }
      if (0 == (d[0].size() & 3)) {
        x(0, i) = d[0](4 * i);
        x(1, i) = d[0](4 * i + 1);
        x(2, i) = d[0](4 * i + 2);
        x(3, i) = d[0](4 * i + 3);
      } else {
        x(0, i) = d[0](4 * i);
        x(1, i) = d[0](4 * i + 1);
        x(2, i) = 0;
        x(3, i) = 0;
      }
    }
  } else {
    if (1 == codeword_num) {
      M_layer_symb = d[0].size() / v;
      x.resize(v, M_layer_symb);
      for (i = 0; i < M_layer_symb; i++) {
        for (j = 0; j < v; j++) {
          x(j, i) = d[0](v * i + j);
        }
      }
    } else {
      if (2 == v) {
        M_layer_symb = d[0].size();
        x.resize(2, M_layer_symb);
        x.row(0) = d[0].transpose();
        x.row(1) = d[1].transpose();
      } else if (3 == v) {
        M_layer_symb = d[0].size();
        x.resize(3, M_layer_symb);
        x.row(0) = d[0];
        for (i = 0; i < M_layer_symb; i++) {
          x(1, i) = d[1](2 * i);
          x(2, i) = d[1](2 * i + 1);
        }
      } else if (4 == v) {
        M_layer_symb = d[0].size() / 2;
        x.resize(4, M_layer_symb);
        for (i = 0; i < M_layer_symb; i++) {
          x(0, i) = d[0](2 * i);
          x(1, i) = d[0](2 * i + 1);
          x(2, i) = d[1](2 * i);
          x(3, i) = d[1](2 * i + 1);
        }
      } else if (5 == v) {
        M_layer_symb = d[0].size() / 2;
        x.resize(5, M_layer_symb);
        for (i = 0; i < M_layer_symb; i++) {
          x(0, i) = d[0](2 * i);
          x(1, i) = d[0](2 * i + 1);
          x(2, i) = d[1](3 * i);
          x(3, i) = d[1](3 * i + 1);
          x(4, i) = d[1](3 * i + 2);
        }
      } else if (6 == v) {
        M_layer_symb = d[0].size() / 3;
        x.resize(6, M_layer_symb);
        for (i = 0; i < M_layer_symb; i++) {
          x(0, i) = d[0](3 * i);
          x(1, i) = d[0](3 * i + 1);
          x(2, i) = d[0](3 * i + 2);
          x(3, i) = d[1](3 * i);
          x(4, i) = d[1](3 * i + 1);
          x(5, i) = d[1](3 * i + 2);
        }
      } else if (7 == v) {
        M_layer_symb = d[0].size() / 3;
        x.resize(7, M_layer_symb);
        for (i = 0; i < M_layer_symb; i++) {
          x(0, i) = d[0](3 * i);
          x(1, i) = d[0](3 * i + 1);
          x(2, i) = d[0](3 * i + 2);
          x(3, i) = d[1](4 * i);
          x(4, i) = d[1](4 * i + 1);
          x(5, i) = d[1](4 * i + 2);
          x(6, i) = d[1](4 * i + 3);
        }
      } else {
        M_layer_symb = d[0].size() / 4;
        x.resize(8, M_layer_symb);
        for (i = 0; i < M_layer_symb; i++) {
          x(0, i) = d[0](4 * i);
          x(1, i) = d[0](4 * i + 1);
          x(2, i) = d[0](4 * i + 2);
          x(3, i) = d[0](4 * i + 3);
          x(4, i) = d[1](4 * i);
          x(5, i) = d[1](4 * i + 1);
          x(6, i) = d[1](4 * i + 2);
          x(7, i) = d[1](4 * i + 3);
        }
      }
    }
  }
}

//! 预编码函数。出自 ：3GPP 36.211 6.3.4节
/*!
  \param x ： 复数float型，预编码输入矩阵，比如x(v, i)，输入
  \param P ： 发射天线端口数，输入
  \param M_0_symb ： 层映射后层0的序列长度，输入
  \param Transmission_Scheme ： 传输方式，输入
  \param Codebook_index0 ： 码字0的列向量，输入
  \param Codebook_index1 ： 码字1的列向量，输入
  \param alternativeCodeBookEnabledFor4TX_r12 : bool型，输入
  \param y ： 复数float型，预编码输出矩阵，比如y(p, i)，输入
  \return 无返回值
*/
void Precoding(const cfmat &x,  //x(v, i)
               int P,
               int M_0_symb,
               Transmission_Scheme_E Transmission_Scheme,
               ivec &Codebook_index0,
               ivec &Codebook_index1,
               bool alternativeCodeBookEnabledFor4TX_r12,
               cfmat &y)  //y(p, i)
{
  int v = x.rows();
  int M_layer_symb = x.cols();
  int M_ap_symb = 0;
  int i = 0;
#if 1 == ERROR_REPORT
  if (v > P) {
    Error("Precoding Error: v > P");
  }
#endif
  if (Single_Antenna_Port == Transmission_Scheme || Dual_layer == Transmission_Scheme || Up_to_8_layer == Transmission_Scheme) {
    M_ap_symb = M_layer_symb;
    y.resize(P, M_ap_symb);
    y = x;
  } else if (Transmit_Diversity == Transmission_Scheme) {
    if (2 == P) {
      M_ap_symb = 2 * M_layer_symb;
      y.resize(P, M_ap_symb);
      for (i = 0; i < M_layer_symb; i++) {
        y(0, 2 * i) = SQRT_2_RECIP * x(0, i);
        y(1, 2 * i) = -SQRT_2_RECIP * conj(x(1, i));
        y(0, 2 * i + 1) = SQRT_2_RECIP * x(1, i);
        y(1, 2 * i + 1) = SQRT_2_RECIP * conj(x(0, i));
      }
    } else {
      if (0 == (M_0_symb & 3)) {
        M_ap_symb = 4 * M_layer_symb;
      } else {
        M_ap_symb = 4 * M_layer_symb - 2;
      }
      y.resize(P, M_ap_symb);
      for (i = 0; i < M_layer_symb - 1; i++) {
        y(0, 4 * i) = SQRT_2_RECIP * x(0, i);
        y(1, 4 * i) = 0;
        y(2, 4 * i) = -SQRT_2_RECIP * conj(x(1, i));
        y(3, 4 * i) = 0;
        y(0, 4 * i + 1) = SQRT_2_RECIP * x(1, i);
        y(1, 4 * i + 1) = 0;
        y(2, 4 * i + 1) = SQRT_2_RECIP * conj(x(0, i));
        y(3, 4 * i + 1) = 0;
        y(0, 4 * i + 2) = 0;
        y(1, 4 * i + 2) = SQRT_2_RECIP * x(2, i);
        y(2, 4 * i + 2) = 0;
        y(3, 4 * i + 2) = -SQRT_2_RECIP * conj(x(3, i));
        y(0, 4 * i + 3) = 0;
        y(1, 4 * i + 3) = SQRT_2_RECIP * x(3, i);
        y(2, 4 * i + 3) = 0;
        y(3, 4 * i + 3) = SQRT_2_RECIP * conj(x(2, i));
      }
      if (0 == (M_0_symb & 3)) {
        y(0, 4 * i) = SQRT_2_RECIP * x(0, i);
        y(1, 4 * i) = 0;
        y(2, 4 * i) = -SQRT_2_RECIP * conj(x(1, i));
        y(3, 4 * i) = 0;
        y(0, 4 * i + 1) = SQRT_2_RECIP * x(1, i);
        y(1, 4 * i + 1) = 0;
        y(2, 4 * i + 1) = SQRT_2_RECIP * conj(x(0, i));
        y(3, 4 * i + 1) = 0;
        y(0, 4 * i + 2) = 0;
        y(1, 4 * i + 2) = SQRT_2_RECIP * x(2, i);
        y(2, 4 * i + 2) = 0;
        y(3, 4 * i + 2) = -SQRT_2_RECIP * conj(x(3, i));
        y(0, 4 * i + 3) = 0;
        y(1, 4 * i + 3) = SQRT_2_RECIP * x(3, i);
        y(2, 4 * i + 3) = 0;
        y(3, 4 * i + 3) = SQRT_2_RECIP * conj(x(2, i));
      } else {
        y(0, 4 * i) = SQRT_2_RECIP * x(0, i);
        y(1, 4 * i) = 0;
        y(2, 4 * i) = -SQRT_2_RECIP * conj(x(1, i));
        y(3, 4 * i) = 0;
        y(0, 4 * i + 1) = SQRT_2_RECIP * x(1, i);
        y(1, 4 * i + 1) = 0;
        y(2, 4 * i + 1) = SQRT_2_RECIP * conj(x(0, i));
        y(3, 4 * i + 1) = 0;
      }
    }
  } else if (Closed_Loop_SM == Transmission_Scheme || MU_MIMO == Transmission_Scheme) {
    M_ap_symb = M_layer_symb;
    y.resize(P, M_ap_symb);
    if (2 == P) {
      if (1 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_2P1v[Codebook_index0(i)] * x.col(i);
        }
      } else {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_2P2v[Codebook_index0(i)] * x.col(i);
        }
      }
    } else if (4 == P) {
      if (alternativeCodeBookEnabledFor4TX_r12) {
        if (1 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P1v_CSI[Codebook_index0(i) * 16 + Codebook_index1(i)] * x.col(i);
          }
        } else if (2 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P2v_CSI[Codebook_index0(i) * 16 + Codebook_index1(i)] * x.col(i);
          }
        } else if (3 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P3v[Codebook_index1(i)] * x.col(i);
          }
        } else {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P4v[Codebook_index1(i)] * x.col(i);
          }
        }
      } else {
        if (1 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P1v[Codebook_index0(i)] * x.col(i);
          }
        } else if (2 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P2v[Codebook_index0(i)] * x.col(i);
          }
        } else if (3 == v) {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P3v[Codebook_index0(i)] * x.col(i);
          }
        } else {
          for (i = 0; i < M_layer_symb; i++) {
            y.col(i) = Codebook_4P4v[Codebook_index0(i)] * x.col(i);
          }
        }
      }
    } else if (alternativeCodeBookEnabledFor4TX_r12) {
      if (1 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P1v_CSI[Codebook_index0(i) * 16 + Codebook_index1(i)] * x.col(i);
        }
      } else if (2 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P2v_CSI[Codebook_index0(i) * 16 + Codebook_index1(i)] * x.col(i);
        }
      } else if (3 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P3v_CSI[Codebook_index0(i) * 16 + Codebook_index1(i)] * x.col(i);
        }
      } else if (4 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P4v_CSI[Codebook_index0(i) * 8 + Codebook_index1(i)] * x.col(i);
        }
      } else if (5 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P5v_CSI[Codebook_index0(i)] * x.col(i);
        }
      } else if (6 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P6v_CSI[Codebook_index0(i)] * x.col(i);
        }
      } else if (7 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P7v_CSI[Codebook_index0(i)] * x.col(i);
        }
      } else {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_8P8v_CSI[Codebook_index0(i)] * x.col(i);
        }
      }
    }
  } else {
    M_ap_symb = M_layer_symb;
    y.resize(P, M_ap_symb);
    if (2 == P) {
      for (i = 0; i < M_layer_symb; i++) {
        y.col(i) = Codebook_2P2v[0] * Codebook_D_2v[i & 1] * Codebook_U_2v * x.col(i);
      }
    } else {
      if (2 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_4P2v[((i / v) & 3) + 12] * Codebook_D_2v[i & 1] * Codebook_U_2v * x.col(i);
        }
      } else if (3 == v) {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_4P3v[((i / v) & 3) + 12] * Codebook_D_3v[i % 3] * Codebook_U_3v * x.col(i);
        }
      } else {
        for (i = 0; i < M_layer_symb; i++) {
          y.col(i) = Codebook_4P4v[((i / v) & 3) + 12] * Codebook_D_4v[i & 3] * Codebook_U_4v * x.col(i);
        }
      }
    }
  }
}
