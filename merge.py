import os
import cv2
import numpy as np
from glob import glob
from PIL import Image, ImageDraw, ImageFont

# Settings
IMAGE_DIR = "multiplex_fish_images"
EXTENSIONS = ['*.jpg', '*.jpeg', '*.png', '*.bmp', '*.tiff']
GENE_COLORS = [
    (0, 0, 255),     # Red
    (0, 255, 0),     # Green
    (255, 0, 0),     # Blue
    (255, 255, 0),   # Cyan
    (255, 0, 255),   # Magenta
    (0, 255, 255),   # Yellow
    (255, 165, 0),   # Orange
    (128, 0, 128),   # Purple
    (0, 128, 128),   # Teal
    (128, 128, 0),   # Olive
]

# Helper Functions
def load_images_from_folder(folder):
    image_paths = []
    for ext in EXTENSIONS:
        image_paths.extend(glob(os.path.join(folder, ext)))
    return sorted(image_paths)

def assign_color_and_merge(base, overlay, color):
    gray = cv2.cvtColor(overlay, cv2.COLOR_BGR2GRAY)
    gray_norm = gray / 255.0
    color_overlay = np.zeros_like(base, dtype=np.float32)
    for i in range(3):
        color_overlay[:, :, i] = gray_norm * color[i]
    merged = cv2.add(base.astype(np.float32), color_overlay)
    return np.clip(merged, 0, 255).astype(np.uint8)

def desaturate(image, factor=0.25):
    """Blend color image with grayscale version."""
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray_3ch = cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)
    return cv2.addWeighted(image, 1 - factor, gray_3ch, factor, 0)

def add_color_legend(image, gene_names, gene_colors, side="right"):
    img_pil = Image.fromarray(image)
    draw = ImageDraw.Draw(img_pil)

    # Scale legend sizes based on image size
    scale = img_pil.width / 1000  # proportional scaling factor
    circle_radius = int(15 * scale)
    legend_spacing = int(60 * scale)
    font_size = int(40 * scale)
    margin_top = int(30 * scale)

    try:
        font = ImageFont.truetype("ariali.ttf", font_size)  # Scaled font
    except:
        font = ImageFont.load_default()
        print("Italic font not found. Using default.")

    # Legend position for right side
    x_text = img_pil.width - int(200 * scale)  # dynamic shift
    x_circle = x_text - (circle_radius * 2 + 10)

    for i, (gene, color) in enumerate(zip(gene_names, gene_colors)):
        y = margin_top + i * legend_spacing
        draw.ellipse(
            (x_circle, y, x_circle + 2 * circle_radius, y + 2 * circle_radius),
            fill=color
        )
        draw.text((x_text, y), gene, fill=(255, 255, 255), font=font)

    return np.array(img_pil)

# Main Pipeline
image_paths = load_images_from_folder(IMAGE_DIR)
gene_names = [os.path.splitext(os.path.basename(p))[0] for p in image_paths]
print("Detected genes:", gene_names)

# Load base image and initialize composite
base_img = cv2.imread(image_paths[0])
composite = np.zeros_like(base_img, dtype=np.uint8)

# Merge each image with assigned color
for i, path in enumerate(image_paths):
    img = cv2.imread(path)
    color = GENE_COLORS[i % len(GENE_COLORS)]
    composite = assign_color_and_merge(composite, img, color)

# Desaturate for softer blend
composite = desaturate(composite, factor=0.00)

# Add gene-color legend only on the right
composite = add_color_legend(composite, gene_names, GENE_COLORS, side="right")

# Save and display
output_path = "merged_composite.jpg"
cv2.imwrite(output_path, composite)
cv2.imshow("Merged Composite", composite)
cv2.waitKey(0)
cv2.destroyAllWindows()

