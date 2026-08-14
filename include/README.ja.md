# include

このディレクトリは MatrixHub v2.0 の C ヘッダライブラリ入口です。

他の言語: [中文](README.md) / [English](README.en.md)

| パス | 用途 |
| --- | --- |
| `matrix.h` | Matrix 型と主要な行列演算 |
| `state.h` | 数値型、表示精度、許容誤差、エラー設定 |
| `solver_plugin/` | 線形方程式、LP、MIP solver プラグイン |

`Matrix*` を返す関数は通常新しい行列を作成します。使用後は `M_free()` で解放します。
