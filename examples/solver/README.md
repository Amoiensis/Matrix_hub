# MatrixHub 求解器用法

本目录展示 MatrixHub v2.0 的 LP/MIP 文件求解器用法。求解器读取小规模 `.lp` 文件，调用 `include/solver_plugin/` 中的 LP/MIP 插件完成计算；它是面向轻量部署和简易开发使用的求解器组件。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

## 构建

从仓库根目录构建：

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/Matrix_Hub_v2_0_solver lp  examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/ortools_mip_array.lp
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/lpp0033.lp --progress=25
./build/Matrix_Hub_v2_0_solver mip examples/solver/problems/gt2.lp --progress=25 --node-limit=5000
```

## 常用参数

```text
--progress[=N]          每 N 次迭代/节点显示进度
--output-dir DIR        写出人类可读的结果报告
--time-limit SECONDS    最大求解时间
--gap-limit VALUE       gap 达到给定值后停止
--node-limit N          MIP 最大处理节点数
--open-limit N          MIP 最大 open node 数
--method bb|cut-bb      普通 B&B 或 root cuts + B&B
--cuts off|N            关闭 root cuts 或设置 cut 轮数
--heuristic off|root|N  控制 primal heuristic 的触发频率
--repair-limit N        限制 heuristic 修复步数
--branch first|most     分支变量选择
--node-select dfs|best-bound
--bound-mode parent|tight
--feas-tol VALUE
--int-tol VALUE
```

## 发布内置问题

发布目录只保留 4 个小 `.lp` 文件：

| 文件 | 来源 | 说明 |
| --- | --- | --- |
| `ortools_mip.lp` | Google OR-Tools MIP 示例 | 极小 LP/MIP smoke 示例 |
| `ortools_mip_array.lp` | Google OR-Tools array MIP 示例 | 极小 binary-array 示例 |
| `lpp0033.lp` | MIPLIB3 `p0033` 转换为 LP 格式 | 小型整数 benchmark |
| `gt2.lp` | MIPLIB3 `gt2` 转换为 LP 格式 | 小型 mixed-integer benchmark |

其他本地 benchmark 文件放在 `local_data/solver_problems/`，该目录被 Git 忽略。后续更大的、处理好的 `.lp` 文件建议通过 release/data URL 单独下载。

## 下载来源

需要更多测试数据时，可以从这些公开集合获取：

- OR-Tools LP/MIP 教程：
  - https://developers.google.com/optimization/lp/lp_example
  - https://developers.google.com/optimization/mip/mip_example
  - https://developers.google.com/optimization/mip/mip_var_array
- MIPLIB3 compressed MPS：
  - https://miplib2010.zib.de/miplib3/miplib3/
- MIPLIB 2017：
  - https://miplib.zib.de/download.html
  - https://miplib.zib.de/downloads/benchmark-v2.test
- Netlib LP：
  - https://www.netlib.org/lp/data/
- COR@L MIP：
  - https://coral.ise.lehigh.edu/data-sets/mixed-integer-instances/

当前 parser 支持常见 CPLEX-LP 文本格式的紧凑子集。

## 转换工具

`tools/benchmark_to_lp.py` 可以在 Python CPLEX 或 OR-Tools ModelBuilder 可用时筛选并转换 benchmark 文件。

```bash
python3 \
  examples/solver/tools/benchmark_to_lp.py \
  --url-list examples/solver/benchmark_candidates.txt \
  --download \
  --out-dir /tmp/matrixhub_solver_data/lp_export \
  --min-kb 1 --max-kb 20 --max-output-kb 120 --limit 12 --converter cplex
```

除内置小样例外，转换得到的大型 benchmark 建议保存在仓库外部。
