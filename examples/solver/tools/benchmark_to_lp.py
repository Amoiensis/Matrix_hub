#!/usr/bin/env python3
"""
Download/filter public LP/MIP benchmark files and export selected models to LP.

This utility is intentionally outside MatrixHub's C code. It helps prepare
medium-size benchmark files for examples/solver without vendoring large
datasets in the repository.

Preferred converter:
    cplex  - robust MPS/LP import and LP export when IBM CPLEX is installed.
Fallback:
    ortools - uses ModelBuilder import/export for MPS/LP files.

Examples:
    python benchmark_to_lp.py --input-dir /tmp/mipdata --out-dir converted_lp --max-output-kb 1536
    python benchmark_to_lp.py --input-archive benchmark.zip --out-dir converted_lp --max-output-kb 1536
    python benchmark_to_lp.py --miplib-names 50v-10,seymour1 --download --out-dir converted_lp
"""

from __future__ import annotations

import argparse
import bz2
import gzip
import os
import shutil
import sys
import tarfile
import tempfile
import urllib.parse
import urllib.request
import zipfile
from dataclasses import dataclass
from pathlib import Path


MIPLIB_URL_PATTERN = "https://miplib.zib.de/WebData/instances/{name}.mps.gz"
SUPPORTED_SUFFIXES = (
    ".mps",
    ".mps.gz",
    ".mps.bz2",
    ".lp",
    ".lp.gz",
    ".lp.bz2",
)


@dataclass
class Candidate:
    name: str
    source: Path
    size_bytes: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Filter benchmark instances by file size and export to LP format."
    )
    parser.add_argument("--input-dir", type=Path, help="Directory with .mps/.lp files.")
    parser.add_argument("--input-archive", type=Path, help="ZIP/TAR archive to scan.")
    parser.add_argument("--url-list", type=Path, help="Text file with one URL per line.")
    parser.add_argument(
        "--miplib-test-url",
        help="URL of a MIPLIB .test file; matching instance URLs are discovered by HEAD.",
    )
    parser.add_argument(
        "--miplib-names",
        help="Comma-separated MIPLIB instance names; downloads from the standard URL pattern.",
    )
    parser.add_argument(
        "--download",
        action="store_true",
        help="Allow network downloads for --url-list or --miplib-names.",
    )
    parser.add_argument("--out-dir", type=Path, required=True, help="Output directory.")
    parser.add_argument("--work-dir", type=Path, default=Path("/tmp/matrixhub_solver_data"))
    parser.add_argument("--min-kb", type=int, default=200, help="Minimum source file size.")
    parser.add_argument("--max-kb", type=int, default=1024, help="Maximum source file size.")
    parser.add_argument(
        "--max-output-kb",
        type=int,
        default=0,
        help="Optional maximum exported LP file size; oversized exports are removed.",
    )
    parser.add_argument("--limit", type=int, default=8, help="Maximum number of models to export.")
    parser.add_argument(
        "--converter",
        choices=("auto", "cplex", "ortools"),
        default="auto",
        help="Model converter backend.",
    )
    parser.add_argument(
        "--keep-intermediate",
        action="store_true",
        help="Keep downloaded/extracted/decompressed source files in --work-dir.",
    )
    return parser.parse_args()


def has_supported_suffix(path: Path | str) -> bool:
    name = str(path).lower()
    return any(name.endswith(suffix) for suffix in SUPPORTED_SUFFIXES)


def base_model_name(path: Path | str) -> str:
    name = Path(str(path)).name
    for suffix in (".mps.gz", ".mps.bz2", ".lp.gz", ".lp.bz2", ".mps", ".lp"):
        if name.lower().endswith(suffix):
            return name[: -len(suffix)]
    return Path(name).stem


def in_size_range(size_bytes: int, min_kb: int, max_kb: int) -> bool:
    return min_kb * 1024 <= size_bytes <= max_kb * 1024


def scan_directory(root: Path, min_kb: int, max_kb: int) -> list[Candidate]:
    candidates: list[Candidate] = []
    for path in root.rglob("*"):
        if not path.is_file() or not has_supported_suffix(path):
            continue
        size_bytes = path.stat().st_size
        if in_size_range(size_bytes, min_kb, max_kb):
            candidates.append(Candidate(base_model_name(path), path, size_bytes))
    return candidates


def extract_from_archive(
    archive: Path, work_dir: Path, min_kb: int, max_kb: int, limit: int
) -> list[Candidate]:
    candidates: list[Candidate] = []
    extract_dir = work_dir / (archive.stem + "_selected")
    extract_dir.mkdir(parents=True, exist_ok=True)

    if zipfile.is_zipfile(archive):
        with zipfile.ZipFile(archive) as zf:
            for info in zf.infolist():
                if info.is_dir() or not has_supported_suffix(info.filename):
                    continue
                if not in_size_range(info.file_size, min_kb, max_kb):
                    continue
                target = extract_dir / Path(info.filename).name
                with zf.open(info) as src, target.open("wb") as dst:
                    shutil.copyfileobj(src, dst)
                candidates.append(Candidate(base_model_name(target), target, target.stat().st_size))
                if len(candidates) >= limit:
                    break
        return candidates

    mode = "r:*"
    if tarfile.is_tarfile(archive):
        with tarfile.open(archive, mode) as tf:
            for member in tf.getmembers():
                if not member.isfile() or not has_supported_suffix(member.name):
                    continue
                if not in_size_range(member.size, min_kb, max_kb):
                    continue
                src = tf.extractfile(member)
                if src is None:
                    continue
                target = extract_dir / Path(member.name).name
                with target.open("wb") as dst:
                    shutil.copyfileobj(src, dst)
                candidates.append(Candidate(base_model_name(target), target, target.stat().st_size))
                if len(candidates) >= limit:
                    break
        return candidates

    raise ValueError(f"unsupported archive: {archive}")


def download_file(url: str, out_dir: Path) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    name = Path(urllib.parse.urlparse(url).path).name
    if not name:
        raise ValueError(f"cannot infer filename from URL: {url}")
    target = out_dir / name
    print(f"[download] {url}")
    with urllib.request.urlopen(url) as response, target.open("wb") as out:
        shutil.copyfileobj(response, out)
    print(f"[download] saved {target} ({target.stat().st_size / 1024:.1f} KB)")
    return target


def read_url_text(url: str) -> str:
    with urllib.request.urlopen(url) as response:
        return response.read().decode("utf-8")


def remote_size(url: str) -> int | None:
    request = urllib.request.Request(url, method="HEAD")
    try:
        with urllib.request.urlopen(request) as response:
            length = response.headers.get("Content-Length")
            return int(length) if length is not None else None
    except Exception as exc:
        print(f"[head-skip] {url}: {exc}")
        return None


def discover_miplib_urls_from_test(args: argparse.Namespace) -> list[str]:
    if not args.miplib_test_url:
        return []
    if not args.download:
        raise ValueError("network discovery requested; pass --download to allow it")
    urls: list[str] = []
    text = read_url_text(args.miplib_test_url)
    print(f"[discover] reading {args.miplib_test_url}")
    candidate_limit = args.limit * 8
    if candidate_limit < args.limit:
        candidate_limit = args.limit
    for raw_line in text.splitlines():
        name = raw_line.strip()
        if not name or name.startswith("#"):
            continue
        if name.endswith(".mps.gz"):
            name = name[:-7]
        url = MIPLIB_URL_PATTERN.format(name=name)
        size = remote_size(url)
        if size is None:
            continue
        if in_size_range(size, args.min_kb, args.max_kb):
            print(f"[candidate] {Path(url).name}: {size / 1024:.1f} KB")
            urls.append(url)
            if len(urls) >= candidate_limit:
                break
        else:
            print(f"[skip-head] {Path(url).name}: {size / 1024:.1f} KB")
    return urls


def collect_downloads(args: argparse.Namespace) -> list[Path]:
    urls: list[str] = discover_miplib_urls_from_test(args)
    if args.url_list:
        for line in args.url_list.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                urls.append(line)
    if args.miplib_names:
        for name in args.miplib_names.split(","):
            name = name.strip()
            if name:
                urls.append(MIPLIB_URL_PATTERN.format(name=name))
    if urls and not args.download:
        raise ValueError("network download requested; pass --download to allow it")
    return [download_file(url, args.work_dir / "downloads") for url in urls]


def decompress_if_needed(path: Path, temp_dir: Path) -> Path:
    lower = path.name.lower()
    if lower.endswith(".gz"):
        target = temp_dir / path.name[:-3]
        with gzip.open(path, "rb") as src, target.open("wb") as dst:
            shutil.copyfileobj(src, dst)
        return target
    if lower.endswith(".bz2"):
        target = temp_dir / path.name[:-4]
        with bz2.open(path, "rb") as src, target.open("wb") as dst:
            shutil.copyfileobj(src, dst)
        return target
    return path


def convert_with_cplex(src: Path, dst: Path) -> None:
    import cplex

    model = cplex.Cplex()
    model.set_log_stream(None)
    model.set_error_stream(None)
    model.set_warning_stream(None)
    model.set_results_stream(None)
    model.read(str(src))
    model.write(str(dst), filetype="lp")


def convert_with_ortools(src: Path, dst: Path) -> None:
    from ortools.linear_solver.python import model_builder

    model = model_builder.ModelBuilder()
    lower = src.name.lower()
    if lower.endswith(".mps"):
        ok = model.import_from_mps_file(str(src))
    elif lower.endswith(".lp"):
        ok = model.import_from_lp_file(str(src))
    else:
        raise ValueError(f"OR-Tools converter needs .mps or .lp after decompression: {src}")
    if not ok:
        raise RuntimeError(f"OR-Tools could not import {src}")
    dst.write_text(model.export_to_lp_string(obfuscate=False), encoding="utf-8")


def choose_converter(name: str) -> str:
    if name != "auto":
        return name
    try:
        import cplex  # noqa: F401

        return "cplex"
    except Exception:
        return "ortools"


def export_candidate(
    candidate: Candidate,
    out_dir: Path,
    converter: str,
    temp_dir: Path,
    max_output_kb: int,
) -> Path | None:
    source = decompress_if_needed(candidate.source, temp_dir)
    out_path = out_dir / f"{candidate.name}.lp"
    out_dir.mkdir(parents=True, exist_ok=True)
    if converter == "cplex":
        convert_with_cplex(source, out_path)
    elif converter == "ortools":
        convert_with_ortools(source, out_path)
    else:
        raise ValueError(f"unknown converter: {converter}")
    print(
        f"[convert] {candidate.source.name} ({candidate.size_bytes / 1024:.1f} KB)"
        f" -> {out_path.name} ({out_path.stat().st_size / 1024:.1f} KB)"
    )
    if (max_output_kb > 0) and (out_path.stat().st_size > max_output_kb * 1024):
        print(f"[skip-output] {out_path.name}: larger than {max_output_kb} KB")
        out_path.unlink()
        return None
    return out_path


def main() -> int:
    args = parse_args()
    args.work_dir.mkdir(parents=True, exist_ok=True)
    args.out_dir.mkdir(parents=True, exist_ok=True)
    min_kb, max_kb = args.min_kb, args.max_kb
    converter = choose_converter(args.converter)
    print(f"[config] size range={min_kb}-{max_kb} KB converter={converter}")

    candidates: list[Candidate] = []
    if args.input_dir:
        candidates.extend(scan_directory(args.input_dir, min_kb, max_kb))
    if args.input_archive:
        candidates.extend(
            extract_from_archive(args.input_archive, args.work_dir, min_kb, max_kb, args.limit)
        )
    for path in collect_downloads(args):
        size_bytes = path.stat().st_size
        if has_supported_suffix(path) and in_size_range(size_bytes, min_kb, max_kb):
            candidates.append(Candidate(base_model_name(path), path, size_bytes))
        else:
            print(f"[skip] {path.name}: {size_bytes / 1024:.1f} KB outside selected range")

    candidates = sorted(candidates, key=lambda item: item.size_bytes)
    if not candidates:
        print("[result] no matching instances found")
        return 1

    temp_parent = None if args.keep_intermediate else tempfile.TemporaryDirectory()
    temp_dir = args.work_dir / "tmp" if args.keep_intermediate else Path(temp_parent.name)
    temp_dir.mkdir(parents=True, exist_ok=True)
    success_count = 0
    try:
        for candidate in candidates:
            if success_count >= args.limit:
                break
            try:
                out_path = export_candidate(
                    candidate, args.out_dir, converter, temp_dir, args.max_output_kb
                )
                if out_path is not None:
                    success_count += 1
            except Exception as exc:
                print(f"[failed] {candidate.source}: {exc}")
    finally:
        if temp_parent is not None:
            temp_parent.cleanup()

    print(f"[result] exported {success_count} LP files into {args.out_dir}")
    return 0 if success_count > 0 else 1


if __name__ == "__main__":
    sys.exit(main())
