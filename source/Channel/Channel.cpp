/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Channel.cpp
    \brief Channel实现文件
    内容 ：Channel_C类相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Channel/Channel.h"

//! 多径信息获取
/*!
\param Channel_Specific ： 信道参数集合，输入/输出
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Get_Path_Info(Channel_Specific_S &Channel_Specific, const Cell_C &Cell) {
  int i = 0;
  int path_idx = 0;
  float sum_power = 0;
  float f = 0;
  float Sample_unit = (float)Cell.Cell_Specific.OverSample_unit;
  switch (Channel_Specific.ChannelType) {
    case AWGN:
      Channel_Specific.Path_Num = 1;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.InterDelay[0] = floor(Channel_Specific.Delay_t[0] / Sample_unit);
      Channel_Specific.FractionalDelay[0] = Channel_Specific.Delay_t[0] / Sample_unit - Channel_Specific.InterDelay[0];
      Channel_Specific.Relative_V[0] = 1.0f;
      Channel_Specific.InterpType = InterpHold;
      break;
    case EPA:
      Channel_Specific.Path_Num = 7;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[1] = 0.00000003f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[2] = 0.00000007f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[3] = 0.00000009f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[4] = 0.00000011f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[5] = 0.00000019f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[6] = 0.00000041f + Channel_Specific.TimeOffset;
      for (i = 0; i < Channel_Specific.Path_Num; i++) {
        Channel_Specific.InterDelay[i] = floor(Channel_Specific.Delay_t[i] / Sample_unit);
        Channel_Specific.FractionalDelay[i] = Channel_Specific.Delay_t[i] / Sample_unit - Channel_Specific.InterDelay[i];
      }
      sum_power = 1.0f + powf(10.0f, -1.0f / 10.0f) + powf(10.0f, -2.0f / 10.0f) + powf(10.0f, -3.0f / 10.0f) + powf(10.0f, -8.0f / 10.0f) + powf(10.0f, -17.2f / 10.0f) + powf(10.0f, -20.8f / 10.0f);
      Channel_Specific.Relative_V[0] = MySqrt(1.0f / sum_power);
      Channel_Specific.Relative_V[1] = MySqrt(powf(10.0f, -1.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[2] = MySqrt(powf(10.0f, -2.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[3] = MySqrt(powf(10.0f, -3.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[4] = MySqrt(powf(10.0f, -8.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[5] = MySqrt(powf(10.0f, -17.2f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[6] = MySqrt(powf(10.0f, -20.8f / 10.0f) / sum_power);
      Channel_Specific.InterpType = InterpCubic;
      break;
    case EVA:
      Channel_Specific.Path_Num = 9;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[1] = 0.00000003f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[2] = 0.00000015f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[3] = 0.00000031f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[4] = 0.00000037f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[5] = 0.00000071f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[6] = 0.00000109f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[7] = 0.00000173f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[8] = 0.00000251f + Channel_Specific.TimeOffset;
      for (i = 0; i < Channel_Specific.Path_Num; i++) {
        Channel_Specific.InterDelay[i] = floor(Channel_Specific.Delay_t[i] / Sample_unit);
        Channel_Specific.FractionalDelay[i] = Channel_Specific.Delay_t[i] / Sample_unit - Channel_Specific.InterDelay[i];
      }
      sum_power = 1.0f + powf(10.0f, -1.5f / 10.0f) + powf(10.0f, -1.4f / 10.0f) + powf(10.0f, -3.6f / 10.0f) + powf(10.0f, -0.6f / 10.0f) + powf(10.0f, -9.1f / 10.0f) + powf(10.0f, -7.0f / 10.0f) + powf(10.0f, -12.0f / 10.0f) + powf(10.0f, -16.9f / 10.0f);
      Channel_Specific.Relative_V[0] = MySqrt(1.0f / sum_power);
      Channel_Specific.Relative_V[1] = MySqrt(powf(10.0f, -1.5f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[2] = MySqrt(powf(10.0f, -1.4f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[3] = MySqrt(powf(10.0f, -3.6f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[4] = MySqrt(powf(10.0f, -0.6f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[5] = MySqrt(powf(10.0f, -9.1f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[6] = MySqrt(powf(10.0f, -7.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[7] = MySqrt(powf(10.0f, -12.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[8] = MySqrt(powf(10.0f, -16.9f / 10.0f) / sum_power);
      Channel_Specific.InterpType = InterpCubic;
      break;
    case ETU:
      Channel_Specific.Path_Num = 9;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[1] = 0.00000005f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[2] = 0.00000012f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[3] = 0.00000020f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[4] = 0.00000023f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[5] = 0.00000050f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[6] = 0.00000160f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[7] = 0.00000230f + Channel_Specific.TimeOffset;
      Channel_Specific.Delay_t[8] = 0.00000500f + Channel_Specific.TimeOffset;
      for (i = 0; i < Channel_Specific.Path_Num; i++) {
        Channel_Specific.InterDelay[i] = floor(Channel_Specific.Delay_t[i] / Sample_unit);
        Channel_Specific.FractionalDelay[i] = Channel_Specific.Delay_t[i] / Sample_unit - Channel_Specific.InterDelay[i];
      }
      sum_power = powf(10.0f, -1.0f / 10.0f) + powf(10.0f, -1.0f / 10.0f) + powf(10.0f, -1.0f / 10.0f) + 1.0f + 1.0f + 1.0f + powf(10.0f, -3.0f / 10.0f) + powf(10.0f, -5.0f / 10.0f) + powf(10.0f, -7.0f / 10.0f);
      Channel_Specific.Relative_V[0] = MySqrt(powf(10.0f, -1.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[1] = MySqrt(powf(10.0f, -1.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[2] = MySqrt(powf(10.0f, -1.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[3] = MySqrt(1.0f / sum_power);
      Channel_Specific.Relative_V[4] = MySqrt(1.0f / sum_power);
      Channel_Specific.Relative_V[5] = MySqrt(1.0f / sum_power);
      Channel_Specific.Relative_V[6] = MySqrt(powf(10.0f, -3.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[7] = MySqrt(powf(10.0f, -5.0f / 10.0f) / sum_power);
      Channel_Specific.Relative_V[8] = MySqrt(powf(10.0f, -7.0f / 10.0f) / sum_power);
      Channel_Specific.InterpType = InterpCubic;
      break;
    case HST:
      Channel_Specific.Path_Num = 1;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.InterDelay[0] = floor(Channel_Specific.Delay_t[0] / Sample_unit);
      Channel_Specific.FractionalDelay[0] = Channel_Specific.Delay_t[0] / Sample_unit - Channel_Specific.InterDelay[0];
      Channel_Specific.Relative_V[0] = 1.0f;
      Channel_Specific.InterpType = InterpHold;
    case HST_SFN:
      Channel_Specific.Path_Num = 4;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V[0] = 1.0f;
      Channel_Specific.Relative_V[1] = 1.0f;
      Channel_Specific.Relative_V[2] = 1.0f;
      Channel_Specific.Relative_V[3] = 1.0f;
      Channel_Specific.InterpType = InterpHold;
      break;
    case B_2_4:
      Channel_Specific.Path_Num = 2;
      Channel_Specific.Delay_t.resize(Channel_Specific.Path_Num);
      Channel_Specific.InterDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.FractionalDelay.resize(Channel_Specific.Path_Num);
      Channel_Specific.Relative_V.resize(Channel_Specific.Path_Num);
      Channel_Specific.Delay_t[0] = Channel_Specific.TimeOffset;
      Channel_Specific.InterDelay[0] = floor(Channel_Specific.Delay_t[0] / Sample_unit);
      Channel_Specific.FractionalDelay[0] = Channel_Specific.Delay_t[0] / Sample_unit - Channel_Specific.InterDelay[0];
      Channel_Specific.Relative_V[0] = MySqrt(1.0f / (1.0f + Channel_Specific.a * Channel_Specific.a));
      Channel_Specific.Delay_t[1] = Channel_Specific.TimeOffset + Channel_Specific.tao_d;
      Channel_Specific.InterDelay[1] = floor(Channel_Specific.Delay_t[1] / Sample_unit);
      Channel_Specific.FractionalDelay[1] = Channel_Specific.Delay_t[1] / Sample_unit - Channel_Specific.InterDelay[1];
      Channel_Specific.Relative_V[1] = MySqrt(Channel_Specific.a / (1.0f + Channel_Specific.a * Channel_Specific.a));
      Channel_Specific.InterpType = InterpHold;
  }
  //Channel_Specific.Path_Num = 1;
  //Channel_Specific.InterpType = InterpHold;
  Channel_Specific.FractionalDelayCoef.resize(Channel_Specific.Path_Num);
  for (i = 0; i < Channel_Specific.Path_Num; i++) {
    f = Channel_Specific.FractionalDelay[i];
    if (InterpLinear == Channel_Specific.InterpType) {
      Channel_Specific.FractionalDelayCoef[i].resize(2);
      Channel_Specific.FractionalDelayCoef[i][0] = 1.0f - f;  // x(n)
      Channel_Specific.FractionalDelayCoef[i][1] = f;         // x(n+1)
    } else if (InterpCubic == Channel_Specific.InterpType) {
      Channel_Specific.FractionalDelayCoef[i].resize(4);
      Channel_Specific.FractionalDelayCoef[i][0] = -f * f * f / 6.0f + f * f / 2.0f - f / 3.0f;  // x(n-1)
      Channel_Specific.FractionalDelayCoef[i][1] = f * f * f / 2.0f - f * f - f / 2.0f + 1.0f;   // x(n)
      Channel_Specific.FractionalDelayCoef[i][2] = -f * f * f / 2.0f + f * f / 2.0f + f;         // x(n+1)
      Channel_Specific.FractionalDelayCoef[i][3] = f * f * f / 6.0f - f / 6.0f;                  // x(n+2)
    }
  }
  Channel_Specific.MaxDelay = 0;
  Channel_Specific.MinDelay = 10000;
  for (path_idx = 0; path_idx < Channel_Specific.Path_Num; path_idx++) {
    if (Channel_Specific.InterDelay[path_idx] > Channel_Specific.MaxDelay) {
      Channel_Specific.MaxDelay = Channel_Specific.InterDelay[path_idx];
    }
    if (Channel_Specific.InterDelay[path_idx] < Channel_Specific.MinDelay) {
      Channel_Specific.MinDelay = Channel_Specific.InterDelay[path_idx];
    }
  }
  if (InterpLinear == Channel_Specific.InterpType) {
    Channel_Specific.MaxDelay += 1;
  } else if (InterpCubic == Channel_Specific.InterpType) {
    Channel_Specific.MinDelay -= 1;
    Channel_Specific.MaxDelay += 2;
  }
  Channel_Specific.SNR_Linear_Relative_V.resize(Channel_Specific.Path_Num);
  Channel_Specific.Phase.resize(Channel_Specific.Path_Num);
  Channel_Specific.InterDelay_Dyn.resize(Channel_Specific.Path_Num);
  for (path_idx = 0; path_idx < Channel_Specific.Path_Num; path_idx++) {
    Channel_Specific.SNR_Linear_Relative_V(path_idx) = powf(10.0f, Channel_Specific.SNR / 20.0f) * Channel_Specific.Relative_V[path_idx];
    Channel_Specific.Phase[path_idx] = 0.0f;
    Channel_Specific.InterDelay_Dyn[path_idx].resize(Cell.Cell_Specific.OverSample_Num);
  }
  Channel_Specific.MaxPathDataLen = (Cell.Cell_Specific.OverSample_Num + MAXDELAYSAMPLES + MAXTASAMPLES);
  Channel_Specific.h_buffer.resize(Channel_Specific.MaxPathDataLen * Channel_Specific.Path_Num);
  Channel_Specific.TxNum = Cell.Cell_Specific.CRSPortNum;
  for (i = 0; i < (Channel_Specific.MaxPathDataLen * Channel_Specific.Path_Num); i++) {
    Channel_Specific.h_buffer[i].resize(Channel_Specific.RxNum, Channel_Specific.TxNum);
  }
  Channel_Specific.InitPhase.resize(Channel_Specific.RxNum, Channel_Specific.TxNum);
  Channel_Specific.InitPhase.setConstant(1.0f);
  if (2 == Channel_Specific.RxNum && 2 == Channel_Specific.TxNum) {
    Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f),
        cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f);
  } else if (2 == Channel_Specific.RxNum && 4 == Channel_Specific.TxNum) {
    Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
        cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f);
  } else if (2 == Channel_Specific.RxNum && 8 == Channel_Specific.TxNum) {
    Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
        cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f);
  }
#if IDEAL_H_METHOD > 0
  Channel_Specific.Ideal_h.resize(2 * Cell.Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI);
  for (i = 0; i < (2 * Cell.Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
    Channel_Specific.Ideal_h[i].resize(Channel_Specific.RxNum, MAX_PHY_ANTENNA_NUM);
    Channel_Specific.Ideal_h[i].setZero();
  }
#endif
}

//! 由于调度导致天线数可能变化，动态多径信息获取
/*!
  \param Channel_Specific ： 信道参数集合，输入/输出
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Get_Path_Info_Dyn(Channel_Specific_S &Channel_Specific, const Cell_C &Cell) {
  int i = 0;
  if (Cell.Cell_Specific.TxNum != Channel_Specific.TxNum) {
    Channel_Specific.TxNum = Cell.Cell_Specific.TxNum;
    for (i = 0; i < (Channel_Specific.MaxPathDataLen * Channel_Specific.Path_Num); i++) {
      Channel_Specific.h_buffer[i].resize(Channel_Specific.RxNum, Channel_Specific.TxNum);
    }
    Channel_Specific.InitPhase.resize(Channel_Specific.RxNum, Channel_Specific.TxNum);
    Channel_Specific.InitPhase.setConstant(1.0f);
    if (2 == Channel_Specific.RxNum && 2 == Channel_Specific.TxNum) {
      Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f),
          cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f);
    } else if (2 == Channel_Specific.RxNum && 4 == Channel_Specific.TxNum) {
      Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
          cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f);
    } else if (2 == Channel_Specific.RxNum && 8 == Channel_Specific.TxNum) {
      Channel_Specific.InitPhase << cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f), cpxf(0.0f, 1.0f),
          cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(1.0f, 0.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f), cpxf(0.0f, -1.0f);
    }
  }
}

//! Channel_C类构造函数（主要Jake's模型相关参数产生），出自：https://en.wikipedia.org/wiki/Rayleigh_fading, DENT model for Jake's
/*!
  \return 无
*/
Channel_C::Channel_C() {
  float alpha, beta;
  int k, n;
  long int seed_local = 0;
  fmat A;
  Hadamard(A, JAKES_MODEL_M);
  Combined_A.resize(JAKES_MODEL_M, JAKES_MODEL_M);
  Cos_Alpha.resize(JAKES_MODEL_M);
  Ceta.resize(JAKES_MODEL_M);
  for (k = 0; k < JAKES_MODEL_M; k++) {
    for (n = 0; n < JAKES_MODEL_M; n++) {
      beta = PI * n / (float)JAKES_MODEL_M;
      Combined_A(k, n) = MySqrt(2.0f / (float)JAKES_MODEL_M) * A(k, n) * MyExpCpx(beta);
    }
  }
  for (n = 0; n < JAKES_MODEL_M; n++) {
    alpha = PI * (n - 0.5f) / 2.0f / (float)JAKES_MODEL_M;
    Cos_Alpha(n) = (double)MyCos(alpha);
    //Ceta(n) = uniform(0.0f, 2.0f * PI, &seed_local);
    Ceta(n) = (double)uniform(0.0f, 2.0f * PI, &seed_local) + 0.25f;
  }
}

//! Channel_C类析构函数
/*!
  \return 无
*/
Channel_C::~Channel_C() {
}

//! Channel_C类初始化函数
/*!
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Channel_C::InitParas(const vector<Cell_C> &Cell) {
  int cell_idx = 0;
  int ue_cell_idx = 0;
  for (ue_cell_idx = 0; ue_cell_idx < (int)Channel_Specific.size(); ue_cell_idx++) {
    cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
    Get_Path_Info(Channel_Specific[ue_cell_idx], Cell[cell_idx]);
    Channel_Specific[ue_cell_idx].last_n_subframe = Simuctrl.start_n_subframe - 1;
    Channel_Specific[ue_cell_idx].InstantFirst = true;
  }
  RxData.resize(Channel_Specific[0].RxNum, MAXDELAYSAMPLES + MAXTASAMPLES + 2 * Cell[0].Cell_Specific.OverSample_Num);
  RxData.setZero();
}

//! 每TTI Channel参数更新函数
/*!
  \param n_subframe ： 子帧序号，输入
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Channel_C::UpdataParas(int n_subframe, const vector<Cell_C> &Cell) {
  int cell_idx = 0;
  int ue_cell_idx = 0;
  Simuctrl.n_subframe = n_subframe;
  for (ue_cell_idx = 0; ue_cell_idx < (int)Channel_Specific.size(); ue_cell_idx++) {
    cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
    Channel_Specific[ue_cell_idx].DataLen = Cell[cell_idx].Cell_Specific.OverSample_DataLen;
    Get_Path_Info_Dyn(Channel_Specific[ue_cell_idx], Cell[cell_idx]);
  }
}

//! 每TTI多径信号h获取函数
/*!
  \param ue_cell_idx ： 当前UE连接的小区序号，输入
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Channel_C::Get_h_buffer(int ue_cell_idx, const Cell_C &Cell) {
  int s = 0;
  int path_idx = 0;
  int idx_offset = 0;
  double t_offset = 0.0;
  double t = 0.0;
  int r = 0;
  int p = 0;
  int n = 0;
  double temp = 0.0;
  double fd = 0.0;
  cfvec MyCos_value(JAKES_MODEL_M);
  float cmp_value = 0.0f;
  float mod_value = 0.0f;
  float f1 = 0.0f;
  float f2 = 0.0f;
  float f3 = 0.0f;
  float f4 = 0.0f;
  float t_mod = 0.0f;
  cpxf c_temp[4] = {0.0f};
  float y = 0.0f;
  float x1 = 0.0f;
  float x2 = 0.0f;
  float x3 = 0.0f;
  float x4 = 0.0f;
  float p1 = 0.0f;
  float p2 = 0.0f;
  float p3 = 0.0f;
  float p4 = 0.0f;
  float sum_p = 0.0f;
  float t1 = 0.0f;
  float t2 = 0.0f;
  float t3 = 0.0f;
  float t4 = 0.0f;
  ivec Path_Offset;
  if (HST == Channel_Specific[ue_cell_idx].ChannelType) {
    t = (double)Channel_Specific[ue_cell_idx].StartTime + (double)(Channel_Specific[ue_cell_idx].last_n_subframe + 1) * 0.001;
    cmp_value = Channel_Specific[ue_cell_idx].Ds / Channel_Specific[ue_cell_idx].Speed;
    mod_value = 2.0f * cmp_value;
    for (s = 0; s < Cell.Cell_Specific.OverSample_Num * (Simuctrl.n_subframe - Channel_Specific[ue_cell_idx].last_n_subframe - 1); s++) {
      t_mod = fmodf((float)t, mod_value);
      if (t_mod <= cmp_value) {
        temp = 0.5f * Channel_Specific[ue_cell_idx].Ds - Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      } else {
        temp = -1.5f * Channel_Specific[ue_cell_idx].Ds + Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      }
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    idx_offset = MAXTASAMPLES;
    for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
      t_mod = fmodf((float)t, mod_value);
      if (t_mod <= cmp_value) {
        temp = 0.5f * Channel_Specific[ue_cell_idx].Ds - Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      } else {
        temp = -1.5f * Channel_Specific[ue_cell_idx].Ds + Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      }
      c_temp[0] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(0) * MyExpCpx_2PI(Channel_Specific[ue_cell_idx].Phase[0]);
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + s] = c_temp[0] * Channel_Specific[ue_cell_idx].InitPhase;
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    for (s = Channel_Specific[ue_cell_idx].DataLen; s < Cell.Cell_Specific.OverSample_Num; s++) {
      t_mod = fmodf((float)t, mod_value);
      if (t_mod <= cmp_value) {
        temp = 0.5f * Channel_Specific[ue_cell_idx].Ds - Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      } else {
        temp = -1.5f * Channel_Specific[ue_cell_idx].Ds + Channel_Specific[ue_cell_idx].Speed * t_mod;
        f1 = Channel_Specific[ue_cell_idx].DopplerShift * (temp / MySqrt(Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin + temp * temp));
      }
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    Channel_Specific[ue_cell_idx].last_n_subframe = Simuctrl.n_subframe;
  } else if (HST_SFN == Channel_Specific[ue_cell_idx].ChannelType)  //R4-1610696, 4 tap
  {
    x1 = 0.0f;
    x2 = Channel_Specific[ue_cell_idx].Ds;
    x3 = 2.0f * Channel_Specific[ue_cell_idx].Ds;
    x4 = 3.0f * Channel_Specific[ue_cell_idx].Ds;
    t = (double)Channel_Specific[ue_cell_idx].StartTime + (double)(Channel_Specific[ue_cell_idx].last_n_subframe + 1) * 0.001;
    for (s = 0; s < Cell.Cell_Specific.OverSample_Num * (Simuctrl.n_subframe - Channel_Specific[ue_cell_idx].last_n_subframe - 1); s++) {
      y = Channel_Specific[ue_cell_idx].Speed * t;
      p1 = (y - x1) * (y - x1) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p2 = (y - x2) * (y - x2) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p3 = (y - x3) * (y - x3) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p4 = (y - x4) * (y - x4) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      f1 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x1) * MySqrt_Recip(p1);
      f2 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x2) * MySqrt_Recip(p2);
      f3 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x3) * MySqrt_Recip(p3);
      f4 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x4) * MySqrt_Recip(p4);
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[1] += ((double)f2 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[2] += ((double)f3 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[3] += ((double)f4 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    idx_offset = MAXTASAMPLES;
    for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
      y = Channel_Specific[ue_cell_idx].Speed * t;
      p1 = (y - x1) * (y - x1) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p2 = (y - x2) * (y - x2) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p3 = (y - x3) * (y - x3) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p4 = (y - x4) * (y - x4) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      sum_p = 1.0f / p1 + 1.0f / p2 + 1.0f / p3 + 1.0f / p4;
      f1 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x1) * MySqrt_Recip(p1);
      f2 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x2) * MySqrt_Recip(p2);
      f3 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x3) * MySqrt_Recip(p3);
      f4 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x4) * MySqrt_Recip(p4);
      t1 = (MySqrt(p1) - Channel_Specific[ue_cell_idx].Dmin) / 3.0e8f;
      t2 = (MySqrt(p2) - Channel_Specific[ue_cell_idx].Dmin) / 3.0e8f;
      t3 = (MySqrt(p3) - Channel_Specific[ue_cell_idx].Dmin) / 3.0e8f;
      t4 = (MySqrt(p4) - Channel_Specific[ue_cell_idx].Dmin) / 3.0e8f;
      p1 = MySqrt_Recip(p1 * sum_p);
      p2 = MySqrt_Recip(p2 * sum_p);
      p3 = MySqrt_Recip(p3 * sum_p);
      p4 = MySqrt_Recip(p4 * sum_p);
      c_temp[0] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(0) * p1 * MyExpCpx_2PI(Channel_Specific[ue_cell_idx].Phase[0]);
      c_temp[1] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(1) * p2 * MyExpCpx_2PI(Channel_Specific[ue_cell_idx].Phase[1]);
      c_temp[2] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(2) * p3 * MyExpCpx_2PI(Channel_Specific[ue_cell_idx].Phase[2]);
      c_temp[3] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(3) * p4 * MyExpCpx_2PI(Channel_Specific[ue_cell_idx].Phase[3]);
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + s] = c_temp[0] * Channel_Specific[ue_cell_idx].InitPhase;
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + Channel_Specific[ue_cell_idx].MaxPathDataLen + s] = c_temp[1] * Channel_Specific[ue_cell_idx].InitPhase;
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + 2 * Channel_Specific[ue_cell_idx].MaxPathDataLen + s] = c_temp[2] * Channel_Specific[ue_cell_idx].InitPhase;
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + 3 * Channel_Specific[ue_cell_idx].MaxPathDataLen + s] = c_temp[3] * Channel_Specific[ue_cell_idx].InitPhase;
      Channel_Specific[ue_cell_idx].InterDelay_Dyn[0][s] = round(t1 / (float)Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].InterDelay_Dyn[1][s] = round(t2 / (float)Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].InterDelay_Dyn[2][s] = round(t3 / (float)Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].InterDelay_Dyn[3][s] = round(t4 / (float)Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[1] += ((double)f2 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[2] += ((double)f3 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[3] += ((double)f4 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    for (s = Channel_Specific[ue_cell_idx].DataLen; s < Cell.Cell_Specific.OverSample_Num; s++) {
      y = Channel_Specific[ue_cell_idx].Speed * t;
      p1 = (y - x1) * (y - x1) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p2 = (y - x2) * (y - x2) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p3 = (y - x3) * (y - x3) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      p4 = (y - x4) * (y - x4) + Channel_Specific[ue_cell_idx].Dmin * Channel_Specific[ue_cell_idx].Dmin;
      f1 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x1) * MySqrt_Recip(p1);
      f2 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x2) * MySqrt_Recip(p2);
      f3 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x3) * MySqrt_Recip(p3);
      f4 = -Channel_Specific[ue_cell_idx].DopplerShift * (y - x4) * MySqrt_Recip(p4);
      Channel_Specific[ue_cell_idx].Phase[0] += ((double)f1 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[1] += ((double)f2 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[2] += ((double)f3 * Cell.Cell_Specific.OverSample_unit);
      Channel_Specific[ue_cell_idx].Phase[3] += ((double)f4 * Cell.Cell_Specific.OverSample_unit);
      t += Cell.Cell_Specific.OverSample_unit;
    }
    Channel_Specific[ue_cell_idx].last_n_subframe = Simuctrl.n_subframe;
  } else if (B_2_4 == Channel_Specific[ue_cell_idx].ChannelType) {
    t = (double)Channel_Specific[ue_cell_idx].StartTime + (double)Simuctrl.n_subframe * 0.001;
    idx_offset = MAXTASAMPLES;
    for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + s] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(0) * Channel_Specific[ue_cell_idx].InitPhase;
      c_temp[0] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(1) * MyExpCpx_2PI(-Channel_Specific[ue_cell_idx].DopplerShift * t);
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + Channel_Specific[ue_cell_idx].MaxPathDataLen + s] = c_temp[0] * Channel_Specific[ue_cell_idx].InitPhase;
      t += Cell.Cell_Specific.OverSample_unit;
    }
  } else if (AWGN == Channel_Specific[ue_cell_idx].ChannelType) {
    path_idx = 0;
    idx_offset = Channel_Specific[ue_cell_idx].MinDelay + MAXTASAMPLES;
    Channel_Specific[ue_cell_idx].h_buffer[idx_offset] = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(path_idx) * Channel_Specific[ue_cell_idx].InitPhase;
    for (s = 1; s < (Channel_Specific[ue_cell_idx].MaxDelay - Channel_Specific[ue_cell_idx].MinDelay + Channel_Specific[ue_cell_idx].DataLen); s++) {
      Channel_Specific[ue_cell_idx].h_buffer[idx_offset + s] = Channel_Specific[ue_cell_idx].h_buffer[idx_offset];
    }
  } else {
    Path_Offset.resize(Channel_Specific[ue_cell_idx].Path_Num);
    for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
      Path_Offset(path_idx) = path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + Channel_Specific[ue_cell_idx].MinDelay + MAXTASAMPLES;
    }
    fd = (double)Channel_Specific[ue_cell_idx].DopplerShift;
    for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
      t_offset = (double)ue_cell_idx * 80.0 + (double)Simuctrl.n_subframe * 0.001 + (double)r * 50.0 + (double)Channel_Specific[ue_cell_idx].InstantOffset;
      for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
        t_offset += (double)p * 20.0;
        t = t_offset + (double)Channel_Specific[ue_cell_idx].MinDelay * Cell.Cell_Specific.OverSample_unit;
        for (s = 0; s < (Channel_Specific[ue_cell_idx].MaxDelay - Channel_Specific[ue_cell_idx].MinDelay + Channel_Specific[ue_cell_idx].DataLen); s++) {
          temp = fd * t;
          for (n = 0; n < JAKES_MODEL_M; n++) {
            MyCos_value(n) = Combined_A(0, n) * MySin_2PI((float)(temp * Cos_Alpha(n) + Ceta(n)));
          }
          FHT(MyCos_value, LOG2_JAKES_MODEL_M);
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            Channel_Specific[ue_cell_idx].h_buffer[Path_Offset(path_idx) + s](r, p) = Channel_Specific[ue_cell_idx].SNR_Linear_Relative_V(path_idx) * MyCos_value(path_idx);
          }
          t += Cell.Cell_Specific.OverSample_unit;
        }
      }
    }
    /*for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++)
        {
            for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++)
            {
                for (s = 0; s < (Channel_Specific[ue_cell_idx].MaxDelay - Channel_Specific[ue_cell_idx].MinDelay + Channel_Specific[ue_cell_idx].DataLen); s++)
                {
                    for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++)
                    {
                        //Channel_Specific[ue_cell_idx].h_buffer[Path_Offset(path_idx) + s](r, p) = Channel_Specific[ue_cell_idx].h_buffer[Path_Offset(path_idx) + 0](0, 0) * Channel_Specific[ue_cell_idx].InitPhase(r, p);
                        Channel_Specific[ue_cell_idx].h_buffer[Path_Offset(path_idx) + s](r, p) = Channel_Specific[ue_cell_idx].h_buffer[Path_Offset(path_idx) + 0](r, p);
                    }
                }
            }
        }*/
  }
}

//! 无线信道处理函数，Y=HX
/*!
  \param Cell ： 连接的小区，输入
  \return 无
*/
void Channel_C::Process(const vector<Cell_C> &Cell) {
  int cell_idx = 0;
  int ue_cell_idx = 0;
  int s = 0;
  int r = 0;
  int p = 0;
  int path_idx = 0;
  int s_prime = 0;
  int h_s_prime[4] = {0};
  int data_s_prime[4] = {0};
  ivec Path_Offset_1[4], Path_Offset_2[4];
  cfvec Tx_temp(4);
#if IDEAL_H_METHOD > 0
  int l = 0;
  int i = 0;
#endif
  for (r = 0; r < Channel_Specific[0].RxNum; r++) {
    RxData.row(r).segment(0, MAXDELAYSAMPLES + MAXTASAMPLES + Cell[0].Cell_Specific.OverSample_Num) = RxData.row(r).segment(Cell[0].Cell_Specific.OverSample_Num, MAXDELAYSAMPLES + MAXTASAMPLES + Cell[0].Cell_Specific.OverSample_Num);
    RxData.row(r).segment(MAXDELAYSAMPLES + MAXTASAMPLES + Cell[0].Cell_Specific.OverSample_Num, Cell[0].Cell_Specific.OverSample_Num).setZero();
  }
  for (ue_cell_idx = 0; ue_cell_idx < (int)Channel_Specific.size(); ue_cell_idx++) {
    cell_idx = UE_Connect_Cell_idx[ue_cell_idx];
    if (Channel_Specific[ue_cell_idx].DataLen > 0) {
      if (!Channel_Specific[ue_cell_idx].InstantFlag || Channel_Specific[ue_cell_idx].InstantFirst) {
        Get_h_buffer(ue_cell_idx, Cell[cell_idx]);
        Channel_Specific[ue_cell_idx].InstantFirst = false;
      }
      Path_Offset_1[0].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_1[1].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_1[2].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_1[3].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_2[0].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_2[1].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_2[2].resize(Channel_Specific[ue_cell_idx].Path_Num);
      Path_Offset_2[3].resize(Channel_Specific[ue_cell_idx].Path_Num);
      if (InterpHold == Channel_Specific[ue_cell_idx].InterpType && HST_SFN == Channel_Specific[ue_cell_idx].ChannelType) {
        for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
          s_prime = MAXTASAMPLES + s;
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            data_s_prime[0] = s_prime + Channel_Specific[ue_cell_idx].InterDelay_Dyn[path_idx][s] + Cell[cell_idx].Cell_Specific.OverSample_Num;
            h_s_prime[0] = s_prime + Channel_Specific[ue_cell_idx].InterDelay_Dyn[path_idx][s] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                RxData(r, data_s_prime[0]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].BB(p, s);
              }
            }
          }
        }
#if IDEAL_H_METHOD > 0
        for (i = 0; i < (Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
          Channel_Specific[ue_cell_idx].Ideal_h[i] = Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI];
          Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI].setZero();
        }
        for (l = 0; l < Cell[cell_idx].IdealSamplePosNum; l++) {
          s = Cell[cell_idx].IdealSamplePos[l];
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            s_prime = (s + Channel_Specific[ue_cell_idx].InterDelay_Dyn[path_idx][s] + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[0] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * l;
            h_s_prime[0] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay_Dyn[path_idx][s] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[0]](r, p) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].Ideal_h(p, l);
              }
            }
          }
        }
#endif
      } else if (InterpHold == Channel_Specific[ue_cell_idx].InterpType) {
        for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
          Path_Offset_1[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num;
          Path_Offset_2[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
        }
        for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            data_s_prime[0] = s + Path_Offset_1[0](path_idx);
            h_s_prime[0] = s + Path_Offset_2[0](path_idx);
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                RxData(r, data_s_prime[0]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].BB(p, s);
              }
            }
          }
        }
#if IDEAL_H_METHOD > 0
        for (i = 0; i < (Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
          Channel_Specific[ue_cell_idx].Ideal_h[i] = Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI];
          Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI].setZero();
        }
        for (l = 0; l < Cell[cell_idx].IdealSamplePosNum; l++) {
          s = Cell[cell_idx].IdealSamplePos[l];
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[0] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[0] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[0]](r, p) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].Ideal_h(p, l);
              }
            }
          }
        }
#endif
      } else if (InterpLinear == Channel_Specific[ue_cell_idx].InterpType) {
        for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
          Path_Offset_1[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num;
          Path_Offset_1[1](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num + 1;
          Path_Offset_2[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
          Path_Offset_2[1](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 1;
        }
        for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            data_s_prime[0] = s + Path_Offset_1[0](path_idx);
            h_s_prime[0] = s + Path_Offset_2[0](path_idx);
            data_s_prime[1] = s + Path_Offset_1[1](path_idx);
            h_s_prime[1] = s + Path_Offset_2[1](path_idx);
            for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
              Tx_temp(0) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][0] * Cell[cell_idx].BB(p, s);
              Tx_temp(1) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][1] * Cell[cell_idx].BB(p, s);
              for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
                RxData(r, data_s_prime[0]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Tx_temp(0);
                RxData(r, data_s_prime[1]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[1]](r, p) * Tx_temp(1);
              }
            }
          }
        }
#if IDEAL_H_METHOD > 0
        for (i = 0; i < (Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
          Channel_Specific[ue_cell_idx].Ideal_h[i] = Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI];
          Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI].setZero();
        }
        for (l = 0; l < Cell[cell_idx].IdealSamplePosNum; l++) {
          s = Cell[cell_idx].IdealSamplePos[l];
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[0] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[0] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + 1 + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[1] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[1] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 1;
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[0]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][0] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].Ideal_h(p, l);
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[1]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][1] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[1]](r, p) * Cell[cell_idx].Ideal_h(p, l);
              }
            }
          }
        }
#endif
      } else {
        for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
          Path_Offset_1[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num - 1;
          Path_Offset_1[1](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num;
          Path_Offset_1[2](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num + 1;
          Path_Offset_1[3](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_Num + 2;
          Path_Offset_2[0](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen - 1;
          Path_Offset_2[1](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
          Path_Offset_2[2](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 1;
          Path_Offset_2[3](path_idx) = MAXTASAMPLES + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 2;
        }
        for (s = 0; s < Channel_Specific[ue_cell_idx].DataLen; s++) {
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            data_s_prime[0] = s + Path_Offset_1[0](path_idx);
            h_s_prime[0] = s + Path_Offset_2[0](path_idx);
            data_s_prime[1] = s + Path_Offset_1[1](path_idx);
            h_s_prime[1] = s + Path_Offset_2[1](path_idx);
            data_s_prime[2] = s + Path_Offset_1[2](path_idx);
            h_s_prime[2] = s + Path_Offset_2[2](path_idx);
            data_s_prime[3] = s + Path_Offset_1[3](path_idx);
            h_s_prime[3] = s + Path_Offset_2[3](path_idx);
            for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
              Tx_temp(0) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][0] * Cell[cell_idx].BB(p, s);
              Tx_temp(1) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][1] * Cell[cell_idx].BB(p, s);
              Tx_temp(2) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][2] * Cell[cell_idx].BB(p, s);
              Tx_temp(3) = Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][3] * Cell[cell_idx].BB(p, s);
              for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
                RxData(r, data_s_prime[0]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Tx_temp(0);
                RxData(r, data_s_prime[1]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[1]](r, p) * Tx_temp(1);
                RxData(r, data_s_prime[2]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[2]](r, p) * Tx_temp(2);
                RxData(r, data_s_prime[3]) += Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[3]](r, p) * Tx_temp(3);
              }
            }
          }
        }
#if IDEAL_H_METHOD > 0
        for (i = 0; i < (Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI); i++) {
          Channel_Specific[ue_cell_idx].Ideal_h[i] = Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI];
          Channel_Specific[ue_cell_idx].Ideal_h[i + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * MAX_SYMBNUM_TTI].setZero();
        }
        for (l = 0; l < Cell[cell_idx].IdealSamplePosNum; l++) {
          s = Cell[cell_idx].IdealSamplePos[l];
          for (path_idx = 0; path_idx < Channel_Specific[ue_cell_idx].Path_Num; path_idx++) {
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] - 1 + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[0] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[0] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen - 1;
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[1] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[1] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen;
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + 1 + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[2] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[2] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 1;
            s_prime = (Channel_Specific[ue_cell_idx].InterDelay[path_idx] + 2 + Cell[cell_idx].Cell_Specific.OverSample_N_FFT) % Cell[cell_idx].Cell_Specific.OverSample_N_FFT;
            data_s_prime[3] = s_prime + Cell[cell_idx].Cell_Specific.OverSample_N_FFT * (l + MAX_SYMBNUM_TTI);
            h_s_prime[3] = MAXTASAMPLES + s + Channel_Specific[ue_cell_idx].InterDelay[path_idx] + path_idx * Channel_Specific[ue_cell_idx].MaxPathDataLen + 2;
            for (r = 0; r < Channel_Specific[ue_cell_idx].RxNum; r++) {
              for (p = 0; p < Channel_Specific[ue_cell_idx].TxNum; p++) {
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[0]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][0] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[0]](r, p) * Cell[cell_idx].Ideal_h(p, l);
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[1]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][1] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[1]](r, p) * Cell[cell_idx].Ideal_h(p, l);
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[2]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][2] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[2]](r, p) * Cell[cell_idx].Ideal_h(p, l);
                Channel_Specific[ue_cell_idx].Ideal_h[data_s_prime[3]](r, p) += Channel_Specific[ue_cell_idx].FractionalDelayCoef[path_idx][3] * Channel_Specific[ue_cell_idx].h_buffer[h_s_prime[3]](r, p) * Cell[cell_idx].Ideal_h(p, l);
              }
            }
          }
        }
#endif
      }
    }
  }
}
