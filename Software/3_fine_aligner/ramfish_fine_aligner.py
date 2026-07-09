import io
import os
import cv2
import uuid
import numpy as np
import SimpleITK as sitk
import webbrowser
from threading import Timer
from fastapi import FastAPI, UploadFile, File, Request, BackgroundTasks
from fastapi.responses import JSONResponse, HTMLResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
import uvicorn

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# Set up local output directory
OUTPUT_DIR = "aligned_outputs"
os.makedirs(OUTPUT_DIR, exist_ok=True)
app.mount("/outputs", StaticFiles(directory=OUTPUT_DIR), name="outputs")

# Global dictionary to store task progress
tasks = {}

# ==========================================
# FRONTEND: HTML + JS UI
# ==========================================
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Multiplex Deformable Image Aligner</title>
  <script src="https://cdn.tailwindcss.com"></script>
</head>
<body class="bg-gradient-to-br from-purple-100 via-pink-50 to-blue-100 text-gray-800 min-h-screen flex flex-col">

  <main class="flex-grow max-w-7xl w-full mx-auto p-4 md:p-8 flex flex-col gap-6">
    <div class="text-center md:text-left mt-4">
        <h1 class="text-3xl md:text-4xl font-bold text-gray-900 mb-2">Multiplex Deformable Tissue Aligner</h1>
        <p class="text-gray-700 max-w-2xl text-sm md:text-base">
            Perform sequential B-Spline registration across multiple FISH cycles. 
        </p>
    </div>

    <div class="bg-white bg-opacity-90 rounded-2xl shadow-xl overflow-hidden flex flex-col md:flex-row border border-purple-100 flex-grow min-h-[600px]">
        
        <div class="w-full md:w-1/3 bg-gray-900 text-white p-6 md:p-8 flex flex-col gap-5 overflow-y-auto max-h-[800px]">
            <div class="flex items-center justify-between bg-gray-800 p-3 rounded-xl border border-gray-700">
                <label class="text-sm font-semibold text-white">Total FISH Rounds:</label>
                <input type="number" id="roundCount" min="2" max="20" value="10" 
                       class="w-16 bg-gray-700 text-white text-center rounded-md border-gray-600 focus:ring-purple-500 py-1">
            </div>

            <form id="multiplexForm" class="flex flex-col gap-5">
                <div id="dynamicInputs" class="flex flex-col gap-5"></div>

                <button type="submit" id="submitBtn" class="w-full bg-purple-600 hover:bg-purple-700 text-white font-bold py-3 px-4 rounded-xl transition duration-200 shadow-md mt-4">
                    Run Batch Alignment
                </button>
            </form>
        </div>

        <div class="w-full md:w-2/3 p-4 md:p-8 flex flex-col bg-gray-50/50 relative overflow-y-auto">
            
            <div id="loading" class="hidden absolute inset-0 bg-white/95 z-10 flex-col items-center justify-center gap-4 px-10">
                <div class="w-14 h-14 border-4 border-purple-600 border-t-transparent rounded-full animate-spin"></div>
                
                <div class="w-full max-w-md text-center">
                    <p id="progressMessage" class="text-gray-900 font-bold text-lg mb-2">Uploading files...</p>
                    <div class="w-full bg-gray-200 rounded-full h-3 mb-2 shadow-inner">
                        <div id="progressBar" class="bg-purple-600 h-3 rounded-full transition-all duration-300" style="width: 0%"></div>
                    </div>
                    <p id="progressPercentage" class="text-purple-700 font-semibold text-sm">0%</p>
                </div>
            </div>
            
            <div id="previewGallery" class="hidden w-full grid grid-cols-1 md:grid-cols-2 gap-4"></div>

            <div id="placeholderText" class="text-center p-8 mt-20">
                <div class="mx-auto h-16 w-16 text-purple-300 mb-4 bg-purple-50 rounded-full flex items-center justify-center">
                    <svg class="h-8 w-8" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z" />
                    </svg>
                </div>
                <p class="text-gray-700 font-semibold text-lg">Awaiting Image Sequence</p>
                <p class="text-gray-500 text-xs mt-1">Files are saved locally on the server upon completion.</p>
            </div>
        </div>
    </div>
  </main>

  <script>
    const roundCountInput = document.getElementById('roundCount');
    const dynamicInputsContainer = document.getElementById('dynamicInputs');

    function buildUI() {
        const rounds = parseInt(roundCountInput.value);
        let html = `
            <div class="bg-gray-800/60 p-4 rounded-xl border border-purple-500/50">
                <label class="block text-sm font-semibold text-purple-400 mb-1">Round 1 (Global Fixed Reference)</label>
                <p class="text-xs text-gray-400 mb-3">Ground truth structural anchor (e.g., DAPI). Required.</p>
                <input type="file" id="fixed_ref" name="fixed_ref" accept="image/*" required
                       class="block w-full text-xs text-gray-400 file:mr-3 file:py-1.5 file:px-3 file:rounded-md file:border-0 file:text-xs file:font-semibold file:bg-purple-600 file:text-white">
            </div>
        `;

        for(let i = 2; i <= rounds; i++) {
            html += `
                <div class="bg-gray-800/60 p-4 rounded-xl border border-gray-700">
                    <div class="flex justify-between items-baseline mb-1">
                        <label class="block text-sm font-semibold text-pink-400">Round ${i} Alignment</label>
                        <span class="text-[10px] text-gray-500 uppercase tracking-wider">Optional</span>
                    </div>
                    <div class="mb-4">
                        <p class="text-xs text-gray-300 font-medium mb-1">Structural Reference (Moving DAPI)</p>
                        <input type="file" name="round_${i}_ref" accept="image/*"
                               class="block w-full text-xs text-gray-400 file:mr-3 file:py-1 file:px-2 file:rounded file:bg-gray-700 file:text-white">
                    </div>
                    <div>
                        <p class="text-xs text-gray-300 font-medium mb-1">Target Genes (Select Multiple)</p>
                        <input type="file" name="round_${i}_targets" accept="image/*" multiple
                               class="block w-full text-xs text-gray-400 file:mr-3 file:py-1 file:px-2 file:rounded file:bg-pink-600 file:text-white">
                    </div>
                </div>
            `;
        }
        dynamicInputsContainer.innerHTML = html;
    }

    roundCountInput.addEventListener('change', buildUI);
    buildUI();

    document.getElementById('multiplexForm').addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const formElement = document.getElementById('multiplexForm');
        const formData = new FormData(formElement);
        formData.append('total_rounds', roundCountInput.value);

        // Reset and show loader
        document.getElementById('submitBtn').disabled = true;
        document.getElementById('progressBar').style.width = "0%";
        document.getElementById('progressPercentage').innerText = "0%";
        document.getElementById('progressMessage').innerText = "Uploading to local server...";
        
        document.getElementById('loading').classList.remove('hidden');
        document.getElementById('loading').classList.add('flex');
        document.getElementById('placeholderText').classList.add('hidden');
        document.getElementById('previewGallery').classList.add('hidden');
        document.getElementById('previewGallery').innerHTML = '';

        try {
            // Initiate the Job (Note the relative URL)
            const startResponse = await fetch('/align_multiplex', {
                method: 'POST',
                body: formData
            });

            if (!startResponse.ok) {
                const err = await startResponse.json();
                throw new Error(err.error || "Failed to start alignment task.");
            }

            const { task_id } = await startResponse.json();

            // Poll for progress every 800ms
            const pollInterval = setInterval(async () => {
                try {
                    const statusRes = await fetch(`/status/${task_id}`);
                    const statusData = await statusRes.json();

                    // Update UI
                    document.getElementById('progressBar').style.width = `${statusData.progress}%`;
                    document.getElementById('progressPercentage').innerText = `${statusData.progress}%`;
                    document.getElementById('progressMessage').innerText = statusData.message;

                    // Check for completion
                    if (statusData.status === 'completed') {
                        clearInterval(pollInterval);
                        renderGallery(statusData.files);
                    } else if (statusData.status === 'error') {
                        clearInterval(pollInterval);
                        throw new Error(statusData.error);
                    }
                } catch (err) {
                    clearInterval(pollInterval);
                    handleError(err);
                }
            }, 800);

        } catch (error) {
            handleError(error);
        }
    });

    function renderGallery(files) {
        if (!files || files.length === 0) {
            document.getElementById('previewGallery').innerHTML = `<p class="col-span-full text-sm text-yellow-600 font-medium mb-4">Processing finished, but no target files were aligned.</p>`;
        } else {
            let galleryHtml = `<h2 class="col-span-full text-xl font-bold text-gray-800 mb-2">Alignment Complete</h2>`;
            files.forEach(file => {
                galleryHtml += `
                    <div class="bg-white p-2 rounded-lg shadow border border-gray-200">
                        <p class="text-xs font-bold text-gray-700 mb-2 truncate" title="${file.filename}">${file.filename}</p>
                        <img src="${file.url}" class="w-full h-48 object-cover bg-black rounded" alt="${file.filename}">
                        <a href="${file.url}" download="${file.filename}" class="mt-2 block text-center text-xs bg-gray-100 hover:bg-gray-200 text-gray-800 py-1 rounded">Download</a>
                    </div>
                `;
            });
            document.getElementById('previewGallery').innerHTML = galleryHtml;
        }
        
        document.getElementById('previewGallery').classList.remove('hidden');
        resetUI();
    }

    function handleError(error) {
        console.error(error);
        alert("Error:\\n" + error.message);
        document.getElementById('placeholderText').classList.remove('hidden');
        resetUI();
    }

    function resetUI() {
        document.getElementById('loading').classList.add('hidden');
        document.getElementById('loading').classList.remove('flex');
        document.getElementById('submitBtn').disabled = false;
    }
  </script>
</body>
</html>
"""

# ==========================================
# BACKEND: IMAGE PROCESSING LOGIC
# ==========================================

def read_image_bytes(file_bytes: bytes) -> np.ndarray:
    nparr = np.frombuffer(file_bytes, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH)
    if img is None:
        raise ValueError("File could not be decoded. Ensure it is a valid 2D image.")
    return img

def match_fixed_shape_centered(moving_img: np.ndarray, fixed_shape: tuple) -> np.ndarray:
    fixed_h, fixed_w = fixed_shape
    moving_h, moving_w = moving_img.shape
    if (fixed_h, fixed_w) == (moving_h, moving_w):
        return moving_img
    canvas = np.zeros((fixed_h, fixed_w), dtype=moving_img.dtype)
    start_y_canvas = max(0, (fixed_h - moving_h) // 2)
    start_x_canvas = max(0, (fixed_w - moving_w) // 2)
    start_y_moving = max(0, (moving_h - fixed_h) // 2)
    start_x_moving = max(0, (moving_w - fixed_w) // 2)
    copy_h = min(fixed_h, moving_h)
    copy_w = min(fixed_w, moving_w)
    canvas[start_y_canvas:start_y_canvas+copy_h, start_x_canvas:start_x_canvas+copy_w] = \
        moving_img[start_y_moving:start_y_moving+copy_h, start_x_moving:start_x_moving+copy_w]
    return canvas

def compute_transforms(fixed_ref_np, moving_ref_np):
    fixed_image = sitk.GetImageFromArray(fixed_ref_np.astype(np.float32))
    moving_image = sitk.GetImageFromArray(moving_ref_np.astype(np.float32))

    fixed_image_reg = sitk.Normalize(fixed_image)
    moving_image_reg = sitk.Normalize(moving_image)

    blur_filter = sitk.DiscreteGaussianImageFilter()
    blur_filter.SetVariance(4.0)
    fixed_image_reg = blur_filter.Execute(fixed_image_reg)
    moving_image_reg = blur_filter.Execute(moving_image_reg)

    affine_reg = sitk.ImageRegistrationMethod()
    affine_reg.SetMetricAsMattesMutualInformation(numberOfHistogramBins=50)
    affine_reg.SetMetricSamplingStrategy(affine_reg.RANDOM)
    affine_reg.SetMetricSamplingPercentage(0.1)
    affine_reg.SetInterpolator(sitk.sitkLinear)
    affine_reg.SetOptimizerAsGradientDescent(learningRate=1.0, numberOfIterations=100)
    affine_reg.SetOptimizerScalesFromPhysicalShift()
    initial_affine = sitk.CenteredTransformInitializer(
        fixed_image_reg, moving_image_reg, sitk.AffineTransform(2), 
        sitk.CenteredTransformInitializerFilter.GEOMETRY
    )
    affine_reg.SetInitialTransform(initial_affine, inPlace=False)
    affine_transform = affine_reg.Execute(fixed_image_reg, moving_image_reg)

    affine_resampler = sitk.ResampleImageFilter()
    affine_resampler.SetReferenceImage(fixed_image_reg)
    affine_resampler.SetInterpolator(sitk.sitkLinear)
    affine_resampler.SetDefaultPixelValue(0)
    affine_resampler.SetTransform(affine_transform)
    moving_image_affine = affine_resampler.Execute(moving_image_reg)

    bspline_reg = sitk.ImageRegistrationMethod()
    bspline_reg.SetMetricAsMattesMutualInformation(numberOfHistogramBins=50)
    bspline_reg.SetMetricSamplingStrategy(bspline_reg.RANDOM)
    bspline_reg.SetMetricSamplingPercentage(0.1)
    bspline_reg.SetOptimizerAsLBFGSB(gradientConvergenceTolerance=1e-5, numberOfIterations=50)
    bspline_reg.SetInterpolator(sitk.sitkLinear)
    initial_bspline = sitk.BSplineTransformInitializer(fixed_image_reg, [8, 8])
    bspline_reg.SetInitialTransform(initial_bspline, inPlace=False)
    bspline_reg.SetShrinkFactorsPerLevel(shrinkFactors=[4, 2, 1])
    bspline_reg.SetSmoothingSigmasPerLevel(smoothingSigmas=[2, 1, 0])
    bspline_reg.SmoothingSigmasAreSpecifiedInPhysicalUnitsOn()
    bspline_transform = bspline_reg.Execute(fixed_image_reg, moving_image_affine)

    return affine_transform, bspline_transform, fixed_image_reg

def apply_transforms(affine_transform, bspline_transform, fixed_image_ref, moving_target_np):
    moving_target = sitk.GetImageFromArray(moving_target_np.astype(np.float32))

    affine_resampler = sitk.ResampleImageFilter()
    affine_resampler.SetReferenceImage(fixed_image_ref)
    affine_resampler.SetInterpolator(sitk.sitkLinear)
    affine_resampler.SetDefaultPixelValue(0)
    affine_resampler.SetTransform(affine_transform)
    moving_target_affine = affine_resampler.Execute(moving_target)

    bspline_resampler = sitk.ResampleImageFilter()
    bspline_resampler.SetReferenceImage(fixed_image_ref)
    bspline_resampler.SetInterpolator(sitk.sitkLinear)
    bspline_resampler.SetDefaultPixelValue(0)
    bspline_resampler.SetTransform(bspline_transform)

    aligned_target_sitk = bspline_resampler.Execute(moving_target_affine)
    return sitk.GetArrayFromImage(aligned_target_sitk).astype(moving_target_np.dtype)

# --- BACKGROUND WORKER ---
def process_alignment_task(task_id: str, parsed_data: dict):
    try:
        tasks[task_id]["message"] = "Initializing images in memory..."
        fixed_ref_np = read_image_bytes(parsed_data["fixed_ref"])
        
        rounds_data = parsed_data["rounds"]
        total_steps = sum(1 + len(rnd["targets"]) for rnd in rounds_data)
        
        if total_steps == 0:
            tasks[task_id]["progress"] = 100
            tasks[task_id]["status"] = "completed"
            tasks[task_id]["message"] = "No valid rounds submitted."
            return

        current_step = 0
        results = []

        for rnd in rounds_data:
            r = rnd["round"]
            tasks[task_id]["message"] = f"Computing B-Spline Transform for Round {r}..."
            
            moving_ref_raw = read_image_bytes(rnd["ref"])
            moving_ref_np = match_fixed_shape_centered(moving_ref_raw, fixed_ref_np.shape)

            # Heavy computation
            affine_tx, bspline_tx, fixed_img_sitk = compute_transforms(fixed_ref_np, moving_ref_np)
            
            current_step += 1
            tasks[task_id]["progress"] = int((current_step / total_steps) * 90)

            for tgt in rnd["targets"]:
                filename = tgt["filename"]
                tasks[task_id]["message"] = f"Applying warp to {filename} (Round {r})..."
                
                target_raw = read_image_bytes(tgt["bytes"])
                target_np = match_fixed_shape_centered(target_raw, fixed_ref_np.shape)
                
                aligned_np = apply_transforms(affine_tx, bspline_tx, fixed_img_sitk, target_np)
                
                safe_filename = f"R{r}_aligned_{filename}"
                save_path = os.path.join(OUTPUT_DIR, safe_filename)
                cv2.imwrite(save_path, aligned_np)
                
                results.append({
                    "round": r,
                    "filename": safe_filename,
                    "url": f"/outputs/{safe_filename}"  # Adjusted to relative path
                })
                
                current_step += 1
                tasks[task_id]["progress"] = int((current_step / total_steps) * 90)

        tasks[task_id]["progress"] = 100
        tasks[task_id]["message"] = "Alignment completely successfully!"
        tasks[task_id]["files"] = results
        tasks[task_id]["status"] = "completed"

    except Exception as e:
        print(f"TASK ERROR: {str(e)}")
        tasks[task_id]["status"] = "error"
        tasks[task_id]["error"] = str(e)


# ==========================================
# API ENDPOINTS
# ==========================================

@app.get("/", response_class=HTMLResponse)
async def serve_frontend():
    """Serves the main HTML UI."""
    return HTMLResponse(content=HTML_TEMPLATE)

@app.post("/align_multiplex")
async def align_multiplex(request: Request, background_tasks: BackgroundTasks):
    try:
        form = await request.form()
        task_id = str(uuid.uuid4())
        
        fixed_ref_file = form.get("fixed_ref")
        if not fixed_ref_file or not getattr(fixed_ref_file, 'filename', None):
            return JSONResponse(status_code=400, content={"error": "Missing Round 1 Fixed Reference."})
            
        total_rounds = int(form.get("total_rounds", 1))
        
        # Package data to send to the thread
        parsed_data = {
            "fixed_ref": await fixed_ref_file.read(),
            "rounds": []
        }

        for r in range(2, total_rounds + 1):
            moving_ref_file = form.get(f"round_{r}_ref")
            target_files = form.getlist(f"round_{r}_targets")
            
            if not moving_ref_file or not getattr(moving_ref_file, 'filename', None):
                continue
                
            valid_targets = [t for t in target_files if getattr(t, 'filename', None)]
            if not valid_targets:
                continue

            parsed_data["rounds"].append({
                "round": r,
                "ref": await moving_ref_file.read(),
                "targets": [{"filename": t.filename, "bytes": await t.read()} for t in valid_targets]
            })

        tasks[task_id] = {
            "status": "processing",
            "progress": 0,
            "message": "Files uploaded successfully. Queuing job...",
            "files": [],
            "error": None
        }

        background_tasks.add_task(process_alignment_task, task_id, parsed_data)

        return {"task_id": task_id}

    except Exception as e:
        print(f"CRITICAL ERROR: {str(e)}")
        return JSONResponse(status_code=500, content={"error": str(e)})

@app.get("/status/{task_id}")
async def get_status(task_id: str):
    if task_id not in tasks:
        return JSONResponse(status_code=404, content={"error": "Task not found"})
    return tasks[task_id]


def open_browser():
    """Opens the local server in the default web browser."""
    webbrowser.open_new("http://127.0.0.1:8001")


if __name__ == "__main__":
    print("=====================================================")
    print("  Starting RAM-FISH Multiplex Server...              ")
    print("  Opening browser automatically...                   ")
    print("=====================================================")
    
    # Trigger the browser to open after 1.5 seconds
    Timer(1.5, open_browser).start()
    
    uvicorn.run(app, host="127.0.0.1", port=8001)