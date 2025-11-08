/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Channel.h
    \brief Channel头文件
    内容 ：Channel_C类定义，相关函数声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Channel_h_
#define _Channel_h_

#include "../Common/Common.h"
#include "../Common/Definitions.h"
#include "../Common/Typedefs.h"
#include "../ENodeB/ENodeB.h"

//! Dent模型参数，即产生的多径数
const int JAKES_MODEL_M = 16;
//! Dent模型参数，即FHT阶数
const int LOG2_JAKES_MODEL_M = 4;
//! 最大延迟样点数
const int MAXDELAYSAMPLES = 512;
//! 最大提前样点数
const int MAXTASAMPLES = 512;

void Get_Path_Info(Channel_Specific_S &Channel_Specific, const Cell_C &Cell);

void Get_Path_Info_Dyn(Channel_Specific_S &Channel_Specific, const Cell_C &Cell);

//! Channel类
class Channel_C {
 public:
  Channel_C();
  ~Channel_C();

  void InitParas(const vector<Cell_C> &Cell);
  void UpdataParas(int n_subframe, const vector<Cell_C> &Cell);
  void Get_h_buffer(int ue_cell_idx, const Cell_C &Cell);
  void Process(const vector<Cell_C> &Cell);

  SimuCtrl_S Simuctrl;                          //!< 控制参数集合
  vector<Channel_Specific_S> Channel_Specific;  //!< Cell级参数集合
  vector<int> UE_Connect_Cell_idx;              //!< 当前UE连接的小区，小区序号集合
  vector<int> UE_Connect_Cell_innerUE_idx;      //!< 当前UE连接的小区，该小区内该UE的序号
  cfmat RxData;                                 //!< 接收数据
 private:
  cfmat Combined_A;  //!< Dent模型相关，A(k, n)
  dvec Cos_Alpha;    //!< Dent模型相关，Cos_Alpha(n)
  dvec Ceta;         //!< Dent模型相关，Ceta(n)
};

#endif
