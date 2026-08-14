import os
import sys
import cv2
import numpy as np
import imageio.v3 as iio
from pathlib import Path
import streamlit as st
import scyjava
from scipy.ndimage import gaussian_filter as cpu_gaussian_filter
import warnings

warnings.filterwarnings("ignore")

# ==========================================
# HARDWARE ACCELERATION DETECTION
# ==========================================
try:
    import cupy as cp
    from cupyx.scipy.ndimage import gaussian_filter as gpu_gaussian_filter
    _ = cp.zeros((1,))
    HAS_GPU = True
except Exception:
    HAS_GPU = False

# ==========================================
# STREAMLIT 16GB UPLOAD CONFIGURATION
# ==========================================
def setup_streamlit_config():
    streamlit_dir = Path.cwd() / ".streamlit"
    config_path = streamlit_dir / "config.toml"
    
    if not config_path.exists():
        streamlit_dir.mkdir(parents=True, exist_ok=True)
        config_content = """[server]
maxUploadSize = 16384
enableCORS = false
enableXsrfProtection = false
"""
        with open(config_path, "w") as f:
            f.write(config_content)
        return True
    return False

CONFIG_JUST_CREATED = setup_streamlit_config()

# ==========================================
# JAVA / BIO-FORMATS ENVIRONMENT PATCH
# ==========================================
conda_prefix = os.environ.get("CONDA_PREFIX", sys.prefix)
java_path = Path(conda_prefix) / "Library"
if not os.environ.get("JAVA_HOME"):
    os.environ["JAVA_HOME"] = str(java_path)

scyjava.config.endpoints.append('com.google.guava:guava:31.1-jre')

from aicsimageio import AICSImage

try:
    from aicsimageio.readers.bioformats_reader import BioformatsReader
    HAS_BIOFORMATS = True
except ImportError:
    HAS_BIOFORMATS = False

SUPPORTED_EXTENSIONS = [
    '.ome.tif', '.ome.tiff', '.qptiff', '.dicom', '.mrcs', 
    '.tif', '.tiff', '.oir', '.oib', '.czi', '.nd2', '.ims', '.lif', '.vsi', 
    '.lsm', '.zvi', '.lei', '.ics', '.ids', '.stk', '.dv', '.r3d', '.im3', 
    '.dcm', '.mrc', '.rec', '.png', '.jpg', '.jpeg', '.bmp'
]

CLEAN_EXTS = list(set([ext.split('.')[-1] for ext in SUPPORTED_EXTENSIONS]))

# ==========================================
# CORE PROCESSING LOGIC
# ==========================================
def apply_ramfish_normalization(plane_data, do_norm, blur_sigma, percentile_val, target_intensity):
    """Applies GPU/CPU background subtraction and percentile scaling."""
    if not do_norm:
        return plane_data.astype(np.float32)

    # Hardware routing
    xp = cp if HAS_GPU else np
    blur_filter = gpu_gaussian_filter if HAS_GPU else cpu_gaussian_filter
    
    img_data = xp.array(plane_data.astype(np.float32))
    
    # 1. Background Subtraction
    background = blur_filter(img_data, sigma=blur_sigma)
    img_bg_subtracted = img_data - background
    img_bg_subtracted[img_bg_subtracted < 0] = 0 
    
    # 2. Percentile Peak Calculation
    p_target = float(xp.percentile(img_bg_subtracted, percentile_val))
    
    if p_target == 0:
        return plane_data.astype(np.float32) # Fallback if blank
        
    # 3. Scaling
    scaling_factor = target_intensity / p_target
    img_normalized = img_bg_subtracted * scaling_factor
    
    if HAS_GPU:
        return cp.asnumpy(img_normalized)
    return img_normalized

def format_and_resize(plane_f32, target_format, res_mode, custom_scale):
    """Clips to bit-depth and resizes spatially."""
    # 1. Bit Depth Conversion
    if target_format.lower() in ['.jpg', '.jpeg', '.png', '.bmp']:
        # Map 0-60000 to 0-255 safely
        final_img = np.clip((plane_f32 / 65535.0) * 255.0, 0, 255).astype(np.uint8)
    else:
        final_img = np.clip(plane_f32, 0, 65535).astype(np.uint16)

    # 2. Spatial Resizing
    h, w = final_img.shape[:2]
    new_w, new_h = w, h

    if res_mode == "2K (2048 x 2048)":
        new_w, new_h = 2048, 2048
    elif res_mode == "4K (4096 x 4096)":
        new_w, new_h = 4096, 4096
    elif res_mode == "Custom Scaling Factor":
        new_w = int(w * custom_scale)
        new_h = int(h * custom_scale)

    if (new_w, new_h) != (w, h):
        interpolation = cv2.INTER_LANCZOS4 if new_w > w else cv2.INTER_AREA
        final_img = cv2.resize(final_img, (new_w, new_h), interpolation=interpolation)

    return final_img

def process_and_export(file_path, output_dir, target_fmt, apply_mip, norm_settings, res_settings):
    file_path = Path(file_path)
    ext = file_path.suffix.lower()
    
    try:
        if ext in ['.oir', '.lif', '.vsi'] and HAS_BIOFORMATS:
            img = AICSImage(file_path, reader=BioformatsReader)
        else:
            img = AICSImage(file_path)
            
        raw_volume = img.get_image_data("CZYX")
        num_channels = raw_volume.shape[0]
        num_z = raw_volume.shape[1]
        
        exported_files = []
        prefix = "NORM_" if norm_settings["do_norm"] else "RAW_"
        
        for c in range(num_channels):
            # 1. ALWAYS export single slices (or single image if no z-stack)
            for z in range(num_z):
                plane_data = raw_volume[c][z]
                z_label = f"_Z{z}" if num_z > 1 else ""
                file_suffix = f"_C{c}{z_label}{target_fmt}"
                
                norm_plane = apply_ramfish_normalization(plane_data, **norm_settings)
                final_img = format_and_resize(norm_plane, target_fmt, **res_settings)
                
                clean_name = "".join(ch for ch in file_path.stem if ch.isprintable()).strip()
                out_name = output_dir / f"{prefix}{clean_name}{file_suffix}"
                iio.imwrite(str(out_name), final_img)
                exported_files.append(out_name.name)
                
            # 2. IN ADDITION, export MIP if requested and it is a z-stack
            if num_z > 1 and apply_mip:
                plane_data = np.max(raw_volume[c], axis=0)
                file_suffix = f"_C{c}_MIP{target_fmt}"
                
                norm_plane = apply_ramfish_normalization(plane_data, **norm_settings)
                final_img = format_and_resize(norm_plane, target_fmt, **res_settings)
                
                clean_name = "".join(ch for ch in file_path.stem if ch.isprintable()).strip()
                out_name = output_dir / f"{prefix}{clean_name}{file_suffix}"
                iio.imwrite(str(out_name), final_img)
                exported_files.append(out_name.name)
                    
        return True, exported_files
        
    except Exception as e:
        return False, str(e)

# ==========================================
# STREAMLIT UI 
# ==========================================
st.set_page_config(page_title="RAMFISH Unified Processor", layout="wide")

if CONFIG_JUST_CREATED:
    st.success("⚙️ Configured local server to accept 16 GB file uploads!")
    st.warning("⚠️ **Action Required:** Please restart the application for the 16 GB limit to take effect.\n\nGo to your terminal, press `Ctrl+C`, and run `streamlit run ramfish_unified_processor.py` again.")
    st.stop()

st.title("🔬 RAMFISH Unified Processor")
st.markdown("Ingest proprietary microscopy files, apply RAMFISH background normalization (GPU-accelerated), and export to standardized high-res formats in a single pass.")

col1, col2, col3 = st.columns([1.5, 1, 1.2])

with col1:
    st.subheader("1. Locate Data")
    uploaded_files = st.file_uploader(
        "Drag and drop files here", 
        accept_multiple_files=True,
        type=CLEAN_EXTS,
        help="Supports .oir, .czi, .lif, .tif, and more."
    )
    
    if uploaded_files:
        st.success(f"Loaded {len(uploaded_files)} file(s) ready for processing.")

    st.info(f"**Hardware Engine:** {'🚀 GPU (CUDA)' if HAS_GPU else '💻 CPU (SciPy)'}")

with col2:
    st.subheader("2. Export Settings")
    target_format = st.selectbox("Output Format:", [".tif", ".jpg", ".png", ".bmp"])
    apply_mip = st.checkbox("Export Max Intensity Projection (MIP) along with Z-slices", value=True)
    
    res_mode = st.selectbox(
        "Target Resolution:",
        ["Original Resolution", "2K (2048 x 2048)", "4K (4096 x 4096)", "Custom Scaling Factor"]
    )
    
    custom_scale = 1.0
    if res_mode == "Custom Scaling Factor":
        custom_scale = st.slider("Scale Factor:", min_value=0.25, max_value=4.0, value=1.0, step=0.25)
        
    output_dir = Path.cwd() / "normalized_data"

with col3:
    st.subheader("3. RAMFISH Normalization")
    do_norm = st.toggle("Apply RAMFISH Normalization", value=True)
    
    blur_sigma = st.number_input("Gaussian Blur Sigma (radius):", min_value=1.0, max_value=200.0, value=50.0, disabled=not do_norm)
    percentile_val = st.number_input("Robust Percentile Bound:", min_value=90.0, max_value=100.0, value=99.5, step=0.1, disabled=not do_norm)
    target_int = st.number_input("Target Intensity Scaling:", min_value=1000.0, max_value=65535.0, value=60000.0, disabled=not do_norm)

st.divider()

if st.button("🚀 Execute Unified Pipeline", type="primary", use_container_width=True):
    if not uploaded_files:
        st.warning("No files uploaded to process.")
    else:
        output_dir.mkdir(parents=True, exist_ok=True)
        temp_dir = Path.cwd() / "temp_uploads"
        temp_dir.mkdir(parents=True, exist_ok=True)
        
        progress_bar = st.progress(0)
        status_text = st.empty()
        
        success_count = 0
        total_files = len(uploaded_files)
        
        norm_settings = {
            "do_norm": do_norm,
            "blur_sigma": blur_sigma,
            "percentile_val": percentile_val,
            "target_intensity": target_int
        }
        
        res_settings = {
            "res_mode": res_mode,
            "custom_scale": custom_scale
        }
        
        for i, uploaded_file in enumerate(uploaded_files):
            status_text.text(f"Processing ({i+1}/{total_files}): {uploaded_file.name}...")
            
            temp_file_path = temp_dir / uploaded_file.name
            with open(temp_file_path, "wb") as f:
                f.write(uploaded_file.getbuffer())
            
            success, msg = process_and_export(
                temp_file_path, output_dir, target_format, apply_mip, 
                norm_settings, res_settings
            )
            
            try:
                temp_file_path.unlink()
            except Exception:
                pass
            
            if success:
                success_count += 1
            else:
                st.error(f"Failed to process {uploaded_file.name}: {msg}")
                
            progress_bar.progress((i + 1) / total_files)
            
        status_text.text(f"Complete! Successfully exported {success_count} out of {total_files} files to '{output_dir.name}'.")
        st.balloons()
        
        try:
            temp_dir.rmdir()
        except Exception:
            pass