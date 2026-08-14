# apps

本目录保存 MatrixHub v2.0 的两个命令行程序源码。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

| 文件 | 构建产物 | 说明 |
| --- | --- | --- |
| `matrixhub_cli.c` | `Matrix_Hub_v2_0_cli` | 类 Matlab `.m` 脚本计算器，支持 REPL 和脚本文件 |
| `matrixhub_solver.c` | `Matrix_Hub_v2_0_solver` | 读取 `.lp` 文件的 LP/MIP 求解器 |

从仓库根目录构建：

```bash
cmake -S . -B build
cmake --build build
```

运行示例：

```bash
./build/Matrix_Hub_v2_0_cli examples/scripts/basic.m
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
```
