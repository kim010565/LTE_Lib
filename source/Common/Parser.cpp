/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Parser.cpp
    \brief Parser实现文件
    内容 ：输入文件解析相关实现
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#include "../../header/Common/Parser.h"

//! Parser_C类构造函数
Parser_C::Parser_C() {
}

//! Parser_C类析构函数
Parser_C::~Parser_C() {
}

//! Parser_C类读取文件函数。用=分割“key变量”与“key变量的值”；自动屏蔽//注释内容；多个iter分割符号next_iter
/*!
  \param filename ： 路径及文件名，输入
  \return 无返回值
*/
void Parser_C::LoadFile(const string &filename) {
  ifstream infile;
  string str_line = "";
  string::size_type equal_div_pos = 0;
  string str_key = "";
  string str_value = "";
  size_t line_pos, line_end, line_len;
  vector<string> vec;
  int n = 0;
  infile.open(filename.c_str());
#if 1 == ERROR_REPORT
  if (!infile) {
    Error("Error opening file: %s", filename.c_str());
  }
#endif
  str_filename = filename;  //记录文件名
  iter_num = 0;
  m_Strings.resize(1);
  m_Values.resize(1);
  m_Lines.resize(1);
  while (!infile.eof()) {
    getline(infile, str_line);
    n++;
    if (str_line.find("//") != str_line.npos)  //去除//注释行
    {
      line_pos = str_line.find("//");
      line_end = str_line.find("\n", line_pos);
      line_len = line_end - line_pos;
      str_line.erase(line_pos, line_len);
    }
    boost::trim(str_line);
    if (!str_line.empty()) {
      if (str_line.compare("next_iter") == 0)  //多个iter分割符号next_iter
      {
        iter_num++;
        m_Strings.resize(iter_num + 1);
        m_Values.resize(iter_num + 1);
        m_Lines.resize(iter_num + 1);
        m_Strings[iter_num] = m_Strings[iter_num - 1];
        m_Values[iter_num] = m_Values[iter_num - 1];
        m_Lines[iter_num] = m_Lines[iter_num - 1];
      } else if (str_line.npos != (equal_div_pos = str_line.find("=")))  //用=分割“key变量”与“key变量的值”
      {
        str_key = str_line.substr(0, equal_div_pos);
        str_value = str_line.substr(equal_div_pos + 1, str_line.size() - 1);
        boost::trim(str_key);
        boost::trim(str_value);
        m_Strings.resize(iter_num + 1);
        m_Strings[iter_num][str_key] = str_line;  //key变量
        m_Values[iter_num][str_key] = str_value;  //key变量的值
        m_Lines[iter_num][str_key] = n;           //行号
      }
#if 1 == ERROR_REPORT
      else {
        Error("%s Error in Line %d: %s", filename.c_str(), n, str_line.c_str());
      }
#endif
    }
  }
  iter_num++;  //iter个数
  infile.close();
}

//! Parser_C类读取仿真iter个数
/*!
  \return 仿真iter个数
*/
int Parser_C::GetIterNum() {
  return iter_num;
}

//! Parser_C类读取第iter_idx次迭代、第l3个UE，第l2个Cell、第l1个Item、第l0个参数，变量名称为str_key的值
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \param l3 ： 第l3个UE（多组参数通过"#"分割UE），输入
  \param l2 ： 第l2个Cell（多组参数通过";"分割Cell），输入
  \param l1 ： 第l1个Item（多组参数通过"|"分割Items），输入
  \param l0 ： 第l0个参数（一组内参数通过","分割参数），输入
  \param v_default ： 默认值（如果不存在，则设为默认值），输入
  \return 读取值，string类
*/
string Parser_C::ReadString(const string &str_key, int iter_idx, int l3, int l2, int l1, int l0, string v_default)  //读取第l1组，第l0个参数
{
  string temp = v_default;
  map<string, string>::iterator it;
  vector<string> SplitVec0;
  vector<string> SplitVec1;
  vector<string> SplitVec2;
  vector<string> SplitVec3;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    if (l3 < (int)SplitVec3.size()) {
      boost::split(SplitVec2, SplitVec3[l3], boost::is_any_of(";"));  //多组参数通过";"分割Cell
      if (l2 < (int)SplitVec2.size()) {
        boost::split(SplitVec1, SplitVec2[l2], boost::is_any_of("|"));  //多组参数通过"|"分割Items
        if (l1 < (int)SplitVec1.size()) {
          boost::split(SplitVec0, SplitVec1[l1], boost::is_any_of(","));  //一组内参数通过","分割参数
          if (l0 < (int)SplitVec0.size()) {
            boost::trim(SplitVec0[l0]);
            temp = SplitVec0[l0];
          }
        }
      }
    }
  }
  return temp;
}

//! Parser_C类读取第iter_idx次迭代、第l3个UE，第l2个Cell、第l1个Item、第l0个参数，变量名称为str_key的值
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \param l3 ： 第l3个UE（多组参数通过"#"分割UE），输入
  \param l2 ： 第l2个Cell（多组参数通过";"分割Cell），输入
  \param l1 ： 第l1个Item（多组参数通过"|"分割Items），输入
  \param l0 ： 第l0个参数（一组内参数通过","和“~”分割参数），输入
  \param v_default ： 默认值（如果不存在，则设为默认值），输入
  \return 读取值，float型
*/
float Parser_C::ReadValue(const string &str_key, int iter_idx, int l3, int l2, int l1, int l0, float v_default)  //读取第l1组，第l0个参数
{
  char *pEnd;
  float temp = v_default;
  vector<map<string, string>>::iterator iter1;
  map<string, string>::iterator it;
  vector<string> SplitVec_seg;
  vector<string> SplitVec0;
  vector<string> SplitVec1;
  vector<string> SplitVec2;
  vector<string> SplitVec3;
  vector<float> values;
  float a = 0;
  float b = 0;
  int i = 0;
  float j = 0;
  int k = 0;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    if (l3 < (int)SplitVec3.size()) {
      boost::split(SplitVec2, SplitVec3[l3], boost::is_any_of(";"));  //多组参数通过";"分割Cell
      if (l2 < (int)SplitVec2.size()) {
        boost::split(SplitVec1, SplitVec2[l2], boost::is_any_of("|"));  //多组参数通过"|"分割Items
        if (l1 < (int)SplitVec1.size()) {
          boost::split(SplitVec0, SplitVec1[l1], boost::is_any_of(","));  //一组内参数通过","和“~”分割参数
          values.resize(SplitVec0.size());
          for (i = 0, k = 0; i < (int)SplitVec0.size(); i++) {
            boost::split(SplitVec_seg, SplitVec0[i], boost::is_any_of("~"));
            if (2 == (int)SplitVec_seg.size()) {
              boost::trim(SplitVec_seg[0]);
              a = strtof(SplitVec_seg[0].c_str(), &pEnd);
#if 1 == ERROR_REPORT
              if (0 != strcmp(pEnd, "")) {
                Error("%s Error in Line: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
              }
#endif
              boost::trim(SplitVec_seg[1]);
              b = strtof(SplitVec_seg[1].c_str(), &pEnd);
#if 1 == ERROR_REPORT
              if (0 != strcmp(pEnd, "")) {
                Error("%s Error in Line: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
              }
#endif
              values.resize(values.size() + (size_t)MyFabs(a - b));
              if (a < b) {
                for (j = a; j <= b; j++) {
                  values[k++] = j;
                  if (l0 == (k - 1)) {
                    temp = values[l0];
                    break;
                  }
                }
              } else {
                for (j = a; j <= b; j--) {
                  values[k++] = j;
                  if (l0 == (k - 1)) {
                    temp = values[l0];
                    break;
                  }
                }
              }
            } else if (1 == (int)SplitVec_seg.size()) {
              boost::trim(SplitVec_seg[0]);
              values[k++] = strtof(SplitVec_seg[0].c_str(), &pEnd);
#if 1 == ERROR_REPORT
              if (0 != strcmp(pEnd, "")) {
                Error("%s Error in Line %d: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
              }
#endif
              if (l0 == (k - 1)) {
                temp = values[l0];
                break;
              }
            }
#if 1 == ERROR_REPORT
            else {
              Error("%s Error in Line %d: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
            }
#endif
          }
        }
      }
    }
  }
  return temp;
}

//! Parser_C类统计第iter_idx次迭代下，变量名称为str_key的UE参数个数
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \return 变量名称为str_key的UE参数个数，int型
*/
int Parser_C::Getl3Num(const string &str_key, int iter_idx)  //统计参数组的组数
{
  int temp = 0;
  map<string, string>::iterator it;
  vector<string> SplitVec3;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    temp = SplitVec3.size();
  }
  return temp;
}

//! Parser_C类统计第iter_idx次迭代、第l3个UE下，变量名称为str_key的Cell参数个数
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \param l3 ： 第l3个UE（多组参数通过"#"分割UE），输入
  \return 变量名称为str_key的Cell参数个数，int型
*/
int Parser_C::Getl2Num(const string &str_key, int iter_idx, int l3)  //统计参数组的组数
{
  int temp = 0;
  map<string, string>::iterator it;
  vector<string> SplitVec2;
  vector<string> SplitVec3;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    if (l3 < (int)SplitVec3.size()) {
      boost::split(SplitVec2, SplitVec3[l3], boost::is_any_of(";"));  //多组参数通过";"分割Cell
      temp = SplitVec2.size();
    }
  }
  return temp;
}

//! Parser_C类统计第iter_idx次迭代、第l3个UE、第l2个Cell下，变量名称为str_key的Item参数个数
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \param l3 ： 第l3个UE（多组参数通过"#"分割UE），输入
  \param l2 ： 第l2个Cell（多组参数通过";"分割Cell），输入
  \return 变量名称为str_key的Item参数个数，int型
*/
int Parser_C::Getl1Num(const string &str_key, int iter_idx, int l3, int l2)  //统计参数组的组数
{
  int temp = 0;
  map<string, string>::iterator it;
  vector<string> SplitVec1;
  vector<string> SplitVec2;
  vector<string> SplitVec3;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    if (l3 < (int)SplitVec3.size()) {
      boost::split(SplitVec2, SplitVec3[l3], boost::is_any_of(";"));  //多组参数通过";"分割Cell
      if (l2 < (int)SplitVec2.size()) {
        boost::split(SplitVec1, SplitVec2[l2], boost::is_any_of("|"));  //多组参数通过"|"分割Items
        temp = SplitVec1.size();
      }
    }
  }
  return temp;
}

//! Parser_C类统计第iter_idx次迭代、第l3个UE、第l2个Cell、第l1个Item下，变量名称为str_key的一组内参数个数
/*!
  \param str_key ： 变量名称，输入
  \param iter_idx ： 第iter_idx次迭代，输入
  \param l3 ： 第l3个UE（多组参数通过"#"分割UE），输入
  \param l2 ： 第l2个Cell（多组参数通过";"分割Cell），输入
  \param l1 ： 第l1个Item（多组参数通过"|"分割Items），输入
  \return 变量名称为str_key的一组内参数个数，int型
*/
int Parser_C::Getl0Num(const string &str_key, int iter_idx, int l3, int l2, int l1)  //统计参数个数
{
  char *pEnd;
  int temp = 0;
  map<string, string>::iterator it;
  std::vector<std::string> SplitVec_seg;
  vector<string> SplitVec0;
  vector<string> SplitVec1;
  vector<string> SplitVec2;
  vector<string> SplitVec3;
  float a = 0;
  float b = 0;
  int i = 0;
  int m = 0;
#if 1 == ERROR_REPORT
  if (iter_idx >= iter_num) {
    Error("Func %s iter_idx=%d error", __func__, iter_idx);
  }
#endif
  it = m_Values[iter_idx].find(str_key);
  if (it != m_Values[iter_idx].end()) {
    boost::split(SplitVec3, m_Values[iter_idx].find(str_key)->second, boost::is_any_of("#"));  //多组参数通过"#"分割UE
    if (l3 < (int)SplitVec3.size()) {
      boost::split(SplitVec2, SplitVec3[l3], boost::is_any_of(";"));  //多组参数通过";"分割Cell
      if (l2 < (int)SplitVec2.size()) {
        boost::split(SplitVec1, SplitVec2[l2], boost::is_any_of("|"));  //多组参数通过"|"分割Items
        if (l1 < (int)SplitVec1.size()) {
          boost::split(SplitVec0, SplitVec1[l1], boost::is_any_of(","));  //一组内参数通过","和“~”分割
          m = (int)SplitVec0.size();
          for (i = 0; i < (int)SplitVec0.size(); i++) {
            boost::split(SplitVec_seg, SplitVec0[i], boost::is_any_of("~"));
            if (2 == (int)SplitVec_seg.size()) {
              boost::trim(SplitVec_seg[0]);
              a = strtof(SplitVec_seg[0].c_str(), &pEnd);
#if 1 == ERROR_REPORT
              if (0 != strcmp(pEnd, "")) {
                Error("%s Error in Line %d: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
              }
#endif
              boost::trim(SplitVec_seg[1]);
              b = strtof(SplitVec_seg[1].c_str(), &pEnd);
#if 1 == ERROR_REPORT
              if (0 != strcmp(pEnd, "")) {
                Error("%s Error in Line %d: %s", str_filename.c_str(), m_Lines[iter_idx].find(str_key)->second, (m_Strings[iter_idx].find(str_key)->second).c_str());
              }
#endif
              m = m + MyFabs(a - b);
            }
          }
          temp = m;
        }
      }
    }
  }
  return temp;
}
