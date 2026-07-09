import os
import cv2
import json
import warnings
import numpy as np
import pandas as pd
from pathlib import Path
from PIL import Image
from skimage.feature import blob_log
from concurrent.futures import ProcessPoolExecutor, as_completed
from tqdm import tqdm

warnings.filterwarnings("ignore")

# ==========================================
# CONFIGURATION MANAGER
# ==========================================
def load_or_create_config(base_dir):
    config_path = base_dir / "config.json"
    
    default_config = {
        "settings": {
            "blob_min_sigma": 2,
            "blob_max_sigma": 5,
            "blob_num_sigma": 4,
            "blob_threshold": 0.40,
            "intensity_cutoff": 0.30,
            "synthetic_spot_radius": 2
        },
        "descriptions": {
            "blob_min_sigma": "The minimum expected spot radius (in pixels). Decrease this value if small, sharp points are being missed.",
            "blob_max_sigma": "The maximum expected spot radius (in pixels). Increase this value if larger, brighter spots are being ignored.",
            "blob_num_sigma": "The number of intermediate size steps checked between min and max sigma. Default is 4.",
            "blob_threshold": "The initial peak-detection sensitivity. Lower values capture dimmer spots; higher values filter for high-contrast peaks.",
            "intensity_cutoff": "The absolute brightness barrier used to eliminate tissue autofluorescence. Scaled strictly from 0.0 to 1.0. Formula: (Desired Camera Cutoff Count) / (Max Bit Depth). E.g., 20000 / 65535 = 0.305.",
            "synthetic_spot_radius": "The radius (in pixels) of the rendered dots in the synthetic output images. Increase this value to make the spots larger in the final multiplexed merge."
        }
    }
    
    if not config_path.exists():
        print("⚙️  No config file found. Generating default 'config.json'...")
        with open(config_path, 'w') as f:
            json.dump(default_config, f, indent=4)
        return default_config
        
    try:
        with open(config_path, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"❌ Error reading config file: {e}. Utilizing default parameters.")
        return default_config

# ==========================================
# ISOLATED WORKER FUNCTION 
# ==========================================
def process_single_image(file_path, out_dirs, config):
    gene_name = file_path.stem
    
    if "dapi" in gene_name.lower():
        return {"status": "skipped", "gene": gene_name, "reason": "Nuclear stain"}

    settings = config.get("settings", config)
    synth_radius = int(settings.get("synthetic_spot_radius", 2))

    try:
        # 1. Bulletproof Image Reading via OpenCV (Bypasses skimage Path crashes)
        img = cv2.imread(str(file_path), cv2.IMREAD_UNCHANGED)
        if img is None:
            return {"status": "error", "gene": gene_name, "reason": "OpenCV failed to decode image (unsupported format or corrupted)"}
            
        # 2. Universal Grayscale Extraction
        if img.ndim == 3:
            if img.shape[2] >= 3:
                # Safely drop alpha channels (PNGs) and convert standard BGR to grayscale
                img_gray = cv2.cvtColor(img[:, :, :3], cv2.COLOR_BGR2GRAY)
            else:
                img_gray = img[:, :, 0]
        else:
            img_gray = img
            
        # 3. Standardize Data Range to Float [0.0, 1.0] for LoG Math
        if img_gray.dtype == np.uint8:
            img_float = img_gray.astype(np.float32) / 255.0
        elif img_gray.dtype == np.uint16:
            img_float = img_gray.astype(np.float32) / 65535.0
        else:
            img_float = img_gray.astype(np.float32)
            
        # 4. Prepare UI visualization canvases (strictly 8-bit RGB to prevent PIL crashes)
        img_8u = cv2.normalize(img_gray, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
        img_rgb = cv2.cvtColor(img_8u, cv2.COLOR_GRAY2RGB) 
        synth_rgb = np.zeros_like(img_rgb)
        
        # 5. Core Blob Detection
        blobs = blob_log(
            img_float, 
            min_sigma=float(settings.get("blob_min_sigma", 2)), 
            max_sigma=float(settings.get("blob_max_sigma", 5)), 
            num_sigma=int(settings.get("blob_num_sigma", 4)), 
            threshold=float(settings.get("blob_threshold", 0.40))
        )
        
        if len(blobs) == 0:
            return {"status": "success", "gene": gene_name, "count": 0}
            
        raw_y, raw_x, raw_r = blobs[:, 0], blobs[:, 1], blobs[:, 2]
        
        # Extract pixel intensities from the standardized float map
        raw_intensities = np.array([img_float[int(y), int(x)] for y, x in zip(raw_y, raw_x)])
        
        # 6. Hard Intensity Filter to eliminate tissue autofluorescence
        valid_mask = raw_intensities > float(settings.get("intensity_cutoff", 0.30))
        
        y_coords = raw_y[valid_mask]
        x_coords = raw_x[valid_mask]
        radii = raw_r[valid_mask]
        intensities = raw_intensities[valid_mask]
        
        final_count = len(y_coords)
        if final_count == 0:
             return {"status": "success", "gene": gene_name, "count": 0}

        # 7. Draw Verification Annotations
        for y, x, r in zip(y_coords, x_coords, radii):
            center_coords = (int(x), int(y))
            
            # Validation Ring (Red layout for PIL array)
            val_radius = int(r * np.sqrt(2)) + 2 
            cv2.circle(img_rgb, center_coords, val_radius, (255, 0, 0), 1, cv2.LINE_AA)
            
            # Synthetic Dot (White) using the new JSON parameter
            cv2.circle(synth_rgb, center_coords, synth_radius, (255, 255, 255), -1, cv2.LINE_AA)
            
        # 8. Output generation
        Image.fromarray(img_rgb).save(out_dirs["img"] / f"{gene_name}_val.jpg", quality=95)
        Image.fromarray(synth_rgb).save(out_dirs["synth"] / f"{gene_name}.jpg", quality=95)
        
        gene_df = pd.DataFrame({
            'x': np.round(x_coords, 2),
            'y': np.round(y_coords, 2),
            'gene': gene_name,
            'intensity': np.round(intensities, 3) 
        })
        gene_df.to_csv(out_dirs["csv"] / f"{gene_name}_spots.csv", index=False)
        
        return {"status": "success", "gene": gene_name, "count": final_count}
        
    except Exception as e:
        return {"status": "error", "gene": gene_name, "reason": str(e)}

# ==========================================
# MAIN PIPELINE ORCHESTRATOR
# ==========================================
def run_ramfish_pipeline_parallel(input_folder_path):
    input_dir = Path(input_folder_path)
    raw_dir = input_dir / "rawdata"
    
    CONFIG = load_or_create_config(input_dir)
    
    dirs = {
        "csv": input_dir / "data_sheets" / "individual_csvs",
        "img": input_dir / "spot_images",
        "synth": input_dir / "synthetic_spots"
    }
    for d in dirs.values():
        d.mkdir(parents=True, exist_ok=True)
        
    if not raw_dir.exists():
        raw_dir.mkdir(parents=True, exist_ok=True)
        print(f"📁 Created '{raw_dir.name}' folder in {input_dir}.")
        print("⚠️ Please place your images inside the 'rawdata' folder and run the script again.")
        return
        
    # Include all common spatial biology array formats
    exts = ['*.jpg', '*.jpeg', '*.tif', '*.tiff', '*.png', '*.bmp']
    image_files = [f for ext in exts for f in raw_dir.glob(ext)]
    
    if not image_files:
        print(f"No images found in the '{raw_dir.name}' folder to process.")
        return
        
    print(f"🚀 Initializing RAM-FISH Pipeline...")
    print(f"📁 Found {len(image_files)} images in 'rawdata'.")
    print(f"⚙️  Spinning up CPU workers using local configuration...\n")

    summary_stats = []
    error_log = []

    with ProcessPoolExecutor(max_workers=1) as executor:
        futures = {
            executor.submit(process_single_image, file, dirs, CONFIG): file 
            for file in image_files
        }
        
        for future in tqdm(as_completed(futures), total=len(image_files), desc="Processing Batch"):
            result = future.result()
            
            if result["status"] == "success":
                summary_stats.append({'Gene': result["gene"], 'Total_Spots': result["count"]})
            elif result["status"] == "skipped":
                pass 
            else:
                # Immediate console feedback so it doesn't crash in silence
                print(f"\n❌ Error processing {result['gene']}: {result['reason']}") 
                error_log.append({'File': result["gene"], 'Error': result["reason"]})

    summary_df = pd.DataFrame(summary_stats)
    summary_df.to_csv(input_dir / "data_sheets" / "BATCH_SUMMARY_COUNTS.csv", index=False)
    
    print("\n✅ Batch Processing Complete!")
    print(f"📊 Total Transcripts Detected: {summary_df['Total_Spots'].sum() if not summary_df.empty else 0}")
    
    if error_log:
        print(f"\n⚠️ Encountered {len(error_log)} errors during processing.")
        pd.DataFrame(error_log).to_csv(input_dir / "data_sheets" / "ERROR_LOG.csv", index=False)

    print("\n📦 Compiling Master Spatial Coordinate File...")
    all_csvs = list(dirs["csv"].glob("*_spots.csv"))
    if all_csvs:
        master_df = pd.concat([pd.read_csv(f) for f in all_csvs], ignore_index=True)
        master_out = input_dir / "data_sheets" / "ramfish_master_spots.csv"
        master_df.to_csv(master_out, index=False)
        print(f"✅ Master CSV compiled and saved to: {master_out.name}")

if __name__ == "__main__":
    run_ramfish_pipeline_parallel(".")