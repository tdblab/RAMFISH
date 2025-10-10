1) For merging files.
merge.py takes pre-aligned single channel images of individual genes (up to 10 genes). The image format can be jpg, bmp, png, and tiff. 
The name of the image files should be gene_name.format. For example, spalt.jpg, optix.jpg, omb.jpg, etc. The algorithm will automatically 
place the name of the genes and the associated color in the top-right of the composite image. 

For execution, create a folder and copy the merge.py file (GitHub link). Create a subfolder named multiplex_fish_images and place all your 
images there. Make sure images are of the same dimensions. Install the latest version of Python. Open the command prompt (or terminal on Mac) 
and move to the location where merge.py is copied. To install the dependencies, type:

pip install opencv-python numpy

To run the file type:

python merge.py

2) For running the simulations.

Install dependencies

pip install matplotlib numpy scipy pillow

To run the file type:

python filename.py

3) For creating probes by web interface.

https://tdblab.github.io/hcrprobedesigner/hcr_22.1.html

4) For the excel sheets for creating probes.

The first sheet (CDS_gene_extractor.xml) can be used to extract 25 bps nucleotides for both the forward and reverse strands of the chosen 
HCR primary probe binding sites and reverse-complement them. The second step on the next sheet (Amplifier_adder.xml) adds the amplifiers 
specific to the selected pairs of complementary sites. Briefly, a candidate gene coding sequence file from a repository such as NCBI 
(https://ncbi.nlm.nih.gov/) is downloaded and copied and pasted into the first column of the CDS_gene_extractor file. A reverse complement 
macro (https://github.com/LJI-Bioinformatics/Excel-Reverse-Complement) is loaded into the MS Excel sheet. This produces 25 bp pairs of 
nucleotides, which are copied and pasted to the amplifier_adder.xml file that contains the secondary fluorescent-specific amplifier sequence. 

5) For 3D printing of files.

For 3D printing, the microscope plate (Olympus FV3000 specific) and the magnetic lock, download the CAD files from the github link. Open 
the .stl file in any 3D printer file slicer. In the current work, we have used Ultimaker Cura (https://ultimaker.com/de/software/ultimaker-cura/#downloads).
In the slicing software, select your 3D printer 
and use PLA+ or PETG specific settings with an infill density of 50%. Save your .gcode file in an external memory (USB) and 3D print. Note: 
Black filament is ideal. But the user can select any color.




