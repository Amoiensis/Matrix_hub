cmake_minimum_required(VERSION 3.21)

project(MatrixHub VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(MatrixHub INTERFACE)
target_include_directories(MatrixHub INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT MSVC)
        target_link_libraries(MatrixHub INTERFACE m)
endif()

add_library(MatrixHubSolvers INTERFACE)
target_link_libraries(MatrixHubSolvers INTERFACE MatrixHub)

add_executable(Matrix_Hub_v2_0_demo
        examples/c_library/main.c)
target_link_libraries(Matrix_Hub_v2_0_demo PRIVATE MatrixHubSolvers)

add_executable(Matrix_Hub_v2_0_cli
        apps/matrixhub_cli.c)
target_link_libraries(Matrix_Hub_v2_0_cli PRIVATE MatrixHub)

add_executable(Matrix_Hub_v2_0_solver
        apps/matrixhub_solver.c)
target_link_libraries(Matrix_Hub_v2_0_solver PRIVATE MatrixHubSolvers)
