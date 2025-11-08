cmake_minimum_required(VERSION 3.10)
set(name_project "LTE_lib")

project(LTE_lib LANGUAGES C CXX CUDA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
file(GLOB mainfile "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/Channel/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/Common/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/ENodeB/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/Simulation/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/UE/*.cpp")
add_executable(${name_project} ${mainfile})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pg -march=native -mtune=native -fpermissive") 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/eigen-3.3.7")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/")

target_link_libraries(${name_project} stdc++fs)
