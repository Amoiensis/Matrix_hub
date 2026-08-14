![MatrixHub_logo](./docs/img/MatrixHub_logo.png)

# Matrix Hub v2.0

言語: [中文](README.md) / [English](README.en.md) / [日本語](README.ja.md)

Matrix Hub v2.0 は、行列計算と最適化 solver のための軽量ツールキットです。小さな C コードベースで、組み込みやすく、スクリプト実行でき、コマンドラインからも使える行列計算機能を提供することを目標にしています。組み込み配置、軽量アプリケーション、小規模な工学計算、制御アルゴリズムや最適化プロトタイプなどの二次開発に利用できます。

利用形態は、行列演算用 C ヘッダライブラリ、Matlab 風 `.m` スクリプト/対話式計算器、プラグインまたは単独実行ファイルとして使える軽量 LP/MIP solver の 3 つです。事前ビルド済みの計算器と solver は約 100KB 規模で、配置と組み込みがしやすい構成です。CLI と solver は WebAssembly 版としても提供しており、ブラウザから直接利用できます。

## ダウンロードしてすぐ使う

現在、Apple Silicon macOS と Windows x64 向けの事前ビルド済み実行パッケージを提供しています。ダウンロードして解凍すれば、そのまま利用できます。その他の環境ではソースからビルドするか、作者までメールで相談してください。

| プラットフォーム | ダウンロード | サイズ | 内容 |
| --- | --- | --- | --- |
| macOS Apple Silicon | [MatrixHub-v2.0-macos-arm64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-macos-arm64.zip) | CLI: 119 KB<br>Solver: 136 KB | CLI スクリプト計算器、LP/MIP solver、help、サンプル |
| Windows x64 | [MatrixHub-v2.0-windows-x64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-windows-x64.zip) | CLI: 345 KB<br>Solver: 365 KB | CLI スクリプト計算器、LP/MIP solver、help、サンプル |

## ブラウザで使う

行列スクリプト計算器や LP/MIP solver をすぐ試したい場合は、WebAssembly 版をブラウザから利用できます。計算はブラウザ内でローカルに実行されるため、デモ、小規模問題の確認、インストールできない環境での利用に向いています。

| Web 版 | URL | 内容 |
| --- | --- | --- |
| MatrixHub CLI | https://www.peking.ren/matrixhub_cli/ | Matlab 風 `.m` スクリプト計算器と対話入力 |
| MatrixHub Solver | https://www.peking.ren/matrixhub_solver/ | LP/MIP `.lp` ファイル solver、ブラウザ内ローカル実行 |

## プロジェクト情報

| 項目 | 内容 |
| --- | --- |
| Project | https://github.com/Amoiensis/Matrix_hub |
| Author | Sebin (Xiping.Yu) |
| Email | Amoiensis@outlook.com |

質問、提案、利用例などがあればメールで連絡してください。

## 利用形態

| 形態 | 主な用途 | 入口 |
| --- | --- | --- |
| C ヘッダライブラリ | C/C++ での行列演算と二次開発。制御・最適化プロトタイプなど | `include/matrix.h`, `include/solver_plugin/*.h` |
| 対話式スクリプト計算器 | Matlab 風 `.m` ファイル実行と `>>` REPL による逐次入力 | `build/Matrix_Hub_v2_0_cli` |
| 軽量 LP/MIP solver | 約 100KB 規模の solver/plugin。`.lp` ファイル、進捗、gap、結果レポートに対応 | `build/Matrix_Hub_v2_0_solver` |

## ディレクトリ

```text
include/                  C ヘッダライブラリ
include/solver_plugin/    Linear/LP/MIP solver プラグイン
apps/                     対話式スクリプト計算器と LP/MIP solver
examples/c_library/       C ライブラリ利用例
examples/scripts/         MatrixHub .m スクリプト例
examples/solver/          solver の使い方、問題ファイル、変換ツール
help/                     関数、コマンド、solver の help
```

## 対応する演算

| 分類 | CLI / スクリプト | C API / プラグイン | 説明 |
| --- | --- | --- | --- |
| 行列作成 | `A = [1 2; 3 4]` | `Matrix_gen` | 連続データから行列を作成 |
| 基本行列 | `eye`, `zeros`, `ones`, `hilbert` | `M_I`, `M_Zeros`, `M_Ones`, `Hilbert` | 単位行列、零行列、一行列、Hilbert 行列 |
| 四則演算 | `A+B`, `A-B`, `A*B`, `2*A` | `M_add_sub`, `M_mul`, `M_numul` | 行列演算とスカラー演算 |
| 要素ごとの演算 | `pmul(A,B)`, `pdiv(A,B)` | `M_pmuldiv` | Hadamard 積と除算 |
| 索引とスライス | `A(1,2)`, `A[:,1]` | `M_Cut`, `M_Sample` | スクリプトでは 1 始まり |
| 基本量 | `det`, `tr`, `norm`, `cond`, `rank` | `M_det`, `M_tr`, `M_norm`, `M_cond`, `M_rank` | 行列の性質を確認 |
| 逆行列と疑似逆 | `inv(A)`, `pinv(A)` | `M_Inverse`, `M_pinv` | 小規模な行列計算向け |
| 変換 | `uptri`, `lowtri`, `diatri`, `householder`, `hessenberg` | 対応する `M_*` helper | 初等変換と直交変換ルーチン |
| 分解 | `[Q,R]=qr(A)`, `[U,S,V]=svd(A)` | `M_QR`, `M_SVD` | 多出力構文に対応 |
| 固有値解析 | `[D,E]=eigen(A)`, `eigen_max(A)` | `M_eigen`, `M_eigen_max` | 固有値と固有ベクトル |
| スクリプト制御 | `for`, `if/elif/else`, `vars`, `clean`, `del` | - | REPL とスクリプト処理 |
| 線形方程式 | - | `Linear_Solve` | `A * X = B` を稠密ガウス消去で解く |
| LP/MIP solver | solver CLI | `LP_Solver`, `MIP_Solver` | 軽量 LP/MIP プラグイン |

## C ライブラリとして使う

```c
#include "matrix.h"
#include "solver_plugin/plugin_Linear_Solver.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"
```

より詳しい C ライブラリ利用例は [examples/c_library/main.c](examples/c_library/main.c) を参照してください。行列作成、線形方程式 solver、LP/MIP プラグイン呼び出し、メモリ解放の流れを確認できます。

例：

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

## 対話式スクリプト計算器

```bash
./build/Matrix_Hub_v2_0_cli
./build/Matrix_Hub_v2_0_cli examples/scripts/basic.m
./build/Matrix_Hub_v2_0_cli --debug debug.txt examples/scripts/basic.m
```

```text
>> A = [1 2; 3 4]
>> B = inv(A)
>> [Q, R] = qr(A)
>> vars
>> help qr
>> clean
>> quit
```

スクリプトモードは行列リテラル、範囲、索引、スライス、ネストした関数、多出力関数、`for`、`if/elif/else`、`clear/clean/del/vars/help` をサポートします。

## 軽量 LP/MIP Solver

LP は Linear Programming（線形計画）、MIP は Mixed-Integer Programming（混合整数計画）を意味します。Matrix Hub の solver は線形目的関数、線形制約、連続/整数/バイナリ変数を対象にしており、一般的な MILP（Mixed-Integer Linear Programming）に近い用途で使えます。

問題はおおまかに次の形で表せます。

```text
LP:
  minimize or maximize   c^T x
  subject to             A x <= b
                         l <= x <= u

MIP / MILP:
  minimize or maximize   c^T x + d^T y
  subject to             A x + B y <= b
                         l <= x <= u
                         y_i integer or binary
```

参考: [Linear programming](https://en.wikipedia.org/wiki/Linear_programming), [Mixed-integer programming / Integer programming](https://en.wikipedia.org/wiki/Mixed-integer_programming).

```bash
./build/Matrix_Hub_v2_0_solver --help
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp \
  --progress=25 --node-limit=5000 --time-limit=30 --gap-limit=0.05 \
  --output-dir solver_report_store
```

よく使うオプションは `--progress`、`--output-dir`、`--time-limit`、`--gap-limit`、`--node-limit`、`--open-limit`、`--method`、`--branch`、`--node-select`、`--bound-mode` です。

配布用 problem は `gt2.lp`、`ortools_mip.lp`、`ortools_mip_array.lp`、`lpp0033.lp` の 4 個だけです。大きな benchmark の入手元は [examples/solver/README.md](examples/solver/README.md) にまとめています。

## ヘルプ

MatrixHub の help は、利用場面ごとに参照できます。

| 場面 | 使い方 |
| --- | --- |
| C API | CLI で `help M_mul`、`help M_QR`、`help c_library` を実行、または `help/M_mul.txt` と `include/matrix.h` を直接参照 |
| CLI 計算器 | REPL で `help`、`help script_mode`、`help script_functions`、`help vars` を実行 |
| LP/MIP solver | `./build/Matrix_Hub_v2_0_solver --help`、または CLI で `help solver_mode` を実行 |

```text
help
help script_mode
help script_functions
help solver_mode
help M_mul
```

主要 help は中国語・英語併記です。この README は [中文](README.md) と [English](README.en.md) もあります。

## ビルド

```bash
cmake -S . -B build
cmake --build build
```

主な実行ファイル：

```text
build/Matrix_Hub_v2_0_demo      C ライブラリ利用例
build/Matrix_Hub_v2_0_cli       対話式スクリプト計算器
build/Matrix_Hub_v2_0_solver    軽量 LP/MIP ファイル solver
```

## メンテナンス

v2.0 の配布ツリーには現在のソースだけを置きます。過去のバージョンディレクトリは旧 main/archive 分岐の履歴として保存します。
