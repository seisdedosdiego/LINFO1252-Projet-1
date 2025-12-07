#!/usr/bin/env python3

import csv
import math
from pathlib import Path

import matplotlib.pyplot as plt

def load_csv_group_by_threads(filename, threads_col_name, time_col_index=2):
    data = {}
    with open(filename, newline="") as f:
        reader = csv.reader(f, delimiter=';')
        header = next(reader)

        try:
            threads_idx = header.index(threads_col_name)
        except ValueError:
            raise RuntimeError(f"Colonne '{threads_col_name}' non trouvée dans {filename}: {header}")

        for row in reader:
            if not row or len(row) <= time_col_index:
                continue
            try:
                threads = int(row[threads_idx])
            except ValueError:
                continue
            try:
                real_time = float(row[time_col_index])
            except ValueError:
                continue

            data.setdefault(threads, []).append(real_time)

    return data

def compute_stats_per_threads(data_dict):
    xs = sorted(data_dict.keys())
    means = []
    stds = []

    for t in xs:
        values = data_dict[t]
        if not values:
            means.append(0.0)
            stds.append(0.0)
            continue
        mean = sum(values) / len(values)
        if len(values) > 1:
            var = sum((v - mean) ** 2 for v in values) / (len(values) - 1)
            std = math.sqrt(var)
        else:
            std = 0.0
        means.append(mean)
        stds.append(std)
    return xs, means, stds

def plot_with_errorbars(xs, means, stds, title, xlabel, ylabel, output_file):
    plt.figure()
    plt.errorbar(xs, means, yerr=stds, marker='o', capsize=5, linestyle='-')
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.ylim(bottom=0)
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.xticks(xs)
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

def main():
    base_dir = Path(__file__).resolve().parent.parent
    graphs_dir = base_dir / "graphs"
    measures_dir = base_dir / "measures"

    graphs_dir.mkdir(exist_ok=True)

    philo_csv = measures_dir / "philosophes.csv"
    prod_csv = measures_dir / "prodcons.csv"
    rw_csv = measures_dir / "lecteurs_ecrivains.csv"
    tas_csv = measures_dir / "test_tas.csv"
    tatas_csv = measures_dir / "test_tatas.csv"

    # ---------- Philosophes ----------
    philo_data = load_csv_group_by_threads(philo_csv, threads_col_name="threads", time_col_index=2)
    xs, means, stds = compute_stats_per_threads(philo_data)
    plot_with_errorbars(
        xs, means, stds,
        title="",
        xlabel="Nombre de philosophes (threads)",
        ylabel="Temps d'execution [s]",
        output_file=graphs_dir / "philosophes.pdf",
    )

    # ---------- Producteurs / Consommateurs ----------
    prod_data = load_csv_group_by_threads(prod_csv, threads_col_name="threads_prod", time_col_index=3)
    xs, means, stds = compute_stats_per_threads(prod_data)
    plot_with_errorbars(
        xs, means, stds,
        title="",
        xlabel="Nombre de producteurs (== consommateurs)",
        ylabel="Temps d'execution [s]",
        output_file=graphs_dir / "prodcons.pdf",
    )

    # ---------- Lecteurs / Écrivains ----------
    rw_data = load_csv_group_by_threads(rw_csv, threads_col_name="ecrivains", time_col_index=3)
    xs, means, stds = compute_stats_per_threads(rw_data)
    plot_with_errorbars(
        xs, means, stds,
        title="",
        xlabel="Nombre d'écrivains (== lecteurs)",
        ylabel="Temps d'execution [s]",
        output_file=graphs_dir / "lecteurs_ecrivains.pdf",
    )

    # ---------- Test TAS (spinlock seul) ----------
    tas_data = load_csv_group_by_threads(tas_csv, threads_col_name="threads", time_col_index=2)
    xs, means, stds = compute_stats_per_threads(tas_data)
    plot_with_errorbars(
        xs, means, stds,
        title="",
        xlabel="Nombre de threads",
        ylabel="Temps d'execution [s]",
        output_file=graphs_dir / "test_tas.pdf",
    )

    # ---------- TAS vs TATAS (même graphe) ----------
    tas_data = load_csv_group_by_threads(tas_csv, threads_col_name="threads", time_col_index=2)
    tatas_data = load_csv_group_by_threads(tatas_csv, threads_col_name="threads", time_col_index=2)

    xs_tas, means_tas, stds_tas = compute_stats_per_threads(tas_data)
    xs_tatas, means_tatas, stds_tatas = compute_stats_per_threads(tatas_data)

    plt.figure()
    plt.errorbar(
        xs_tas, means_tas, yerr=stds_tas,
        marker="o", capsize=5, linestyle="-", label="TAS"
    )
    plt.errorbar(
        xs_tatas, means_tatas, yerr=stds_tatas,
        marker="s", capsize=5, linestyle="--", label="TTAS"
    )

    plt.title("Verrou TAS vs TATAS")
    plt.xlabel("Nombre de threads")
    plt.ylabel("Temps d'exécution [s]")
    plt.ylim(bottom=0)
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.xticks(xs_tas)
    plt.legend()
    plt.tight_layout()
    plt.savefig(graphs_dir / "tas_vs_tatas.pdf")
    plt.close()


if __name__ == "__main__":
    main()

