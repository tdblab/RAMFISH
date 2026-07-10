import os
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import pearsonr
from itertools import combinations
from PIL import Image
from skimage.metrics import structural_similarity as ssim
from sklearn.metrics import normalized_mutual_info_score

# --- Configuration & Publication Styling ---
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.size'] = 11
plt.rcParams['axes.linewidth'] = 1.5

image_dir = "."  # Scans the current directory
background_threshold = 10 # Ignores pixels darker than this value to remove empty space
valid_extensions = ('.tif', '.tiff', '.png', '.jpg', '.jpeg', '.bmp')

# Scans directory for any file matching the valid extensions (case-insensitive)
image_files = [
    os.path.join(image_dir, f) for f in os.listdir(image_dir) 
    if f.lower().endswith(valid_extensions)
]

if len(image_files) < 2:
    print("Need at least 2 valid images in the folder to compare!")
else:
    print(f"Found {len(image_files)} images. Loading data...")
    
    images = {}
    for file_path in image_files:
        file_name = os.path.basename(file_path)
        # Use PIL to open the image and convert to grayscale ('L')
        img = Image.open(file_path).convert('L')
        # Keep as 2D array for SSIM analysis
        img_data = np.array(img).astype(np.float32)
        images[file_name] = img_data
    
    for (name1, img1), (name2, img2) in combinations(images.items(), 2):
        print(f"\nComparing {name1} and {name2}...")
        
        if img1.shape != img2.shape:
            print(f"  Skipping: Dimension mismatch ({img1.shape} vs {img2.shape})")
            continue
        
        # --- Normalization for SSIM and NMI ---
        def normalize(img):
            return (img - np.min(img)) / (np.max(img) - np.min(img) + 1e-8)
        
        img1_norm = normalize(img1)
        img2_norm = normalize(img2)

        # 1. Calculate SSIM (Requires 2D arrays)
        ssim_score, _ = ssim(img1_norm, img2_norm, data_range=1.0, full=True)
        print(f"  SSIM: {ssim_score:.4f}")

        # Flatten arrays for Pearson, NMI, and plotting
        img1_flat = img1.flatten()
        img2_flat = img2.flatten()
        img1_norm_flat = img1_norm.flatten()
        img2_norm_flat = img2_norm.flatten()

        # 2. Calculate overall Pearson r
        r, _ = pearsonr(img1_flat, img2_flat)
        print(f"  Overall Pearson r: {r:.4f}")
        
        # 3. Calculate NMI
        bins = 100
        img1_binned = np.digitize(img1_norm_flat, np.linspace(0, 1, bins))
        img2_binned = np.digitize(img2_norm_flat, np.linspace(0, 1, bins))
        nmi_score = normalized_mutual_info_score(img1_binned, img2_binned)
        print(f"  NMI: {nmi_score:.4f}")

        # --- Filter out background for the plot ---
        valid_indices = (img1_flat > background_threshold) | (img2_flat > background_threshold)
        x_plot = img1_flat[valid_indices]
        y_plot = img2_flat[valid_indices]
        
        if len(x_plot) == 0:
            print("  Skipping plot: No foreground signal found.")
            continue
        
        # --- PUBLICATION READY 2D DENSITY PLOT ---
        fig, ax = plt.subplots(figsize=(6, 6), dpi=300)
        
        # Hexagonal Binning aggregates data points to handle millions of pixels elegantly
        hb = ax.hexbin(x_plot, y_plot, gridsize=160, cmap='viridis', mincnt=1, bins='log', edgecolors='none')
        
        # Professional color bar customization
        cb = fig.colorbar(hb, ax=ax, pad=0.02)
        cb.set_label('Log10 Pixel Density', fontsize=11, fontweight='bold')
        cb.ax.tick_params(labelsize=9)
        
        # Trendline - Made much thicker and prominent (linewidth=4)
        m, b_val = np.polyfit(x_plot, y_plot, 1)
        x_line = np.linspace(0, 255, 100)
        ax.plot(x_line, m * x_line + b_val, color='#e02424', linewidth=4, linestyle='-', label='Linear Fit', zorder=5)
        
        # Adjusting short names or labels for safe rendering without string truncation
        short_name1 = name1.split('.')[0]
        short_name2 = name2.split('.')[0]
        
        # Set standardized limits and labels
        ax.set_xlim(-5, 260)
        ax.set_ylim(-5, 260)
        ax.set_xlabel(f"{short_name1} Intensity", fontsize=12, fontweight='bold', labelpad=10)
        ax.set_ylabel(f"{short_name2} Intensity", fontsize=12, fontweight='bold', labelpad=10)
        
        # Large, high-visibility stats box (fontsize=18, bold)
        stats_text = f"SSIM = {ssim_score:.3f}\nNMI  = {nmi_score:.3f}\nCorr = {r:.3f}"
        props = dict(boxstyle='square,pad=0.6', facecolor='white', alpha=0.95, edgecolor='#aaaaaa', linewidth=1.5)
        ax.text(0.06, 0.94, stats_text, transform=ax.transAxes, fontsize=18, fontweight='bold',
                verticalalignment='top', bbox=props, fontfamily='monospace', zorder=6)
        
        ax.grid(True, linestyle='--', alpha=0.3, linewidth=0.8)
        ax.legend(loc='lower right', frameon=True, fontsize=10)
        
        # Tight layouts cleanly lock formatting for print panels
        plt.tight_layout()
        save_name = f"density_{short_name1}_vs_{short_name2}.png"
        plt.savefig(save_name, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"  Saved publication-ready density graph as {save_name}")

print("\nBatch processing complete.")