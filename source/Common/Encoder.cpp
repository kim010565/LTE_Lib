/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Encoder.cpp
    \brief Encoder实现文件
    内容 ：编码相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Encoder.h"

//! 增加CRC校验bit
/*!
  \param crc_enum ： CRC类型，输入
  \param data_inout ： 二进制列向量，输入/输出
  \return 无返回值
*/
void AddCRC(Crc_E crc_enum, bvec &data_inout)  //增加CRC校验bit
{
  int k;
  int crc_length;
  int mask_bits;
  int fbt;
  int shift_register;
  int input_size = data_inout.size();
  bvec temp(data_inout);
  k = (int)crc_enum;
  mask_bits = CrcMask[k];
  crc_length = CrcLength[k];
  shift_register = 0;
  data_inout.resize(input_size + crc_length);
  data_inout.segment(0, input_size) = temp;
  for (k = 0; k < input_size; k++) {
    fbt = ((shift_register & 1) ^ data_inout(k)) * mask_bits;
    shift_register = (shift_register ^ fbt) >> 1;
  }
  for (k = 0; k < crc_length; k++) {
    data_inout(input_size + k) = shift_register & 0x1;
    shift_register >>= 1;
  }
}

//! 咬尾卷积码编码。出自 ：3GPP 36.212 5.1.3.1节
/*!
  \param data_in ： 二进制列向量，输入
  \param data_out ： 二进制矩阵，矩阵行数3（即d0，d1，d2），输出
  \return 无返回值
*/
void TBCC_Encoder(const bvec &data_in, bmat &data_out)  //咬尾卷积码编码
{
  bin u, e1, e2, e3, e4, e5, e6;
  int i = 0;
  int input_size = data_in.size();
  data_out.resize(3, input_size);
  e1 = data_in(input_size - 1);
  e2 = data_in(input_size - 2);
  e3 = data_in(input_size - 3);
  e4 = data_in(input_size - 4);
  e5 = data_in(input_size - 5);
  e6 = data_in(input_size - 6);
  for (i = 0; i < input_size; i++) {
    u = data_in(i);
    data_out(0, i) = u ^ e2 ^ e3 ^ e5 ^ e6;
    data_out(1, i) = u ^ e1 ^ e2 ^ e3 ^ e6;
    data_out(2, i) = u ^ e1 ^ e2 ^ e4 ^ e6;
    e6 = e5;
    e5 = e4;
    e4 = e3;
    e3 = e2;
    e2 = e1;
    e1 = u;
  }
}

//! Turbo编码。出自 ：3GPP 36.212 5.1.3.2节
/*!
  \param data_in ： 二进制列向量，输入
  \param data_out ： 二进制矩阵，矩阵行数3（即d0，d1，d2），输出
  \return 无返回值
*/
void Turbo_Encoder(const bvec &data_in, bmat &data_out)  //Turbo编码
{
  bin u, e1, e2, e3;
  int f1 = 0;
  int f2 = 0;
  int pi_i = 0;
  int i = 0;
  int input_size = data_in.size();
  data_out.resize(3, input_size + 4);
  //Sys & Parity1
  e1 = 0;
  e2 = 0;
  e3 = 0;
  for (i = 0; i < input_size; i++) {
    data_out(0, i) = data_in(i);
    u = data_in(i) ^ e2 ^ e3;
    data_out(1, i) = u ^ e1 ^ e3;
    e3 = e2;
    e2 = e1;
    e1 = u;
  }
  //Parity1 Trellis termination
  data_out(0, input_size) = e2 ^ e3;
  data_out(1, input_size) = e1 ^ e3;
  e3 = e2;
  e2 = e1;
  e1 = 0;
  data_out(2, input_size) = e2 ^ e3;
  data_out(0, input_size + 1) = e1 ^ e3;
  e3 = e2;
  e2 = e1;
  e1 = 0;
  data_out(1, input_size + 1) = e2 ^ e3;
  data_out(2, input_size + 1) = e1 ^ e3;
  //interleaver, find f1, f2
  for (i = 0; i < 188; i++) {
    if (TurboInterleaverTable[i][0] == input_size) {
      f1 = TurboInterleaverTable[i][1];
      f2 = TurboInterleaverTable[i][2];
      break;
    }
  }
  //Parity2
  e1 = 0;
  e2 = 0;
  e3 = 0;
  for (i = 0; i < input_size; i++) {
    pi_i = ((f1 + f2 * i) % input_size);
    pi_i = ((pi_i * i) % input_size);
    u = data_in(pi_i) ^ e2 ^ e3;
    data_out(2, i) = u ^ e1 ^ e3;
    e3 = e2;
    e2 = e1;
    e1 = u;
  }
  //Parity2 Trellis termination
  data_out(0, input_size + 2) = e2 ^ e3;
  data_out(1, input_size + 2) = e1 ^ e3;
  e3 = e2;
  e2 = e1;
  e1 = 0;
  data_out(2, input_size + 2) = e2 ^ e3;
  data_out(0, input_size + 3) = e1 ^ e3;
  e3 = e2;
  e2 = e1;
  e1 = 0;
  data_out(1, input_size + 3) = e2 ^ e3;
  data_out(2, input_size + 3) = e1 ^ e3;
}

//! 码块级联及添加CB CRC。出自 ：3GPP 36.212 5.1.2节
/*!
  \param TB_bits ： 二进制列向量，输入
  \param CB_bits ： 二进制列向量数组，数组大小即CB个数，输出
  \return 填充bit长度F
*/
int CB_Segmentation_CB_CRC_Attachment(const bvec &TB_bits, bvecs &CB_bits)  // 36.212 CB_Segmentation_CB_CRC_Attachment
{
  int i = 0;
  int k = 0;
  int r = 0;
  int s = 0;
  const int Z = 6144;
  int B = TB_bits.size();
  int L = 0;
  int C = 0;
  int B_prime = 0;
  int K_r = 0;
  int K_plus = 0;
  int K_minus = 0;
  int C_plus = 0;
  int C_minus = 0;
  int delta_K = 0;
  int F = 0;
  if (B <= Z) {
    L = 0;
    C = 1;
    B_prime = B;
  } else {
    L = 24;
    C = ceil((float)B / (float)(Z - L));
    B_prime = B + C * L;
  }
  while (i < 188) {
    if (C * TurboInterleaverTable[i][0] >= B_prime) {
      K_plus = TurboInterleaverTable[i][0];
      break;
    }
    i++;
  }
  if (1 == C) {
    C_plus = 1;
    K_minus = 0;
    C_minus = 0;
  } else {
    while (i >= 0) {
      if (TurboInterleaverTable[i][0] < K_plus) {
        K_minus = TurboInterleaverTable[i][0];
        break;
      }
      i--;
    }
    delta_K = K_plus - K_minus;
    C_minus = floor((float)(C * K_plus - B_prime) / (float)delta_K);
    C_plus = C - C_minus;
  }
  F = C_plus * K_plus + C_minus * K_minus - B_prime;
  CB_bits.resize(C);
  CB_bits[0].resize(F);
  for (k = 0; k < F; k++) {
    CB_bits[0](k) = 0;  //Insert of filler bits, NULL
  }
  k = F;
  s = 0;
  for (r = 0; r < C; r++) {
    if (r < C_minus) {
      K_r = K_minus;
    } else {
      K_r = K_plus;
    }
    CB_bits[r].resize(K_r - L);
    while (k < (K_r - L)) {
      CB_bits[r](k) = TB_bits(s);
      k = k + 1;
      s = s + 1;
    }
    if (C > 1) {
      AddCRC(CRC24B, CB_bits[r]);
    }
    k = 0;
  }
  return F;
}

//! Turbo的速率匹配。出自 ：3GPP 36.212 5.1.4.1节
/*!
  \param data_in ： 二进制矩阵，矩阵行数3（即d0，d1，d2），输入
  \param r ： 码块序号，输入
  \param N_IR ： N_IR，输入
  \param DL_SCH_PCH_Flag ： 标志，true ：DL-SCH和PCh传输；false ：UL-SCH和MCH传输，输入
  \param C ： 码块个数，输入
  \param G_prime ： G_prime，输入
  \param N_L ： 一个传输块对应的层数，输入
  \param Q_m ： 调制方式，输入
  \param rv_idx ： RV版本，输入
  \param F ： 码块分割时的填充bit长度，输入
  \param data_out ： 二进制列向量，输出
  \return 无
*/
void Rate_Matching_Turbo(const bmat &data_in,
                         int r,
                         int N_IR,
                         bool DL_SCH_PCH_Flag,
                         int C,
                         int G_prime,
                         int N_L,
                         int Q_m,
                         int rv_idx,
                         int F,
                         bvec &data_out) {
  int i = 0;
  int j = 0;
  int k = 0;
  int row = 0;
  int col = 0;
  int D = data_in.cols();
  const int C_TC_subblock = 32;
  int R_TC_subblock = ceil((float)D / (float)C_TC_subblock);
  int K_PI = R_TC_subblock * C_TC_subblock;
  int N_D = R_TC_subblock * C_TC_subblock - D;
  const int P[] = {0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30, 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31};
  int gamma = 0;
  int E = 0;
  int k0 = 0;
  int N_cb = (DL_SCH_PCH_Flag) ? MIN(floor((float)N_IR / (float)C), 3 * K_PI) : (3 * K_PI);
  gamma = G_prime % C;
  if (r <= (C - gamma - 1)) {
    E = N_L * Q_m * floor((float)G_prime / (float)C);
  } else {
    E = N_L * Q_m * ceil((float)G_prime / (float)C);
  }
  data_out.resize(E);
  data_out.setZero();
  row = 0;
  col = (2 * ceil((float)N_cb / (float)(8 * R_TC_subblock)) * rv_idx + 2);
  k0 = R_TC_subblock * col;
  k = 0;
  j = k0;
  i = 0;
  while (k < E) {
    col = col % (3 * C_TC_subblock);
    //For d0
    if (col < C_TC_subblock) {
      i = row * C_TC_subblock + P[col];
      if (i >= (N_D + F)) {
        data_out(k) = data_in(0, i - N_D);
        k++;
      }
    }
    //For d1
    else if (0 == (col & 1)) {
      i = row * C_TC_subblock + P[(col - C_TC_subblock) >> 1];
      if (i >= (N_D + F)) {
        data_out(k) = data_in(1, i - N_D);
        k++;
      }
    }
    //For d2
    else {
      i = (row * C_TC_subblock + P[(col - C_TC_subblock) >> 1] + 1) % K_PI;
      if (i >= N_D) {
        data_out(k) = data_in(2, i - N_D);
        k++;
      }
    }
    j++;
    if (j == N_cb) {
      j = 0;
      row = 0;
      col = 0;
    } else {
      //For d0
      if (col < C_TC_subblock) {
        if (row == (R_TC_subblock - 1)) {
          col = col + 1;
          row = 0;
        } else {
          row = row + 1;
        }
      }
      //For d1
      else if (0 == (col & 1)) {
        col = col + 1;
      }
      //For d2
      else {
        if (row == (R_TC_subblock - 1)) {
          col = col + 1;
          row = 0;
        } else {
          col = col - 1;
          row = row + 1;
        }
      }
    }
  }
}

//! TBCC的速率匹配。出自 ：3GPP 36.212 5.1.4.2节
/*!
  \param data_in ： 二进制矩阵，矩阵行数3（即d0，d1，d2），输入
  \param E ： 一个TTI内的可用bit数，输入
  \param data_out ： 二进制列向量，输出
  \return 无
*/
void Rate_Matching_TBCC(const bmat &data_in,
                        int E,
                        bvec &data_out) {
  int i = 0;
  int k = 0;
  int row = 0;
  int col = 0;
  int D = data_in.cols();
  const int C_TC_subblock = 32;
  int R_TC_subblock = ceil((float)D / (float)C_TC_subblock);
  int N_D = R_TC_subblock * C_TC_subblock - D;
  const int P[] = {1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30};
  data_out.resize(E);
  row = 0;
  col = 0;
  k = 0;
  i = 0;
  while (k < E) {
    col = col % (3 * C_TC_subblock);
    i = P[col & 31];
    for (row = 0; row < R_TC_subblock; row++) {
      if (i >= N_D) {
        data_out(k) = data_in(col >> 5, i - N_D);
        k++;
      }
      if (k == E) {
        break;
      }
      i = i + C_TC_subblock;
    }
    col = col + 1;
  }
}
