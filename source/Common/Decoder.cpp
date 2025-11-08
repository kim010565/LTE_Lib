/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Decoder.cpp
    \brief Decoder实现文件
    内容 ：译码相关函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Decoder.h"

//! CRC校验
/*!
  \param crc_enum ： CRC类型，输入
  \param data_in ： 二进制列向量，输入
  \return CRC校验结果，bool型
*/
bool CheckCRC(Crc_E crc_enum, const bvec &data_in)  //CRC校验
{
  int k;
  int crc_length;
  int fbt;
  int len;
  int shift_register;
  int mask_bits;
  bool crc_pass = true;
  int nbits = data_in.size();
  k = (int)crc_enum;
  mask_bits = CrcMask[k];
  crc_length = CrcLength[k];
  len = nbits - crc_length;
  bin new_crc[crc_length];
  shift_register = 0;
  //Calculate New CRC
  for (k = 0; k < len; k++) {
    fbt = ((shift_register & 1) ^ data_in(k)) * mask_bits;
    shift_register = (shift_register ^ fbt) >> 1;
  }
  for (k = 0; k < crc_length; k++) {
    new_crc[k] = shift_register & 1;
    shift_register >>= 1;
  }
  //Comp New & Org
  for (k = 0; k < crc_length; k++) {
    if (new_crc[k] != data_in(len + k)) {
      crc_pass = false;
      break;
    }
  }
  return (crc_pass);
}

//! Viterbi Trellis状态转移图
vector<Trellis_S> trellis;  //Viterbi Trellis状态转移图

//! Viterbi_Trellis产生
void Viterbi_Trellis_Gen() {
  const int constraint_length = 7;                     //卷积码约束长度=记忆长度+1， 参考36.212 5.1.3.1节
  const int NumStates = 1 << (constraint_length - 1);  //卷积码状态数
  const int MaxPolynom = 3;                            //卷积码生成多项式个数
  const int Polynom[] = {0133, 0171, 0165};            //卷积码生成多项式， 参考36.212 5.1.3.1节
  trellis.resize(NumStates);
  const int mask = NumStates - 1;
  int state = 0;
  int polynom_idx = 0;
  //构造Trellis状态转移图
  for (state = 0; state < NumStates; state++) {
    //input，即当前状态的最高一个bit
    trellis[state].input = ((state >> (constraint_length - 2)) & 1);
    //pre_state 0，可能的上次状态0，状态最低bit=0
    trellis[state].pre_state[0] = ((state << 1) & mask);
    //pre_state 1，可能的上次状态1，状态最低bit=1
    trellis[state].pre_state[1] = (((state << 1) | 1) & mask);
    //上次状态0/1，对应的各parity输出bit，并调制 ：0<->1, 1<->-1
    for (polynom_idx = 0; polynom_idx < MaxPolynom; polynom_idx++) {
      //output 0
      trellis[state].output[polynom_idx][0] = 1 - 2 * (BitCount(Polynom[polynom_idx] & (state << 1)) & 1);
      //output 1
      trellis[state].output[polynom_idx][1] = 1 - 2 * (BitCount(Polynom[polynom_idx] & ((state << 1) | 1)) & 1);
    }
  }
}

//! 咬尾卷积译码，出自 ：论文“Wrap around Viterbi algorithm (WAVA) for tail biting convolutional code”，基于Circular Viterbi algorithm(CVA)
/*!
  \param LLR ： float型矩阵，矩阵行数3（即d0，d1，d2），输入
  \param bits ： 二进制列向量，输出
  \return 无
*/
void WAVA_Viterbi(const fmat &LLR, bvec &bits) {
  const int constraint_length = 7;                     //卷积码约束长度=记忆长度+1， 参考36.212 5.1.3.1节
  const int NumStates = 1 << (constraint_length - 1);  //卷积码状态数
  const int MaxPolynom = 3;                            //卷积码生成多项式个数
  int total_bits_length = LLR.cols();                  //卷积编码前数据长度, 1/3编码
  float path_metric[NumStates];                        //所有状态的路径度量值
  float path_metric_temp[NumStates];
  imat max_survivor_path_pre_state(total_bits_length, NumStates);  //最大幸存路径pre_state，Row: time, Col: curr_stat
  imat max_survivor_path_intput(total_bits_length, NumStates);     //最大幸存路径intput，Row: time, Col: curr_stat
  float branch_metric_0;
  float branch_metric_1;
  float hypothesis_metric_0;
  float hypothesis_metric_1;
  int state = 0;
  int polynom_idx = 0;
  int iter = 0;
  int n = 0;
  int best_state = -1;
  int init_state = -1;
  float metric_temp = -1000.0f;
  int count = 0;
  int best_state_temp = -1;
  bits.resize(total_bits_length);
  //Init，初始化各状态度量值
  for (state = 0; state < NumStates; state++) {
    path_metric[state] = 0.0f;
  }
  //循环迭代译码，迭代次数MAX_TBCC_ITER_NUM
  for (iter = 0; iter < MAX_TBCC_ITER_NUM; iter++) {
    for (n = 0; n < total_bits_length; n++) {
      //由于采用蝶形计算，状态遍历次数减半
      for (state = 0; state < (NumStates >> 1); state++) {
        //BMG (Branch Metric Generation)，分支度量计算
        branch_metric_0 = 0;  //上次状态0对应的分支度量值
        branch_metric_1 = 0;  //上次状态1对应的分支度量值
        for (polynom_idx = 0; polynom_idx < MaxPolynom; polynom_idx++) {
          branch_metric_0 += (float)trellis[state].output[polynom_idx][0] * LLR(polynom_idx, n);
          branch_metric_1 += (float)trellis[state].output[polynom_idx][1] * LLR(polynom_idx, n);
        }
        //ACS (Addition Comparison Selection) , use legs of butterfly，“加比选”模块，采用蝶形，分成low和high两种当前状态
        //low part，当前状态最高位=0
        hypothesis_metric_0 = path_metric[trellis[state].pre_state[0]] + branch_metric_0;  //状态转移0的概率=上次状态0概率+对应状态转移度量值
        hypothesis_metric_1 = path_metric[trellis[state].pre_state[1]] + branch_metric_1;  //状态转移1的概率=上次状态1概率+对应状态转移度量值
        //选择状态转移0/1中的概率大的保存
        if (hypothesis_metric_0 >= hypothesis_metric_1) {
          path_metric_temp[state] = hypothesis_metric_0;                        //更新各状态度量值，临时变量，防止影响当前n时刻其他状态的计算
          max_survivor_path_pre_state(n, state) = trellis[state].pre_state[0];  //更新幸存路径中n时刻，度量值较大的上次状态，以便回溯
          max_survivor_path_intput(n, state) = trellis[state].input;            //更新幸存路径中n时刻，度量值较大的输入值，以便回溯
        } else {
          path_metric_temp[state] = hypothesis_metric_1;
          max_survivor_path_pre_state(n, state) = trellis[state].pre_state[1];
          max_survivor_path_intput(n, state) = trellis[state].input;
        }
        //high part，当前状态最高位=1
        hypothesis_metric_0 = path_metric[trellis[state + (NumStates >> 1)].pre_state[0]] - branch_metric_0;  //状态转移0的概率=上次状态0概率+对应状态转移度量值。当前状态最高位=1，所以度量值*-1
        hypothesis_metric_1 = path_metric[trellis[state + (NumStates >> 1)].pre_state[1]] - branch_metric_1;  //状态转移1的概率=上次状态0概率+对应状态转移度量值。当前状态最高位=1，所以度量值*-1
        //选择状态转移0/1中的概率大的保存
        if (hypothesis_metric_0 >= hypothesis_metric_1) {
          path_metric_temp[state + (NumStates >> 1)] = hypothesis_metric_0;                                           //更新各状态度量值，临时变量，防止影响当前n时刻其他状态的计算
          max_survivor_path_pre_state(n, state + (NumStates >> 1)) = trellis[state + (NumStates >> 1)].pre_state[0];  //更新幸存路径中n时刻，度量值较大的上次状态，以便回溯
          max_survivor_path_intput(n, state + (NumStates >> 1)) = trellis[state + (NumStates >> 1)].input;            //更新幸存路径中n时刻，度量值较大的输入值，以便回溯
        } else {
          path_metric_temp[state + (NumStates >> 1)] = hypothesis_metric_1;
          max_survivor_path_pre_state(n, state + (NumStates >> 1)) = trellis[state + (NumStates >> 1)].pre_state[1];
          max_survivor_path_intput(n, state + (NumStates >> 1)) = trellis[state + (NumStates >> 1)].input;
        }
      }
      //更新各状态度量值
      for (state = 0; state < NumStates; state++) {
        path_metric[state] = path_metric_temp[state];
      }
    }
    //TB (Traceback)，回溯
    best_state = -1;
    init_state = -1;
    metric_temp = -1000;
    //寻找最后时刻，所有状态中，度量值最大的结束状态best_state
    for (state = 0; state < NumStates; state++) {
      if (path_metric[state] > metric_temp) {
        metric_temp = path_metric[state];
        best_state = state;
      }
    }
    //根据结束状态best_state，回溯得到初始状态init_state
    init_state = best_state;
    for (n = total_bits_length - 1; n >= 0; n--) {
      init_state = max_survivor_path_pre_state(n, init_state);
    }
    //如果结束状态best_state=初始状态init_state，则译码成功，回溯得到V_bits，并跳出迭代
    //if ((init_state == best_state)  && ((MAX_TBCC_ITER_NUM - 1) == iter))
    if (init_state == best_state) {
      for (n = total_bits_length - 1; n >= 0; n--) {
        bits(n) = max_survivor_path_intput(n, best_state);
        best_state = max_survivor_path_pre_state(n, best_state);
      }
      break;
    }
    //如果结束状态best_state!=初始状态init_state，但是达到最大迭代次数
    else if ((init_state != best_state) && ((MAX_TBCC_ITER_NUM - 1) == iter)) {
      count = 0;
      //缓存度量值最大的结束状态best_state
      best_state_temp = best_state;
      //从结束状态中，度量值由大到小遍历，寻找结束状态best_state=初始状态init_state的结束状态best_state
      while ((init_state != best_state) && (count < NumStates)) {
        init_state = -1;
        metric_temp = -1000.0f;
        path_metric_temp[best_state] = -1;
        best_state = -1;
        for (state = 0; state < NumStates; state++) {
          if (path_metric_temp[state] > metric_temp) {
            metric_temp = path_metric_temp[state];
            best_state = state;
          }
        }
        init_state = best_state;
        for (n = total_bits_length - 1; n >= 0; n--) {
          init_state = max_survivor_path_pre_state(n, init_state);
        }
        count++;
      }
      //上述遍历不成功，则使用度量值最大的结束状态best_state
      if (count == NumStates) {
        best_state = best_state_temp;
      }
      //根据结束状态best_state，回溯得到初始状态init_state
      for (n = total_bits_length - 1; n >= 0; n--) {
        bits(n) = max_survivor_path_intput(n, best_state);
        best_state = max_survivor_path_pre_state(n, best_state);
      }
    }
  }
}

ivec interleaver_patten;                      //!< 交织序列
fvec P_priori_P1;                             //!< P1先验信息
fvec P_priori_P2;                             //!< P1先验信息
fmat P_alpha;                                 //!< 前向概率，Row ：state，Col ：时刻n
fmat P_beta;                                  //!< 后向概率，Row ：state，Col ：时刻n
fmat P_gamma_P1_0;                            //!< P1状态转移概率,输入0, Row:state, Col ：时刻n
fmat P_gamma_P1_1;                            //!< P1状态转移概率，输入1, Row:state, Col ：时刻n
fmat P_gamma_P2_0;                            //!< P2状态转移概率，输入0, Row:state, Col ：时刻n
fmat P_gamma_P2_1;                            //!< P2状态转移概率，输入1, Row:state, Col ：时刻n
fvec llr_output;                              //!< LLR输出临时变量
vector<Trellis_Backward_S> trellis_backward;  //!< 后向Trellis状态转移图
vector<Trellis_Forward_S> trellis_forward;    //!< 前向Trellis状态转移图

//! MaxLogMAP算法
/*!
  \param P_priori ： float型列向量，先验信息，输入/输出
  \param P_gamma_0 ： float型列向量，校验位0，输入
  \param P_gamma_1 ： float型列向量，校验位1，输入
  \param LLR_sys ： float型列向量，系统位，输入
  \param total_bits_length ： 序列长度（包含结尾bit），输入
  \param llr ： float型类向量，更新后的llr，输出
  \param trellis_forward ： float型列向量，前向Trellis状态转移图，输入
  \param trellis_backward ： float型列向量，后向Trellis状态转移图，输入
  \return 无
*/
void MaxLogMAP(fvec &P_priori,
               const fmat &P_gamma_0,
               const fmat &P_gamma_1,
               const fvec &LLR_sys,
               int total_bits_length,
               fvec &llr,
               const vector<Trellis_Forward_S> &trellis_forward,
               const vector<Trellis_Backward_S> &trellis_backward) {
  int state = 0;
  int n = 0;
  int m = 0;
  float temp[6];
  //Init，初始化起始状态、结束状态，0状态开始，0状态结束
  P_alpha(0, 0) = 0.0f;
  P_beta(0, total_bits_length) = 0.0f;
  for (state = 1; state < Turbo_NumStates; state++) {
    P_alpha(state, 0) = -TURBO_INFINITY;
    P_beta(state, total_bits_length) = -TURBO_INFINITY;
  }
  //Forward recursion
  for (n = 1; n <= total_bits_length; n++) {
    m = n - 1;
    for (state = 0; state < Turbo_NumStates; state++) {
      temp[0] = P_alpha(trellis_backward[state].pre_state[0], m) + P_priori(n - 1) + P_gamma_0(trellis_backward[state].pre_state[0], m);
      temp[1] = P_alpha(trellis_backward[state].pre_state[1], m) - P_priori(n - 1) + P_gamma_1(trellis_backward[state].pre_state[1], m);
      P_alpha(state, n) = (temp[0] >= temp[1]) ? temp[0] : temp[1];
    }
    for (state = 1; state < Turbo_NumStates; state++) {
      P_alpha(state, n) -= P_alpha(0, n);
    }
    P_alpha(0, n) = 0.0f;
  }
  //Backward recursion
  for (n = (total_bits_length - 1); n >= 0; n--) {
    m = n + 1;
    temp[4] = -TURBO_INFINITY;
    temp[5] = -TURBO_INFINITY;
    for (state = 0; state < Turbo_NumStates; state++) {
      //Updata Beta
      temp[0] = P_priori(n) + P_gamma_0(state, n) + P_beta(trellis_forward[state].next_state[0], m);
      temp[1] = -P_priori(n) + P_gamma_1(state, n) + P_beta(trellis_forward[state].next_state[1], m);
      P_beta(state, n) = (temp[0] >= temp[1]) ? temp[0] : temp[1];
      //calculate LLR
      temp[2] = P_alpha(state, n) + temp[0];  //当前状态、当前时刻input=0的概率
      temp[3] = P_alpha(state, n) + temp[1];  //当前状态、当前时刻input=1的概率
      if (temp[5] < temp[2])                  //当前时刻，所有状态中，input=0的最大的概率
      {
        temp[5] = temp[2];
      }
      if (temp[4] < temp[3])  //当前时刻，所有状态中，input=1的最大的概率
      {
        temp[4] = temp[3];
      }
    }
    for (state = 1; state < Turbo_NumStates; state++) {
      P_beta(state, n) -= P_beta(0, n);
    }
    P_beta(0, n) = 0.0f;
    //output probability
    llr(n) = temp[5] - temp[4];
  }
  //extrinsic probability
  for (n = 0; n < (total_bits_length - 3); n++) {
    P_priori(n) = TURBO_SCALE * (0.5f * llr(n) - P_priori(n) - LLR_sys(n));
    if (P_priori(n) >= TURBO_INFINITY) {
      P_priori(n) = TURBO_INFINITY;
    } else if (P_priori(n) <= -TURBO_INFINITY) {
      P_priori(n) = -TURBO_INFINITY;
    }
  }
}

//! Turbo_Trellis产生
void Turbo_Trellis_Gen()  //Turbo_Trellis产生
{
  const int g[2] = {013, 015};  //生成多项式
  int state = 0;
  const int mask = Turbo_NumStates - 1;
  int u[2];
  bin input_bit[2];
  int pre_state[2];
  trellis_backward.resize(Turbo_NumStates);
  trellis_forward.resize(Turbo_NumStates);
  //构造Trellis状态转移图
  for (state = 0; state < Turbo_NumStates; state++) {
    //input，两种可能的输入
    trellis_forward[state].input[0] = 0;
    trellis_forward[state].input[1] = 1;
    //对应两种输入bit下的下次状态
    //next_state 0
    u[0] = (BitCount(g[0] & state) & 1) ^ 0;  //反馈bit
    trellis_forward[state].next_state[0] = (state >> 1) | (u[0] << 2);
    //next_state 1
    u[1] = (BitCount(g[0] & state) & 1) ^ 1;  //反馈bit
    trellis_forward[state].next_state[1] = (state >> 1) | (u[1] << 2);
    //对应两种输入bit下，对应的输出bit，并调制 ：0<->1, 1<->-1
    //output 0
    trellis_forward[state].output[0] = 1 - 2 * ((BitCount(g[1] & state) & 1) ^ u[0]);
    //output 1
    trellis_forward[state].output[1] = 1 - 2 * ((BitCount(g[1] & state) & 1) ^ u[1]);
  }
  for (state = 0; state < Turbo_NumStates; state++) {
    //转移到当前状态前，两种可能的上次状态
    //pre_state 0
    pre_state[0] = ((state << 1) & mask);
    //pre_state 1
    pre_state[1] = (((state << 1) | 1) & mask);
    //由于存在反馈，对应两种可能的上次状态，存在两种当前输入bit
    //input
    u[0] = ((state >> 2) & 1);  //反馈bit
    input_bit[0] = (BitCount(g[0] & pre_state[0]) & 1) ^ u[0];
    trellis_backward[state].input[input_bit[0]] = input_bit[0];
    trellis_backward[state].pre_state[input_bit[0]] = pre_state[0];
    u[1] = ((state >> 2) & 1);  //反馈bit
    input_bit[1] = (BitCount(g[0] & pre_state[1]) & 1) ^ u[1];
    trellis_backward[state].input[input_bit[1]] = input_bit[1];
    trellis_backward[state].pre_state[input_bit[1]] = pre_state[1];
    //两种可能的上次状态，对应的输出bit
    //output 0
    trellis_backward[state].output[input_bit[0]] = 1 - 2 * ((BitCount(g[1] & pre_state[0]) & 1) ^ u[0]);
    //output 1
    trellis_backward[state].output[input_bit[1]] = 1 - 2 * ((BitCount(g[1] & pre_state[1]) & 1) ^ u[1]);
  }
}

//! Turbo译码
/*!
  \param LLR ： float型列向量，LLR，输入
  \param bits ： 二进制列向量，输入
  \param cb_crc_flag ： 提前中断标志，true ：开启CRC校验，提前中断；false ：不开启，输入
  \return 无
*/
bool Turbo_Decoder(const fmat &LLR, bvec &bits, bool cb_crc_flag)  //Turbo译码
{
  int total_bits_length = LLR.cols();                           //编码器每parity输出的bit长度，包括尾bit
  int info_bits_length = total_bits_length - tail_bits_length;  //码块大小
  int tail_t_length = 3;                                        //尾bit对应的每parity输入bit长度
  int i = 0;
  int state = 0;
  int iter = 0;
  int n = 0;
  int interleaver_n = 0;
  int f1 = 0;
  int f2 = 0;
  int pi_i = 0;
  bool cb_crc = true;
  fvec LLR_sys(total_bits_length);
  fvec LLR_sys_interleaver(total_bits_length);
  interleaver_patten.resize(info_bits_length);   //交织序列
  P_priori_P1.resize(info_bits_length + 4);      //P1先验信息
  P_priori_P2.resize(info_bits_length + 4);      //P1先验信息
  P_alpha.resize(8, info_bits_length + 4);       //前向概率，Row ：state，Col ：时刻n
  P_beta.resize(8, info_bits_length + 4);        //后向概率，Row ：state，Col ：时刻n
  P_gamma_P1_0.resize(8, info_bits_length + 3);  //P1状态转移概率,输入0, Row:state, Col ：时刻n
  P_gamma_P1_1.resize(8, info_bits_length + 3);  //P1状态转移概率，输入1, Row:state, Col ：时刻n
  P_gamma_P2_0.resize(8, info_bits_length + 3);  //P2状态转移概率，输入0, Row:state, Col ：时刻n
  P_gamma_P2_1.resize(8, info_bits_length + 3);  //P2状态转移概率，输入1, Row:state, Col ：时刻n
  llr_output.resize(info_bits_length + 4);       //LLR输出临时变量
  bits.resize(info_bits_length);
  //bits.setZero();
  //interleaver patten
  for (i = 0; i < 188; i++) {
    if (TurboInterleaverTable[i][0] == info_bits_length) {
      f1 = TurboInterleaverTable[i][1];
      f2 = TurboInterleaverTable[i][2];
      break;
    }
  }
  for (i = 0; i < info_bits_length; i++) {
    pi_i = ((f1 + f2 * i) % info_bits_length);
    pi_i = ((pi_i * i) % info_bits_length);
    interleaver_patten(i) = pi_i;  //交织序列
  }
  LLR_sys = LLR.row(0);
  for (i = 0; i < info_bits_length; i++) {
    LLR_sys_interleaver(i) = LLR_sys(interleaver_patten(i));
  }
  //priori probability or called extrinsic probability，初始化先验信息
  P_priori_P1.setZero();
  P_priori_P2.setZero();
  //gamma calc，状态转移概率gamma计算
  for (n = 0; n < info_bits_length; n++) {
    interleaver_n = interleaver_patten(n);
    for (state = 0; state < Turbo_NumStates; state++) {
      P_gamma_P1_0(state, n) = (LLR(0, n) + (float)trellis_forward[state].output[0] * LLR(1, n));
      P_gamma_P1_1(state, n) = (-LLR(0, n) + (float)trellis_forward[state].output[1] * LLR(1, n));
      P_gamma_P2_0(state, n) = (LLR(0, interleaver_n) + (float)trellis_forward[state].output[0] * LLR(2, n));
      P_gamma_P2_1(state, n) = (-LLR(0, interleaver_n) + (float)trellis_forward[state].output[1] * LLR(2, n));
    }
  }
  for (state = 0; state < Turbo_NumStates; state++) {
    P_gamma_P1_0(state, info_bits_length) = (LLR(0, info_bits_length) + (float)trellis_forward[state].output[0] * LLR(1, info_bits_length));
    P_gamma_P1_1(state, info_bits_length) = (-LLR(0, info_bits_length) + (float)trellis_forward[state].output[1] * LLR(1, info_bits_length));
    P_gamma_P2_0(state, info_bits_length) = (LLR(0, info_bits_length + 2) + (float)trellis_forward[state].output[0] * LLR(1, info_bits_length + 2));
    P_gamma_P2_1(state, info_bits_length) = (-LLR(0, info_bits_length + 2) + (float)trellis_forward[state].output[1] * LLR(1, info_bits_length + 2));
    P_gamma_P1_0(state, info_bits_length + 1) = (LLR(2, info_bits_length) + (float)trellis_forward[state].output[0] * LLR(0, info_bits_length + 1));
    P_gamma_P1_1(state, info_bits_length + 1) = (-LLR(2, info_bits_length) + (float)trellis_forward[state].output[1] * LLR(0, info_bits_length + 1));
    P_gamma_P2_0(state, info_bits_length + 1) = (LLR(2, info_bits_length + 2) + (float)trellis_forward[state].output[0] * LLR(0, info_bits_length + 3));
    P_gamma_P2_1(state, info_bits_length + 1) = (-LLR(2, info_bits_length + 2) + (float)trellis_forward[state].output[1] * LLR(0, info_bits_length + 3));
    P_gamma_P1_0(state, info_bits_length + 2) = (LLR(1, info_bits_length + 1) + (float)trellis_forward[state].output[0] * LLR(2, info_bits_length + 1));
    P_gamma_P1_1(state, info_bits_length + 2) = (-LLR(1, info_bits_length + 1) + (float)trellis_forward[state].output[1] * LLR(2, info_bits_length + 1));
    P_gamma_P2_0(state, info_bits_length + 2) = (LLR(1, info_bits_length + 3) + (float)trellis_forward[state].output[0] * LLR(2, info_bits_length + 3));
    P_gamma_P2_1(state, info_bits_length + 2) = (-LLR(1, info_bits_length + 3) + (float)trellis_forward[state].output[1] * LLR(2, info_bits_length + 3));
  }
  //MAP iteration
  for (iter = 0; iter < MAX_TURBO_ITER_NUM; iter++) {
    //first constituent code
    MaxLogMAP(P_priori_P1, P_gamma_P1_0, P_gamma_P1_1, LLR_sys, (info_bits_length + tail_t_length), llr_output, trellis_forward, trellis_backward);
    //second constituent code
    for (n = 0; n < info_bits_length; n++) {
      P_priori_P2(n) = P_priori_P1(interleaver_patten[n]);
    }
    MaxLogMAP(P_priori_P2, P_gamma_P2_0, P_gamma_P2_1, LLR_sys_interleaver, (info_bits_length + tail_t_length), llr_output, trellis_forward, trellis_backward);
    //can use a scaling factor for p_priori to fast convergence
    for (n = 0; n < info_bits_length; n++) {
      P_priori_P1(interleaver_patten[n]) = P_priori_P2(n);
    }
    //Hard Decision
    for (n = 0; n < info_bits_length; n++) {
      bits(interleaver_patten(n)) = (llr_output(n) > 0.0f) ? 0 : 1;
    }
    //Check CRC for terminate iteration
    if (cb_crc_flag) {
      cb_crc = CheckCRC(CRC24B, bits);
      if (cb_crc) {
        break;
      }
    }
  }
  return cb_crc;
}

//! 解码块级联及添加CB CRC。出自 ：3GPP 36.212 5.1.2节
/*!
  \param c ： 二进制列向量数组，数组大小即CB个数，输入
  \param b ： 二进制列向量，输出
  \param B ： 传输块长度，输入
  \return 无
*/
void DeCB_Segmentation_CB_CRC_Attachment(const bvecs &c, bvec &b, int B) {
  int i = 0;
  int k = 0;
  int r = 0;
  int s = 0;
  const int Z = 6144;
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
  b.resize(B);
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
  k = F;
  s = 0;
  for (r = 0; r < C; r++) {
    if (r < C_minus) {
      K_r = K_minus;
    } else {
      K_r = K_plus;
    }
    while (k < (K_r - L)) {
      b(s) = c[r](k);
      k = k + 1;
      s = s + 1;
    }
    k = 0;
  }
}

//! 计算码块数、各码块长度。出自 ：3GPP 36.212 5.1.2节
/*!
  \param B ： 传输块长度，输入
  \param K_r ： int型列向量，值为各码块大小，输出
  \param F ： 码块分割时，填充bit长度，输出
  \return 码块数
*/
int Calc_CBSize(int B, ivec &K_r, int *F)  //计算码块数、各码块长度
{
  int i = 0;
  int r = 0;
  const int Z = 6144;
  int L = 0;
  int C = 0;
  int B_prime = 0;
  int K_plus = 0;
  int K_minus = 0;
  int C_plus = 0;
  int C_minus = 0;
  int delta_K = 0;
  if (B <= Z) {
    L = 0;
    C = 1;
    B_prime = B;
  } else {
    L = 24;
    C = ceil((float)B / (float)(Z - L));
    B_prime = B + C * L;
  }
  K_r.resize(C);
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
  *F = C_plus * K_plus + C_minus * K_minus - B_prime;
  for (r = 0; r < C; r++) {
    if (r < C_minus) {
      K_r(r) = K_minus;
    } else {
      K_r(r) = K_plus;
    }
  }
  return C;
}

//解Rate_Matching for turbo
//! 解Turbo的速率匹配。出自 ：3GPP 36.212 5.1.4.1节
/*!
  \param data_in ： float型列向量，输入
  \param D ： D=K+4，输入
  \param N_IR ： N_IR，输入
  \param DL_SCH_PCH_Flag ： 标志，true ：DL-SCH和PCh传输；false ：UL-SCH和MCH传输，输入
  \param C ： 码块个数，输入
  \param rv_idx ： RV版本，输入
  \param F ： 码块分割时的填充bit长度，输入
  \param data_out ： float型矩阵，矩阵行数3（即d0，d1，d2），输出
  \return 无
*/
void DeRate_Matching_Turbo(const fvec &data_in,
                           int D,
                           int N_IR,
                           bool DL_SCH_PCH_Flag,
                           int C,
                           int rv_idx,
                           int F,
                           fmat &data_out) {
  int i = 0;
  int j = 0;
  int k = 0;
  int row = 0;
  int col = 0;
  const int C_TC_subblock = 32;
  int R_TC_subblock = ceil((float)D / (float)C_TC_subblock);
  int K_PI = R_TC_subblock * C_TC_subblock;
  int N_D = R_TC_subblock * C_TC_subblock - D;
  const int P[] = {0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30, 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31};
  int E = data_in.size();
  int k0 = 0;
  int N_cb = (DL_SCH_PCH_Flag) ? MIN(floor((float)N_IR / (float)C), 3 * K_PI) : (3 * K_PI);
  data_out.resize(3, D);
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
        data_out(0, i - N_D) += data_in(k);
        k++;
      }
    }
    //For d1
    else if (0 == (col & 1)) {
      i = row * C_TC_subblock + P[(col - C_TC_subblock) >> 1];
      if (i >= (N_D + F)) {
        data_out(1, i - N_D) += data_in(k);
        k++;
      }
    }
    //For d2
    else {
      i = (row * C_TC_subblock + P[(col - C_TC_subblock) >> 1] + 1) % K_PI;
      if (i >= N_D) {
        data_out(2, i - N_D) += data_in(k);
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

//! 解TBCC的速率匹配。出自 ：3GPP 36.212 5.1.4.2节
/*!
  \param data_in ： float型列向量，输入
  \param D ： D=K，输入
  \param data_out ： float型矩阵，矩阵行数3（即d0，d1，d2），输出
  \return 无
*/
void DeRate_Matching_TBCC(const fvec &data_in,
                          int D,
                          fmat &data_out) {
  int i = 0;
  int k = 0;
  int row = 0;
  int col = 0;
  const int C_TC_subblock = 32;
  int R_TC_subblock = ceil((float)D / (float)C_TC_subblock);
  int N_D = R_TC_subblock * C_TC_subblock - D;
  const int P[] = {1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30};
  int E = data_in.size();
  data_out.resize(3, D);
  data_out.setZero();
  row = 0;
  col = 0;
  k = 0;
  i = 0;
  while (k < E) {
    col = col % (3 * C_TC_subblock);
    i = P[col & 31];
    for (row = 0; row < R_TC_subblock; row++) {
      if (i >= N_D) {
        data_out(col >> 5, i - N_D) += data_in(k);
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
