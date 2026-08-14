cmake_minimum_required(VERSION 3.21)

project(MatrixHubCExample LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(MATRIXHUB_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." CACHE PATH
    "Path to the MatrixHub repository root")

add_executable(matrixhub_c_example main.c)
target_include_directories(matrixhub_c_example PRIVATE
        "${MATRIXHUB_ROOT}/include")
if(NOT MSVC)
        target_link_libraries(matrixhub_c_example PRIVATE m)
endif()
