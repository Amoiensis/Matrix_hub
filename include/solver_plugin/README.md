# solver_plugin

本目录提供可选 solver 插件。它们不放进 `matrix.h` 的核心 API，用户按需包含。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

| 插件 | 说明 |
| --- | --- |
| `plugin_Linear_Solver.h` | 稠密高斯消元线性方程求解器，求解 `A * X = B` |
| `plugin_LP_Sover.h` | LP 求解器，默认修订单纯形，可选内点法 |
| `plugin_MIP_Solver.h` | 轻量 MIP 求解器，包含初步 cuts 和 branch-and-bound 框架 |

这些插件强调代码简洁、接口直接，适合轻量部署和简易开发使用。
