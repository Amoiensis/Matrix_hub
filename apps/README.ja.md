# apps

このディレクトリには MatrixHub v2.0 のコマンドラインプログラムのソースを置きます。

他の言語: [中文](README.md) / [English](README.en.md)

| ファイル | 実行ファイル | 用途 |
| --- | --- | --- |
| `matrixhub_cli.c` | `Matrix_Hub_v2_0_cli` | Matlab 風 `.m` スクリプト計算器 |
| `matrixhub_solver.c` | `Matrix_Hub_v2_0_solver` | `.lp` ファイル用 LP/MIP solver |

ビルド：

```bash
cmake -S . -B build
cmake --build build
```
