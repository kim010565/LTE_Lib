/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file ENodeB.h
    \brief ENodeB头文件
    内容 ：Cell_C类定义，相关函数声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _ENodeB_h_
#define _ENodeB_h_

#include "../Common/Common.h"
#include "../Common/Definitions.h"
#include "../Common/Typedefs.h"

//! Cell类
class Cell_C {
 public:
  Cell_C();
  ~Cell_C();

  void InitParas();
  void UpdataParas(int n_subframe);

  void PSS_Gen();
  void SSS_Gen();
  void CRS_Gen();
  void DMRS_Port5_Gen(int cell_ue_idx);
  void DMRS_Port7to14_Gen(int cell_ue_idx);
  void EPDCCH_DMRS_Gen(int cell_ue_idx);
  void CSI_RS_Gen(int cell_ue_idx);

  void PBCH_Gen();
  void PCFICH_Gen();
  void PHICH_Gen();
  void DCI_Gen(int cell_ue_idx, bvec &DCI_bits);
  void PDCCH_Gen();
  void EPDCCH_DMP(int cell_ue_idx);
  void EPDCCH_Gen(int cell_ue_idx);
  int Calc_PDSCH_G_symb(int cell_ue_idx);
  void Calc_Codebook_index(int cell_ue_idx, int M_layer_symb);
  void PDSCH_MappingToRE(int cell_ue_idx, const cfmat &symbs_in);
  void PDSCH_Gen(int cell_ue_idx);

  void RandomBeamforming(int cell_ue_idx);
  void OFDM_BB_Signal_Gen();

  void Process();
  void GetMedianCQI(int cell_idx);

  SimuCtrl_S Simuctrl;                        //!< 控制参数集合
  Cell_Specific_S Cell_Specific;              //!< Cell级参数集合
  vector<UE_Specific_S> UE_Specific;          //!< UE级参数集合
  vector<ReTransmit_S> ReTransmit;            //!< 重传相关集合
  vector<Feedback_S> Feedback;                //!< 反馈相关集合
  vector<int> Cell_Connect_UE_idx;            //!< 当前Cell所连接的UE，UE的序号集合
  vector<int> Cell_Connect_UE_innerCell_idx;  //!< 当前Cell所连接的UE，该UE内该Cell的序号
  cfmat BB;                                   //!< 基带信号，BB(PortIdx, t)
  cfmat Ideal_h;                              //!< 时域理想信道估计，仅包含发端天线相关性和发端频偏影响
  vector<int> IdealSamplePos;                 //!< 时域理想信道估计抽取样点位置
  int IdealSamplePosNum;                      //!< 时域理想信道估计抽取符号间隔样点数
 private:
  cfmat PBCH_Symbs;  //!< PBCH符号产生缓存

  cfmats RE;                                                         //!< RE时偏资源，RE[PortIdx](k, l)
  Eigen::Matrix<RE_Type_E, Eigen::Dynamic, Eigen::Dynamic> RE_Type;  //!< RE时偏资源类型，RE_Type(k, l)
  double Phase;                                                      //!< 发端频偏相关，存在的历史相位
  int last_n_subframe;                                               //!< 发端频偏相关，上一次发射的子帧序号
};

#endif
