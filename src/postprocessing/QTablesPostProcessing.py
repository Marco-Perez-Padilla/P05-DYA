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

Archivo QTablesPostProcessing.py — Post-processing tool for VND-RL Q-value evolution CSV files.

Handles large CSV files (tens of thousands of rows) gracefully by offering
adaptive binning, keeping plots legible with proper scaling.

Usage:
  # Single file with adaptive binning
  python QTablesPostProcessing.py --files q_evolution_wlp01.dzn.csv --bins 500

  # All instances in one directory, binned to 500 points
  python QTablesPostProcessing.py --dir data/output/q_evolution/default --all --bins 500

  # Compare two configurations for a specific instance, SVG output
  python QTablesPostProcessing.py --dirs data/output/q_evolution/default data/output/q_evolution/vnd-rl-100 --instance wlp01 --output comparison.svg

  # Compare two configurations for all shared instances with binning
  python QTablesPostProcessing.py --dirs data/output/q_evolution/default data/output/q_evolution/vnd-rl-100 --all --bins 300 --output all_comparison.svg

Options:
  --files FILE [FILE ...]   One or more CSV files to plot directly.
  --dir DIR                 Directory containing q_evolution_*.csv files.
  --dirs DIR [DIR ...]      Multiple directories (each treated as a separate config).
  --instance INST           Comma-separated instance base names (e.g. wlp01,wlp02).
  --all                     Select all instances found in the directory/directories.
  --max_iter N              Truncate to the first N rows (global row index).
  --bins N                  Aggregate data into N equal-width bins (mean per bin).
  --output FILE             Save figure to FILE (PNG/PDF/SVG supported).
  --no-legend               Suppress legend.
"""

from __future__ import annotations

import argparse
import os
import sys
from typing import Dict, List, Optional, Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


CSV_PREFIX = "q_evolution_"
CSV_SUFFIX = ".csv"

PLOT_COLOURS = ["tab:blue", "tab:orange", "tab:green", "tab:red"]
PLOT_LINESTYLES = ["-", "--", "-.", ":"]

FIGURE_WIDTH_SINGLE = 12
FIGURE_HEIGHT_SINGLE = 6
FIGURE_WIDTH_MULTI = 6
FIGURE_HEIGHT_MULTI = 4.5


class QEvolutionData:
    """
    Container for Q-value evolution data loaded from a single CSV file.
    Provides lazy-loading, data validation, and preprocessing capabilities.
    """

    def __init__(self, filepath: str, instance_name: str, config_name: str = ""):
        """
        Initialize data container with file path and metadata.

        @param filepath      Absolute or relative path to the CSV file.
        @param instance_name Human-readable label for the instance (e.g. "wlp01.dzn").
        @param config_name   Label for the configuration/run (e.g. "vnd-rl-100").
        """
        self.filepath = filepath
        self.instance_name = instance_name
        self.config_name = config_name
        self._dataframe: Optional[pd.DataFrame] = None
        self._is_partial: bool = False

    @property
    def df(self) -> pd.DataFrame:
        """
        Lazy-load and return the internal DataFrame.
        """
        if self._dataframe is None:
            self._load_from_file()
        return self._dataframe

    @property
    def ls_columns(self) -> List[str]:
        """
        Return all columns starting with 'LS' (Local Search columns).
        """
        return [col for col in self.df.columns if col.startswith("LS")]

    @property
    def is_partial(self) -> bool:
        """
        Check if data contains NaN values (indicating partial/incomplete data).
        """
        _ = self.df
        return self._is_partial

    @property
    def label(self) -> str:
        """
        Generate a human-readable label for the dataset.
        """
        base = f"{self.config_name} / {self.instance_name}" if self.config_name else self.instance_name
        return f"{base}  ⟳" if self.is_partial else base

    @property
    def n_rows(self) -> int:
        """
        Return the number of rows in the loaded DataFrame.
        """
        return len(self.df)

    def get_data(self, max_iter: Optional[int] = None) -> pd.DataFrame:
        """
        Retrieve raw data, optionally truncated to max_iter rows.

        @param max_iter Maximum number of rows to return. None means no limit.
        @return DataFrame with the requested rows.
        """
        data = self.df
        if max_iter is not None:
            data = data.iloc[:max_iter]
        return data

    def get_binned(self, max_iter: Optional[int] = None, num_bins: int = 300) -> pd.DataFrame:
        """
        Aggregate data into equal-width intervals by computing mean per bin.
        Reduces large datasets while preserving overall trends.

        @param max_iter Maximum number of rows to consider before binning.
        @param num_bins Number of bins to create.
        @return DataFrame with one row per bin (mean of LS columns in that bin).
        """
        dataframe = self.get_data(max_iter)
        row_count = len(dataframe)
        
        if row_count <= num_bins:
            return dataframe
        
        ls_cols = self.ls_columns
        dataframe = dataframe.copy()
        dataframe["__bin__"] = pd.cut(dataframe.index, bins=num_bins, labels=False)
        result = dataframe.groupby("__bin__")[ls_cols].mean().reset_index(drop=True)
        return result

    def _load_from_file(self) -> None:
        """
        Load CSV file with error handling and data validation.
        Sets _dataframe to empty DataFrame on failure.
        """
        try:
            header = pd.read_csv(self.filepath, nrows=0).columns.tolist()
        except Exception as exc:
            print(f"[WARNING] Could not read '{self.filepath}': {exc}", file=sys.stderr)
            self._dataframe = pd.DataFrame()
            return

        dtype_map = {col: np.float32 for col in header if col != "iter"}
        try:
            dataframe = pd.read_csv(self.filepath, dtype=dtype_map, on_bad_lines="skip")
        except Exception as exc:
            print(f"[WARNING] Failed to parse '{self.filepath}': {exc}", file=sys.stderr)
            self._dataframe = pd.DataFrame()
            return

        dataframe.drop(columns=["iter"], errors="ignore", inplace=True)
        rows_before = len(dataframe)
        dataframe.dropna(inplace=True)
        
        if rows_before > len(dataframe):
            self._is_partial = True
        
        dataframe.reset_index(drop=True, inplace=True)
        self._dataframe = dataframe


class QEvolutionLoader:
    """
    Factory class for loading QEvolutionData from various sources.
    Supports single files, directories, and multi-directory configurations.
    """

    @staticmethod
    def from_files(filepaths: List[str]) -> List[QEvolutionData]:
        """
        Load data from a list of CSV files.
        Instance names are extracted from filenames.

        @param filepaths List of paths to CSV files.
        @return List of QEvolutionData objects.
        """
        datasets = []
        for filepath in filepaths:
            basename = os.path.basename(filepath)
            instance_name = QEvolutionLoader._extract_instance_name(basename)
            datasets.append(QEvolutionData(filepath, instance_name))
        return datasets

    @staticmethod
    def from_directory(directory: str, config_name: str = "",
                       instance_filter: Optional[List[str]] = None) -> List[QEvolutionData]:
        """
        Load all q_evolution_*.csv files from a directory.
        Optionally filter by instance names.

        @param directory       Path to the directory.
        @param config_name     Label to attach to every loaded object.
        @param instance_filter If given, only files whose instance name appears in this list are loaded.
        @return List of QEvolutionData objects.
        """
        if not os.path.isdir(directory):
            raise FileNotFoundError(f"Directory not found: {directory}")
        
        datasets = []
        for filename in sorted(os.listdir(directory)):
            if not (filename.startswith(CSV_PREFIX) and filename.endswith(CSV_SUFFIX)):
                continue
            
            instance_name = QEvolutionLoader._extract_instance_name(filename)
            
            if instance_filter and instance_name not in instance_filter:
                continue
            
            filepath = os.path.join(directory, filename)
            datasets.append(QEvolutionData(filepath, instance_name, config_name))
        
        return datasets

    @staticmethod
    def from_directories(directories: List[str],
                         instance_filter: Optional[List[str]] = None) -> Dict[str, List[QEvolutionData]]:
        """
        Load datasets from multiple directories (one per configuration).
        Returns a dictionary mapping config names to lists of datasets.

        @param directories     List of directory paths, one per configuration.
        @param instance_filter If given, only these instance names are loaded.
        @return Ordered dict {config_name: [QEvolutionData, ...]}.
        """
        result = {}
        for directory in directories:
            config_name = os.path.basename(os.path.normpath(directory))
            datasets = QEvolutionLoader.from_directory(directory, config_name, instance_filter)
            if datasets:
                result[config_name] = datasets
        return result

    @staticmethod
    def available_instances(directory: str) -> List[str]:
        """
        Get a sorted list of all available instance names in a directory.

        @param directory Path to the directory.
        @return Sorted list of instance names found.
        """
        instance_names = []
        for filename in sorted(os.listdir(directory)):
            if filename.startswith(CSV_PREFIX) and filename.endswith(CSV_SUFFIX):
                instance_names.append(QEvolutionLoader._extract_instance_name(filename))
        return instance_names

    @staticmethod
    def _extract_instance_name(filename: str) -> str:
        """
        Extract instance name from CSV filename by removing prefix and suffix.

        @param filename CSV filename (e.g. q_evolution_wlp01.dzn.csv).
        @return Instance name (e.g. wlp01.dzn).
        """
        name = filename
        if name.startswith(CSV_PREFIX):
            name = name[len(CSV_PREFIX):]
        if name.endswith(CSV_SUFFIX):
            name = name[:-len(CSV_SUFFIX)]
        return name


class QEvolutionPlotter:
    """
    Main plotting engine for Q-value evolution visualization.
    Supports single/multi-instance plots, config comparisons, and adaptive scaling.
    """

    def __init__(self, max_iter: Optional[int] = None, legend: bool = True, bins: int = 0):
        """
        Initialize plotter with display preferences and preprocessing parameters.

        @param max_iter Truncate data to this many rows before plotting.
        @param legend   Whether to draw a legend.
        @param bins     Number of bins for aggregation (0 = adaptive or none).
        """
        self.max_iter = max_iter
        self.legend = legend
        self.bins = bins

    def plot_single(self, data: QEvolutionData) -> plt.Figure:
        """
        Create a single-panel plot showing Q-value evolution for one dataset.
        Uses adaptive y-axis scaling based on data range.

        @param data The dataset to visualise.
        @return Matplotlib Figure object.
        """
        figure, axes = plt.subplots(figsize=(FIGURE_WIDTH_SINGLE, FIGURE_HEIGHT_SINGLE))
        preprocessed_data = self._preprocess(data)
        x_values = preprocessed_data.index.to_numpy(dtype=np.float64)
        y_min, y_max = self._compute_y_limits(preprocessed_data)

        for column_name, colour, linestyle in zip(data.ls_columns, PLOT_COLOURS, PLOT_LINESTYLES):
            axes.plot(x_values, preprocessed_data[column_name], label=column_name, 
                     color=colour, linestyle=linestyle, linewidth=1.8, alpha=0.85)

        axes.set_title(f"Q-value evolution — {data.label}", fontsize=13, fontweight="bold")
        axes.set_xlabel("Global iteration", fontsize=11)
        axes.set_ylabel("Q value", fontsize=11)
        axes.set_ylim(y_min, y_max)
        axes.grid(True, linestyle="--", alpha=0.3)
        if self.legend:
            axes.legend(fontsize=10, loc="best")
        
        figure.tight_layout()
        return figure

    def plot_multi_instance(self, datasets: List[QEvolutionData]) -> plt.Figure:
        """
        Create subplot grid showing one dataset per subplot.
        Each instance has independent y-axis scaling for better visibility.

        @param datasets List of datasets (different instances, same config).
        @return Matplotlib Figure object.
        """
        datasets = [dataset for dataset in datasets if not dataset.df.empty]
        dataset_count = len(datasets)
        
        if dataset_count == 0:
            raise ValueError("No data to plot.")

        subplot_cols = min(3, dataset_count)
        subplot_rows = (dataset_count + subplot_cols - 1) // subplot_cols
        figure, axes = plt.subplots(subplot_rows, subplot_cols, 
                                    figsize=(FIGURE_WIDTH_MULTI * subplot_cols, 
                                            FIGURE_HEIGHT_MULTI * subplot_rows), 
                                    squeeze=False)
        axes = axes.flatten()

        for subplot_idx, dataset in enumerate(sorted(datasets, key=lambda d: d.instance_name)):
            axes_current = axes[subplot_idx]
            preprocessed_data = self._preprocess(dataset)
            x_values = preprocessed_data.index.to_numpy(dtype=np.float64)
            y_min, y_max = self._compute_y_limits(preprocessed_data)

            for column_name, colour, linestyle in zip(dataset.ls_columns, PLOT_COLOURS, PLOT_LINESTYLES):
                axes_current.plot(x_values, preprocessed_data[column_name], label=column_name,
                                 color=colour, linestyle=linestyle, linewidth=1.4, alpha=0.85)

            axes_current.set_title(dataset.instance_name, fontsize=11, fontweight="bold")
            axes_current.set_ylim(y_min, y_max)
            axes_current.grid(True, linestyle="--", alpha=0.25)
            if self.legend and subplot_idx == 0:
                axes_current.legend(fontsize="small", loc="best")

        for axes_idx in range(dataset_count, len(axes)):
            axes[axes_idx].set_visible(False)

        figure.suptitle(f"Q-value evolution — {datasets[0].config_name}", 
                       fontsize=14, fontweight="bold", y=0.995)
        figure.tight_layout()
        return figure

    def plot_config_comparison_single_instance(
            self, instance_name: str,
            data_by_config: Dict[str, QEvolutionData]) -> plt.Figure:
        """
        Create comparison plot for a single instance across multiple configurations.
        All subplots share the same y-axis range for fair comparison.

        @param instance_name   Name of the instance being compared.
        @param data_by_config  Mapping of config_name -> QEvolutionData for that instance.
        @return Matplotlib Figure object.
        """
        config_names = sorted(data_by_config.keys())
        config_count = len(config_names)
        
        subplot_cols = min(3, config_count)
        subplot_rows = (config_count + subplot_cols - 1) // subplot_cols
        figure, axes = plt.subplots(subplot_rows, subplot_cols,
                                    figsize=(FIGURE_WIDTH_MULTI * subplot_cols,
                                            FIGURE_HEIGHT_MULTI * subplot_rows),
                                    squeeze=False)
        axes = axes.flatten()

        # Compute global y-limits for consistency
        all_dataframes = [self._preprocess(data_by_config[cfg]) for cfg in config_names]
        global_y_min, global_y_max = self._compute_y_limits(
            pd.concat(all_dataframes, ignore_index=True)
        )

        for subplot_idx, config_name in enumerate(config_names):
            axes_current = axes[subplot_idx]
            dataset = data_by_config[config_name]
            preprocessed_data = self._preprocess(dataset)
            x_values = preprocessed_data.index.to_numpy(dtype=np.float64)

            for column_name, colour, linestyle in zip(dataset.ls_columns, PLOT_COLOURS, PLOT_LINESTYLES):
                axes_current.plot(x_values, preprocessed_data[column_name], label=column_name,
                                 color=colour, linestyle=linestyle, linewidth=1.5, alpha=0.85)

            axes_current.set_title(f"{config_name}  (n={len(dataset.df):,})", 
                                  fontsize=11, fontweight="bold")
            axes_current.set_ylim(global_y_min, global_y_max)
            axes_current.grid(True, linestyle="--", alpha=0.25)
            if self.legend and subplot_idx == 0:
                axes_current.legend(fontsize="small", loc="best")

        for axes_idx in range(config_count, len(axes)):
            axes[axes_idx].set_visible(False)

        figure.suptitle(f"Config comparison — {instance_name}", 
                       fontsize=14, fontweight="bold", y=0.995)
        figure.tight_layout()
        return figure

    def plot_config_comparison_multi_instance(
            self, data_by_config: Dict[str, List[QEvolutionData]]) -> List[plt.Figure]:
        """
        Create separate figures for each configuration.
        Each figure contains subplots for all instances in that configuration.

        @param data_by_config Mapping of config_name -> list of datasets.
        @return List of Matplotlib Figure objects (one per configuration).
        """
        figures = []
        for config_name, datasets in data_by_config.items():
            figure = self.plot_multi_instance(datasets)
            figure.suptitle(f"Q-value evolution — {config_name}", 
                           fontsize=14, fontweight="bold", y=0.995)
            figures.append(figure)
        return figures

    def _preprocess(self, data: QEvolutionData) -> pd.DataFrame:
        """
        Apply data truncation and adaptive binning.
        Bins large datasets automatically when no explicit bin count given.

        @param data The dataset to preprocess.
        @return Preprocessed DataFrame ready for plotting.
        """
        dataframe = data.get_data(self.max_iter)
        row_count = len(dataframe)
        
        # Automatic adaptive binning for very large datasets
        if row_count > 5000 and self.bins == 0:
            adaptive_bins = max(300, row_count // 20)
            dataframe = data.get_binned(self.max_iter, adaptive_bins)
        elif self.bins > 0:
            # User-specified binning takes precedence
            dataframe = data.get_binned(self.max_iter, self.bins)
        
        return dataframe

    def _compute_y_limits(self, dataframe: pd.DataFrame) -> Tuple[float, float]:
        """
        Compute adaptive y-axis limits based on data statistics.
        Adds intelligent padding to emphasize variations in the data.
        Handles both normal Q values in [0, 1] and exploded values from
        normalised reward (which can exceed 1.0 significantly).

        @param dataframe Preprocessed DataFrame with LS columns.
        @return Tuple (y_min, y_max) for setting axis limits.
        """
        ls_columns = [col for col in dataframe.columns if col.startswith("LS")]
        all_values = dataframe[ls_columns].values.flatten()
        all_values = all_values[~np.isnan(all_values)]
        
        if len(all_values) == 0:
            return 0.0, 1.0
        
        min_value = float(np.min(all_values))
        max_value = float(np.max(all_values))
        value_range = max_value - min_value
        
        # Detect if values exceed the normal Q range [0, 1]
        has_exploded = max_value > 1.0
        
        # Compute padding: larger when range is small (to emphasize changes)
        if value_range < 0.1:
            padding = 0.15
        elif value_range < 0.3:
            padding = 0.10
        else:
            padding = 0.05
        
        if has_exploded:
            # For exploded values, don't clamp to 1.0; use actual max + padding
            y_min = max(0.0, min_value - padding * value_range)
            y_max = max_value + padding * value_range
        else:
            # Normal Q values: keep within [0, 1] with padding
            y_min = max(0.0, min_value - padding * value_range)
            y_max = min(1.0, max_value + padding * value_range)
        
        # Ensure minimum spacing for visibility
        if y_max - y_min < 0.1:
            center = (y_min + y_max) / 2.0
            y_min = max(0.0, center - 0.05)
            y_max = center + 0.05  # no limit when exploded
        
        return y_min, y_max

def parse_arguments() -> argparse.Namespace:
    """
    Parse command-line arguments for data source, filtering, and display options.

    @return Parsed arguments namespace.
    """
    parser = argparse.ArgumentParser(
        description="Plot Q-value evolution from VND-RL CSV output files with adaptive scaling."
    )
    
    source_group = parser.add_argument_group("Input sources (choose one)")
    source_group.add_argument("--files", nargs="+", metavar="FILE", 
                             help="One or more CSV files to plot directly.")
    source_group.add_argument("--dir", metavar="DIR",
                             help="Directory with q_evolution_*.csv files (single configuration).")
    source_group.add_argument("--dirs", nargs="+", metavar="DIR",
                             help="Multiple directories to compare (each is a separate configuration).")

    selection_group = parser.add_argument_group("Instance selection")
    selection_group.add_argument("--instance", metavar="INST",
                                help="Comma-separated instance base names (e.g., wlp01,wlp02).")
    selection_group.add_argument("--all", action="store_true",
                                help="Select all instances found in the directory/directories.")

    display_group = parser.add_argument_group("Display options")
    display_group.add_argument("--max_iter", type=int, default=None, metavar="N",
                              help="Truncate data to the first N rows.")
    display_group.add_argument("--bins", type=int, default=0, metavar="N",
                              help="Aggregate data into N equal-width bins (recommended for large datasets).")
    display_group.add_argument("--output", metavar="FILE",
                              help="Save figure to FILE (PNG/PDF/SVG or other matplotlib format).")
    display_group.add_argument("--no-legend", action="store_true",
                              help="Suppress legend in the plot.")
    
    return parser.parse_args()


def interactive_instance_selection(available_instances: List[str]) -> List[str]:
    """
    Display available instances and allow user to select by number or 'a' for all.

    @param available_instances Sorted list of instance names to choose from.
    @return List of selected instance names.
    """
    print("Available instances:")
    for idx, instance_name in enumerate(available_instances, 1):
        print(f"  {idx:3d}. {instance_name}")
    
    user_input = input("Enter numbers (space-separated) or 'a' for all: ").strip()
    
    if user_input.lower() == "a":
        return available_instances
    
    try:
        indices = [int(x) - 1 for x in user_input.split()]
        selected = [available_instances[i] for i in indices if 0 <= i < len(available_instances)]
        return selected
    except ValueError:
        print("[ERROR] Invalid selection. Please enter numbers or 'a'.", file=sys.stderr)
        sys.exit(1)


def save_or_display_figure(figure: plt.Figure, output_path: Optional[str]) -> None:
    """
    Save figure to file or display interactively based on output_path.

    @param figure      Matplotlib Figure to save or show.
    @param output_path Path to save the figure, or None to display interactively.
    """
    if output_path:
        figure.savefig(output_path, dpi=150, bbox_inches="tight")
        print(f"Figure saved to '{output_path}'")
    else:
        plt.show()


def main() -> None:
    """
    Main entry point: parse arguments, load data, create plots, and save/display output.
    """
    args = parse_arguments()
    
    # Create plotter with parsed arguments
    plotter = QEvolutionPlotter(
        max_iter=args.max_iter,
        legend=not args.no_legend,
        bins=args.bins,
    )

    # Parse instance filter from comma-separated string
    instance_filter: Optional[List[str]] = None
    if args.instance:
        instance_filter = [name.strip() for name in args.instance.split(",")]

    # Mode: explicit files 
    if args.files:
        datasets = QEvolutionLoader.from_files(args.files)
        if len(datasets) == 1:
            figure = plotter.plot_single(datasets[0])
        else:
            figure = plotter.plot_multi_instance(datasets)
        save_or_display_figure(figure, args.output)

    # Mode: single directory
    elif args.dir:
        config_name = os.path.basename(os.path.normpath(args.dir))
        
        if not args.all and instance_filter is None:
            available = QEvolutionLoader.available_instances(args.dir)
            instance_filter = interactive_instance_selection(available)
        
        datasets = QEvolutionLoader.from_directory(args.dir, config_name, instance_filter)
        
        if len(datasets) == 1:
            figure = plotter.plot_single(datasets[0])
        else:
            figure = plotter.plot_multi_instance(datasets)
        
        save_or_display_figure(figure, args.output)

    # Mode: multiple directories
    elif args.dirs:
        if not args.all and instance_filter is None:
            available = QEvolutionLoader.available_instances(args.dirs[0])
            instance_filter = interactive_instance_selection(available)

        data_by_config = QEvolutionLoader.from_directories(args.dirs, instance_filter)

        # Collect all unique instance names
        all_instances = sorted({
            dataset.instance_name 
            for datasets in data_by_config.values() 
            for dataset in datasets
        })
        
        if instance_filter:
            all_instances = [inst for inst in all_instances if inst in instance_filter]

        # Single instance: compare all configurations
        if len(all_instances) == 1:
            instance_name = all_instances[0]
            single_data_by_config = {}
            
            for config_name, datasets in data_by_config.items():
                matching_dataset = next(
                    (d for d in datasets if d.instance_name == instance_name), 
                    None
                )
                if matching_dataset:
                    single_data_by_config[config_name] = matching_dataset
            
            figure = plotter.plot_config_comparison_single_instance(
                instance_name, single_data_by_config
            )

            if args.output:
                base, ext = os.path.splitext(args.output)
                os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)
                output_file = f"{base}_{instance_name}{ext}"
            else:
                output_file = None

            save_or_display_figure(figure, output_file)
        
        # Multiple instances: separate figures per configuration
        else:
            figures = plotter.plot_config_comparison_multi_instance(data_by_config)
            
            if args.output:
                base, ext = os.path.splitext(args.output)
                os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)
                for (config_name, datasets), figure in zip(data_by_config.items(), figures):
                    output_file = f"{base}_{config_name}{ext}"
                    figure.savefig(output_file, dpi=150, bbox_inches="tight")
                    print(f"Figure saved to '{output_file}'")
            else:
                for figure in figures:
                    figure.show()
                input("Press Enter to close all figures...")
    
    else:
        print("[ERROR] Provide --files, --dir, or --dirs.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()