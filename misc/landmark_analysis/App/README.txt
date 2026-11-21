RAMFISH landmark tools

1. Align your rounds in RAMFISH Studio (Import & normalize -> Rough alignment -> Fine alignment).
   Use the images in Output/<project>/1_normalized (before alignment), 2_rough_aligned and 3_fine_aligned.

2. Place the landmarks: open ramfish_landmark_check.html in Chrome, Edge or Firefox (works offline).
   - Drop in the Round 1 image and, for each later round, its images before alignment, after rough and after fine alignment.
   - Enter the pixel size (µm/px). Click the landmarks on Round 1, then the same landmarks on every other image.
   - Results, p-values (Wilcoxon signed-rank, paired) and plots appear at the bottom; download them as SVG/PNG/Excel.
   - "Save landmarks (Excel)" keeps your points; "Open landmarks (Excel/CSV)" reopens them (try the two example files).
   - "Make figure" (under Results) builds a figure from the reference image and the images of one aligned round:
     the images with the landmarks (reference positions green, aligned positions magenta), the distance plot and
     close-ups of every landmark. Download it as SVG (text stays editable, Arial) or PNG.

3. Same analysis from the command line (Python 3 with numpy, pandas, scipy, matplotlib, openpyxl):
     python ramfish_landmark_error.py example_landmarks_wing1.xlsx --out results_wing1
     python ramfish_landmark_error.py all_wings.xlsx --group-by specimen --out results_all
   One Excel file with a "specimen" column can hold all wings/larvae; --group-by specimen then tests across specimens.
