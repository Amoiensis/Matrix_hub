# C 库调用示例

本目录展示如何在一个普通 C 程序中引用 MatrixHub 头文件和 solver 插件。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

独立构建：

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

示例内容：

- 创建矩阵并求解线性方程 `A * x = b`
- 调用 LP solver
- 调用 MIP solver
- 使用 `M_free()` 释放 MatrixHub 创建的矩阵
