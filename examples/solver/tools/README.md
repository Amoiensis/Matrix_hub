# solver tools

本目录保存辅助转换脚本。当前主要工具是 `benchmark_to_lp.py`，用于从公开 benchmark 中筛选并转换 `.lp` 文件。

其他语言: [English](README.en.md) / [日本語](README.ja.md)

示例：

```bash
python3 \
  examples/solver/tools/benchmark_to_lp.py \
  --url-list examples/solver/benchmark_candidates.txt \
  --download \
  --out-dir /tmp/matrixhub_solver_data/lp_export \
  --min-kb 1 --max-kb 20 --max-output-kb 120 --limit 12 --converter cplex
```

转换结果建议保存在仓库外部或 `local_data/` 中。
