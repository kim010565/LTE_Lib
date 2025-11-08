/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Files.h
    \brief Files头文件，用于打印输出文件
    内容 ：自定义打印输出文件相关函数
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Files_h_
#define _Files_h_

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include "Typedefs.h"

//! 自定义Files_S结构体
typedef struct
{
  FILE *fp;        //!< FILE型指针fp
  char *filename;  //!< char型指针filename
} Files_S;

extern vector<Files_S> Files;
extern FILE *CurFile;
extern string FilesPath;
extern string FilesPathUE;
extern string FilesPathUE_Cell;
extern string FilesPathIter;

void MakeDir(const char *pathname);
void WithFile(const char *filename);
void FilePrint(const char *format, ...);
void CloseFile(const char *filename);
void CloseAllFiles();
void CloseFilesExcept(const char *filename);

/*void GNUPlot(const char *filename,
             const char *xycol,
             const char *name,
             const char *xlabel,
             const char *ylabel,
             const char *title,
             const char *style);*/

#endif
