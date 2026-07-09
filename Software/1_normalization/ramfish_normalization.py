import os
import json
import numpy as np
import cupy as cp
import imageio.v3 as iio
from cupyx.scipy.ndimage import gaussian_filter
from pathlib import Path
import warnings

warnings.filterwarnings("ignore")

CONFIG_FILE = Path("config.json")

def load_or_create_config():
    """Loads config.json or creates a default one with descriptions if missing."""
    default_config = {
      "settings": {
        "raw_dir": "rawdata",
        "out_dir": "normalized_data",
        "output_prefix": "NORM_",
        "valid_extensions": [".tif", ".tiff", ".png", ".jpg", ".jpeg", ".bmp"],
        "target_intensity": 60000.0,
        "blur_sigma": 50,
        "percentile": 99.5
      },
      "descriptions": {
        "raw_dir": "The directory containing your raw, high-resolution input images.",
        "out_dir": "The output directory where normalized images will be saved.",
        "output_prefix": "A prefix prepended to the original filename for the output file.",
        "valid_extensions": "A list of image file extensions the script will search for and process.",
        "target_intensity": "The peak intensity value to scale the target percentile up to (e.g., 60000.0).",
        "blur_sigma": "The standard deviation (radius) for the Gaussian filter used to estimate and subtract background.",
        "percentile": "The intensity percentile (0-100) used to determine the scaling upper bound (e.g., 99.5 to avoid hot pixels)."
      }
    }
    
    if not CONFIG_FILE.exists():
        print(f"⚠️  {CONFIG_FILE.name} not found. Generating default configuration layout...")
        with open(CONFIG_FILE, "w") as f:
            json.dump(default_config, f, indent=4)
        return default_config
    
    try:
        with open(CONFIG_FILE, "r") as f:
            return json.load(f)
    except Exception as e:
        print(f"❌ Error reading {CONFIG_FILE.name}: {e}. Falling back to default settings.")
        return default_config

def normalize_fish_rounds_gpu():
    # Load configuration
    config = load_or_create_config()
    settings = config["settings"]
    
    # Setup directories from config
    raw_dir = Path(settings["raw_dir"])
    out_dir = Path(settings["out_dir"]) 
    out_dir.mkdir(parents=True, exist_ok=True)
    
    # Define formats and file identifiers from config
    valid_extensions = {ext.lower() for ext in settings["valid_extensions"]}
    prefix = settings["output_prefix"]
    
    # Grab all valid image files (excluding already normalized ones)
    files = [
        f for f in raw_dir.iterdir() 
        if f.is_file() 
        and f.suffix.lower() in valid_extensions 
        and not f.name.startswith(prefix)
    ]
    
    if not files:
        print(f"Error: No valid image files found in the '{raw_dir.resolve()}' directory.")
        return
        
    print(f"Found {len(files)} ultra-high-res images. Firing up GPU acceleration...")

    # Load processing constants from config
    target_intensity = float(settings["target_intensity"])
    blur_sigma = float(settings["blur_sigma"])
    percentile_val = float(settings["percentile"])

    for file in files:
        print(f"Processing {file.name}...")
        
        # 1. Load via CPU using imageio
        try:
            img_cpu = iio.imread(file).astype(np.float32)
        except Exception as e:
            print(f"  -> Error reading {file.name}: {e}")
            continue
        
        # Scientific Grayscale Conversion
        if img_cpu.ndim == 3:
            img_cpu = np.max(img_cpu, axis=-1)
        elif img_cpu.ndim == 4:
            img_cpu = np.max(img_cpu[..., :3], axis=-1)
            
        # 2. Beam single-channel grayscale data to GPU
        img_gpu = cp.array(img_cpu)
        
        # 3. GPU-Accelerated Background Subtraction using JSON parameter
        background_gpu = gaussian_filter(img_gpu, sigma=blur_sigma)
        img_bg_subtracted = img_gpu - background_gpu
        img_bg_subtracted[img_bg_subtracted < 0] = 0 
        
        # 4. GPU-Accelerated Percentile Calculation using JSON parameter
        p_target = cp.percentile(img_bg_subtracted, percentile_val) 
        
        # Output configuration setup
        out_name = f"{prefix}{file.name}"
        output_path = out_dir / out_name

        if p_target == 0:
            print(f"  -> Warning: {file.name} appears blank. Skipping scaling.")
            final_img_cpu = img_cpu.astype(np.uint16)
        else:
            # 5. Calculate Scale & Normalize (Still on GPU)
            scaling_factor = target_intensity / float(p_target)
            img_normalized = img_bg_subtracted * scaling_factor
            
            # 6. Clip and Convert to 16-bit
            img_normalized = cp.clip(img_normalized, 0, 65535).astype(cp.uint16)
            
            # 7. Beam back to CPU
            final_img_cpu = cp.asnumpy(img_normalized)

        # Format-Specific Saving
        if file.suffix.lower() in ['.jpg', '.jpeg']:
            final_img_cpu = (final_img_cpu / 256).astype(np.uint8)
            
        iio.imwrite(output_path, final_img_cpu)
        
        if p_target != 0:
            print(f"  -> Saved {out_name} (Scaling factor: {scaling_factor:.2f})")

    print("\n✅ All hybridization rounds successfully processed and saved in their original formats!")

if __name__ == "__main__":
    normalize_fish_rounds_gpu()