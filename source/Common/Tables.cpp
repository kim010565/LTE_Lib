/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Tables.cpp
    \brief Tables实现文件
    内容 ：一些常用表格相关实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Tables.h"

//! SINR->CQI门限映射
float CQI_Th[15] =
    {
        0.0f, 0.508077f, 0.610106f, 0.739803f, 0.972041f, 1.302223f, 1.831931f, 2.713121f, 3.368965f, 5.633765f, 8.583656f, 13.959759f, 19.555340f, 37.470684f, 44.609177f};

cfvecs PSS_d;           //!< PSS频域序列。出自 ：3GPP 36.211 6.11.1.1节
fvec SSS_s;             //!< SSS频域序列的s部分。出自 ：3GPP 36.211 6.11.2.1节
fvec SSS_c;             //!< SSS频域序列的c部分。出自 ：3GPP 36.211 6.11.2.1节
fvec SSS_z;             //!< SSS频域序列的z部分。出自 ：3GPP 36.211 6.11.2.1节
cfvecs PSS_FFT_d_2048;  //!< PSS频域序列经过2048点FFT变化后的序列。用于快速相关
cfvecs PSS_FFT_d_128;   //!< PSS频域序列经过128点FFT变化后的序列。用于快速相关

cfvecs Codebook_2P1v;  //!< 2P1v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节
cfmats Codebook_2P2v;  //!< 2P2v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节
cfvecs Codebook_4P1v;  //!< 4P1v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节
cfmats Codebook_4P2v;  //!< 4P2v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节
cfmats Codebook_4P3v;  //!< 4P3v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节
cfmats Codebook_4P4v;  //!< 4P4v下码本。出自 ： 3GPP 36.211 6.3.4.2.3节

cfvecs Codebook_4P1v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，4P1v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_4P2v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，4P2v下码本。出自 ： 3GPP 36.213 7.2.4节
cfvecs Codebook_8P1v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P1v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P2v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P2v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P3v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P3v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P4v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P4v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P5v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P5v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P6v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P6v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P7v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P7v下码本。出自 ： 3GPP 36.213 7.2.4节
cfmats Codebook_8P8v_CSI;  //!< alternativeCodeBookEnabledFor4TX_r12=true，8P8v下码本。出自 ： 3GPP 36.213 7.2.4节

cfmat Codebook_U_2v;   //!< Large delay CDD、2v下的U矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节
cfmat Codebook_U_3v;   //!< Large delay CDD、3v下的U矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节
cfmat Codebook_U_4v;   //!< Large delay CDD、4v下的U矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节
cfmats Codebook_D_2v;  //!< Large delay CDD、2v下的D矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节
cfmats Codebook_D_3v;  //!< Large delay CDD、3v下的D矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节
cfmats Codebook_D_4v;  //!< Large delay CDD、4v下的D矩阵。出自 ： 3GPP 36.211 6.3.4.2.2节

bmat CFI_code_word;  //!< CFI编码码字。出自 ： 3GPP 36.212 5.3.4.1节

cfmat PHICH_Orth_Seq_NormalCP;    //!< PHICH Normal CP下的正交序列。出自 ： 3GPP 36.211 6.9.1节
cfmat PHICH_Orth_Seq_ExtendedCP;  //!< PHICH Extended CP下的正交序列。出自 ： 3GPP 36.211 6.9.1节

Eigen::Matrix2f Corr_MID_2;   //!< 2天线中相关矩阵Chol分解后的下三角阵。 3GPP 36.101 附录B.2.3.2
Eigen::Matrix2f Corr_HIGH_2;  //!< 2天线高相关矩阵Chol分解后的下三角阵。 3GPP 36.101 附录B.2.3.2
Eigen::Matrix4f Corr_MID_4;   //!< 4天线中相关矩阵Chol分解后的下三角阵。 3GPP 36.101 附录B.2.3.2
Eigen::Matrix4f Corr_HIGH_4;  //!< 4天线高相关矩阵Chol分解后的下三角阵。 3GPP 36.101 附录B.2.3.2

imat MLD_Candidated_idxs_QPSK;    //!< QPSK下，MLD算法后续路径集合
imat MLD_Candidated_idxs_16QAM;   //!< 16QAM下，MLD算法后续路径集合
imat MLD_Candidated_idxs_64QAM;   //!< 64QAM下，MLD算法后续路径集合
imat MLD_Candidated_idxs_256QAM;  //!< 256QAM下，MLD算法后续路径集合

//! 初始化常用表格函数
/*!
  \return 无
*/
void InitTables() {
  int i = 0;
  int j = 0;
  //PSS/SSS
  int u = 0;
  int n = 0;
  const float PSS_Root_Idx[3] = {25.0f, 29.0f, 34.0f};
  bin SSS_x[31];
  //Codebook
  cfmat Codebook_u;
  cfmats Codebook_W;
  cpxf cpx_tmp = 0.0f;
  const int Codebook_4P2v_col[16] = {4, 2, 2, 2, 4, 4, 3, 3, 2, 4, 3, 3, 2, 3, 3, 2};
  const int Codebook_4P3v_col[2][16] =
      {
          {2, 2, 2, 2, 2, 2, 3, 3, 2, 3, 2, 3, 2, 2, 2, 2},
          {4, 3, 3, 3, 4, 4, 4, 4, 4, 4, 3, 4, 3, 3, 3, 3}};
  const int Codebook_4P4v_col[3][16] =
      {
          {1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1},
          {2, 2, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3, 2, 3, 2, 2},
          {3, 3, 1, 1, 3, 3, 2, 2, 3, 3, 2, 2, 3, 2, 1, 3}};
  int i1 = 0;
  int i2 = 0;
  int m = 0;
  int m_prime = 0;
  int m_prime_prime = 0;
  cpxf fi_n = 0.0f;
  cpxf fi_n_prime = 0.0f;
  cfvec v_m;
  cfvec v_m_prime;
  cfvec v_m_prime_prime;
  Eigen::FFT<float> fft;
  cfvec fft_in;
  cfvec fft_out;
  //PSS/SSS
  PSS_d.resize(3);
  for (u = 0; u < 3; u++) {
    PSS_d[u].resize(62);
    for (n = 0; n <= 30; n++) {
      PSS_d[u](n) = MyExpCpx_2PI(-PSS_Root_Idx[u] * n * (n + 1) / (2.0f * 63.0f));
    }
    for (n = 31; n <= 61; n++) {
      PSS_d[u](n) = MyExpCpx_2PI(-PSS_Root_Idx[u] * (n + 1) * (n + 2) / (2.0f * 63.0f));
    }
  }
  SSS_s.resize(31);
  SSS_c.resize(31);
  SSS_z.resize(31);
  SSS_x[0] = 0;
  SSS_x[1] = 0;
  SSS_x[2] = 0;
  SSS_x[3] = 0;
  SSS_x[4] = 1;
  for (i = 0; i <= 25; i++) {
    SSS_x[i + 5] = (SSS_x[i + 2] + SSS_x[i]) & 1;
  }
  for (i = 0; i <= 30; i++) {
    SSS_s(i) = 1.0f - 2.0f * (float)SSS_x[i];
  }
  for (i = 0; i <= 25; i++) {
    SSS_x[i + 5] = (SSS_x[i + 3] + SSS_x[i]) & 1;
  }
  for (i = 0; i <= 30; i++) {
    SSS_c(i) = 1.0f - 2.0f * (float)SSS_x[i];
  }
  for (i = 0; i <= 25; i++) {
    SSS_x[i + 5] = (SSS_x[i + 4] + SSS_x[i + 2] + SSS_x[i + 1] + SSS_x[i]) & 1;
  }
  for (i = 0; i <= 30; i++) {
    SSS_z(i) = 1.0f - 2.0f * (float)SSS_x[i];
  }
  PSS_FFT_d_2048.resize(9);
  PSS_FFT_d_128.resize(3);
  for (i = 0; i < 9; i++) {
    PSS_FFT_d_2048[i].resize(2048);
  }
  for (i = 0; i < 3; i++) {
    PSS_FFT_d_128[i].resize(128);
  }
  for (i = 0; i < 3; i++) {
    fft_in.resize(128);
    fft_in.setZero();
    fft_in.segment(1, 31) = PSS_d[i].segment(31, 31);
    fft_in.segment(97, 31) = PSS_d[i].segment(0, 31);
    fft.inv(fft_out, fft_in);
    fft_in.resize(2048);
    fft_in.setZero();
    fft_in.segment(0, 128) = fft_out.segment(0, 128);
    fft.fwd(fft_out, fft_in);
    PSS_FFT_d_2048[i] = fft_out.conjugate();
  }
  for (i = 0; i < 3; i++) {
    fft_in.resize(128);
    fft_in.setZero();
    fft_in.segment(1, 31) = PSS_d[i].segment(31, 31);
    fft_in.segment(97, 31) = PSS_d[i].segment(0, 31);
    fft.inv(fft_out, fft_in);
    fft_in.resize(2048);
    fft_in.setZero();
    for (j = 0; j < 128; j++) {
      fft_in[j] = fft_out[j] * MyExpCpx_2PI(15000.0f * 0.001f / 1920.0f * (float)j);
    }
    fft.fwd(fft_out, fft_in);
    PSS_FFT_d_2048[i + 3] = fft_out.conjugate();
  }
  for (i = 0; i < 3; i++) {
    fft_in.resize(128);
    fft_in.setZero();
    fft_in.segment(1, 31) = PSS_d[i].segment(31, 31);
    fft_in.segment(97, 31) = PSS_d[i].segment(0, 31);
    fft.inv(fft_out, fft_in);
    fft_in.resize(2048);
    fft_in.setZero();
    for (j = 0; j < 128; j++) {
      fft_in[j] = fft_out[j] * MyExpCpx_2PI(-15000.0f * 0.001f / 1920.0f * (float)j);
    }
    fft.fwd(fft_out, fft_in);
    PSS_FFT_d_2048[i + 6] = fft_out.conjugate();
  }
  for (i = 0; i < 3; i++) {
    fft_in.resize(128);
    fft_in.setZero();
    fft_in.segment(1, 31) = PSS_d[i].segment(31, 31);
    fft_in.segment(97, 31) = PSS_d[i].segment(0, 31);
    fft.inv(fft_out, fft_in);
    for (j = 0; j < 128; j++) {
      PSS_FFT_d_128[i](j) = conj(fft_out(j)) / abs(fft_out(j));
    }
  }
  //Codebook
  Codebook_2P1v.resize(4);
  for (i = 0; i < 4; i++) {
    Codebook_2P1v[i].resize(2);
  }
  Codebook_2P1v[0] << cpxf(SQRT_2_RECIP, 0.0f), cpxf(SQRT_2_RECIP, 0.0f);
  Codebook_2P1v[1] << cpxf(SQRT_2_RECIP, 0.0f), cpxf(-SQRT_2_RECIP, 0.0f);
  Codebook_2P1v[2] << cpxf(SQRT_2_RECIP, 0.0f), cpxf(0.0f, SQRT_2_RECIP);
  Codebook_2P1v[3] << cpxf(SQRT_2_RECIP, 0.0f), cpxf(0.0f, -SQRT_2_RECIP);
  Codebook_2P2v.resize(3);
  for (i = 0; i < 3; i++) {
    Codebook_2P2v[i].resize(2, 2);
  }
  Codebook_2P2v[0] << cpxf(SQRT_2_RECIP, 0.0f), 0.0f, 0.0f, cpxf(SQRT_2_RECIP, 0.0f);
  Codebook_2P2v[1] << cpxf(0.5f, 0.0f), cpxf(0.5f, 0.0f), cpxf(0.5f, 0.0f), cpxf(-0.5f, 0.0f);
  Codebook_2P2v[2] << cpxf(0.5f, 0.0f), cpxf(0.5f, 0.0f), cpxf(0.0f, 0.5f), cpxf(0.0f, -0.5f);
  Codebook_u.resize(4, 16);
  Codebook_u << 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      -1.0f, cpxf(0.0f, -1.0f), 1.0f, cpxf(0.0f, 1.0f),
      cpxf(-SQRT_2_RECIP, -SQRT_2_RECIP), cpxf(SQRT_2_RECIP, -SQRT_2_RECIP), cpxf(SQRT_2_RECIP, SQRT_2_RECIP), cpxf(-SQRT_2_RECIP, SQRT_2_RECIP),
      -1.0f, cpxf(0.0f, -1.0f), 1.0f, cpxf(0.0f, 1.0f),
      -1.0f, -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f, 1.0f,
      cpxf(0.0f, -1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, 1.0f),
      1.0f, -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f, 1.0f,
      -1.0f, cpxf(0.0f, 1.0f), 1.0f, cpxf(0.0f, -1.0f),
      cpxf(SQRT_2_RECIP, -SQRT_2_RECIP), cpxf(-SQRT_2_RECIP, -SQRT_2_RECIP), cpxf(-SQRT_2_RECIP, SQRT_2_RECIP), cpxf(SQRT_2_RECIP, SQRT_2_RECIP),
      1.0f, cpxf(0.0f, -1.0f), -1.0f, cpxf(0.0f, 1.0f),
      1.0f, -1.0f, -1.0f, 1.0f;
  Codebook_W.resize(16);
  for (i = 0; i < 16; i++) {
    Codebook_W[i].resize(4, 4);
    cpx_tmp = Codebook_u.col(i).adjoint() * Codebook_u.col(i);
    Codebook_W[i] = Eigen::MatrixXcf::Identity(4, 4) - 2.0f * Codebook_u.col(i) * Codebook_u.col(i).adjoint() / cpx_tmp;
  }
  Codebook_4P1v.resize(16);
  for (i = 0; i < 16; i++) {
    Codebook_4P1v[i].resize(4);
    Codebook_4P1v[i] = Codebook_W[i].col(0);
  }
  //cout << Codebook_4P1v[0] <<endl;
  Codebook_4P2v.resize(16);
  for (i = 0; i < 16; i++) {
    Codebook_4P2v[i].resize(4, 2);
    Codebook_4P2v[i].col(0) = Codebook_W[i].col(0) * SQRT_2_RECIP;
    Codebook_4P2v[i].col(1) = Codebook_W[i].col(Codebook_4P2v_col[i] - 1) * SQRT_2_RECIP;
  }
  //cout << Codebook_4P2v[0] <<endl;
  Codebook_4P3v.resize(16);
  for (i = 0; i < 16; i++) {
    Codebook_4P3v[i].resize(4, 3);
    Codebook_4P3v[i].col(0) = Codebook_W[i].col(0) / MySqrt(3.0f);
    Codebook_4P3v[i].col(1) = Codebook_W[i].col(Codebook_4P3v_col[0][i] - 1) / MySqrt(3.0f);
    Codebook_4P3v[i].col(2) = Codebook_W[i].col(Codebook_4P3v_col[1][i] - 1) / MySqrt(3.0f);
  }
  Codebook_4P4v.resize(16);
  for (i = 0; i < 16; i++) {
    Codebook_4P4v[i].resize(4, 4);
    Codebook_4P4v[i].col(0) = Codebook_W[i].col(Codebook_4P4v_col[0][i] - 1) * 0.5f;
    Codebook_4P4v[i].col(1) = Codebook_W[i].col(Codebook_4P4v_col[1][i] - 1) * 0.5f;
    Codebook_4P4v[i].col(2) = Codebook_W[i].col(Codebook_4P4v_col[2][i] - 1) * 0.5f;
    Codebook_4P4v[i].col(3) = Codebook_W[i].col(3) * 0.5f;
  }
  Codebook_4P1v_CSI.resize(16 * 16);
  v_m_prime.resize(2);
  v_m_prime(0) = 1;
  for (i1 = 0; i1 < 16; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      m = i1 + (i2 / 4) * 8;
      n = (i2 & 3) * 8 + (i2 / 4) * 2;
      v_m_prime(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI((float)m / 32.0f));
      fi_n_prime = cpxf(MyCos_2PI((float)n / 32.0f), MySin_2PI((float)n / 32.0f));
      Codebook_4P1v_CSI[i1 * 16 + i2].resize(4);
      Codebook_4P1v_CSI[i1 * 16 + i2](0) = v_m_prime(0) * 0.5f;
      Codebook_4P1v_CSI[i1 * 16 + i2](1) = v_m_prime(1) * 0.5f;
      Codebook_4P1v_CSI[i1 * 16 + i2](2) = fi_n_prime * v_m_prime(0) * 0.5f;
      Codebook_4P1v_CSI[i1 * 16 + i2](3) = fi_n_prime * v_m_prime(1) * 0.5f;
    }
  }
  Codebook_4P2v_CSI.resize(16 * 16);
  v_m_prime_prime.resize(2);
  v_m_prime_prime(0) = 1;
  for (i1 = 0; i1 < 16; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      if (i2 < 8) {
        m = i1 + (i2 / 2) * 8;
        m_prime = i1 + (i2 / 2) * 8;
      } else {
        m = i1 + ((i2 / 2) & 1) * 8;
        m_prime = i1 + MIN((((i2 - 8) / 2) + 1) * 8, 24);
      }
      n = i2 % 2;
      v_m_prime(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI((float)m / 32.0f));
      v_m_prime_prime(1) = cpxf(MyCos_2PI((float)m_prime / 32.0f), MySin_2PI((float)m_prime / 32.0f));
      fi_n = cpxf(MyCos(PI * (float)n / 2.0f), MySin(PI * (float)n / 2.0f));
      Codebook_4P2v_CSI[i1 * 16 + i2].resize(4, 2);
      Codebook_4P2v_CSI[i1 * 16 + i2](0, 0) = v_m_prime(0) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](0, 1) = v_m_prime_prime(0) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](1, 0) = v_m_prime(1) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](1, 1) = v_m_prime_prime(1) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](2, 0) = fi_n * v_m_prime(0) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](2, 1) = -fi_n * v_m_prime_prime(0) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](3, 0) = fi_n * v_m_prime(1) / MySqrt(8.0f);
      Codebook_4P2v_CSI[i1 * 16 + i2](3, 1) = -fi_n * v_m_prime_prime(1) / MySqrt(8.0f);
    }
  }
  Codebook_8P1v_CSI.resize(16 * 16);
  v_m.resize(4);
  v_m(0) = 1;
  for (i1 = 0; i1 < 16; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      m = 2 * i1 + i2 / 4;
      n = i2 & 3;
      v_m(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI(2.0f * PI * (float)m / 32.0f));
      v_m(2) = cpxf(MyCos_2PI(2.0f * (float)m / 32.0f), MySin_2PI(2.0f * (float)m / 32.0f));
      v_m(3) = cpxf(MyCos_2PI(3.0f * (float)m / 32.0f), MySin_2PI(3.0f * (float)m / 32.0f));
      fi_n = cpxf(MyCos(PI * (float)n / 2.0f), MySin(PI * (float)n / 2.0f));
      Codebook_8P1v_CSI[i1 * 16 + i2].resize(8);
      Codebook_8P1v_CSI[i1 * 16 + i2](0) = v_m(0) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](1) = v_m(1) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](2) = v_m(2) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](3) = v_m(3) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](4) = fi_n * v_m(0) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](5) = fi_n * v_m(1) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](6) = fi_n * v_m(2) / MySqrt(8.0f);
      Codebook_8P1v_CSI[i1 * 16 + i2](7) = fi_n * v_m(3) / MySqrt(8.0f);
    }
  }
  Codebook_8P2v_CSI.resize(16 * 16);
  v_m_prime.resize(4);
  v_m_prime(0) = 1;
  for (i1 = 0; i1 < 16; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      if (i2 < 8) {
        m = 2 * i1 + i2 / 2;
        m_prime = 2 * i1 + i2 / 2;
      } else {
        m = 2 * i1 + (i2 & 1);
        m_prime = 2 * i1 + MIN((((i2 - 8) / 2) + 1), 3);
      }
      n = i2 % 2;
      v_m(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI((float)m / 32.0f));
      v_m(2) = cpxf(MyCos_2PI(2.0f * (float)m / 32.0f), MySin_2PI(2.0f * (float)m / 32.0f));
      v_m(3) = cpxf(MyCos_2PI(3.0f * (float)m / 32.0f), MySin_2PI(3.0f * (float)m / 32.0f));
      v_m_prime(1) = cpxf(MyCos_2PI((float)m_prime / 32.0f), MySin_2PI((float)m_prime / 32.0f));
      v_m_prime(2) = cpxf(MyCos_2PI(2.0f * (float)m_prime / 32.0f), MySin_2PI(2.0f * (float)m_prime / 32.0f));
      v_m_prime(3) = cpxf(MyCos_2PI(3.0f * (float)m_prime / 32.0f), MySin_2PI(3.0f * (float)m_prime / 32.0f));
      fi_n = cpxf(MyCos(PI * (float)n / 2.0f), MySin(PI * (float)n / 2.0f));
      Codebook_8P2v_CSI[i1 * 16 + i2].resize(8, 2);
      Codebook_8P2v_CSI[i1 * 16 + i2](0, 0) = v_m(0) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](0, 1) = v_m_prime(0) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](1, 0) = v_m(1) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](1, 1) = v_m_prime(1) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](2, 0) = v_m(2) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](2, 1) = v_m_prime(2) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](3, 0) = v_m(3) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](3, 1) = v_m_prime(3) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](4, 0) = fi_n * v_m(0) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](4, 1) = -fi_n * v_m_prime(0) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](5, 0) = fi_n * v_m(1) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](5, 1) = -fi_n * v_m_prime(1) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](6, 0) = fi_n * v_m(2) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](6, 1) = -fi_n * v_m_prime(2) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](7, 0) = fi_n * v_m(3) / 4.0f;
      Codebook_8P2v_CSI[i1 * 16 + i2](7, 1) = -fi_n * v_m_prime(3) / 4.0f;
    }
  }
  Codebook_8P3v_CSI.resize(4 * 16);
  v_m_prime_prime.resize(4);
  v_m_prime_prime(0) = 1;
  for (i1 = 0; i1 < 4; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      m = 8 * i1 + (i2 & 1) * 8 + (i2 / 4) * 2;
      if (2 == i2 || 6 == i2 || 10 == i2 || 14 == i2) {
        m_prime = 8 * i1 + (i2 / 4) * 2 + 8;
      } else {
        m_prime = 8 * i1 + (i2 / 4) * 2;
      }
      if (3 == i2 || 7 == i2 || 11 == i2 || 15 == i2) {
        m_prime_prime = 8 * i1 + (i2 / 4) * 2;
      } else {
        m_prime_prime = 8 * i1 + (i2 / 4) * 2 + 8;
      }
      v_m(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI((float)m / 32.0f));
      v_m(2) = cpxf(MyCos_2PI(2.0f * (float)m / 32.0f), MySin_2PI(2.0f * (float)m / 32.0f));
      v_m(3) = cpxf(MyCos_2PI(3.0f * (float)m / 32.0f), MySin_2PI(3.0f * (float)m / 32.0f));
      v_m_prime(1) = cpxf(MyCos_2PI((float)m_prime / 32.0f), MySin_2PI((float)m_prime / 32.0f));
      v_m_prime(2) = cpxf(MyCos_2PI(2.0f * (float)m_prime / 32.0f), MySin_2PI(2.0f * (float)m_prime / 32.0f));
      v_m_prime(3) = cpxf(MyCos_2PI(3.0f * (float)m_prime / 32.0f), MySin_2PI(3.0f * (float)m_prime / 32.0f));
      v_m_prime_prime(1) = cpxf(MyCos_2PI((float)m_prime_prime / 32.0f), MySin_2PI((float)m_prime_prime / 32.0f));
      v_m_prime_prime(2) = cpxf(MyCos_2PI(2.0f * (float)m_prime_prime / 32.0f), MySin_2PI(2.0f * (float)m_prime_prime / 32.0f));
      v_m_prime_prime(3) = cpxf(MyCos_2PI(3.0f * (float)m_prime_prime / 32.0f), MySin_2PI(3.0f * (float)m_prime_prime / 32.0f));
      Codebook_8P3v_CSI[i1 * 16 + i2].resize(8, 3);
      Codebook_8P3v_CSI[i1 * 16 + i2](0, 0) = v_m(0) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](0, 1) = v_m_prime(0) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](0, 2) = v_m_prime_prime(0) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](1, 0) = v_m(1) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](1, 1) = v_m_prime(1) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](1, 2) = v_m_prime_prime(1) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](2, 0) = v_m(2) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](2, 1) = v_m_prime(2) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](2, 2) = v_m_prime_prime(2) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](3, 0) = v_m(3) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](3, 1) = v_m_prime(3) / MySqrt(24.0f);
      Codebook_8P3v_CSI[i1 * 16 + i2](3, 2) = v_m_prime_prime(3) / MySqrt(24.0f);
      if (0 == (i2 & 3) || 1 == (i2 & 3)) {
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 0) = v_m(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 1) = -v_m_prime(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 2) = -v_m_prime_prime(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 0) = v_m(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 1) = -v_m_prime(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 2) = -v_m_prime_prime(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 0) = v_m(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 1) = -v_m_prime(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 2) = -v_m_prime_prime(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 0) = v_m(3) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 1) = -v_m_prime(3) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 2) = -v_m_prime_prime(3) / MySqrt(24.0f);
      } else {
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 0) = v_m(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 1) = v_m_prime(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](4, 2) = -v_m_prime_prime(0) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 0) = v_m(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 1) = v_m_prime(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](5, 2) = -v_m_prime_prime(1) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 0) = v_m(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 1) = v_m_prime(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](6, 2) = -v_m_prime_prime(2) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 0) = v_m(3) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 1) = v_m_prime(3) / MySqrt(24.0f);
        Codebook_8P3v_CSI[i1 * 16 + i2](7, 2) = -v_m_prime_prime(3) / MySqrt(24.0f);
      }
    }
  }
  Codebook_8P4v_CSI.resize(4 * 8);
  for (i1 = 0; i1 < 4; i1++) {
    for (i2 = 0; i2 < 8; i2++) {
      m = 8 * i1 + (i2 / 2) * 2;
      m_prime = 8 * i1 + (i2 / 2) * 2 + 8;
      n = i2 & 1;
      v_m(1) = cpxf(MyCos_2PI((float)m / 32.0f), MySin_2PI((float)m / 32.0f));
      v_m(2) = cpxf(MyCos_2PI(2.0f * (float)m / 32.0f), MySin_2PI(2.0f * (float)m / 32.0f));
      v_m(3) = cpxf(MyCos_2PI(3.0f * (float)m / 32.0f), MySin_2PI(3.0f * (float)m / 32.0f));
      v_m_prime(1) = cpxf(MyCos_2PI((float)m_prime / 32.0f), MySin_2PI((float)m_prime / 32.0f));
      v_m_prime(2) = cpxf(MyCos_2PI(2.0f * (float)m_prime / 32.0f), MySin_2PI(2.0f * (float)m_prime / 32.0f));
      v_m_prime(3) = cpxf(MyCos_2PI(3.0f * (float)m_prime / 32.0f), MySin_2PI(3.0f * (float)m_prime / 32.0f));
      fi_n = cpxf(MyCos(PI * (float)n / 2.0f), MySin(PI * (float)n / 2.0f));
      Codebook_8P4v_CSI[i1 * 8 + i2].resize(8, 4);
      Codebook_8P4v_CSI[i1 * 8 + i2](0, 0) = v_m(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](0, 1) = v_m_prime(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](0, 2) = v_m(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](0, 3) = v_m_prime(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](1, 0) = v_m(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](1, 1) = v_m_prime(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](1, 2) = v_m(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](1, 3) = v_m_prime(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](2, 0) = v_m(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](2, 1) = v_m_prime(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](2, 2) = v_m(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](2, 3) = v_m_prime(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](3, 0) = v_m(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](3, 1) = v_m_prime(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](3, 2) = v_m(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](3, 3) = v_m_prime(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](4, 0) = fi_n * v_m(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](4, 1) = fi_n * v_m_prime(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](4, 2) = -fi_n * v_m(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](4, 3) = -fi_n * v_m_prime(0) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](5, 0) = fi_n * v_m(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](5, 1) = fi_n * v_m_prime(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](5, 2) = -fi_n * v_m(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](5, 3) = -fi_n * v_m_prime(1) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](6, 0) = fi_n * v_m(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](6, 1) = fi_n * v_m_prime(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](6, 2) = -fi_n * v_m(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](6, 3) = -fi_n * v_m_prime(2) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](7, 0) = fi_n * v_m(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](7, 1) = fi_n * v_m_prime(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](7, 2) = -fi_n * v_m(3) / MySqrt(32.0f);
      Codebook_8P4v_CSI[i1 * 8 + i2](7, 3) = -fi_n * v_m_prime(3) / MySqrt(32.0f);
    }
  }
  Codebook_8P5v_CSI.resize(4);
  for (i1 = 0; i1 < 4; i1++) {
    i2 = 0;
    Codebook_8P5v_CSI[i1].resize(8, 5);
    Codebook_8P5v_CSI[i1](0, 0) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](0, 1) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](0, 2) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](0, 3) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](0, 4) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](1, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](1, 1) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](1, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](1, 3) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](1, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](2, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](2, 1) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](2, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](2, 3) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](2, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](3, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](3, 1) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](3, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](3, 3) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](3, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](4, 0) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](4, 1) = -1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](4, 2) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](4, 3) = -1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](4, 4) = 1.0f / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](5, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](5, 1) = -cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](5, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](5, 3) = -cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](5, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](6, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](6, 1) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](6, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](6, 3) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](6, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](7, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](7, 1) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](7, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](7, 3) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(40.0f);
    Codebook_8P5v_CSI[i1](7, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(40.0f);
  }
  Codebook_8P6v_CSI.resize(4);
  for (i1 = 0; i1 < 4; i1++) {
    i2 = 0;
    Codebook_8P6v_CSI[i1].resize(8, 6);
    Codebook_8P6v_CSI[i1](0, 0) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](0, 1) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](0, 2) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](0, 3) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](0, 4) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](0, 5) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 1) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 3) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](1, 5) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 1) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 3) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](2, 5) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 1) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 3) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](3, 5) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 0) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 1) = -1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 2) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 3) = -1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 4) = 1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](4, 5) = -1.0f / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 1) = -cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 3) = -cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](5, 5) = -cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 1) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 3) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](6, 5) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 1) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 3) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
    Codebook_8P6v_CSI[i1](7, 5) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(48.0f);
  }
  Codebook_8P7v_CSI.resize(4);
  for (i1 = 0; i1 < 4; i1++) {
    i2 = 0;
    Codebook_8P7v_CSI[i1].resize(8, 7);
    Codebook_8P7v_CSI[i1](0, 0) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 1) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 2) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 3) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 4) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 5) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](0, 6) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 1) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 3) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 5) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](1, 6) = cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 1) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 3) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 5) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](2, 6) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 1) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 3) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 5) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](3, 6) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 0) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 1) = -1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 2) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 3) = -1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 4) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 5) = -1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](4, 6) = 1.0f / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 1) = -cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 3) = -cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 5) = -cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](5, 6) = cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 1) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 3) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 5) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](6, 6) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 1) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 3) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 5) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / MySqrt(56.0f);
    Codebook_8P7v_CSI[i1](7, 6) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / MySqrt(56.0f);
  }
  Codebook_8P8v_CSI.resize(4);
  for (i1 = 0; i1 < 4; i1++) {
    i2 = 0;
    Codebook_8P8v_CSI[i1].resize(8, 8);
    Codebook_8P8v_CSI[i1](0, 0) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 1) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 2) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 3) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 4) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 5) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 6) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](0, 7) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](1, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 1) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 3) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 5) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 6) = cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](1, 7) = cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 1) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 3) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 5) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 6) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](2, 7) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 1) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 3) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 5) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 6) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](3, 7) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](4, 0) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 1) = -1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 2) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 3) = -1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 4) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 5) = -1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 6) = 1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](4, 7) = -1.0f / 8.0f;
    Codebook_8P8v_CSI[i1](5, 0) = cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 1) = -cpxf(MyCos_2PI((float)(2 * i1) / 32.0f), MySin_2PI((float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 2) = cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 3) = -cpxf(MyCos_2PI((float)(2 * i1 + 8) / 32.0f), MySin_2PI((float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 4) = cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 5) = -cpxf(MyCos_2PI((float)(2 * i1 + 16) / 32.0f), MySin_2PI((float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 6) = cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](5, 7) = -cpxf(MyCos_2PI((float)(2 * i1 + 24) / 32.0f), MySin_2PI((float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 0) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 1) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 2) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 3) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 4) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 5) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 6) = cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](6, 7) = -cpxf(MyCos_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(2.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 0) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 1) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 2) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 3) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 8) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 4) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 5) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 16) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 6) = cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
    Codebook_8P8v_CSI[i1](7, 7) = -cpxf(MyCos_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f), MySin_2PI(3.0f * (float)(2 * i1 + 24) / 32.0f)) / 8.0f;
  }
  Codebook_U_2v.resize(2, 2);
  Codebook_U_2v << SQRT_2_RECIP, SQRT_2_RECIP, SQRT_2_RECIP, cpxf(MyCos_2PI(1.0f / 2.0f), -MySin_2PI(1.0f / 2.0f)) * SQRT_2_RECIP;
  Codebook_U_3v.resize(3, 3);
  Codebook_U_3v << 1.0f / MySqrt(3.0f), 1.0f / MySqrt(3.0f), 1.0f / MySqrt(3.0f),
      1.0f / MySqrt(3.0f), cpxf(MyCos_2PI(1.0f / 3.0f), -MySin_2PI(1.0f / 3.0f)) / MySqrt(3.0f), cpxf(MyCos_2PI(2.0f / 3.0f), -MySin_2PI(2.0f / 3.0f)) / MySqrt(3.0f),
      1.0f / MySqrt(3.0f), cpxf(MyCos_2PI(2.0f / 3.0f), -MySin_2PI(2.0f / 3.0f)) / MySqrt(3.0f), cpxf(MyCos_2PI(4.0f / 3.0f), -MySin_2PI(4.0f / 3.0f)) / MySqrt(3.0f);
  Codebook_U_4v.resize(4, 4);
  Codebook_U_4v << 1.0f / 2.0f, 1.0f / 2.0f, 1.0f / 2.0f, 1.0f / 2.0f,
      1.0f / 2.0f, cpxf(MyCos_2PI(1.0f / 4.0f), -MySin_2PI(1.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(2.0f / 4.0f), -MySin_2PI(2.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(3.0f / 4.0f), -MySin_2PI(3.0f / 4.0f)) / 2.0f,
      1.0f / 2.0f, cpxf(MyCos_2PI(2.0f / 4.0f), -MySin_2PI(2.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(4.0f / 4.0f), -MySin_2PI(4.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(6.0f / 4.0f), -MySin_2PI(6.0f / 4.0f)) / 2.0f,
      1.0f / 2.0f, cpxf(MyCos_2PI(3.0f / 4.0f), -MySin_2PI(3.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(6.0f / 4.0f), -MySin_2PI(6.0f / 4.0f)) / 2.0f, cpxf(MyCos_2PI(9.0f / 4.0f), -MySin_2PI(9.0f / 4.0f)) / 2.0f;
  Codebook_D_2v.resize(2);
  Codebook_D_2v[0].resize(2, 2);
  Codebook_D_2v[0].setZero();
  Codebook_D_2v[0](0, 0) = 1.0f;
  Codebook_D_2v[0](1, 1) = 1.0f;
  Codebook_D_2v[1].resize(2, 2);
  Codebook_D_2v[1].setZero();
  Codebook_D_2v[1](0, 0) = 1.0f;
  Codebook_D_2v[1](1, 1) = -1.0f;
  Codebook_D_3v.resize(3);
  Codebook_D_3v[0].resize(3, 3);
  Codebook_D_3v[0].setZero();
  Codebook_D_3v[0](0, 0) = 1.0f;
  Codebook_D_3v[0](1, 1) = 1.0f;
  Codebook_D_3v[0](2, 2) = 1.0f;
  Codebook_D_3v[1].resize(3, 3);
  Codebook_D_3v[1].setZero();
  Codebook_D_3v[1](0, 0) = 1.0f;
  Codebook_D_3v[1](1, 1) = cpxf(MyCos_2PI(1.0f / 3.0f), -MySin_2PI(1.0f / 3.0f));
  Codebook_D_3v[1](2, 2) = cpxf(MyCos_2PI(2.0f / 3.0f), -MySin_2PI(2.0f / 3.0f));
  Codebook_D_3v[2].resize(3, 3);
  Codebook_D_3v[2].setZero();
  Codebook_D_3v[2](0, 0) = 1.0f;
  Codebook_D_3v[2](1, 1) = cpxf(MyCos_2PI(2.0f / 3.0f), -MySin_2PI(2.0f / 3.0f));
  Codebook_D_3v[2](2, 2) = cpxf(MyCos_2PI(2.0f * 2.0f / 3.0f), -MySin_2PI(2.0f * 2.0f / 3.0f));
  Codebook_D_4v.resize(4);
  Codebook_D_4v[0].resize(4, 4);
  Codebook_D_4v[0].setZero();
  Codebook_D_4v[0](0, 0) = 1.0f;
  Codebook_D_4v[0](1, 1) = 1.0f;
  Codebook_D_4v[0](2, 2) = 1.0f;
  Codebook_D_4v[0](3, 3) = 1.0f;
  Codebook_D_4v[1].resize(4, 4);
  Codebook_D_4v[1].setZero();
  Codebook_D_4v[1](0, 0) = 1.0f;
  Codebook_D_4v[1](1, 1) = cpxf(MyCos_2PI(1.0f / 4.0f), -MySin_2PI(1.0f / 4.0f));
  Codebook_D_4v[1](2, 2) = cpxf(MyCos_2PI(2.0f / 4.0f), -MySin_2PI(2.0f / 4.0f));
  Codebook_D_4v[1](3, 3) = cpxf(MyCos_2PI(3.0f / 4.0f), -MySin_2PI(3.0f / 4.0f));
  Codebook_D_4v[2].resize(4, 4);
  Codebook_D_4v[2].setZero();
  Codebook_D_4v[2](0, 0) = 1.0f;
  Codebook_D_4v[2](1, 1) = cpxf(MyCos_2PI(2.0f / 4.0f), -MySin_2PI(2.0f / 4.0f));
  Codebook_D_4v[2](2, 2) = cpxf(MyCos_2PI(2.0f * 2.0f / 4.0f), -MySin_2PI(2.0f * 2.0f / 4.0f));
  Codebook_D_4v[2](3, 3) = cpxf(MyCos_2PI(3.0f * 2.0f / 4.0f), -MySin_2PI(3.0f * 2.0f / 4.0f));
  Codebook_D_4v[3].resize(4, 4);
  Codebook_D_4v[3].setZero();
  Codebook_D_4v[3](0, 0) = 1.0f;
  Codebook_D_4v[3](1, 1) = cpxf(MyCos_2PI(3.0f / 4.0f), -MySin_2PI(3.0f / 4.0f));
  Codebook_D_4v[3](2, 2) = cpxf(MyCos_2PI(2.0f * 3.0f / 4.0f), -MySin_2PI(2.0f * 3.0f / 4.0f));
  Codebook_D_4v[3](3, 3) = cpxf(MyCos_2PI(3.0f * 3.0f / 4.0f), -MySin_2PI(3.0f * 3.0f / 4.0f));
  //CFI
  CFI_code_word.resize(4, 32);
  CFI_code_word << 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
      1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0,
      1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  //PHICH
  PHICH_Orth_Seq_NormalCP.resize(8, 4);
  PHICH_Orth_Seq_NormalCP << 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f, 1.0f,
      cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
      cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f),
      cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f),
      cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, 1.0f);
  PHICH_Orth_Seq_ExtendedCP.resize(4, 2);
  PHICH_Orth_Seq_ExtendedCP << 1.0f, 1.0f,
      1.0f, -1.0f,
      cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
      cpxf(0.0f, 1.0f), cpxf(0.0f, -1.0f);
  //Corr
  Corr_MID_2 << 1.0f, 0.0f,
      0.3f, 0.953939201416946f;
  Corr_HIGH_2 << 1.0f, 0.0f,
      0.9f, 0.435889894354067f;
  Corr_MID_4 << 1.0f, 0.0f, 0.0f, 0.0f,
      0.033333333333333f, 0.999444290037663f, 0.0f, 0.0f,
      0.133333333333333f, 0.028904951658486f, 0.990649648458951f, 0.0f,
      0.300000000000000f, 0.123401909003538f, -0.010330183730341f, 0.945867462257995f;
  Corr_HIGH_4 << 1.0f, 0.0f, 0.0f, 0.0f,
      0.099999999500000f, 0.994987442182061f, 0.0f, 0.0f,
      0.399999998000000f, 0.060302269009965f, 0.914529194696511f, 0.0f,
      0.899999995500000f, 0.311561722045610f, -0.304843056725509f, 0.000149071197592f;
  //MLD Candidated_idxs
  MLD_Candidated_idxs_QPSK.resize(4, 4);
  for (i = 0; i < 4; i++) {
    MLD_Candidated_idxs_QPSK.col(i) << i,  // 0
        (i ^ 0b10), (i ^ 0b1),             // 1
        (i ^ 0b11);                        // 2
  }
  MLD_Candidated_idxs_16QAM.resize(16, 16);
  for (i = 0; i < 16; i++) {
    MLD_Candidated_idxs_16QAM.col(i) << i,                                               // 0
        (i ^ 0b1000), (i ^ 0b100), (i ^ 0b10), (i ^ 0b1),                                // 1
        (i ^ 0b11), (i ^ 0b101), (i ^ 0b110), (i ^ 0b1001), (i ^ 0b1010), (i ^ 0b1100),  // 2
        (i ^ 0b111), (i ^ 0b1011), (i ^ 0b1101), (i ^ 0b1110),                           // 3
        (i ^ 0b1111);                                                                    // 4
  }
  MLD_Candidated_idxs_64QAM.resize(64, 64);
  for (i = 0; i < 64; i++) {
    MLD_Candidated_idxs_64QAM.col(i) << i,                                                                                                                               // 0（1个）
        (i ^ 0b100000), (i ^ 0b10000), (i ^ 0b1000), (i ^ 0b100), (i ^ 0b10), (i ^ 0b1),                                                                                 // 1（6个）
        (i ^ 0b11), (i ^ 0b101), (i ^ 0b110), (i ^ 0b1001), (i ^ 0b1010), (i ^ 0b1100),                                                                                  // 2（15个）
        (i ^ 0b10001), (i ^ 0b10010), (i ^ 0b10100), (i ^ 0b11000),                                                                                                      // 2
        (i ^ 0b100001), (i ^ 0b100010), (i ^ 0b100100), (i ^ 0b101000), (i ^ 0b110000),                                                                                  // 2
        (i ^ 0b111), (i ^ 0b1011), (i ^ 0b1101), (i ^ 0b1110),                                                                                                           // 3（20个）
        (i ^ 0b10011), (i ^ 0b10101), (i ^ 0b10110), (i ^ 0b11001), (i ^ 0b11010), (i ^ 0b11100),                                                                        // 3
        (i ^ 0b100011), (i ^ 0b100101), (i ^ 0b100110), (i ^ 0b101001), (i ^ 0b101010), (i ^ 0b101100), (i ^ 0b110001), (i ^ 0b110010), (i ^ 0b110100), (i ^ 0b111000),  // 3
        (i ^ 0b1111), (i ^ 0b10111), (i ^ 0b11011), (i ^ 0b11101), (i ^ 0b11110),                                                                                        // 4（15个）
        (i ^ 0b100111), (i ^ 0b101011), (i ^ 0b110011), (i ^ 0b110101), (i ^ 0b111001), (i ^ 0b111010), (i ^ 0b111100),                                                  // 4
        (i ^ 0x39), (i ^ 0x3A), (i ^ 0x3C),                                                                                                                              // 4
        (i ^ 0b11111), (i ^ 0b101111), (i ^ 0b110111), (i ^ 0b111011), (i ^ 0b111101), (i ^ 0b111110),                                                                   // 5（6个）
        (i ^ 0b111111);                                                                                                                                                  // 6（1个）
  }
  //MLD_Candidated_idxs_256QAM.resize(256, 256);
}
