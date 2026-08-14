# apps

This directory contains the source files for the MatrixHub v2.0 command-line programs.

Other languages: [中文](README.md) / [日本語](README.ja.md)

| File | Binary | Purpose |
| --- | --- | --- |
| `matrixhub_cli.c` | `Matrix_Hub_v2_0_cli` | Matlab-like `.m` script calculator with REPL and file mode |
| `matrixhub_solver.c` | `Matrix_Hub_v2_0_solver` | LP/MIP solver for `.lp` files |

Build from the repository root:

```bash
cmake -S . -B build
cmake --build build
```
