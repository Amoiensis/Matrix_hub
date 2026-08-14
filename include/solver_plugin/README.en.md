# solver_plugin

This directory provides optional solver plugins. They are kept outside the core `matrix.h` API and can be included only when needed.

Other languages: [中文](README.md) / [日本語](README.ja.md)

| Plugin | Purpose |
| --- | --- |
| `plugin_Linear_Solver.h` | Dense Gaussian solver for `A * X = B` |
| `plugin_LP_Sover.h` | LP solver, revised simplex by default with an interior-point option |
| `plugin_MIP_Solver.h` | Lightweight MIP solver with basic cuts and branch-and-bound |

The plugins keep the code compact and the interfaces direct, making them suitable for lightweight deployment and simple application development.
