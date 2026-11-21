"""
Landmark check of multi-round alignment for one image pair.
  reference.jpg : round n (reference)
  raw.jpg       : round n+1 after rough alignment
  aligned.jpg   : round n+1 after fine alignment
Landmark positions are read from an Excel file (placed by eye; not used by the aligner).
Usage:  python zf_landmarks.py [--pixel-size UM] [--xlsx zebrafish_landmarks.xlsx] [--first-letter A] [--out Fig_zebrafish_landmarks]
Output: <out>.png/.pdf/.svg and zebrafish_landmark_distances_<unit>.csv (prefixed with the specimen name)
"""
import argparse, numpy as np, pandas as pd, matplotlib.pyplot as plt
from PIL import Image

ap = argparse.ArgumentParser()
ap.add_argument('--pixel-size', type=float, default=None, help='µm per pixel')
ap.add_argument('--xlsx', default='zebrafish_landmarks.xlsx')
ap.add_argument('--first-letter', default='A', help='letter of the first panel (e.g. F for a second specimen)')
ap.add_argument('--out', default='Fig_zebrafish_landmarks')
ap.add_argument('--ref-label', default='Round n', help='name of the reference round, e.g. "Round 1"')
ap.add_argument('--moving-label', default='Round n+1', help='name of the aligned round, e.g. "Round 12"')
ap.add_argument('--scalebar-um', type=float, default=None, help='scale bar length in µm (needs --pixel-size)')
ap.add_argument('--smooth', type=float, default=0, help='Gaussian smoothing (pixels) for display only')
ap.add_argument('--images', nargs=3, default=['reference.jpg', 'raw.jpg', 'aligned.jpg'], metavar=('REF', 'ROUGH', 'FINE'), help='the three image files')
args = ap.parse_args()
px = args.pixel_size
unit = 'µm' if px else 'px'; k = px or 1.0
L = [chr(ord(args.first_letter.upper()) + i) for i in range(5)]
IMG = dict(zip(['ref', 'rough', 'fine'], args.images))
TITLE = {'ref': f'{args.ref_label}\n(reference)', 'rough': f'{args.moving_label}\nafter rough alignment', 'fine': f'{args.moving_label}\nafter fine alignment'}
MAG, YEL, GRN = '#FF00FF', '#FFFF00', '#00FF00'

df = pd.read_excel(args.xlsx)
pos = {st: df[df.stage == st].set_index('landmark')[['X', 'Y']] for st in IMG}
lms = list(pos['ref'].index)
D = pd.DataFrame({'landmark': lms, 'name': df[df.stage == 'ref'].set_index('landmark').loc[lms, 'landmark_name'].values})
for st, lab in [('rough', 'after_rough'), ('fine', 'after_fine')]:
    d = pos[st].loc[lms] - pos['ref'].loc[lms]
    D[f'{lab}_{unit}'] = (np.hypot(d.X, d.Y) * k).round(2).values
sp_name = str(df.specimen.iloc[0]) if 'specimen' in df else 'specimen'
D.to_csv(f'{sp_name}_landmark_distances_{unit}.csv', index=False)
print(D.to_string(index=False))

def load(f):
    a = np.array(Image.open(f)).astype(float)
    a = a.mean(axis=2) if a.ndim == 3 else a          # 8- or 16-bit grayscale; display is stretched per image
    if args.smooth:
        from scipy.ndimage import gaussian_filter
        a = gaussian_filter(a, args.smooth)
    return a
# put a label on the left of its circle when another landmark sits just to its right
ims = {st: load(f) for st, f in IMG.items()}
SC = ims['ref'].shape[1] / 1932          # sizes below were set for 1932-px-wide images
LEFT_LABEL = set()
for a_ in lms:
    for b_ in lms:
        dx = pos['ref'].loc[b_, 'X'] - pos['ref'].loc[a_, 'X']; dy = abs(pos['ref'].loc[b_, 'Y'] - pos['ref'].loc[a_, 'Y'])
        if a_ != b_ and 0 < dx < 250 * SC and dy < 150 * SC:
            LEFT_LABEL.add(a_)
# Arial; Liberation Sans (same letter widths as Arial) is used only if Arial is not installed
plt.rcParams.update({'font.size': 24, 'font.family': 'sans-serif', 'font.sans-serif': ['Arial', 'Liberation Sans'],
                     'svg.fonttype': 'none', 'pdf.fonttype': 42})
fig = plt.figure(figsize=(22, 16))
gs = fig.add_gridspec(1, 3, left=0.02, right=0.75, top=0.92, bottom=0.42, wspace=0.05)

# A-C: the three images, landmarks marked. On B and C a green circle marks the reference position
for i, st in enumerate(IMG):
    ax = fig.add_subplot(gs[0, i]); im = ims[st]
    ax.imshow(im, cmap='gray', vmax=np.percentile(im, 99.7)); ax.axis('off')
    ax.set_title(f'{L[i]}   {TITLE[st]}', fontsize=26, fontweight='bold', loc='left')
    for lm in lms:
        x, y = pos[st].loc[lm]
        if st != 'ref':
            rx, ry = pos['ref'].loc[lm]
            ax.plot([rx, x], [ry, y], color=GRN, lw=3)
            ax.plot(rx, ry, 'o', ms=22, mfc='none', mec=GRN, mew=5)
        ax.plot(x, y, 'o', ms=22, mfc='none', mec=GRN if st == 'ref' else MAG, mew=5)
        left = lm in LEFT_LABEL
        ax.text(x - 55 * SC if left else x + 55 * SC, y - 35 * SC, lm, color=YEL, fontsize=26, fontweight='bold', ha='right' if left else 'left')
    if px and args.scalebar_um:                       # scale bar, bottom right, in image pixels
        h, w = im.shape; blen = args.scalebar_um / px; m = 0.05 * w
        ax.add_patch(plt.Rectangle((w - m - blen, h - m - 0.012 * h), blen, 0.012 * h, color='white', lw=0))
        ax.text(w - m - blen / 2, h - m - 0.025 * h, f'{args.scalebar_um:g} µm', color='white', fontsize=22, ha='center', va='bottom')

# D: distances
ax = fig.add_axes([0.82, 0.47, 0.165, 0.43])
a, b = D.iloc[:, 2].values, D.iloc[:, 3].values
for i in range(len(D)):
    ax.plot([0, 1], [a[i], b[i]], color='#c9cfd6', lw=1.5, zorder=1)
ax.scatter(np.zeros(len(a)), a, s=260, color='#a7b0bb', edgecolor='white', zorder=3)
ax.scatter(np.ones(len(b)), b, s=260, color='#1f6f78', edgecolor='white', zorder=3)
for x, v in [(0, a), (1, b)]:
    ax.plot([x - 0.15, x + 0.15], [np.median(v)] * 2, color='#111', lw=4, zorder=4)
ax.set_xticks([0, 1]); ax.set_xticklabels(['After rough\nalignment', 'After fine\nalignment'])
ax.set_xlim(-0.4, 1.4); ax.set_ylim(0, max(a) * 1.15)
ax.set_ylabel(f'Distance from reference position ({unit})')
ax.spines[['top', 'right']].set_visible(False); ax.grid(axis='y', color='#e6e6e6'); ax.set_axisbelow(True)
ax.set_title(f'{L[3]}   Landmark distance', fontsize=26, fontweight='bold', loc='left', pad=20)

# E: close-ups of every landmark in the three images (same scale)
sub = fig.add_gridspec(3, len(lms), left=0.05, right=0.99, top=0.355, bottom=0.02, hspace=0.05, wspace=0.04)
RX, RY = int(110 * SC), int(60 * SC)          # half width and half height of each close-up (pixels)
for j, lm in enumerate(lms):
    for i, st in enumerate(IMG):
        ax = fig.add_subplot(sub[i, j]); x, y = pos[st].loc[lm]; im = ims[st]
        xi, yi = int(round(x)), int(round(y))
        x0, y0 = max(xi - RX, 0), max(yi - RY, 0)
        c = im[y0:yi + RY, x0:xi + RX]
        ax.imshow(c, cmap='gray', vmax=np.percentile(im, 99.7), aspect='auto'); ax.set_xticks([]); ax.set_yticks([])
        ax.plot(x - x0, y - y0, 'o', ms=22, mfc='none', mec=GRN if st == 'ref' else MAG, mew=4.5)
        if i == 0:
            ax.set_title(lm, fontsize=26, fontweight='bold')
        if j == 0:
            ax.set_ylabel({'ref': args.ref_label, 'rough': 'Rough', 'fine': 'Fine'}[st], fontsize=24)
fig.text(0.012, 0.375, L[4], fontsize=26, fontweight='bold')
for ext in ('png', 'pdf', 'svg'):
    fig.savefig(f'{args.out}.{ext}', dpi=300, facecolor='white')
print(f'Saved {args.out}.png/.pdf/.svg')
