![MatrixHub_logo](./docs/img/MatrixHub_logo.png)

# Matrix Hub v2.0

Languages: [中文](README.md) / [English](README.en.md) / [日本語](README.ja.md)

Matrix Hub v2.0 is a lightweight toolkit for matrix computing and optimization solving. Its goal is to provide embeddable, scriptable, and command-line matrix computing capabilities with a small C codebase. It is suitable for embedded deployment, lightweight applications, small engineering workflows, and secondary development such as control algorithms, optimization prototypes, and engineering calculations.

The project provides three usage forms: a C header library for matrix operations, a Matlab-like `.m` script and interactive calculator, and a lightweight LP/MIP solver that can be used either as plugins or as a standalone command-line program. The prebuilt calculator and solver stay around the 100 KB scale, making them easy to deploy and integrate. The CLI and solver are also available as WebAssembly pages for direct browser-based use.

## Download And Run

Prebuilt ready-to-run packages are currently provided for Apple Silicon macOS and Windows x64. Download, unzip, and run directly. Other platforms can be built from source, or you can contact the author for help.

| Platform | Download | CLI / Solver Size | Contents |
| --- | --- | --- | --- |
| macOS Apple Silicon | [MatrixHub-v2.0-macos-arm64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-macos-arm64.zip) | approx. 119 KB / 136 KB | CLI script calculator, LP/MIP solver, help files, and examples |
| Windows x64 | [MatrixHub-v2.0-windows-x64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-windows-x64.zip) | approx. 345 KB / 365 KB | CLI script calculator, LP/MIP solver, help files, and examples |

## Use In Browser

For a quick trial of the matrix script calculator or LP/MIP solver, use the WebAssembly builds directly in a browser. Computation runs locally in the browser, which is convenient for demos, small problem checks, and environments where installing binaries is not desirable.

| Web build | URL | Notes |
| --- | --- | --- |
| MatrixHub CLI | https://www.peking.ren/matrixhub_cli/ | Matlab-like `.m` script calculator and interactive input |
| MatrixHub Solver | https://www.peking.ren/matrixhub_solver/ | LP/MIP `.lp` file solver with local browser execution |

## Project Info

| Item | Detail |
| --- | --- |
| Project | https://github.com/Amoiensis/Matrix_hub |
| Author | Sebin (Xiping.Yu) |
| Email | Amoiensis@outlook.com |

Questions, feedback, and application notes are welcome by email.

## Usage Modes

| Mode | Intended use | Entry point |
| --- | --- | --- |
| C header library | Matrix operations and secondary development in C/C++, such as control or optimization prototypes | `include/matrix.h`, `include/solver_plugin/*.h` |
| Interactive script calculator | Run Matlab-like `.m` files or use the `>>` REPL line by line | `build/Matrix_Hub_v2_0_cli` |
| Lightweight LP/MIP solver | Roughly 100 KB solver/plugin for `.lp` files, progress output, gap tracking, and result reports | `build/Matrix_Hub_v2_0_solver` |

## Layout

```text
include/                  public C headers
include/solver_plugin/    linear, LP, and MIP solver plugins
apps/                     interactive script calculator and LP/MIP solver executables
examples/c_library/       standalone C-library example
examples/scripts/         MatrixHub .m script examples
examples/solver/          solver usage, small problem files, and tools
help/                     help topics for functions, commands, and solver mode
```

## Supported Operations

| Area | CLI / script form | C API / plugin | Notes |
| --- | --- | --- | --- |
| Matrix creation | `A = [1 2; 3 4]` | `Matrix_gen` | Create matrices from contiguous data |
| Common matrices | `eye`, `zeros`, `ones`, `hilbert` | `M_I`, `M_Zeros`, `M_Ones`, `Hilbert` | Identity, zero, one, and Hilbert matrices |
| Arithmetic | `A+B`, `A-B`, `A*B`, `2*A` | `M_add_sub`, `M_mul`, `M_numul` | Matrix and scalar arithmetic |
| Element-wise ops | `pmul(A,B)`, `pdiv(A,B)` | `M_pmuldiv` | Hadamard multiply/divide |
| Indexing and slicing | `A(1,2)`, `A[:,1]` | `M_Cut`, `M_Sample` | 1-based indexing in script mode |
| Basic properties | `det`, `tr`, `norm`, `cond`, `rank` | `M_det`, `M_tr`, `M_norm`, `M_cond`, `M_rank` | Common matrix diagnostics |
| Inverse and pseudo-inverse | `inv(A)`, `pinv(A)` | `M_Inverse`, `M_pinv` | Small-scale matrix calculations |
| Transforms | `uptri`, `lowtri`, `diatri`, `householder`, `hessenberg` | corresponding `M_*` helpers | Elementary and orthogonal-transform routines |
| Decompositions | `[Q,R]=qr(A)`, `[U,S,V]=svd(A)` | `M_QR`, `M_SVD` | Multi-output script syntax |
| Eigen analysis | `[D,E]=eigen(A)`, `eigen_max(A)` | `M_eigen`, `M_eigen_max` | Eigenvalues and eigenvectors |
| Script control | `for`, `if/elif/else`, `vars`, `clean`, `del` | - | REPL and script workflow |
| Linear equations | - | `Linear_Solve` | Dense Gaussian solve for `A * X = B` |
| LP/MIP solving | solver CLI | `LP_Solver`, `MIP_Solver` | Lightweight LP/MIP plugins |

## Use As A C Library

Include `matrix.h` directly. Include solver plugin headers only when needed.

```c
#include "matrix.h"
#include "solver_plugin/plugin_Linear_Solver.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"
```

For a more complete C-library example, see [examples/c_library/main.c](examples/c_library/main.c). It covers matrix creation, dense linear solving, LP/MIP plugin calls, and memory cleanup.

Run the example:

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

## Interactive Script Calculator

```bash
./build/Matrix_Hub_v2_0_cli
./build/Matrix_Hub_v2_0_cli examples/scripts/basic.m
./build/Matrix_Hub_v2_0_cli --debug debug.txt examples/scripts/basic.m
```

Example session:

```text
>> A = [1 2; 3 4]
>> B = inv(A)
>> [Q, R] = qr(A)
>> vars
>> help qr
>> clean
>> quit
```

The script mode supports matrix literals, ranges, indexing, slicing, nested calls, multi-output functions, `for` loops, `if/elif/else`, and commands such as `clear`, `clean`, `del`, `vars`, and `help`.

## Lightweight LP/MIP Solver

LP stands for Linear Programming, and MIP stands for Mixed-Integer Programming. Matrix Hub's solver focuses on linear objectives, linear constraints, and continuous/integer/binary variables, so it is close to the common MILP (Mixed-Integer Linear Programming) use case.

A compact problem form is:

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

Background: [Linear programming](https://en.wikipedia.org/wiki/Linear_programming), [Mixed-integer programming / Integer programming](https://en.wikipedia.org/wiki/Mixed-integer_programming).

```bash
./build/Matrix_Hub_v2_0_solver --help
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp \
  --progress=25 --node-limit=5000 --time-limit=30 --gap-limit=0.05 \
  --output-dir solver_report_store
```

Common options include `--progress`, `--output-dir`, `--time-limit`, `--gap-limit`, `--node-limit`, `--open-limit`, `--method`, `--branch`, `--node-select`, and `--bound-mode`.

The release keeps only four small problem files: `gt2.lp`, `ortools_mip.lp`, `ortools_mip_array.lp`, and `lpp0033.lp`. Larger benchmark sources are documented in [examples/solver/README.md](examples/solver/README.md).

## Help

MatrixHub help is organized around the way you use the project:

| Context | How to get help |
| --- | --- |
| C API | In the CLI, run `help M_mul`, `help M_QR`, or `help c_library`; or read `help/M_mul.txt` and `include/matrix.h` directly |
| CLI calculator | In the REPL, run `help`, `help script_mode`, `help script_functions`, or `help vars` |
| LP/MIP solver | Run `./build/Matrix_Hub_v2_0_solver --help`, or use `help solver_mode` in the CLI |

```text
help
help script_mode
help script_functions
help solver_mode
help M_mul
```

Main help topics are bilingual Chinese/English. This README is also available in [中文](README.md) and [日本語](README.ja.md).

## Build

```bash
cmake -S . -B build
cmake --build build
```

Main binaries:

```text
build/Matrix_Hub_v2_0_demo      C-library example
build/Matrix_Hub_v2_0_cli       interactive script calculator
build/Matrix_Hub_v2_0_solver    lightweight LP/MIP file solver
```

## Maintenance Note

The v2.0 release tree keeps only the current source version. Historical source directories are preserved by the old main/archive branch history.
