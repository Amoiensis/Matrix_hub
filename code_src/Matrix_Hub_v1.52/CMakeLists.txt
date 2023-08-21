cmake_minimum_required(VERSION 3.21)

project(Matrix_Hub_v1_52 C)

set(CMAKE_C_STANDARD 99)

add_executable(Matrix_Hub_v1_52_demo matrix.h state.h solver_plugin/plugin_LP_Sover.h demo.c)