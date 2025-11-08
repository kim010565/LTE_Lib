/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Simulation.h
    \brief Simulation头文件
    内容 ：Simulation_C类定义，相关函数声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Simulation_h_
#define _Simulation_h_

#include "../Channel/Channel.h"
#include "../Common/Definitions.h"
#include "../Common/Files.h"
#include "../Common/Parser.h"
#include "../Common/Typedefs.h"
#include "../ENodeB/ENodeB.h"
#include "../UE/UE.h"

//! Simulation_C类定义
class Simulation_C {
 public:
  Simulation_C();
  ~Simulation_C();

  void ParseArgs(int argc, char **argv);
  int GetIterNum();
  void SetSimulationParas(int iter_idx);
  void Process();
  void GetResult();

  SimuCtrl_S Simuctrl;

 private:
  string infile;           //! 仿真参数输入文件
  string outpath;          //! 仿真输出路径
  int runlength;           //! 仿真长度，单位TTI
  int short_scale_factor;  //! 仿真长度缩放比例
  int iter_num;            //! 仿真迭代次数
  Parser_C Parser;         //! 参数输入文件解析器

  vector<Cell_C> Cell;        //! 小区变量，Cell[cell_idx]
  vector<Channel_C> Channel;  //! 无线信道变量，Channel[ue_idx]
  vector<UE_C> UE;            //! UE变量，UE[ue_idx]
};

#endif
