# solver_plugin

このディレクトリには任意で使う solver プラグインを置きます。核心 API の `matrix.h` とは分け、必要なときだけ include します。

他の言語: [中文](README.md) / [English](README.en.md)

| プラグイン | 用途 |
| --- | --- |
| `plugin_Linear_Solver.h` | `A * X = B` 用の稠密ガウス消去 solver |
| `plugin_LP_Sover.h` | LP solver。標準は revised simplex、内点法も選択可能 |
| `plugin_MIP_Solver.h` | 基本 cuts と branch-and-bound を持つ軽量 MIP solver |

これらのプラグインはコードの簡潔さと直接的なインターフェースを重視し、軽量配置と簡易なアプリケーション開発に向いています。
