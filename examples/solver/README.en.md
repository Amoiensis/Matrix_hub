# MatrixHub Solver Usage

This directory documents the MatrixHub v2.0 LP/MIP file solver. The solver reads small `.lp` files and computes through the LP/MIP plugins under `include/solver_plugin/`; it is a solver component for lightweight deployment and simple application development.

Other languages: [中文](README.md) / [日本語](README.ja.md)

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip_array.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/lpp0033.lp --progress=25
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp --progress=25 --node-limit=5000
```

## Options

```text
--progress[=N]          Print progress every N iterations/nodes.
--output-dir DIR        Write a human-readable result report.
--time-limit SECONDS    Stop after wall-clock seconds.
--gap-limit VALUE       Stop when the reported gap is within VALUE.
--node-limit N          Maximum processed MIP nodes.
--open-limit N          Maximum queued open MIP nodes.
--method bb|cut-bb      Plain B&B or root cuts + B&B.
--branch first|most
--node-select dfs|best-bound
--bound-mode parent|tight
```

## Included Problems

Only four small `.lp` files are kept for release: `ortools_mip.lp`, `ortools_mip_array.lp`, `lpp0033.lp`, and `gt2.lp`. Larger benchmark files should stay outside the repository or be downloaded from a release/data URL.

## Sources

Useful public sources include OR-Tools examples, MIPLIB3, MIPLIB 2017, Netlib LP, and COR@L MIP. The helper `tools/benchmark_to_lp.py` can filter and convert selected files when Python CPLEX or OR-Tools ModelBuilder is available.
