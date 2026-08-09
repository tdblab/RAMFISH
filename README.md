# 🔬 RAMFISH Software Suite

<img width="2816" height="959" alt="1" src="https://github.com/user-attachments/assets/05cbdd9a-5e4f-4d34-b316-4d39fa01ff0b" />

The **RAMFISH Software Suite** is an open-source computational pipeline engineered to handle multiplexed spatial-domain datasets generated from the **RAMFISH workflow**.

The suite processes high-resolution microscopy channels through a user-friendly web GUI for proprietary-file conversion and GPU-accelerated intensity normalization, interactive visual rough alignment, automated non-rigid B-Spline registration, and multi-core mRNA spot segmentation.

By standardizing coordinate space across iterative hybridization rounds, RAMFISH enables researchers to reconstruct publication-grade, high-dimensional composite spatial-expression maps across intact tissue architectures.

---

## 🌱 Beginner-Friendly Installation Guide

If you are new to Python or terminals, follow these copy-paste steps to set up a dedicated environment using **Miniconda**.

### Step 1: Install Miniconda

Download and install Miniconda for your operating system from the official documentation:

- **Windows:** Install the Python 3.12 64-bit Miniconda installer. During installation, select **Just Me**. Adding Miniconda to `PATH` is optional; using the Miniconda Prompt is recommended.
- **Linux (x86_64):** Run:

```bash
mkdir -p ~/miniconda3
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda3/miniconda.sh
bash ~/miniconda3/miniconda.sh -b -u -p ~/miniconda3
rm -f ~/miniconda3/miniconda.sh
~/miniconda3/bin/conda init bash
```

- **macOS Apple Silicon (M1/M2/M3/M4):** Install the `MacOSX-arm64` Miniconda package.
- **macOS Intel:** Install the `MacOSX-x86_64` Miniconda package.

Official Miniconda documentation:

https://docs.conda.io/en/latest/

---

### Step 2: Create the RAMFISH Environment

Open **Anaconda Prompt / Miniconda Prompt** on Windows, or a terminal on macOS/Linux.

Run the following commands one at a time:

#### 1. Create the environment

```bash
conda create -n ramfish python=3.12 -y
```

#### 2. Activate the environment

```bash
conda activate ramfish
```

#### 3. Install Java and Bio-Formats support

```bash
conda install -c conda-forge bioformats_jar openjdk maven -y
```

#### 4. Install the core pipeline dependencies

```bash
pip install streamlit pandas numpy plotly tifffile scipy scikit-image "aicsimageio>=4.14,<5" "numcodecs<0.16.0" "starlette==0.46.0" ome-zarr scyjava imagecodecs opencv-python flask SimpleITK fastapi uvicorn python-multipart pillow tqdm
```

#### 5. Optional: Install NVIDIA GPU acceleration

For NVIDIA GPUs supporting CUDA 12.x:

```bash
pip install "cupy-cuda12x[ctk]"
```

For systems using CUDA 11.x, use:

```bash
pip install "cupy-cuda11x[ctk]"
```

> **Note:** CuPy installation should match the CUDA environment supported by your NVIDIA driver/GPU.

---


# 💻 Hardware Requirements

RAMFISH is designed to run on standard desktop workstations, with optional NVIDIA GPU acceleration.

### Recommended Configuration

| Component | Recommended |
|---|---|
| CPU | AMD Ryzen 5 5600X / Intel Core i5-12400F |
| GPU | NVIDIA RTX 3060 |
| RAM | 32 GB DDR4 |
| Storage | SSD recommended |

### High-Performance Configuration

| Component | High Performance |
|---|---|
| CPU | AMD Ryzen Threadripper 9965WX |
| GPU | NVIDIA RTX 5080 / RTX 5090 |
| RAM | 128 GB DDR5 |
| Storage | High-speed NVMe SSD |

### Tested Operating Systems

- Windows 11
- Ubuntu 24.04

> Performance depends strongly on image dimensions, number of channels, number of hybridization rounds, Z-stack size, available RAM, and GPU memory.

---

# 🛠️ Pipeline Workflow

The RAMFISH Software Suite consists of five major processing stages.

```text
Raw Microscopy Data
        │
        ▼
┌─────────────────────────┐
│ 1. Normalization/Export │
│ ramfish_processor.py    │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ 2. Rough Alignment      │
│ ramfish_rough_aligner.py│
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ 3. Fine Alignment       │
│ ramfish_fine_aligner.py │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ 4. mRNA Spot Detection  │
│ ramfish_spotcounter.py  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ 5. Multiplexed Merger   │
│ ramfish_merger.html     │
└────────────┬────────────┘
             │
             ▼
      Multiplexed Spatial
       Expression Map
```
<img width="2870" height="714" alt="2" src="https://github.com/user-attachments/assets/078481dd-f1a7-4c34-9e0f-926e7bf90d37" />

---

### Launch

Activate the environment:

```bash
conda activate ramfish
```

Navigate to the RAMFISH software directory:

```bash
cd path/to/your/RAMFISH/software/folder
```
---

## 1. Normalization & Exporter

### `ramfish_processor.py`

The normalization stage ingests raw microscopy files and prepares standardized images for downstream registration.

Processing includes:

- Robust percentile-based brightness/contrast normalization
- Local background subtraction using Gaussian filtering
- Z-stack MIP generation
- Standardized image export
- Optional GPU acceleration

 ### Run

```bash
streamlit run ramfish_processor.py
```
A browser window should open automatically with the **RAMFISH Unified Processor** interface.
<img width="1832" height="684" alt="ab" src="https://github.com/user-attachments/assets/b7db6227-fc0a-4b6c-b233-df8e566a2562" />


The exported images can be organized into sequential hybridization-round folders.

Example:

```text
RAMFISH_Project/
├── Round_1/
│   ├── DAPI.tif
│   ├── geneA.tif
│   └── geneB.tif
├── Round_2/
│   ├── DAPI.tif
│   ├── geneC.tif
│   └── geneD.tif
└── Round_3/
    ├── DAPI.tif
    └── geneE.tif
```

---

# 2. Rough Aligner

## `ramfish_rough_aligner.py`

The rough aligner provides interactive, browser-based manipulation before automated deformable registration.

This stage is particularly useful when sequential rounds exhibit:

- Large translations
- Rotation
- Scaling differences
- Tissue flipping
- Significant initial geometric displacement

### Setup

Organize normalized images into sequential folders:

```text
Round_1/
Round_2/
Round_3/
...
Round_N/
```

### Run

```bash
python ramfish_rough_aligner.py
```

A local web interface will launch in your browser.
<img width="1282" height="898" alt="3" src="https://github.com/user-attachments/assets/1f201b76-6f05-4a60-8dcd-b04de4fe5db1" />


Select the **Round 1** dataset as the fixed reference and use the interactive controls to adjust:

- Scale
- Rotation
- Translation
- Flip/orientation

Apply the transformation to the selected folder using:

**Apply Rough Alignment to Folder**

Aligned files are saved with the `ALIGNED_` prefix.

---

# 3. Fine Aligner

## `ramfish_fine_aligner.py`

The fine alignment stage performs high-fidelity registration of sequential datasets into the absolute **Round 1 coordinate space**.

The workflow combines:

1. Global affine registration
2. Non-rigid B-Spline deformable registration

This allows the pipeline to compensate for local tissue deformation that cannot be corrected reliably through rigid or affine transformations alone.

### Setup

Place roughly aligned files into their corresponding `Round_X/` directories.

Example:

```text
Round_1/
    ALIGNED_DAPI.tif
    ALIGNED_geneA.tif

Round_2/
    ALIGNED_DAPI.tif
    ALIGNED_geneB.tif
```

### Run

```bash
python ramfish_fine_aligner.py
```
<img width="1171" height="654" alt="4" src="https://github.com/user-attachments/assets/96bddb2b-345e-4d72-bc68-4f028ebfc53a" />


Use the browser interface to:

- Define the static reference/anchor
- Select moving datasets
- Specify multiplexing constraints
- Configure registration parameters
- Run batch alignment

Processed datasets are written to:

```text
aligned_outputs/
```

---

# 4. mRNA Spot Counter

## `ramfish_spotcounter.py`

The spot counter detects fluorescent mRNA transcripts and localized signal clusters using a parallelized **Laplacian of Gaussian (LoG)** blob-detection workflow.

The pipeline supports automatic scale selection and configurable intensity filtering for semi-quantitative spatial profiling.

### Setup

Place registered images into:

```text
rawdata/
```

Rename target images using their gene/channel names.

For example:

```text
rawdata/
├── ci.jpg
├── wg.jpg
├── optix.jpg
└── spalt.jpg
```

### Run

```bash
python ramfish_spotcounter.py
```

### Configuration

Detection parameters can be adjusted through:

```text
config.json
```

Important parameters include:

```json
{
    "blob_min_sigma": "...",
    "blob_max_sigma": "...",
    "intensity_cutoff": "..."
}
```

These parameters can be tuned to control detection scale and suppress tissue autofluorescence or other background signals.

### Outputs

The spot counter generates:

```text
data_sheets/
spot_images/
synthetic_spots/
```

#### `data_sheets/`

Contains individual and master CSV files containing detected spot coordinates and associated measurements.

#### `spot_images/`

Contains validation images showing detected spots for visual quality control.

#### `synthetic_spots/`

Contains binarized/synthetic representations of detected transcript locations for downstream multiplex merging.

---

# 5. Multiplexed Merger

## `ramfish_merger.html`

The merger combines spatial coordinates from individual transcript channels into a unified multiplexed-FISH representation.

### Launch

Open:

```text
ramfish_merger.html
```

directly in a web browser.

The RAMFISH merger can also be accessed through:

https://tirthadasbanerjee.com/assets/tools/ramfishmerger.html

<img width="1296" height="874" alt="5" src="https://github.com/user-attachments/assets/0922ad39-2d02-40a8-be1f-1a3a5763d195" />


### Input

Load the synthetic spot maps generated by the spot counter.

Use the naming convention:

```text
gene_name.jpg
```

The structural reference/template should be:

```text
DAPI.jpg
```

### Available controls

The merger provides controls for:

- Noise floor
- Gamma
- Channel visualization
- Legend placement
- Composite rendering

The final output can be exported as a publication-ready multiplexed spatial expression map.

---


# 🔬 RAMFISH Workflow at a Glance

| Stage | Tool | Purpose |
|---|---|---|
| 1 | `ramfish_processor.py` | Proprietary-file conversion, MIP, normalization |
| 2 | `ramfish_rough_aligner.py` | Interactive coarse spatial alignment |
| 3 | `ramfish_fine_aligner.py` | Affine + non-rigid B-Spline registration |
| 4 | `ramfish_spotcounter.py` | mRNA spot detection and coordinate extraction |
| 5 | `ramfish_merger.html` | Multiplexed spatial-map reconstruction |

---

# 📊 Output

The final RAMFISH output is a high-dimensional spatial expression map in which transcript channels from multiple iterative hybridization rounds are registered into a common coordinate system.

This enables visualization and analysis of multiplexed gene-expression patterns while retaining their spatial relationship to tissue architecture.

---

# 📜 Citation

If you use the RAMFISH Software Suite in your research, please cite the foundational RAM-FISH methodology:

**Rapid Amplified Multiplexed-FISH (RAM-FISH)**

bioRxiv preprint: https://www.biorxiv.org/content/10.1101/2024.12.06.627193v4

**DOI:** `10.1101/2024.12.06.627193`

---

# 🤝 Contributing

Contributions, bug reports, feature requests, and improvements are welcome.

When reporting an issue, please include:

- Operating system
- Python version
- RAMFISH version/commit
- GPU model and CUDA version, if applicable
- Input file format
- Relevant error message or traceback
- A minimal description of the dataset and processing stage where the issue occurred

email to: tirtha_banerjee@u.nus.edu (Tirtha Das Banerjee)

---

# ⚠️ Important Notes

- Always keep an untouched copy of your raw microscopy data.
- Work on normalized/processed copies rather than modifying irreplaceable raw data.
- Proprietary microscopy formats may require compatible Bio-Formats/AICSImageIO support.
- GPU acceleration requires a compatible NVIDIA GPU and CUDA environment.
- Large microscopy datasets can require substantial RAM and temporary disk space.
- Registration quality should be visually inspected before downstream spot quantification.

---

## 🧬 RAMFISH

**Rapid Amplified Multiplexed-FISH Software Suite**

A computational framework for converting iterative multiplexed microscopy rounds into unified spatial transcriptomic maps.
