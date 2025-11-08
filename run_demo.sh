#!/bin/bash

# 当前脚本路径
SCRIPT_PATH=$(dirname "$(readlink -f "$0")")

if [ ! -d ${SCRIPT_PATH}/"external/eigen-3.3.7" ]; then
    cd  ${SCRIPT_PATH}/"external"
    unzip eigen-3.3.7.zip
fi

if [ ! -d ${SCRIPT_PATH}/"external/boost" ]; then
    cd  ${SCRIPT_PATH}/"external"
    unzip boost.zip
fi

cd  ${SCRIPT_PATH}

rm -rf build
mkdir -p build
cd build
cmake ..
make -j8

./LTE_Lib -i ${SCRIPT_PATH}/FDD_Case/TM1_36.101_8.2.1.1.1_1.txt -o ${SCRIPT_PATH}/FDD_Case_Result

cd ..