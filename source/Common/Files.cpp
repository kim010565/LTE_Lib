/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Files.cpp
    \brief Files实现文件，用于打印输出文件
    内容 ：自定义打印输出文件相关函数
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Files.h"

//! 文件打印宏开关，1 ：打印使能；0 ：打印不使能
#define FILESPRINT 1
//#define PICTURESPRINT 1

//! 全局变量Files，用于存放所有需要打印输出的文件信息
vector<Files_S> Files;
//! 全局变量CurFile，当前指向的打印输出文件
FILE *CurFile = NULL;
//! 全局变量FilesPath，打印路径
string FilesPath = ".";
//! 全局变量FilesPathUE，某个UE的打印路径
string FilesPathUE = "";
//! 全局变量FilesPathUE_Cell，某个UE内某Cell的打印路径
string FilesPathUE_Cell = "";
//! 全局变量FilesPathIter，当前iter下的打印路径
string FilesPathIter = "";

//! 建立路径函数。当路径不存在时，自动建立改路径
/*!
  \param pathname ： 路径名，输入
  \return 无返回值
*/
void MakeDir(const char *pathname) {
  char *str = new char[1024];
  strcpy(str, FilesPath.c_str());
  strcat(str, "/");
  strcat(str, pathname);
  if (-1 == access(str, 0)) {
    strcpy(str, "mkdir ");
    strcat(str, FilesPath.c_str());
    strcat(str, "/");
    strcat(str, pathname);
    system(str);
  }
}

//! 修改当前打印输出文件指向函数。指向函数参数指定的文件，如果不存在则新建
/*!
\param filename ： 文件名，输入
  \return 无返回值
*/
void WithFile(const char *filename) {
#if FILESPRINT > 0
  int i = 0;
  int FilesNum = (int)Files.size();
  char *str = new char[1024];
  strcpy(str, FilesPath.c_str());
  strcat(str, "/");
  strcat(str, filename);
  strcat(str, ".txt");
  for (i = 0; i < FilesNum; i++)  //已有该文件名对应的Files变量
  {
    if (0 == strcmp(Files[i].filename, str)) {
      CurFile = Files[i].fp;
      break;
    }
  }
  if (i == FilesNum)  //新建文件名对应的Files变量
  {
    Files.resize(FilesNum + 1);
    Files[i].filename = new char[strlen(FilesPath.c_str()) + 6 + strlen(filename)];
    strcpy(Files[i].filename, FilesPath.c_str());
    strcat(Files[i].filename, "/");
    strcat(Files[i].filename, filename);
    strcat(Files[i].filename, ".txt");
    Files[i].fp = fopen(Files[i].filename, "w+");
#if 1 == ERROR_REPORT
    if (Files[i].fp == NULL) {
      Error("Error opening file: %s", Files[i].filename);
    }
#endif
    CurFile = Files[i].fp;
  }
  delete[] str;
#endif
}

//! 打印输出文件函数。打印输出到当前指向的文件
/*!
  \param format ：类似于printf函数
  \return 无返回值
*/
void FilePrint(const char *format, ...) {
#if FILESPRINT > 0
  if (NULL != CurFile) {
    va_list args;
    va_start(args, format);
    vfprintf(CurFile, format, args);
    va_end(args);
  }
#endif
}

//! 文件关闭函数。关闭函数参数指向的文件
/*!
  \param filename ： 文件名，输入
  \return 无返回值
*/
void CloseFile(const char *filename) {
#if FILESPRINT > 0
  int i = 0;
  int FilesNum = (int)Files.size();
  char *str = new char[1024];
  strcpy(str, FilesPath.c_str());
  strcat(str, "/");
  strcat(str, filename);
  strcat(str, ".txt");
  for (i = 0; i < FilesNum; i++)  //已有该文件名对应的Files变量
  {
    if (0 == strcmp(Files[i].filename, str)) {
      fclose(Files[i].fp);
      delete[] Files[i].filename;
      Files.erase(Files.begin() + i);
      CurFile = NULL;
      break;
    }
  }
  delete[] str;
#endif
}

//! 文件关闭函数。关闭所有文件
/*!
  \return 无返回值
*/
void CloseAllFiles() {
#if FILESPRINT > 0
  int i = 0;
  for (i = 0; i < (int)Files.size(); i++) {
    if (NULL != Files[i].fp) {
      fclose(Files[i].fp);
      delete[] Files[i].filename;
    }
  }
  Files.resize(0);
  CurFile = NULL;
#endif
}

//! 文件关闭函数。关闭函数除参数指向的文件外的所有文件
/*!
  \param filename ： 文件名，输入
  \return 无返回值
*/
void CloseFilesExcept(const char *filename) {
#if FILESPRINT > 0
  int i = 0;
  char *str = new char[1024];
  strcpy(str, FilesPath.c_str());
  strcat(str, "/");
  strcat(str, filename);
  strcat(str, ".txt");
  for (i = 0; i < (int)Files.size(); i++) {
    if (NULL != Files[i].fp && (0 != strcmp(Files[i].filename, str))) {
      fclose(Files[i].fp);
      delete[] Files[i].filename;
      Files.erase(Files.begin() + i);
    }
  }
  CurFile = NULL;
#endif
}

/*void GNUPlot(const char *filename,
             const char *xycol,
             const char *name,
             const char *xlabel,
             const char *ylabel,
             const char *title,
             const char *style)  // lines, points, linespoints or impulses
{
#if PICTURESPRINT > 0 && FILESPRINT > 0
    FILE *gp;
    char *str = new char[1024];
    vector<string> Split_filename;
    vector<string> Split_xycol;
    vector<string> Split_name;
    string temp;
    int i = 0;
    bool OpenFlag = true;
    temp = filename;
    boost::split(Split_filename, temp, boost::is_any_of(","));
    temp = xycol;
    boost::split(Split_xycol, temp, boost::is_any_of(","));
    temp = name;
    boost::split(Split_name, temp, boost::is_any_of(","));
    for (i = 0; i < (int) Split_filename.size(); i++)
    {
        boost::trim(Split_filename[i]);
        strcpy(str, FilesPath.c_str());
        strcat(str, "/");
        strcat(str, Split_filename[i].c_str());
        strcat(str, ".txt");
        if (-1 == access(str, 0))
        {
            OpenFlag = false;
        }
    }
    if (OpenFlag)
    {
        gp = popen("gnuplot", "w");
        fprintf(gp, "set term png size 1024, 1024\n");
        strcpy(str, "set output '");
        strcat(str, FilesPath.c_str());
        strcat(str, "/");
        strcat(str, Split_filename[0].c_str());
        strcat(str, ".png'\n");
        fprintf(gp, str);
        strcpy(str, "set xlabel '");
        strcat(strcat(str, xlabel), "'\n");
        fprintf(gp, str);
        strcpy(str, "set ylabel '");
        strcat(strcat(str, ylabel), "'\n");
        fprintf(gp, str);
        strcpy(str, "set title '");
        strcat(strcat(str, title), "'\n");
        fprintf(gp, str);
        fprintf(gp, "set grid\n");
        strcpy(str, "plot '");
        strcat(str, FilesPath.c_str());
        strcat(str, "/");
        strcat(str, Split_filename[0].c_str());
        strcat(str, ".txt' using ");
        strcat(str, Split_xycol[0].c_str());
        strcat(str, " with ");
        strcat(strcat(str, style), " t '");
        strcat(strcat(str, Split_name[0].c_str()), "'");
        for (i = 1; i < (int) Split_filename.size(); i++)
        {
            strcat(str, ", '");
            strcat(str, FilesPath.c_str());
            strcat(str, "/");
            strcat(str, Split_filename[i].c_str());
            strcat(str, ".txt' using ");
            strcat(str, Split_xycol[i].c_str());
            strcat(str, " with ");
            strcat(strcat(str, style), " t '");
            strcat(strcat(str, Split_name[i].c_str()), "'");
        }
        strcat(str, "\n");
        fprintf(gp, str);
        pclose(gp);
    }
    delete [] str;
#endif
}*/
