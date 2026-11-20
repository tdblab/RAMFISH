"""
Simple figure showing how well round n+1 lines up with round n, before and after fine alignment.

Put three images of the same z-slice in one folder (any of .tif .tiff .png .jpg):
    reference : round n (the image everything is aligned to)
    raw       : round n+1 after rough alignment only
    aligned   : round n+1 after fine alignment
and run:   python alignment_figure.py
(or give names:  python alignment_figure.py reference.tif raw.tif aligned.tif)

Output: alignment_figure.png / .pdf / .tif and alignment_scores.csv
The scores (Pearson r, SSIM, NMI) are computed exactly as in alignment_score.py.
"""
import sys, os, glob, csv
import numpy as np
from PIL import Image
from scipy.ndimage import gaussian_filter
from scipy.stats import pearsonr
from skimage.metrics import structural_similarity as ssim
from sklearn.metrics import normalized_mutual_info_score
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Patch

# ---- settings -----------------------------------------------------------------
ZOOM_BOX = (0.06, 0.03, 0.30, 0.20)   # zoomed area: x, y, width, height as fractions of the image
FONT = 18                             # base font size
REF_COLOR, MOV_COLOR = '#FF00FF', '#00FF00'
BEFORE_COL, AFTER_COL = '#9AA0A6', '#2E7D32'


def find(stem):
    for ext in ('.tif', '.tiff', '.png', '.jpg', '.jpeg'):
        hits = [f for f in glob.glob('*') if f.lower() == stem + ext]
        if hits:
            return hits[0]
    sys.exit(f'Could not find an image called {stem}.tif/.png/.jpg in this folder')


files = sys.argv[1:4] if len(sys.argv) >= 4 else [find('reference'), find('raw'), find('aligned')]
load = lambda f: np.array(Image.open(f).convert('L')).astype(np.float32)
ref, raw, aligned = (load(f) for f in files)
if not (ref.shape == raw.shape == aligned.shape):
    sys.exit('The three images must have the same size')


# ---- scores (same as alignment_score.py) -------------------------------------
def norm01(a):
    return (a - a.min()) / (a.max() - a.min() + 1e-8)


def scores(a, b):
    an, bn = norm01(a), norm01(b)
    s = ssim(an, bn, data_range=1.0)
    r = pearsonr(a.ravel(), b.ravel())[0]
    edges = np.linspace(0, 1, 100)
    m = normalized_mutual_info_score(np.digitize(an.ravel(), edges), np.digitize(bn.ravel(), edges))
    return {'Pearson r': float(r), 'SSIM': float(s), 'NMI': float(m)}


before, after = scores(ref, raw), scores(ref, aligned)
with open('alignment_scores.csv', 'w', newline='') as fh:
    w = csv.writer(fh)
    w.writerow(['metric', 'before_fine_alignment (reference vs raw)', 'after_fine_alignment (reference vs aligned)'])
    for k in before:
        w.writerow([k, round(before[k], 3), round(after[k], 3)])
        print(f'{k:10s} before {before[k]:.3f}   after {after[k]:.3f}')


# ---- color overlay: reference in magenta, round n+1 in green, overlap looks white
def stretch(a):
    a = gaussian_filter(a, 1.5)            # light smoothing for display only
    lo, hi = np.percentile(a, [1, 99.5])
    return np.clip((a - lo) / (hi - lo + 1e-8), 0, 1)


def overlay(a, b):
    A, B = stretch(a), stretch(b)
    return np.dstack([A, B, A])


ov_before, ov_after = overlay(ref, raw), overlay(ref, aligned)
H, W = ref.shape
zx, zy, zw, zh = int(ZOOM_BOX[0] * W), int(ZOOM_BOX[1] * H), int(ZOOM_BOX[2] * W), int(ZOOM_BOX[3] * H)

# ---- figure ------------------------------------------------------------------
plt.rcParams.update({'font.size': FONT, 'font.family': 'sans-serif', 'axes.linewidth': 1.2})
fig = plt.figure(figsize=(16, 11.5))
gs = fig.add_gridspec(2, 3, width_ratios=[1, 1, 0.9], height_ratios=[1.35, 1], wspace=0.08, hspace=0.12)

for col, (img, title) in enumerate([(ov_before, 'Before fine alignment'), (ov_after, 'After fine alignment')]):
    ax = fig.add_subplot(gs[0, col])
    ax.imshow(img)
    ax.add_patch(Rectangle((zx, zy), zw, zh, fill=False, ec='white', lw=2, ls='--'))
    ax.set_title(title, fontsize=FONT + 4, fontweight='bold', pad=10)
    ax.axis('off')
    axz = fig.add_subplot(gs[1, col])
    axz.imshow(img[zy:zy + zh, zx:zx + zw])
    axz.set_title('Zoom of dashed box', fontsize=FONT)
    for s in axz.spines.values():
        s.set_edgecolor('black'); s.set_linewidth(1.5)
    axz.set_xticks([]); axz.set_yticks([])

fig.legend(handles=[Patch(color=REF_COLOR, label='Round n (reference)'),
                    Patch(color=MOV_COLOR, label='Round n+1'),
                    Patch(facecolor='white', edgecolor='black', label='Both overlap')],
           loc='lower center', ncol=3, frameon=False, fontsize=FONT, bbox_to_anchor=(0.36, 0.0))

# bar charts: one small chart per score
sub = gs[:, 2].subgridspec(3, 1, hspace=0.55)
for i, k in enumerate(before):
    ax = fig.add_subplot(sub[i])
    vals = [before[k], after[k]]
    bars = ax.bar(['Before', 'After'], vals, color=[BEFORE_COL, AFTER_COL], width=0.6)
    for b, v in zip(bars, vals):
        ax.text(b.get_x() + b.get_width() / 2, v, f'{v:.3f}', ha='center', va='bottom', fontsize=FONT - 2)
    ax.set_ylim(0, max(vals) * 1.3)
    ax.set_title(k, fontsize=FONT + 1, fontweight='bold')
    ax.spines[['top', 'right']].set_visible(False)
    ax.tick_params(axis='y', labelsize=FONT - 4)
    ax.set_yticks([])
fig.text(0.84, 0.995, 'Match with round n\n(higher = better)', ha='center', va='top', fontsize=FONT - 1, fontstyle='italic')

fig.subplots_adjust(left=0.02, right=0.98, top=0.90, bottom=0.10)
for ext in ('png', 'pdf', 'tif'):
    fig.savefig(f'alignment_figure.{ext}', dpi=300, facecolor='white')
print('Saved alignment_figure.png / .pdf / .tif and alignment_scores.csv')
