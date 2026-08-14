# C Library Example

This directory shows how to use MatrixHub headers and solver plugins from a normal C program.

Other languages: [中文](README.md) / [日本語](README.ja.md)

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

The example creates matrices, solves `A * x = b`, calls the LP solver, calls the MIP solver, and releases matrices with `M_free()`.
