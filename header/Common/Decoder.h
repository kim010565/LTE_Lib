/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Decoder.h
    \brief Decoder头文件
    内容 ：译码相关函数声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Decoder_h_
#define _Decoder_h_

#include "Encoder.h"

//! TBCC生成多项式个数
const int POLYNOM_NUM = 3;
//! TBCC译码迭代次数
const int MAX_TBCC_ITER_NUM = 5;
//! Turbo译码迭代次数
const int MAX_TURBO_ITER_NUM = 8;
//! Turbo译码无穷大值定义
const float TURBO_INFINITY = 32767.0f;
//! Turbo译码信息缩放比例
const float TURBO_SCALE = 0.75f;
//! Turbo状态数
const int Turbo_NumStates = 8;  //状态数
//! Turbo尾bit长度
const int tail_bits_length = 4;  //尾bit长度

//! TBCC Trellis图
typedef struct
{
  bin input;                    //!< 当前输入bit 0 or 1
  int pre_state[2];             //!< 转移到当前状态前，两种可能的上次状态
  int8 output[POLYNOM_NUM][2];  //!< 两种可能的上次状态，对应的各parity输出bit
} Trellis_S;

//! Turbo 后向Trellis图
typedef struct
{
  bin input[2];      //!< 由于存在反馈，对应两种可能的上次状态，存在两种当前输入bit 0 or 1（与TBCC不同）
  int pre_state[2];  //!< 转移到当前状态前，两种可能的上次状态
  int8 output[2];    //!< 两种可能的上次状态，对应的输出bit。Turbo其中一个卷积器的话，码率=1，所以没有POLYNOM_NUM
} Trellis_Backward_S;

//! Turbo 前向Trellis图
typedef struct
{
  bin input[2];       //!< 两种输入bit 0 or 1
  int next_state[2];  //!< 对应两种输入bit下的下次状态
  int8 output[2];     //!< 对应两种输入bit下，对应的输出bit。Turbo其中一个卷积器的话，码率=1，所以没有POLYNOM_NUM
} Trellis_Forward_S;

bool CheckCRC(Crc_E crc_enum, const bvec &data_in);

extern vector<Trellis_S> trellis;
void Viterbi_Trellis_Gen();
void WAVA_Viterbi(const fmat &LLR, bvec &bits);

extern ivec interleaver_patten;
extern fvec P_priori_P1;
extern fvec P_priori_P2;
extern fmat P_alpha;
extern fmat P_beta;
extern fmat P_gamma_P1_0;
extern fmat P_gamma_P1_1;
extern fmat P_gamma_P2_0;
extern fmat P_gamma_P2_1;
extern fvec llr_output;
extern vector<Trellis_Backward_S> trellis_backward;
extern vector<Trellis_Forward_S> trellis_forward;
void MaxLogMAP(fvec &P_priori,
               const fmat &P_gamma_0,
               const fmat &P_gamma_1,
               const fvec &LLR_sys,
               int total_bits_length,
               fvec &llr,
               const vector<Trellis_Forward_S> &trellis_forward,
               const vector<Trellis_Backward_S> &trellis_backward);
void Turbo_Trellis_Gen();
bool Turbo_Decoder(const fmat &LLR, bvec &bits, bool cb_crc_flag);

void DeCB_Segmentation_CB_CRC_Attachment(const bvecs &c, bvec &b, int B);

int Calc_CBSize(int B, ivec &K_r, int *F);

void DeRate_Matching_Turbo(const fvec &data_in,
                           int D,
                           int N_IR,
                           bool DL_SCH_PCH_Flag,
                           int C,
                           int rv_idx,
                           int F,
                           fmat &data_out);

void DeRate_Matching_TBCC(const fvec &data_in,
                          int D,
                          fmat &data_out);

#endif
