"""
ramfish_landmark_error.py

Measures alignment accuracy with landmarks that were NOT used to calculate the alignment:
the distance of each landmark from its Round 1 position before alignment, after rough
alignment and after fine alignment, with paired statistics and figure-ready plots.

Input: the landmark file saved by ramfish_landmark_check.html ("Save landmarks (Excel)"),
or any .xlsx / .csv with one row per landmark per image and the columns
  specimen         e.g. FW1, HW2, ZF1 (optional; one specimen if missing)
  round            round number of the image (1 for the reference)
  stage            ref (Round 1) | unaligned | rough | fine
  landmark         landmark name or number (same name on every image)
  X, Y             point coordinates in pixels
  pixel_size_um    optional, µm per pixel for that specimen (or use --pixel-size)
Rows of the Round 1 reference may be repeated; the first one is used.

Usage:
  python ramfish_landmark_error.py landmarks.xlsx --pixel-size 0.21 --out results
  python ramfish_landmark_error.py landmarks.xlsx --group-by specimen        (several specimens)

Output (in --out):
  landmark_distances.csv      distance of every landmark from its Round 1 position
  landmark_summary.csv        n, mean, SD, median, min, max per group and step
  landmark_statistics.csv     paired two-sided Wilcoxon signed-rank tests between steps
  landmark_results.xlsx       all of the above in one workbook
  Fig_landmark_distance_by_group.png/.svg/.pdf   steps per landmark (or specimen) + all, with p-values
  Fig_landmark_distance_all.png/.svg/.pdf        all landmarks pooled
  Fig_landmark_distance_by_round_<specimen>.png/.svg/.pdf

Statistics: one value per specimen and round. For "All" and for --group-by specimen,
the value is the mean of that round's landmarks, because landmarks of the same round
are not independent. Exact test without ties; normal approximation otherwise.
"""
import argparse
import os

import numpy as np
import pandas as pd
from scipy.stats import wilcoxon
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.ticker import FuncFormatter

STAGES = ["unaligned", "rough", "fine"]
SNAME = {"unaligned": "Before alignment", "rough": "After rough alignment", "fine": "After fine alignment"}
STAGE_IN = {"ref": "ref", "reference": "ref", "round1": "ref", "r1": "ref", "raw": "unaligned", "unaligned": "unaligned",
            "before": "unaligned", "before alignment": "unaligned", "normalized": "unaligned", "rough": "rough",
            "after rough alignment": "rough", "fine": "fine", "after fine alignment": "fine"}
COL = {"unaligned": "#d9a066", "rough": "#a7b0bb", "fine": "#1f6f78"}
DX = {"unaligned": -0.27, "rough": 0.0, "fine": 0.27}


def read_points(path):
    df = pd.read_excel(path, sheet_name=0) if path.lower().endswith((".xlsx", ".xls")) else pd.read_csv(path)
    df.columns = [str(c).strip().lower() for c in df.columns]
    if "x" in df.columns:
        df = df.rename(columns={"x": "X", "y": "Y"})
    need = {"round", "stage", "landmark", "X", "Y"}
    missing = need - set(df.columns)
    if missing:
        raise SystemExit(f"Missing columns: {sorted(missing)}")
    if "specimen" not in df.columns:
        df["specimen"] = "Specimen 1"
    df["specimen"] = df["specimen"].astype(str)
    df["landmark"] = df["landmark"].astype(str).str.strip()
    df["stage"] = df["stage"].astype(str).str.strip().str.lower().map(STAGE_IN)
    if df["stage"].isna().any():
        raise SystemExit("Unknown stage values; use ref, unaligned, rough or fine.")
    return df.dropna(subset=["X", "Y"])


def distances(df, pixel_size):
    rows = []
    for sp, g in df.groupby("specimen", sort=False):
        ref = g[g.stage == "ref"].drop_duplicates("landmark").set_index("landmark")
        if ref.empty:
            raise SystemExit(f"Specimen {sp}: no Round 1 (ref) points.")
        px = pixel_size or (g["pixel_size_um"].dropna().iloc[0] if "pixel_size_um" in g and g["pixel_size_um"].notna().any() else None)
        for _, r in g[g.stage != "ref"].iterrows():
            if r.landmark not in ref.index:
                continue
            q = ref.loc[r.landmark]
            d = float(np.hypot(r.X - q.X, r.Y - q.Y))
            rows.append(dict(specimen=sp, landmark=r.landmark, round=int(r["round"]), stage=r.stage, X_ref=q.X, Y_ref=q.Y,
                             X=r.X, Y=r.Y, distance_px=d, distance_um=d * px if px else np.nan))
    D = pd.DataFrame(rows)
    if D.empty:
        raise SystemExit("No landmark has both a Round 1 point and a later point.")
    return D


def unit_values(D, key, val, group_by, group, stage):
    """Series indexed by (specimen, round)."""
    d = D[D.stage == stage]
    if group != "ALL":
        d = d[d[group_by] == group]
    return d.groupby(["specimen", "round"])[val].mean()


def wilcox(x, y):
    diff = x - y
    if np.allclose(diff, 0):
        return np.nan, 1.0, "no differences"
    ties = len(np.unique(np.round(np.abs(diff[diff != 0]), 9))) < (diff != 0).sum()
    method = "exact" if not ties and len(x) <= 50 else "approx"
    res = wilcoxon(x, y, alternative="two-sided", method=method)
    return float(res.statistic), float(res.pvalue), "exact" if method == "exact" else "normal approximation"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("points", help="landmark file (.xlsx or .csv)")
    ap.add_argument("--pixel-size", type=float, help="µm per pixel (overrides the pixel_size_um column)")
    ap.add_argument("--group-by", choices=["landmark", "specimen"], default="landmark")
    ap.add_argument("--out", default="results")
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)

    D = distances(read_points(a.points), a.pixel_size)
    use_um = D.distance_um.notna().all()
    val, unit = ("distance_um", "µm") if use_um else ("distance_px", "px")
    stages = [s for s in STAGES if (D.stage == s).any()]
    groups = list(dict.fromkeys(D[a.group_by])) + (["ALL"] if D[a.group_by].nunique() > 1 or a.group_by == "landmark" else [])
    gname = lambda g: "All landmarks" if g == "ALL" and a.group_by == "landmark" else ("All specimens" if g == "ALL" else g)

    summ, stats = [], []
    for g in groups:
        for st in stages:
            v = unit_values(D, a.group_by, val, a.group_by, g, st) if g == "ALL" or a.group_by == "specimen" else \
                D[(D[a.group_by] == g) & (D.stage == st)].set_index(["specimen", "round"])[val]
            summ.append(dict(group=gname(g), stage=SNAME[st], n=len(v), mean=v.mean(), sd=v.std(), median=v.median(),
                             min=v.min(), max=v.max(), unit=unit))
        for i in range(len(stages)):
            for j in range(i + 1, len(stages)):
                s1, s2 = stages[i], stages[j]
                if g == "ALL" or a.group_by == "specimen":
                    x, y = unit_values(D, a.group_by, val, a.group_by, g, s1), unit_values(D, a.group_by, val, a.group_by, g, s2)
                else:
                    x = D[(D[a.group_by] == g) & (D.stage == s1)].set_index(["specimen", "round"])[val]
                    y = D[(D[a.group_by] == g) & (D.stage == s2)].set_index(["specimen", "round"])[val]
                idx = x.index.intersection(y.index)
                if len(idx) < 2:
                    continue
                W, p, m = wilcox(x.loc[idx].values, y.loc[idx].values)
                stats.append(dict(group=gname(g), comparison=f"{SNAME[s1]} vs {SNAME[s2]}", n=len(idx),
                                  median_a=float(np.median(x.loc[idx])), median_b=float(np.median(y.loc[idx])), W=W, p_value=p, method=m, unit=unit))
    S = pd.DataFrame(summ).round(3)
    T = pd.DataFrame(stats).round(4)
    out = D.assign(stage=D.stage.map(SNAME)).round(3)
    out.to_csv(os.path.join(a.out, "landmark_distances.csv"), index=False)
    S.to_csv(os.path.join(a.out, "landmark_summary.csv"), index=False)
    T.to_csv(os.path.join(a.out, "landmark_statistics.csv"), index=False)
    with pd.ExcelWriter(os.path.join(a.out, "landmark_results.xlsx")) as xw:
        S.to_excel(xw, sheet_name="summary", index=False)
        T.to_excel(xw, sheet_name="statistics", index=False)
        out.to_excel(xw, sheet_name="distances", index=False)
    print(S.to_string(index=False)); print(); print(T.to_string(index=False))

    # ---------------------------------------------------------------- plots (large fonts)
    plt.rcParams.update({"font.size": 20, "axes.labelsize": 22, "font.family": "DejaVu Sans", "axes.spines.top": False,
                         "axes.spines.right": False, "svg.fonttype": "none", "pdf.fonttype": 42})
    fmt = FuncFormatter(lambda v, _: f"{v:g}")
    ptxt = lambda p: "" if p is None or np.isnan(p) else (f"p = {p:.2g}" if p >= 0.001 else "p < 0.001")
    log = (D[val] > 0).all()
    ylab = f"Distance from Round 1 position ({unit})"

    def series(g, st):
        if g == "ALL" or a.group_by == "specimen":
            return unit_values(D, a.group_by, val, a.group_by, g, st)
        return D[(D[a.group_by] == g) & (D.stage == st)].set_index(["specimen", "round"])[val]

    def pget(g, s1, s2):
        r = T[(T.group == gname(g)) & (T.comparison == f"{SNAME[s1]} vs {SNAME[s2]}")]
        return r.p_value.iloc[0] if len(r) else None

    def strip(ax, keys):
        for i, g in enumerate(keys):
            V = {st: series(g, st) for st in stages}
            for u in V[stages[-1]].index:
                pts = [(i + DX[st], V[st].loc[u]) for st in stages if u in V[st].index]
                ax.plot(*zip(*pts), color="#c9cfd6", lw=1.2, zorder=1)
            for st in stages:
                v = V[st].values
                ax.scatter(np.full(len(v), i + DX[st]), v, s=110, color=COL[st], edgecolor="white", lw=1.2, zorder=3)
                ax.plot([i + DX[st] - 0.1, i + DX[st] + 0.1], [np.median(v)] * 2, color="#111", lw=3.5, zorder=4)
            for s1, s2 in zip(stages, stages[1:]):
                p = pget(g, s1, s2)
                if p is None:
                    continue
                top = max(V[s1].max(), V[s2].max())
                y = top * 1.6 if log else top * 1.12
                ax.plot([i + DX[s1], i + DX[s1], i + DX[s2], i + DX[s2]], [y / 1.08 if log else y * 0.97, y, y, y / 1.08 if log else y * 0.97], color="#222", lw=1.6)
                ax.text(i + (DX[s1] + DX[s2]) / 2, y * (1.07 if log else 1.02), ptxt(p), ha="center", va="bottom", fontsize=15)
        if log:
            ax.set_yscale("log"); ax.yaxis.set_major_formatter(fmt)
            ax.set_ylim(D[val].min() / 2, D[val].max() * 2.5)
        else:
            ax.set_ylim(0, D[val].max() * 1.35)
        ax.set_xticks(range(len(keys))); ax.set_xticklabels([gname(k).replace(" ", "\n", 1) if k == "ALL" else k for k in keys])
        ax.set_xlim(-0.55, len(keys) - 0.45); ax.set_ylabel(ylab)
        ax.grid(axis="y", color="#e6e6e6"); ax.set_axisbelow(True)

    legend = [Line2D([], [], marker="o", ls="", ms=12, color=COL[st], label=SNAME[st]) for st in stages] + \
             [Line2D([], [], color="#111", lw=3.5, label="Median")]

    def save(fig, name):
        fig.tight_layout()
        for ext in ("png", "svg", "pdf"):
            fig.savefig(os.path.join(a.out, f"{name}.{ext}"), dpi=300)
        plt.close(fig)

    fig, ax = plt.subplots(figsize=(max(8, 2.6 * len(groups) + 2), 10))
    strip(ax, groups)
    if groups[-1] == "ALL" and len(groups) > 1:
        ax.axvline(len(groups) - 1.5, color="#bbb", lw=1, ls="--")
    ax.legend(handles=legend, frameon=False, fontsize=17, ncol=2, loc="upper center", bbox_to_anchor=(0.5, -0.17))
    save(fig, "Fig_landmark_distance_by_group")

    if "ALL" in groups:
        fig, ax = plt.subplots(figsize=(6.5, 9))
        strip(ax, ["ALL"])
        ax.set_xticks([DX[st] for st in stages]); ax.set_xticklabels([{"unaligned": "Before", "rough": "Rough", "fine": "Fine"}[s] for s in stages])
        ax.set_xlabel("Alignment step")
        save(fig, "Fig_landmark_distance_all")

    LS = {"unaligned": ":", "rough": "--", "fine": "-"}; AL = {"unaligned": 0.45, "rough": 0.65, "fine": 1}
    pal = ["#c45a2c", "#1f6f78", "#5a7fb8", "#8a6bb8", "#3c8f5a", "#b8862f", "#c2557d", "#4a4a4a"]
    for sp, g in D.groupby("specimen", sort=False):
        rounds = sorted(g["round"].unique()); xs = {r: i for i, r in enumerate(rounds)}
        fig, ax = plt.subplots(figsize=(max(9, 1.1 * len(rounds) + 4), 8.5))
        lms = list(dict.fromkeys(g.landmark))
        for li, lm in enumerate(lms):
            c = pal[li % len(pal)]
            for st in stages:
                v = g[(g.landmark == lm) & (g.stage == st)].set_index("round")[val].reindex(rounds)
                ax.plot(range(len(rounds)), v.values, ls=LS[st], marker="o", ms=10, lw=2.5, color=c, alpha=AL[st],
                        markerfacecolor=c if st == "fine" else "white", markeredgewidth=2)
        ax.set_xticks(range(len(rounds))); ax.set_xticklabels([str(r) for r in rounds]); ax.set_xlabel("Round"); ax.set_ylabel(ylab)
        if log:
            ax.set_yscale("log"); ax.yaxis.set_major_formatter(fmt)
        ax.grid(axis="y", color="#e6e6e6"); ax.set_axisbelow(True)
        h = [Line2D([], [], color=pal[i % len(pal)], marker="o", ms=10, lw=2.5, label=lm) for i, lm in enumerate(lms)]
        h += [Line2D([], [], color="#444", ls=LS[st], lw=2.5, label=SNAME[st]) for st in stages]
        ax.legend(handles=h, frameon=False, fontsize=16, ncol=2, loc="upper center", bbox_to_anchor=(0.5, -0.15))
        save(fig, f"Fig_landmark_distance_by_round_{sp}")
    print("Saved results in", a.out)


if __name__ == "__main__":
    main()
