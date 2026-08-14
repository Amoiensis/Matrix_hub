![MatrixHub_logo](./docs/img/MatrixHub_logo.png)

# Matrix Hub v2.0

语言: [中文](README.md) / [English](README.en.md) / [日本語](README.ja.md)

Matrix Hub v2.0 是一个轻量级矩阵计算与优化求解工具集，核心目标是在较小代码体积下提供可嵌入、可脚本化、可命令行调用的矩阵计算能力。项目采用 C 语言实现，适合嵌入式部署、轻量应用、小规模工程计算，以及在此基础上的二次开发，例如控制算法、二次优化和工程计算原型。

项目包含三种使用形态：矩阵运算 C 头文件库、类 Matlab 的 `.m` 脚本/交互式计算器，以及可作为插件或独立程序使用的轻量 LP/MIP 求解器。预编译求解器与交互式计算器的体积保持在约 100KB 级别，便于部署和集成；CLI 与 solver 也已部署为 WebAssembly 网页版，可以在浏览器中直接访问使用。

## 直接下载使用

当前提供 Apple Silicon macOS 和 Windows x64 的预编译版本，下载解压后即可直接使用。其他平台可以从源码编译，或通过邮件联系作者协助。

| 平台 | 下载 | 内容 |
| --- | --- | --- |
| macOS Apple Silicon | [MatrixHub-v2.0-macos-arm64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-macos-arm64.zip) | CLI 脚本计算器、LP/MIP Solver、help 文档和示例 |
| Windows x64 | [MatrixHub-v2.0-windows-x64.zip](https://github.com/Amoiensis/Matrix_hub/releases/download/v2.0/MatrixHub-v2.0-windows-x64.zip) | CLI 脚本计算器、LP/MIP Solver、help 文档和示例 |

## 网页直接使用

如果只想快速体验矩阵脚本或 LP/MIP 求解器，也可以直接使用 WebAssembly 版本。计算在浏览器本地执行，适合演示、小规模问题验证和无安装环境下的临时使用。

| 网页版 | 入口 | 说明 |
| --- | --- | --- |
| MatrixHub CLI | https://www.peking.ren/matrixhub_cli/ | 类 Matlab `.m` 脚本计算器和交互式输入 |
| MatrixHub Solver | https://www.peking.ren/matrixhub_solver/ | LP/MIP `.lp` 文件求解器，本地化求解 |

## 项目信息

| 项目 | 信息 |
| --- | --- |
| Project | https://github.com/Amoiensis/Matrix_hub |
| Author | Sebin (Xiping.Yu) |
| Email | Amoiensis@outlook.com |

欢迎通过邮件交流使用问题、建议和工程应用场景。

## 三种用法

| 用法 | 目标用户 | 入口 |
| --- | --- | --- |
| C 头文件库 | 在 C/C++ 项目中做矩阵运算和二次开发，如控制算法、优化算法原型 | `include/matrix.h`, `include/solver_plugin/*.h` |
| 交互式脚本计算器 | 像 Matlab `.m` 文件一样运行脚本，也支持 `>>` 逐行交互 | `build/Matrix_Hub_v2_0_cli` |
| 轻量 LP/MIP 求解器 | 约 100KB 级求解器/插件，读取 `.lp` 文件并输出进度、gap 和结果 | `build/Matrix_Hub_v2_0_solver` |

## 目录结构

```text
include/                  C 头文件库入口
include/solver_plugin/    Linear/LP/MIP 求解器插件
apps/                     交互式脚本计算器与轻量 LP/MIP solver 程序
examples/c_library/       C 库调用示例和独立 CMakeLists.txt
examples/scripts/         MatrixHub .m 脚本示例
examples/solver/          LP/MIP 求解器用法、问题文件和转换工具
help/                     函数、命令和 solver 帮助文档
```

## 支持的运算

| 类别 | 脚本/CLI 写法 | C API / 插件 | 说明 |
| --- | --- | --- | --- |
| 矩阵创建 | `A = [1 2; 3 4]` | `Matrix_gen` | 从连续数据生成矩阵 |
| 矩阵复制 | - | `Matrix_copy` | 深拷贝矩阵 |
| 内存释放 | - | `M_free` | 释放 MatrixHub 创建的矩阵 |
| 显示输出 | `print(A)` | `M_print` | 打印矩阵 |
| 单位矩阵 | `eye(n)` | `M_I` | 生成单位矩阵 |
| 零矩阵 | `zeros(m,n)` | `M_Zeros` | 生成全零矩阵 |
| 一矩阵 | `ones(m,n)` | `M_Ones` | 生成全一矩阵 |
| Hilbert 矩阵 | `hilbert(n)` | `Hilbert` | 常用于数值稳定性测试的病态矩阵 |
| 范围向量 | `[2:6]`, `[1:1.2:100]` | - | 支持 start:end 和 start:step:end |
| 加减法 | `A + B`, `A - B` | `M_add_sub` | 矩阵加减 |
| 矩阵乘法 | `A * B` | `M_mul` | 标准矩阵乘法 |
| 标量乘除 | `2*A`, `A/3` | `M_numul` | 标量与矩阵混合计算 |
| 对应元素乘除 | `pmul(A,B)`, `pdiv(A,B)` | `M_pmuldiv` | Hadamard 乘除 |
| 转置 | `A'`, `transpose(A)` | `M_T` | 矩阵转置 |
| 求逆 | `inv(A)` | `M_Inverse` | 适合小规模矩阵计算 |
| 伪逆 | `pinv(A)` | `M_pinv` | 默认逐步改进为更稳定风格 |
| 行列式 | `det(A)` | `M_det` | 方阵行列式 |
| 迹 | `tr(A)`, `trace(A)` | `M_tr` | 方阵对角线求和 |
| 范数 | `norm(A)` | `M_norm` | 支持常用矩阵/向量范数 |
| 条件数 | `cond(A)` | `M_cond` | 基础条件数估计 |
| 秩 | `rank(A)` | `M_rank` | 矩阵秩 |
| 绝对值 | `abs(A)` | `M_abs` | 元素绝对值 |
| 求和 | `sum(A)` | `M_sum` | 元素求和 |
| 最小/最大 | `min(A)`, `max(A)` | `M_min`, `M_max` | 元素最值 |
| 对角处理 | `diag(A)` | `M_Dia_Inv`, related helpers | 对角提取/对角矩阵相关能力 |
| 矩阵尺寸 | `shape(A)`, `size(A)` | `A->row`, `A->column` | 返回 `[rows columns]` |
| 单元素索引 | `A(1,2)` | direct data access | 脚本索引从 1 开始 |
| 切片 | `A[:,1]`, `A[1:2,2:3]` | `M_Cut`, `M_Sample` | 行列截取和采样 |
| 查找 | `find(A)` | `M_find` | 查找满足条件的元素位置 |
| 逻辑比较 | `A == B`, `A < B` | `M_logic`, `M_logic_equal` | 标量/矩阵逻辑判断 |
| 行列交换 | `swap(A,...)` | `M_Swap` | 行或列交换 |
| 初等变换 | - | `M_E_trans`, `Etrans_2_Matrix`, `Etrans_4_Inverse` | 基础初等变换 |
| 上三角化 | `[T,U] = uptri(A)` | `M_Uptri_` | 保留初等变换实现 |
| 下三角化 | `[T,L] = lowtri(A)` | `M_Lowtri_` | 保留初等变换实现 |
| 对角化 | `[Tl,D,Tr] = diatri(A)` | `M_Diatri_` | 基础对角化过程 |
| Householder | `[H,y] = householder(x)` | `householder`, `M_householder` | 正交变换基础 |
| Hessenberg | `[Q,H] = hessenberg(A)` | `M_householder` based path | Hessenberg 变换 |
| QR 分解 | `[Q,R] = qr(A)` | `M_QR` | Householder 风格 QR，支持非方阵 |
| SVD 分解 | `[U,S,V] = svd(A)` | `M_SVD` | 小规模 SVD 分解 |
| 特征值/向量 | `[D,E] = eigen(A)`, `eig(A)` | `M_eigen`, `M_eigen_val` | 特征值和特征向量 |
| 最大特征对 | `[lambda,v] = eigen_max(A)` | `M_eigen_max` | 幂迭代风格最大特征值 |
| 多输出兼容别名 | `qr_q`, `svd_u`, `eigen_vec` | corresponding C helpers | 老接口兼容，新脚本推荐多输出 |
| 循环 | `for k = 1:3 ... end` | - | 脚本控制流 |
| 判断 | `if/elif/else/end` | - | 脚本条件分支 |
| 变量管理 | `vars`, `who`, `clean`, `del A` | - | REPL 工作区管理 |
| 清屏和帮助 | `clear`, `help qr` | `help/*.txt` | 查询命令、函数和 solver 说明 |
| 线性方程 | - | `Linear_Solve` | 稠密高斯消元，避免显式求逆 |
| LP 求解 | solver CLI | `LP_Solver` | 默认修订单纯形，可选内点法 |
| MIP 求解 | solver CLI | `MIP_Solver` | 轻量 cuts + branch-and-bound |

## 用法 1：作为 C 库引用

Matrix Hub 仍保持头文件为主的简单使用方式。直接包含 `include/matrix.h`，需要 solver 时再包含 `include/solver_plugin/*.h`。

```c
#include "matrix.h"
#include "solver_plugin/plugin_Linear_Solver.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"
```

一个最小的线性方程例子：

```c
MATRIX_TYPE a_data[4] = {
    3, 1,
    1, 2
};
MATRIX_TYPE b_data[2] = {9, 8};

Matrix *A = Matrix_gen(2, 2, a_data);
Matrix *b = Matrix_gen(2, 1, b_data);
Matrix *x = Linear_Solve(A, b);

M_print(x);

M_free(A);
M_free(b);
M_free(x);
```

更完整的 C 库调用方式可以查看：[examples/c_library/main.c](examples/c_library/main.c)。该示例覆盖矩阵创建、线性方程求解、LP/MIP 插件调用和内存释放等用法。

运行完整示例：

```bash
cmake -S examples/c_library -B build_c_example -DMATRIXHUB_ROOT=$PWD
cmake --build build_c_example
./build_c_example/matrixhub_c_example
```

## 用法 2：交互式脚本计算器

交互模式会先显示 MatrixHub logo，然后用简洁的 `>>` 提示输入：

```bash
./build/Matrix_Hub_v2_0_cli
```

```text
\ \     __  ___     __      _       __ __     __
 \ \   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /
 / /  / /|_/ / _ `/ __/ __/ /\ \ // _  / // / _ \
/ /  /_/  /_/\_,_/\__/_/ /_//_\_\/_//_/\_,_/_.__/
MatrixHub v2.0 script mode
Project: https://github.com/Amoiensis/Matrix_hub

>> A = [4 1 2; 1 3 0; 2 0 5]
>> b = [7; 8; 9]
>> x = pinv(A) * b
>> residual = norm(A*x - b)
>> [Q, R] = qr(A)
>> vars
>> help qr
```

脚本模式适合放在文档或课程材料里，像 Markdown 代码块一样展示一段完整计算：

```matlab
A = [4 1 2; 1 3 0; 2 0 5];
b = [7; 8; 9];
x = pinv(A) * b
residual = norm(A*x - b)

[Q, R] = qr(A);
[U, S, V] = svd(A);
[D, E] = eigen(A);

for k = 1:3
    P = A + k * eye(3)
end

if det(A) > 0
    sign = 1
elif det(A) < 0
    sign = -1
else
    sign = 0
end
```

运行：

```bash
./build/Matrix_Hub_v2_0_cli examples/scripts/basic.m
./build/Matrix_Hub_v2_0_cli --debug debug.txt examples/scripts/basic.m
```

REPL 支持左/右移动光标，上/下浏览历史输入；脚本中分号会抑制输出，`--debug` 会把变量快照写入文本文件。

## 用法 3：轻量 LP/MIP 求解器

solver 模式用于读取 `.lp` 文件并求解 LP/MIP 问题。LP 是 Linear Programming（线性规划），MIP 是 Mixed-Integer Programming（混合整数规划）；本项目求解器面向线性目标、线性约束和整数/二进制变量，因此更接近常见的 MILP（Mixed-Integer Linear Programming）使用场景。它保持约 100KB 级别的小体积，适合轻量部署和简易开发使用，也可以展示分支定界过程中的 incumbent、bound 和 gap。

问题可粗略写成下面的标准型，其中 `x` 是连续变量，`y` 是整数或二进制变量：

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

背景参考：[Linear programming](https://en.wikipedia.org/wiki/Linear_programming)、[Mixed-integer programming / Integer programming](https://en.wikipedia.org/wiki/Mixed-integer_programming)。

```bash
./build/Matrix_Hub_v2_0_solver --help
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
```

一个带进度和结果报告的 MIP 示例：

```bash
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp \
  --progress=25 \
  --node-limit=5000 \
  --time-limit=30 \
  --gap-limit=0.05 \
  --output-dir solver_report_store
```

输出会类似这样：

```text
MatrixHub v2.0 solver mode
Project: https://github.com/Amoiensis/Matrix_hub

[example parser] parsed vars=188 rows=29 integer=188
[MIP] node=0 open=1 cuts=... incumbent=none lower_bound=-inf upper_bound=none gap=inf event=start
[MIP] node=25 open=... cuts=... incumbent=... lower_bound=... upper_bound=... gap=... event=relax
status = NODE_LIMIT (2)
incumbent_status = FEASIBLE
objective = ...
[example report] wrote solver_report_store/gt2_mip_result.txt
```

常用参数：

```text
--progress[=N]       每 N 次迭代/节点显示进度
--output-dir DIR     输出人类可读的结果报告
--time-limit SEC     最大求解时间
--gap-limit VALUE    达到 gap 后停止
--node-limit N       MIP 最大节点数
--open-limit N       MIP 最大 open node 数
--method bb|cut-bb   B&B 或 root cuts + B&B
--branch first|most  分支变量选择
--node-select dfs|best-bound
--bound-mode parent|tight
```

`examples/solver/problems/` 只保留小型发布样例：`gt2.lp`、`ortools_mip.lp`、`ortools_mip_array.lp`、`lpp0033.lp`。更大的 LP/MIP benchmark 不随仓库提交，下载来源见 [examples/solver/README.md](examples/solver/README.md)。

## 帮助

MatrixHub 的帮助分为三类：

| 场景 | 使用方式 |
| --- | --- |
| C 函数/API | 在 CLI 中输入 `help M_mul`、`help M_QR`、`help c_library`，或直接查看 `help/M_mul.txt`、`include/matrix.h` |
| 命令行计算器 | 在 REPL 中输入 `help`、`help script_mode`、`help script_functions`、`help vars` |
| LP/MIP solver | 运行 `./build/Matrix_Hub_v2_0_solver --help`，或在 CLI 中输入 `help solver_mode` |

常用主题：

```text
help
help script_mode
help script_functions
help solver_mode
help c_library
help M_mul
```

主要帮助入口采用中英双语；单个函数的详细说明保存在对应的 `help/*.txt` 文件中。

## 构建

```bash
cmake -S . -B build
cmake --build build
```

主要产物：

```text
build/Matrix_Hub_v2_0_demo      C 库调用示例
build/Matrix_Hub_v2_0_cli       交互式脚本计算器
build/Matrix_Hub_v2_0_solver    轻量 LP/MIP 文件求解器
```

## 维护说明

v2.0 发布树只保留当前版本源码。历史版本目录由旧 main/archive 分支保存，不再放入主开发目录。
