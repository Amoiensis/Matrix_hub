# include

This directory is the C header-library entry point for MatrixHub v2.0.

Other languages: [中文](README.md) / [日本語](README.ja.md)

| Path | Purpose |
| --- | --- |
| `matrix.h` | Matrix type and core matrix operations |
| `state.h` | Numeric type, precision, tolerance, and error configuration |
| `solver_plugin/` | Optional linear, LP, and MIP solver plugins |

Most functions returning `Matrix*` allocate a new matrix. Release it with `M_free()`.
