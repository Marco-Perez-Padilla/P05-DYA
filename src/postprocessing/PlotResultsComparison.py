#!/usr/bin/env python3
"""
Universidad de La Laguna
Escuela Superior de Ingenieria y Tecnologia
Grado en Ingenieria Informatica
Asignatura: Diseño y Analisis de Algoritmos
Curso: 3º
Practica 5: Algoritmos constructivos y búsquedas por entornos
Autor: Marco Pérez Padilla
Correo: alu0101469348@ull.edu.es
Fecha: 25/04/2026

Archivo PlotResultsComparison.py — Compare solution costs across different algorithms
and configurations against best known values for the MS-CFLP-CI problem.

Generates bar charts showing the total cost obtained by each algorithm for each
instance, with a reference line for the best known value.

Usage:
  # Single results file
  python PlotResultsComparison.py --input resultados.txt

  # Multiple files (each treated as a separate source)
  python PlotResultsComparison.py --input resultados1.txt resultados2.txt

  # All results files in a directory
  python PlotResultsComparison.py --dir data/output/

  # Specific instance(s)
  python PlotResultsComparison.py --input resultados.txt --instance wlp01,wlp02

  # All instances found
  python PlotResultsComparison.py --input resultados.txt --all

  # Save to file
  python PlotResultsComparison.py --input resultados.txt --output comparacion_costes.svg

Options:
  --input FILE [FILE ...]   One or more results files.
  --dir DIR                 Directory containing .txt/.csv files.
  --instance INST           Comma-separated instance names (e.g., wlp01,wlp02).
  --all                     Plot all instances found.
  --output FILE             Save figure to FILE (PNG/PDF/SVG).
  --no-legend               Suppress legend.
  --help                    Show this help message and exit.
"""

import argparse
import os
import sys
from collections import defaultdict
from typing import Dict, List, Optional, Tuple

import matplotlib.pyplot as plt
import numpy as np


# Best known values for instances wlp01 to wlp08
BEST_KNOWN = {
    "wlp01": 28716,
    "wlp02": 52952,
    "wlp03": 64296,
    "wlp04": 84633,
    "wlp05": 103857,
    "wlp06": 111654,
    "wlp07": 162277,
    "wlp08": 187938,
}

# Colors for different algorithms/configurations
BASE_COLORS = [
    "steelblue", "darkorange", "forestgreen", "crimson",
    "mediumpurple", "saddlebrown", "hotpink", "gray",
    "olive", "teal",
]

def get_colors(n: int) -> List[str]:
    """
    Return a list of n distinct colors.
    Uses named colors for n <= 10, and a colormap for larger n.
    """
    if n <= len(BASE_COLORS):
        return BASE_COLORS[:n]
    
    if n <= 20:
        cmap = cm.get_cmap("tab20")
        return [mcolors.to_hex(cmap(i)) for i in range(n)]
    
    cmap = cm.get_cmap("gist_rainbow")
    return [mcolors.to_hex(cmap(i / max(1, n-1))) for i in range(n)]

def extract_instance_name(label: str) -> str:
    """
    Extract instance name from label like 'wlp01.dzn [voraz+LS (VND)]'.
    Returns base name without .dzn extension (e.g., 'wlp01').
    """
    token = label.split()[0]
    if token.endswith(".dzn"):
        token = token[:-4]
    return token


def extract_algorithm_name(label: str) -> str:
    """
    Extract algorithm/configuration name from label.
    'wlp01.dzn [voraz+LS (VND)]' -> 'voraz+LS (VND)'
    'wlp01.dzn [GVNS kmax=5 i=100 (VND-RL)]' -> 'GVNS (VND-RL)'
    """
    if "[" in label and "]" in label:
        algo = label[label.find("[")+1:label.rfind("]")]
        if algo.startswith("GVNS"):
            if "(VND-RL)" in algo:
                return "GVNS (VND-RL)"
            elif "(RVND)" in algo:
                return "GVNS (RVND)"
            elif "(VND)" in algo:
                return "GVNS (VND)"
            else:
                return "GVNS"
        return algo
    return "Unknown"


def find_files_in_directory(directory: str) -> List[str]:
    """Find all .txt and .csv files in a directory."""
    files = []
    for fname in sorted(os.listdir(directory)):
        if fname.endswith(".txt") or fname.endswith(".csv"):
            files.append(os.path.join(directory, fname))
    return files


def load_results(filepaths: List[str]) -> Dict[str, Dict[str, float]]:
    """
    Load results from one or more files.
    Returns: {instance_name: {algorithm_name: cost}}
    """
    data = defaultdict(dict)
    
    for fp in filepaths:
        if not os.path.isfile(fp):
            print(f"[WARNING] File not found: {fp}", file=sys.stderr)
            continue
        
        with open(fp, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("---") or line.startswith("==="):
                    continue
                
                parts = line.split(",")
                if len(parts) < 2:
                    continue
                
                label = parts[0].strip()
                try:
                    cost = float(parts[1].strip())
                except ValueError:
                    continue
                
                instance = extract_instance_name(label)
                algorithm = extract_algorithm_name(label)
                data[instance][algorithm] = cost
    
    return data


def plot_instance(ax, instance: str, algorithms: Dict[str, float],
                  best_known: Optional[float]) -> None:
    """
    Plot a bar chart for one instance comparing all algorithms.
    """
    names = list(algorithms.keys())
    values = list(algorithms.values())
    
    x = np.arange(len(names))
    colors = get_colors(len(names))
    bars = ax.bar(x, values, color=colors, edgecolor="white", linewidth=0.5)
    
    # Add value labels on top of bars
    for bar, val in zip(bars, values):
        ax.text(bar.get_x() + bar.get_width()/2., bar.get_height() + max(values)*0.01,
                f"{val:.0f}", ha="center", va="bottom", fontsize=6, rotation=90)
    
    # Best known reference line
    if best_known is not None:
        ax.axhline(y=best_known, color="red", linestyle="--", linewidth=1.5, alpha=0.7)
        ax.text(len(names)-0.5, best_known + max(values)*0.01,
                f"Best: {best_known}", fontsize=7, color="red", ha="right")
    
    ax.set_xticks(x)
    ax.set_xticklabels(names, rotation=45, ha="right", fontsize=7)
    ax.set_ylabel("Total Cost", fontsize=9)
    ax.set_title(f"{instance}", fontsize=11, fontweight="bold")
    ax.grid(axis="y", linestyle="--", alpha=0.3)
    
    # Adjust y-axis to show best known line clearly
    all_vals = list(values)
    if best_known is not None:
        all_vals.append(best_known)
    y_min = min(all_vals) * 0.85
    y_max = max(all_vals) * 1.15
    ax.set_ylim(y_min, y_max)


def plot_all_instances(data: Dict[str, Dict[str, float]],
                       instance_filter: Optional[List[str]] = None,
                       legend: bool = True) -> plt.Figure:
    """
    Create a grid of subplots, one per instance.
    """
    instances = sorted(data.keys())
    if instance_filter:
        instances = [i for i in instances if i in instance_filter]
    
    n = len(instances)
    if n == 0:
        raise ValueError("No instances to plot.")
    
    cols = min(3, n)
    rows = (n + cols - 1) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(6*cols, 5*rows), squeeze=False)
    axes = axes.flatten()
    
    for idx, instance in enumerate(instances):
        ax = axes[idx]
        best = BEST_KNOWN.get(instance)
        plot_instance(ax, instance, data[instance], best)
    
    for ax in axes[n:]:
        ax.set_visible(False)
    
    fig.suptitle("Solution Cost Comparison Across Algorithms",
                 fontsize=14, fontweight="bold", y=0.995)
    fig.tight_layout()
    return fig


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Plot solution cost comparison across algorithms."
    )
    
    src = parser.add_argument_group("Input sources (choose one)")
    src.add_argument("--input", "-i", nargs="+", metavar="FILE",
                     help="One or more results files.")
    src.add_argument("--dir", metavar="DIR",
                     help="Directory containing .txt/.csv files.")
    
    sel = parser.add_argument_group("Instance selection")
    sel.add_argument("--instance", metavar="INST",
                     help="Comma-separated instance names (e.g., wlp01,wlp02).")
    sel.add_argument("--all", action="store_true",
                     help="Plot all instances found.")
    
    disp = parser.add_argument_group("Display options")
    disp.add_argument("--output", "-o", metavar="FILE",
                      help="Save figure to FILE (PNG/PDF/SVG).")
    disp.add_argument("--no-legend", action="store_true",
                      help="Suppress legend.")
    
    return parser.parse_args()

def _save_or_show_single(fig, input_path: str, output: Optional[str]) -> None:
    """Save or show a single figure."""
    if output:
        base, ext = os.path.splitext(output)
        input_base = os.path.splitext(os.path.basename(input_path))[0]
        if not base.endswith(f"_{input_base}"):
            output_file = f"{base}_{input_base}{ext}"
        else:
            output_file = output
        os.makedirs(os.path.dirname(output_file) or ".", exist_ok=True)
        fig.savefig(output_file, dpi=150, bbox_inches="tight")
        print(f"Figure saved to '{output_file}'")
    else:
        plt.show()


def _save_or_show_multiple(fig, input_path: str, output: Optional[str]) -> None:
    """Save or show a figure when processing multiple files."""
    if output:
        base, ext = os.path.splitext(output)
        input_base = os.path.splitext(os.path.basename(input_path))[0]
        output_file = f"{base}_{input_base}{ext}"
        os.makedirs(os.path.dirname(output_file) or ".", exist_ok=True)
        fig.savefig(output_file, dpi=150, bbox_inches="tight")
        print(f"Figure saved to '{output_file}'")
    else:
        plt.show()

def main() -> None:
    args = parse_arguments()
    
    input_files = []
    if args.input:
        for fp in args.input:
            if os.path.isfile(fp):
                input_files.append(fp)
            else:
                print(f"[WARNING] File not found: {fp}", file=sys.stderr)
    elif args.dir:
        if os.path.isdir(args.dir):
            input_files = find_files_in_directory(args.dir)
        else:
            print(f"[ERROR] Directory not found: {args.dir}", file=sys.stderr)
            sys.exit(1)
    else:
        print("[ERROR] Provide --input or --dir.", file=sys.stderr)
        sys.exit(1)
    
    if not input_files:
        print("[ERROR] No input files found.", file=sys.stderr)
        sys.exit(1)
    
    instance_filter = None
    if args.instance:
        instance_filter = [s.strip() for s in args.instance.split(",")]
    elif not args.all and len(input_files) == 1:
        temp_data = load_results([input_files[0]])
        instances = sorted(temp_data.keys())
        print("Available instances:")
        for i, inst in enumerate(instances, 1):
            print(f"  {i:3d}. {inst}")
        raw = input("Enter numbers (space-separated) or 'a' for all: ").strip()
        if raw.lower() != "a":
            indices = [int(x)-1 for x in raw.split()]
            instance_filter = [instances[i] for i in indices if 0 <= i < len(instances)]
    
    if len(input_files) == 1:
        data = load_results([input_files[0]])
        fig = plot_all_instances(data, instance_filter, legend=not args.no_legend)
        _save_or_show_single(fig, input_files[0], args.output)
    else:
        for fp in input_files:
            data = load_results([fp])
            fig = plot_all_instances(data, instance_filter, legend=not args.no_legend)
            _save_or_show_multiple(fig, fp, args.output)


if __name__ == "__main__":
    main()