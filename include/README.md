# include

本目录是 MatrixHub v2.0 的 C 头文件库入口。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

| 文件/目录 | 说明 |
| --- | --- |
| `matrix.h` | 矩阵结构和主要矩阵运算函数 |
| `state.h` | 数值类型、精度、阈值和错误提示配置 |
| `solver_plugin/` | 可选 solver 插件：线性方程、LP、MIP |

最小引用：

```c
#include "matrix.h"
```

需要求解器时：

```c
#include "solver_plugin/plugin_Linear_Solver.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"
```

返回 `Matrix*` 的函数通常会创建新矩阵，使用后应调用 `M_free()`。
