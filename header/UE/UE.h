/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file UE.h
    \brief UE头文件
    内容 ：UE_C类定义，相关函数声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _UE_h_
#define _UE_h_

#include "../Channel/Channel.h"
#include "../Common/Common.h"
#include "../Common/Definitions.h"
#include "../Common/Typedefs.h"
#include "../ENodeB/ENodeB.h"

//! UE类
class UE_C {
 public:
  UE_C();
  ~UE_C();

  void InitParas();
  void UpdataParas(int n_subframe, const vector<Cell_C> &Cell);

  void RF(Channel_C &Channel);
  void RemoveCP_FFT(int ue_cell_idx);

  void CRS_LSCheEst(int ue_cell_idx);
  void DMRS_Port5_LSCheEst(int ue_cell_idx);
  void DMRS_Port7to14_LSCheEst(int ue_cell_idx);
  void EPDCCH_DMRS_LSCheEst(int ue_cell_idx);
  void CSI_RS_LSCheEst(int ue_cell_idx);

  void CRS_Measurement_1st(int ue_cell_idx);
  void DMRS_Port5_Measurement_1st(int ue_cell_idx);
  void DMRS_Port7to14_Measurement_1st(int ue_cell_idx);
  void EPDCCH_DMRS_Measurement_1st(int ue_cell_idx);
  void CSI_RS_Measurement_1st(int ue_cell_idx);

  void CRS_FilterF(int ue_cell_idx);
  void DMRS_Port5_FilterF(int ue_cell_idx);
  void DMRS_Port7to14_FilterF(int ue_cell_idx);
  void EPDCCH_DMRS_FilterF(int ue_cell_idx);
  void CSI_RS_FilterF(int ue_cell_idx);

  void CRS_FilterT(int ue_cell_idx);
  void DMRS_Port5_FilterT(int ue_cell_idx);
  void DMRS_Port7to14_FilterT(int ue_cell_idx);
  void EPDCCH_DMRS_FilterT(int ue_cell_idx);

  void CRS_Measurement_2nd(int ue_cell_idx);
  void DMRS_Port5_Measurement_2nd(int ue_cell_idx);
  void DMRS_Port7to14_Measurement_2nd(int ue_cell_idx);
  void EPDCCH_DMRS_Measurement_2nd(int ue_cell_idx);
  void CSI_RS_Measurement_2nd(int ue_cell_idx);

  void PSS_Remove(int ue_cell_idx);
  void SSS_Remove(int ue_cell_idx);
  void PBCH_Remove(int ue_cell_idx);
  void PCFICH_Remove(int ue_cell_idx);
  void PHICH_Remove(int ue_cell_idx);
  void EPDCCH_Remove(int ue_cell_idx);

  void FEICIC(int ue_cell_idx);

  void PSS_DT(int ue_cell_idx);
  void SSS_DT(int ue_cell_idx);
  void PBCH_DMP_MIMO_DCC_Blind(int ue_cell_idx);
  void PBCH_DMP_MIMO_DCC(int ue_cell_idx);
  void PCFICH_DMP_MIMO_DCC(int ue_cell_idx);
  void PHICH_DMP_MIMO_DCC(int ue_cell_idx);
  void PDCCH_DMP_MIMO_DCC(int ue_cell_idx);
  void EPDCCH_DMP_MIMO_DCC(int ue_cell_idx);
  void PDSCH_DMP_MIMO_DCC(int ue_cell_idx);

  void PBCH_DCC_Blind(int ue_cell_idx, int CRSPortNum, int SFN_Low2bit);
  void PBCH_DCC(int ue_cell_idx, int CRSPortNum);
  void PCFICH_DCC(int ue_cell_idx);
  void PHICH_DCC(int ue_cell_idx);
  void DCI_Parse(int ue_cell_idx, const bvec &DCI_bits);
  void PDCCH_DCC(int ue_cell_idx);
  void EPDCCH_DCC(int ue_cell_idx);
  void PDSCH_DCC(int ue_cell_idx, int cw_idx, int G_prime);

  void Process(Channel_C &Channel);
  void FeedBack(int ue_idx, std::vector<Cell_C> &Cell);
  void GetResult(int ue_idx);
  void GetMedianCQI(int ue_idx, std::vector<Cell_C> &Cell);

  SimuCtrl_S Simuctrl_Next;                    //!< 下一个TTI，控制参数集合，由于UE处理存在一个TTI延时，所以需要多缓存一个Next的
  vector<Cell_Specific_S> Cell_Specific_Next;  //!< 下一个TTI，Cell级参数集合，由于UE处理存在一个TTI延时，所以需要多缓存一个Next的
  vector<UE_Specific_S> UE_Specific_Next;      //!< 下一个TTI，UE级参数集合，由于UE处理存在一个TTI延时，所以需要多缓存一个Next的

  SimuCtrl_S Simuctrl;                      //!< 当前TTI，控制参数集合
  vector<Cell_Specific_S> Cell_Specific;    //!< 当前TTI，Cell级参数集合
  vector<UE_Specific_S> UE_Specific;        //!< 当前TTI，UE级参数集合
  vector<ReTransmit_S> ReTransmit;          //!< 重传相关集合
  vector<Feedback_S> Feedback;              //!< 反馈相关集合
  vector<Result_S> Result;                  //!< 仿真结果相关集合
  vector<int> UE_Connect_Cell_idx;          //!< 当前UE连接的小区，小区序号集合
  vector<int> UE_Connect_Cell_innerUE_idx;  //!< 当前UE连接的小区，该小区内该UE的序号
  UEType_E UEType;                          //!< UE类型
 private:
  int MaxOverSample_DataLen;  //!< 过采样下，最大的传输样点数
  int TA;                     //!< 20M下，提前接收样点数
  int TA_Target;              //!< 20M下，提前接收样点目标数
  int TA_Target_prime;        //!< 等效带宽下，折算的提前接收样点目标数
  vector<int> AGC;            //!< AGC
  vector<int> Last_AGC;       //!< 历史AGC
  vector<int> Last_AGC_PBCH;  //!< PBCH下的历史AGC

  vector<MEA_S> MEA;  //!< 测量相关
  vector<CHE_S> CHE;  //!< 信道估计相关

  cfmat Wire_Signal;                                                 //!< 无线信号，Wire_Signal(r, n)
  cfmat Wire_Signal_FIR;                                             //!< RC滤波后的无线信号，Wire_Signal(r, n)
  cfmats Cell_Signal;                                                //!< 小区信号，Cell_Signal[cell_idx](r, n)
  cfmats Cell_Signal_5ms;                                            //!< 5ms的小区信号，用于PBCH，Cell_Signal[cell_idx](r, n)
  cfmats Cell_Signal_1p4M_5ms;                                       //!< 5ms 1.4M的小区信号，用于PSS/SSS，Cell_Signal[cell_idx](r, n)
  int Counter_5ms;                                                   //!< 5ms计数器
  cfmats RE;                                                         //!< RE时偏资源，RE[r](k, l)
  Eigen::Matrix<RE_Type_E, Eigen::Dynamic, Eigen::Dynamic> RE_Type;  //!< RE时偏资源类型，RE_Type(k, l)
  fvec V_softbits;                                                   //!< 解调用的软信息LLR
  vector<double> Phase;                                              //!< 历史相位，接收频偏相关
  vector<int> last_n_subframe;                                       //!< 历史子帧序号，接收频偏相关
  vector<vector<int>> IdealSamplePos;                                //!< 当前TTI，时域理想信道估计抽取样点位置
  vector<int> IdealSamplePosNum;                                     //!< 当前TTI，时域理想信道估计抽取符号间隔样点数
  vector<vector<int>> IdealSamplePos_Next;                           //!< 下一个TTI，时域理想信道估计抽取样点位置
  vector<int> IdealSamplePosNum_Next;                                //!< 下一个TTI，时域理想信道估计抽取符号间隔样点数
};

#endif
