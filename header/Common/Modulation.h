/*! Coypright (c) 2017, 金晓成
    All rights reserved
    \file Modulation.h
    \brief Modulation头文件
内容 ：一些调制函数相关声明
    \author 金晓成
    <kim010565@163.com>
    \date 2017年6月7日
*/
#ifndef _Modulation_h_
#define _Modulation_h_

#include "Definitions.h"
#include "Enums.h"
#include "FastFunctions.h"
#include "Tables.h"
#include "Typedefs.h"

//! BPSK标准星座图
const cpxf Constellation_BPSK[] =
    {
        cpxf(1.0f / MySqrt(2.0f), 1.0f / MySqrt(2.0f)), cpxf(-1.0f / MySqrt(2.0f), -1.0f / MySqrt(2.0f))};

//! QPSK标准星座图
const cpxf Constellation_QPSK[] =
    {
        cpxf(1.0f / MySqrt(2.0f), 1.0f / MySqrt(2.0f)), cpxf(1.0f / MySqrt(2.0f), -1.0f / MySqrt(2.0f)),
        cpxf(-1.0f / MySqrt(2.0f), 1.0f / MySqrt(2.0f)), cpxf(-1.0f / MySqrt(2.0f), -1.0f / MySqrt(2.0f))};

//! 16QAM标准星座图
const cpxf Constellation_16QAM[] =
    {
        cpxf(1.0f / MySqrt(10.0f), 1.0f / MySqrt(10.0f)), cpxf(1.0f / MySqrt(10.0f), 3.0f / MySqrt(10.0f)), cpxf(3.0f / MySqrt(10.0f), 1.0f / MySqrt(10.0f)), cpxf(3.0f / MySqrt(10.0f), 3.0f / MySqrt(10.0f)),
        cpxf(1.0f / MySqrt(10.0f), -1.0f / MySqrt(10.0f)), cpxf(1.0f / MySqrt(10.0f), -3.0f / MySqrt(10.0f)), cpxf(3.0f / MySqrt(10.0f), -1.0f / MySqrt(10.0f)), cpxf(3.0f / MySqrt(10.0f), -3.0f / MySqrt(10.0f)),
        cpxf(-1.0f / MySqrt(10.0f), 1.0f / MySqrt(10.0f)), cpxf(-1.0f / MySqrt(10.0f), 3.0f / MySqrt(10.0f)), cpxf(-3.0f / MySqrt(10.0f), 1.0f / MySqrt(10.0f)), cpxf(-3.0f / MySqrt(10.0f), 3.0f / MySqrt(10.0f)),
        cpxf(-1.0f / MySqrt(10.0f), -1.0f / MySqrt(10.0f)), cpxf(-1.0f / MySqrt(10.0f), -3.0f / MySqrt(10.0f)), cpxf(-3.0f / MySqrt(10.0f), -1.0f / MySqrt(10.0f)), cpxf(-3.0f / MySqrt(10.0f), -3.0f / MySqrt(10.0f))};

//! 64QAM标准星座图
const cpxf Constellation_64QAM[] =
    {
        cpxf(3.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(3.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)),
        cpxf(3.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(3.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)),
        cpxf(5.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(5.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)),
        cpxf(5.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(5.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)),

        cpxf(3.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(3.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)),
        cpxf(3.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(3.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(1.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)),
        cpxf(5.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(5.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)),
        cpxf(5.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(5.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(7.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)),

        cpxf(-3.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(-3.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)),
        cpxf(-3.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(-3.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)),
        cpxf(-5.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(-5.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), 3.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), 1.0f / MySqrt(42.0f)),
        cpxf(-5.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(-5.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), 5.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), 7.0f / MySqrt(42.0f)),

        cpxf(-3.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(-3.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)),
        cpxf(-3.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(-3.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(-1.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)),
        cpxf(-5.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(-5.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), -3.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), -1.0f / MySqrt(42.0f)),
        cpxf(-5.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(-5.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), -5.0f / MySqrt(42.0f)), cpxf(-7.0f / MySqrt(42.0f), -7.0f / MySqrt(42.0f))};

//! 256QAM标准星座图
const cpxf Constellation_256QAM[] =
    {
        cpxf(5.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),

        cpxf(5.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(5.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(5.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(7.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(3.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(3.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(1.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(11.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(11.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(9.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(13.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(13.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(15.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),

        cpxf(-5.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 5.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 7.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 3.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 1.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 11.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 9.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 13.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), 15.0f / MySqrt(170.0f)),

        cpxf(-5.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(-5.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-5.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-7.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(-3.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-3.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-1.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -5.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -7.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -3.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -1.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(-11.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-11.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-9.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -11.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -9.0f / MySqrt(170.0f)),
        cpxf(-13.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-13.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -13.0f / MySqrt(170.0f)), cpxf(-15.0f / MySqrt(170.0f), -15.0f / MySqrt(170.0f))};

void Modulation_BPSK(const bvec &bits, cfvec &symbs);
void Modulation_QPSK(const bvec &bits, cfvec &symbs);
void Modulation_16QAM(const bvec &bits, cfvec &symbs);
void Modulation_64QAM(const bvec &bits, cfvec &symbs);
void Modulation_256QAM(const bvec &bits, cfvec &symbs);

void Layer_Mapping(const cfvecs &d,
                   int v,
                   Transmission_Scheme_E Transmission_Scheme,
                   cfmat &x);
void Precoding(const cfmat &x,
               int P,
               int M_0_symb,
               Transmission_Scheme_E Transmission_Scheme,
               ivec &Codebook_index0,
               ivec &Codebook_index1,
               bool alternativeCodeBookEnabledFor4TX_r12,
               cfmat &y);

#endif
