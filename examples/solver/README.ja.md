# MatrixHub Solver の使い方

このディレクトリは MatrixHub v2.0 の LP/MIP ファイル solver の使い方を説明します。solver は小さな `.lp` ファイルを読み、`include/solver_plugin/` の LP/MIP プラグインで計算します。軽量配置と簡易なアプリケーション開発に使える solver コンポーネントです。

他の言語: [中文](README.md) / [English](README.en.md)

## ビルド

```bash
cmake -S . -B build
cmake --build build
```

## 実行

```bash
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip_array.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/lpp0033.lp --progress=25
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp --progress=25 --node-limit=5000
```

## オプション

```text
--progress[=N]          N 回ごとに進捗を表示
--output-dir DIR        読みやすい結果レポートを出力
--time-limit SECONDS    最大実行時間
--gap-limit VALUE       gap 条件で停止
--node-limit N          MIP ノード上限
--open-limit N          open node 上限
--method bb|cut-bb      B&B または root cuts + B&B
--branch first|most
--node-select dfs|best-bound
--bound-mode parent|tight
```

## 同梱 problem

配布用には `ortools_mip.lp`、`ortools_mip_array.lp`、`lpp0033.lp`、`gt2.lp` の 4 個だけを置きます。大きな benchmark はリポジトリ外、または release/data URL から取得する想定です。

## データ入手元

OR-Tools examples、MIPLIB3、MIPLIB 2017、Netlib LP、COR@L MIP などが利用できます。Python CPLEX または OR-Tools ModelBuilder が使える環境では `tools/benchmark_to_lp.py` で変換できます。
