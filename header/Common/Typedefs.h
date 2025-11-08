/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Typedefs.h
    \brief Typedefs头文件
    内容 ：EIGEN宏开关；std标准库/Boost库/Eigen库包含；typedef定义
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Typedefs_h_
#define _Typedefs_h_

//Eigen
//#define EIGEN_VECTORIZE_SSE4_2
//! EIGEN宏开关，断言assert相关不起作用，用于优化加速；代码调试时，建议开启
#define EIGEN_NO_DEBUG
//! EIGEN宏开关，断言assert相关内存分配不起作用，用于优化加速；代码调试时，建议开启
#define EIGEN_NO_MALLOC
//#define EIGEN_RUNTIME_NO_MALLOC
//! EIGEN宏开关，矩阵初始化为0
#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <stdarg.h>
#include <unistd.h>
#include <Eigen/Dense>
#include <Eigen/QR>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <typeinfo>
#include <unsupported/Eigen/FFT>
#include <valarray>

typedef char INT8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;
typedef long long int INT64;
typedef unsigned long long int UINT64;

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int int64;
typedef unsigned long long int uint64;

typedef uint8 bin;  //binary(0,1)

typedef std::complex<double> cpxd;
typedef std::complex<float> cpxf;
typedef std::complex<int> cpxi;
typedef std::complex<bin> cpxb;

typedef Eigen::VectorXd dvec;  //列向量
typedef Eigen::VectorXf fvec;
typedef Eigen::VectorXi ivec;
typedef Eigen::Matrix<bin, Eigen::Dynamic, 1> bvec;
typedef Eigen::VectorXcd cdvec;
typedef Eigen::VectorXcf cfvec;
typedef Eigen::Matrix<int, Eigen::Dynamic, 1> civec;
typedef Eigen::Matrix<cpxb, Eigen::Dynamic, 1> cbvec;

typedef Eigen::RowVectorXd drowvec;  //行向量
typedef Eigen::RowVectorXf frowvec;
typedef Eigen::RowVectorXi irowvec;
typedef Eigen::Matrix<bin, 1, Eigen::Dynamic> browvec;
typedef Eigen::RowVectorXcd cdrowvec;
typedef Eigen::RowVectorXcf cfrowvec;
typedef Eigen::Matrix<int, 1, Eigen::Dynamic> cirowvec;
typedef Eigen::Matrix<cpxb, 1, Eigen::Dynamic> cbrowvec;

typedef Eigen::MatrixXd dmat;  //矩阵
typedef Eigen::MatrixXf fmat;
typedef Eigen::MatrixXi imat;
typedef Eigen::Matrix<bin, Eigen::Dynamic, Eigen::Dynamic> bmat;
typedef Eigen::MatrixXcd cdmat;
typedef Eigen::MatrixXcf cfmat;
typedef Eigen::Matrix<cpxi, Eigen::Dynamic, Eigen::Dynamic> cimat;
typedef Eigen::Matrix<cpxb, Eigen::Dynamic, Eigen::Dynamic> cbmat;

typedef std::vector<dvec, Eigen::aligned_allocator<dvec>> dvecs;  //列向量数组
typedef std::vector<fvec, Eigen::aligned_allocator<fvec>> fvecs;
typedef std::vector<ivec, Eigen::aligned_allocator<ivec>> ivecs;
typedef std::vector<bvec, Eigen::aligned_allocator<bvec>> bvecs;
typedef std::vector<cdvec, Eigen::aligned_allocator<cdvec>> cdvecs;
typedef std::vector<cfvec, Eigen::aligned_allocator<cfvec>> cfvecs;
typedef std::vector<civec, Eigen::aligned_allocator<civec>> civecs;
typedef std::vector<cbvec, Eigen::aligned_allocator<cbvec>> cbvecs;

typedef std::vector<drowvec, Eigen::aligned_allocator<drowvec>> drowvecs;  //行向量数组
typedef std::vector<frowvec, Eigen::aligned_allocator<frowvec>> frowvecs;
typedef std::vector<irowvec, Eigen::aligned_allocator<irowvec>> irowvecs;
typedef std::vector<browvec, Eigen::aligned_allocator<browvec>> browvecs;
typedef std::vector<cdrowvec, Eigen::aligned_allocator<cdrowvec>> cdrowvecs;
typedef std::vector<cfrowvec, Eigen::aligned_allocator<cfrowvec>> cfrowvecs;
typedef std::vector<cirowvec, Eigen::aligned_allocator<cirowvec>> cirowvecs;
typedef std::vector<cbrowvec, Eigen::aligned_allocator<cbrowvec>> cbrowvecs;

typedef std::vector<dmat, Eigen::aligned_allocator<dmat>> dmats;  //矩阵数组
typedef std::vector<fmat, Eigen::aligned_allocator<fmat>> fmats;
typedef std::vector<imat, Eigen::aligned_allocator<imat>> imats;
typedef std::vector<bmat, Eigen::aligned_allocator<bmat>> bmats;
typedef std::vector<cdmat, Eigen::aligned_allocator<cdmat>> cdmats;
typedef std::vector<cfmat, Eigen::aligned_allocator<cdmat>> cfmats;
typedef std::vector<cimat, Eigen::aligned_allocator<cdmat>> cimats;
typedef std::vector<cbmat, Eigen::aligned_allocator<cdmat>> cbmats;

using namespace std;

#endif
