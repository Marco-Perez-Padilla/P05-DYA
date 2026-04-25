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

Archivo ComputeDeviations.py — Compute deviations of obtained solutions with respect
to the best known values for the MS-CFLP-CI problem.

Reads one or more results files (CSV-like: label,cost,time) and produces new
files with additional columns: best known cost, absolute gap, and percentage
deviation.

Usage:
  # Single file
  python ComputeDeviations.py --input resultados.txt

  # Multiple files
  python ComputeDeviations.py --input resultados1.txt resultados2.txt

  # All .txt/.csv files in a directory
  python ComputeDeviations.py --dir data/output/

  # Multiple directories
  python ComputeDeviations.py --dirs data/output/config1 data/output/config2

  # Custom output (only valid with single input)
  python ComputeDeviations.py --input resultados.txt --output analisis/datos.txt

Options:
  --input FILE [FILE ...]   One or more results files to process.
  --dir DIR                 Directory containing .txt/.csv files.
  --dirs DIR [DIR ...]      Multiple directories to process.
  --output FILE             Output file (only with --input single file).
  --help                    Show this help message and exit.
"""

import argparse
import os
import sys


# Best known values for instances wlp01 to wlp08 (min of CPLEX and Gurobi)
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

# Column widths for aligned output
COL_LABEL = 50
COL_COST  = 14
COL_TIME  = 12
COL_BEST  = 14
COL_GAP   = 12
COL_PERC  = 10


def extract_instance_name(label: str) -> str:
    """
    Extract the instance name from a label like 'wlp01.dzn [voraz]'.
    Returns the base name without extension (e.g., 'wlp01').
    """
    token = label.split()[0]
    if token.endswith(".dzn"):
        token = token[:-4]
    return token


def ensure_parent_directory_exists(filepath: str) -> None:
    """
    Create the parent directory of filepath if it does not exist.
    """
    parent = os.path.dirname(filepath)
    if parent:
        os.makedirs(parent, exist_ok=True)


def generate_output_filename(input_path: str, output_suffix: str = "_desviacion") -> str:
    """
    Generate an output filename from an input path by inserting a suffix
    before the extension.
    """
    base, ext = os.path.splitext(input_path)
    return f"{base}{output_suffix}{ext or '.txt'}"


def find_files_in_directory(directory: str) -> list:
    """
    Find all .txt and .csv files in a directory (sorted).
    """
    files = []
    for fname in sorted(os.listdir(directory)):
        if fname.endswith(".txt") or fname.endswith(".csv"):
            files.append(os.path.join(directory, fname))
    return files


def process_file(input_path: str, output_path: str) -> None:
    """
    Process a single results file and write the deviation file.
    """
    ensure_parent_directory_exists(output_path)
    
    with open(input_path, "r") as fin, open(output_path, "w") as fout:
        header = (
            f"{'Label':<{COL_LABEL}}"
            f"{'Cost':>{COL_COST}}"
            f"{'Time(s)':>{COL_TIME}}"
            f"{'BestKnown':>{COL_BEST}}"
            f"{'Gap':>{COL_GAP}}"
            f"{'Dev(%)':>{COL_PERC}}\n"
        )
        fout.write(header)
        fout.write("-" * (COL_LABEL + COL_COST + COL_TIME + COL_BEST + COL_GAP + COL_PERC) + "\n")
        
        for line in fin:
            line = line.strip()
            if not line or line.startswith("---") or line.startswith("==="):
                continue
            
            parts = line.split(",")
            if len(parts) < 2:
                fout.write(line + "\n")
                continue
            
            label = parts[0].strip()
            try:
                cost = float(parts[1].strip())
            except ValueError:
                fout.write(line + "\n")
                continue
            
            time_str = parts[2].strip() if len(parts) > 2 else ""
            
            instance = extract_instance_name(label)
            best = BEST_KNOWN.get(instance)
            
            if best is not None:
                gap = cost - best
                deviation = (gap / best) * 100.0
                fout.write(
                    f"{label:<{COL_LABEL}}"
                    f"{cost:>{COL_COST}.2f}"
                    f"{time_str:>{COL_TIME}}"
                    f"{best:>{COL_BEST}}"
                    f"{gap:>{COL_GAP}.2f}"
                    f"{deviation:>{COL_PERC}.2f}\n"
                )
            else:
                fout.write(
                    f"{label:<{COL_LABEL}}"
                    f"{cost:>{COL_COST}.2f}"
                    f"{time_str:>{COL_TIME}}"
                    f"{'---':>{COL_BEST}}"
                    f"{'---':>{COL_GAP}}"
                    f"{'---':>{COL_PERC}}\n"
                )
    
    print(f"Deviation file saved to '{output_path}'")


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compute deviations of solutions from best known values."
    )
    
    src = parser.add_argument_group("Input sources (choose one)")
    src.add_argument("--input", "-i", nargs="+", metavar="FILE",
                     help="One or more results files to process.")
    src.add_argument("--dir", metavar="DIR",
                     help="Directory containing .txt/.csv files.")
    src.add_argument("--dirs", nargs="+", metavar="DIR",
                     help="Multiple directories to process.")
    
    out = parser.add_argument_group("Output options")
    out.add_argument("--output", "-o", metavar="FILE",
                     help="Output file (only valid with single --input).")
    
    return parser.parse_args()


def main() -> None:
    args = parse_arguments()
    
    input_files = []
    
    if args.input:
        for fp in args.input:
            if os.path.isfile(fp):
                input_files.append(fp)
            else:
                print(f"[WARNING] File not found, skipping: {fp}", file=sys.stderr)
    elif args.dir:
        if not os.path.isdir(args.dir):
            print(f"[ERROR] Directory not found: {args.dir}", file=sys.stderr)
            sys.exit(1)
        input_files = find_files_in_directory(args.dir)
    elif args.dirs:
        for d in args.dirs:
            if os.path.isdir(d):
                input_files.extend(find_files_in_directory(d))
            else:
                print(f"[WARNING] Directory not found, skipping: {d}", file=sys.stderr)
    else:
        print("[ERROR] Provide --input, --dir, or --dirs.", file=sys.stderr)
        sys.exit(1)
    
    if not input_files:
        print("[ERROR] No input files found.", file=sys.stderr)
        sys.exit(1)
    
    if len(input_files) == 1 and args.output:
        base, ext = os.path.splitext(args.output)
        input_base = os.path.splitext(os.path.basename(input_files[0]))[0]
        if not base.endswith(f"_{input_base}"):
            output_file = f"{base}_{input_base}{ext}"
        else:
            output_file = args.output
        process_file(input_files[0], output_file)
    elif len(input_files) == 1 and not args.output:
        output_file = generate_output_filename(input_files[0])
        process_file(input_files[0], output_file)
    else:
        for fp in input_files:
            input_base = os.path.splitext(os.path.basename(fp))[0]
            if args.output:
                base, ext = os.path.splitext(args.output)
                output_file = f"{base}_{input_base}{ext}"
            else:
                output_file = generate_output_filename(fp)
            process_file(fp, output_file)


if __name__ == "__main__":
    main()