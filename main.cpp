/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file main.cpp
    \brief main实现文件
    内容 ：main函数实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "./header/Common/Librarys.h"

//! main函数
/*!
  \param argc ： main函数参数个数，输入
  \param argv ： main函数参数列表，输入
  \return int型，正常则返回0
*/
int main(int argc, char **argv) {
  int i = 0;
  string str;
  clock_t start, end;
  start = clock();
  Simulation_C Simulation;
  Simulation.ParseArgs(argc, argv);  //解析配置参数
  InitTables();
  Viterbi_Trellis_Gen();
  Turbo_Trellis_Gen();
  for (loop = 0; loop < Simulation.GetIterNum(); loop++)  //迭代iter
  {
    sprintf(c_string, "%s/Iter%d", FilesPath.c_str(), loop);
    FilesPathIter = c_string;
    if (-1 == access(FilesPathIter.c_str(), 0)) {
      str = "mkdir " + FilesPathIter;
      system(str.c_str());
    }
    sprintf(c_string, "Iter%d", loop);
    FilesPathIter = c_string;
    seed = 0;
    srand(0);
    for (i = 0; i < 15; i++) {
      CQI_Th[i] = CQI_Th_Org[i];
    }
    Simulation.SetSimulationParas(loop);  //设置仿真参数
    Simulation.Process();                 //仿真
    CloseFilesExcept("Log");
  }
  Simulation.GetResult();
  end = clock();
  WithFile("Log");
  FilePrint("Execution Time: %.3f s", (float)(end - start) / CLOCKS_PER_SEC);
  CloseAllFiles();
  return 0;
}
