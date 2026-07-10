# RAMFISH Software Suite

The **RAMFISH Software Suite (inside the Software folder)** is an open-source computational pipeline engineered to handle multiplexed spatial domain datasets generated from the RAMFISH workflow. 

This pipeline processes high-resolution microscope channels by integrating GPU-accelerated intensity normalization, full-stack visual rough alignment, automated non-rigid B-Spline registration, and multi-core mRNA spot segmentation. By standardizing coordinate space across iterative hybridization rounds and stripping away complex background autofluorescence, the suite empowers researchers to seamlessly reconstruct publication-grade, high-dimensional composite spatial expression maps across intact tissue architectures.

---

## 💻 Hardware Requirements

The pipeline is capable of running on most desktop-grade PCs equipped with a dedicated GPU. 

* **Optimum Configuration:** * **CPU:** AMD Ryzen 5 5600X / Intel Core i5-12400F
  * **GPU:** NVIDIA RTX 3060
  * **RAM:** 32GB DDR4
* **High-Performance Configuration:** * **CPU:** AMD Ryzen Threadripper 9965WX
  * **GPU:** NVIDIA RTX 5080 or 5090
  * **RAM:** 128GB DDR5
* **Operating Systems Tested:** Windows 11, Ubuntu 24.04.

---

## 🛠️ Prerequisites & Installation

### Core Applications
Install the following core applications prior to executing the pipeline:
* **Python 3.13+**: Install from the Microsoft Store or the [official Python website](https://www.python.org/).
* **Web Browser**: [Google Chrome](https://www.google.com/chrome/) or [Mozilla Firefox](https://www.firefox.com/).
* **Imaris Utilities**: Imaris Viewer and Imaris File Converter from the [official website](https://imaris.oxinst.com/).
* **IDE**: [Visual Studio Code (VS Code)](https://code.visualstudio.com/).

### Python Dependencies
Open your terminal and install the required dependencies. 

For modern GPUs (CUDA 12.x), run:
```bash
pip install numpy cupy-cuda12x imageio opencv-python tifffile flask SimpleITK scipy fastapi uvicorn python-multipart pandas pillow scikit-image tqdm
```

If you are running the pipeline on an older GPU (CUDA 11.x), run:
```bash
pip install numpy cupy-cuda11x imageio opencv-python tifffile flask SimpleITK scipy fastapi uvicorn python-multipart pandas pillow scikit-image tqdm
```

---

## 📂 Data Preparation

1. Load the raw image files (imaged at 20x lens at 2k or 4k resolution) from your microscope software (e.g., Olympus FLUOVIEW) into the **Imaris File Converter** and convert them to `.ims` format.
2. Open the `.ims` file in **Imaris Viewer** and export the individual channels, including DAPI, in high-resolution (4000x4000 pixels) at 600 dpi in `.tiff` format.

**Note:** Ensure the boundary lines of your image are not exported along with the image data. There is no need to orient the images at this stage, as this will be handled by the rough aligner module.

---

## 🚀 Pipeline Workflow

### 1. Normalization (`1_normalization`)
To resolve intensity discrepancies across iterative multiplexed imaging cycles, this module uses a GPU-accelerated standardization protocol to equalize brightness and contrast. It converts multichannel inputs into a unified grayscale representation, utilizes a heavy Gaussian blur filter to estimate and subtract non-uniform illumination, and computes a dynamic intensity upper-bound to filter out hot-pixel anomalies.

* **Setup:** Place high-resolution exports from Imaris Viewer into the `rawdata/` directory.
* **Execution:** `python ramfish_normalization.py`
* **Configuration (`config.json`):**
  * `raw_dir` / `out_dir`: Input and destination folder paths.
  * `output_prefix`: Custom text string added to output filenames (default: `NORM_`).
  * `valid_extensions`: Target formats to scan (e.g., `.tif`, `.png`, `.jpg`).
  * `target_intensity`: Rescaling maximum value (default: `60000.0`) to maximize signal dynamic range in 16-bit space.
  * `blur_sigma`: Gaussian convolution filter standard deviation for background illumination modeling.
  * `percentile`: Intensity cutoff percentile (99.0% - 99.9%) to establish true signal maximum while ignoring artifacts.
* **Output:** Compiled in the `normalized_data/` directory.

### 2. Rough Aligner (`2_rough_aligner`)
Rigid SimpleITK-based B-Spline registration frameworks frequently fail when confronted with heavy geometric deformations or tissue flipping. This hybrid interactive application bridges visual micro-manipulation with high-resolution spatial matrices.

* **Setup:** Organize normalized files into their respective sequence folders (`Round_1/`, `Round_2/`, ... `Round_N/`).
* **Execution:** `python ramfish_rough_aligner.py`
* **Usage:** A local web interface will launch in your browser. Select your `Round_1` baseline image as the absolute fixed anchor and corresponding moving reference images for downstream folders. Adjust UI tools (Opacity, Scale, Rotation, X/Y Translate, Flip) until structural features match. Click **"Apply Rough Alignment to Folder"** to batch-transform every other target gene channel in that round's folder. 


* **Output:** Transformed files are updated directly inside their native sequence directory, prefixed as `ALIGNED_`.

### 3. Fine Aligner (`3_fine_aligner`)
Ensures high-fidelity spatial registration by mapping all sequential datasets back to the absolute `Round_1` coordinate grid. It calculates a global affine matrix transformation followed by a non-rigid B-Spline deformable registration algorithm (via SimpleITK) to compensate for localized elastic tissue distortions (stretching, shrinkage).

* **Setup:** Migrate roughly aligned (`ALIGNED_`) entries into the active `Round_X/` folders. Ensure *only* these files occupy the workspace.
* **Execution:** `python ramfish_fine_aligner.py`
* **Usage:** In the browser console, specify multiplex constraints in "Total FISH Rounds". Designate your static anchor in "Round 1 (Global Fixed Reference)". Sequentially load your Structural Reference and Target Genes for subsequent rounds. Select **"Run Batch Alignment"**. 


* **Output:** Compiled in the `aligned_outputs/` directory with standardized prefixes (e.g., `R2_aligned_...`).
* **Note on 3D:** For 3D registration outside this pipeline, cell boundaries can be mapped in Imaris 10.2 via native Image Alignment toolkits.

### 4. Spot Counter (`4_spot_counter`)
Utilizes a parallelized Laplacian of Gaussian (LoG) blob detection algorithm with automatic scale selection to identify fluorescent mRNA transcript spots and localized signal clusters. The engine is optimized for robust, semi-quantitative spatial profiling.

* **Setup:** Migrate registered, high-resolution outputs from the fine-alignment directory to `rawdata/`. Rename files directly to their respective target gene names (e.g., `ci.jpg`).
* **Execution:** `python ramfish_spotcounter.py`
* **Configuration (`config.json`):**
  * `blob_min_sigma`: Minimum expected spot radius in pixels (default: `2`).
  * `blob_max_sigma`: Maximum expected spot radius in pixels (default: `5`).
  * `blob_num_sigma`: Number of intermediate size steps checked (default: `4`).
  * `blob_threshold`: Peak-detection filter sensitivity (default: `0.30`).
  * `intensity_cutoff`: Absolute brightness barrier to eliminate autofluorescence, scaled 0.0 to 1.0 (default: `0.25`).
  * `synthetic_spot_radius`: Rendered dot radius in synthetic output images.
* **Output Structure:**
  * `data_sheets/`: Contains `individual_csvs/` (coordinates mapped per gene), `ERROR_LOG.csv`, `BATCH_SUMMARY_COUNTS.csv`, and the master `ramfish_master_spots.csv`.
  * `spot_images/`: Diagnostic validation images with red circles around verified transcripts.
  * `synthetic_spots/`: Binarized, zero-background synthetic dot images used for final merging.

### 5. Merger (`5_merger`)
Integrates the discrete spatial coordinates of detected spots from all sequential rounds into a unified, high-dimensional multiplexed-FISH map, providing a global view of combinatorial gene expression across the tissue architecture.

* **Execution:** Open `ramfish_merger.html` locally in your browser.
* **Usage:** Load target channels from your `synthetic_spots/` directory. Ensure files follow the `gene_name.file_extension` format (e.g., `ptc.jpg`, `hh.jpg`). **Designate your structural template layer strictly as `DAPI.jpg`**. The tool supports up to 30 distinct multiplex channels simultaneously and allows for dynamic UI configurations (background noise floors, gamma curves, figure legends). 

![Merger UI](images/Figure_S19.jpg)

* **Output:** Generates and exports a publication-grade, multi-color composite spatial map matching native resolution.
* **Web Portal:** An open-access web deployment of this module is actively available at [tirthadasbanerjee.com/assets/tools/ramfishmerger.html](https://tirthadasbanerjee.com/assets/tools/ramfishmerger.html).

---

## 📜 Citation
If you use the RAMFISH Software Suite in your research, please cite the foundational methodology:

> **Rapid Amplified Multiplexed-FISH (RAM-FISH)**
> Preprint available on bioRxiv: [doi: 10.1101/2024.12.06.627193](https://doi.org/10.1101/2024.12.06.627193)
