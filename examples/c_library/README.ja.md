# C ライブラリ利用例

このディレクトリは通常の C プログラムから MatrixHub のヘッダと solver プラグインを使う例です。

他の言語: [中文](README.md) / [English](README.en.md)

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

例では行列作成、`A * x = b` の求解、LP/MIP solver 呼び出し、`M_free()` による解放を示します。
