# RAMFISH Software Suite

The **RAMFISH Software Suite** is an open-source computational pipeline engineered to handle multiplexed spatial domain datasets generated from the RAMFISH workflow. 

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
pip install numpy cupy-cuda12x imageio opencv-python tifffile flask SimpleITK fastapi uvicorn python-multipart pandas pillow scikit-image tqdm
