import os
import cv2
import numpy as np
import tifffile as tiff
import webbrowser
import base64
from threading import Timer
from pathlib import Path
from flask import Flask, request, jsonify, render_template_string

# For native local file dialogs
try:
    import tkinter as tk
    from tkinter import filedialog
except ImportError:
    tk = None

app = Flask(__name__)

# ==========================================
# BACKEND: API AND IMAGE PROCESSING
# ==========================================

@app.route('/api/browse', methods=['POST'])
def api_browse():
    """Opens a native OS dialog to select a file or directory, bypassing browser security limits."""
    if not tk:
        return jsonify({"status": "error", "message": "Tkinter is not available on this system. Please paste the path manually."}), 500
        
    mode = request.json.get('mode', 'file')
    
    try:
        root = tk.Tk()
        root.attributes('-topmost', True)
        root.withdraw()
        
        if mode == 'dir':
            path = filedialog.askdirectory(title="Select Target Folder")
        else:
            path = filedialog.askopenfilename(title="Select Image File", filetypes=[("Images", "*.tif *.tiff *.jpg *.jpeg *.png *.bmp")])
            
        root.destroy()
        
        if path:
            return jsonify({"status": "success", "path": path})
        else:
            return jsonify({"status": "cancelled"})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500


@app.route('/api/preview', methods=['POST'])
def api_preview():
    """Loads a specific image path, normalizes it, and sends a compressed base64 JPEG to the UI."""
    data = request.json
    filepath = Path(data['filepath'])
    
    if not filepath.exists() or not filepath.is_file():
        return jsonify({"status": "error", "message": f"File not found: {filepath}"}), 404
    
    try:
        if filepath.suffix.lower() in ['.tif', '.tiff']:
            img = tiff.imread(str(filepath))
        else:
            img = cv2.imread(str(filepath), cv2.IMREAD_UNCHANGED)
            
        if img is None:
            raise ValueError("Could not decode image data.")

        h, w = img.shape[:2]

        # Convert to 8-bit for web display
        if img.dtype == np.uint16 or img.dtype == np.float32 or img.dtype == np.float64:
            img_8u = cv2.normalize(img, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
        else:
            img_8u = img.copy()

        # Handle channels
        if img_8u.ndim == 3 and img_8u.shape[2] > 3:
            img_8u = img_8u[:, :, :3]  # Strip extra channels for UI preview
        elif img_8u.ndim == 2:
            img_8u = cv2.cvtColor(img_8u, cv2.COLOR_GRAY2BGR)

        # Scale down for browser performance (Max 1000px dimension)
        max_dim = 1000
        if max(h, w) > max_dim:
            scale = max_dim / max(h, w)
            new_w, new_h = int(w * scale), int(h * scale)
            img_resized = cv2.resize(img_8u, (new_w, new_h), interpolation=cv2.INTER_AREA)
        else:
            img_resized = img_8u

        _, buffer = cv2.imencode('.jpg', img_resized, [int(cv2.IMWRITE_JPEG_QUALITY), 85])
        b64_str = base64.b64encode(buffer).decode('utf-8')
        
        return jsonify({
            "status": "success",
            "src": f"data:image/jpeg;base64,{b64_str}",
            "orig_w": int(w),
            "orig_h": int(h),
            "prev_w": img_resized.shape[1],
            "prev_h": img_resized.shape[0]
        })
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500


@app.route('/api/align_folder', methods=['POST'])
def api_align_folder():
    """Applies the affine transform to ALL images inside the designated folder."""
    data = request.json
    dir_path = Path(data['folder'])
    
    if not dir_path.exists() or not dir_path.is_dir():
        return jsonify({"status": "error", "message": f"Target folder not found: {dir_path}"}), 404
    
    tx_ui = float(data.get('tx', 0))
    ty_ui = float(data.get('ty', 0))
    rot = float(data.get('rotation', 0))
    flip = bool(data.get('flip', False))
    img_scale = float(data.get('img_scale', 1.0))
    scale_ratio = float(data.get('scale_ratio', 1.0))
    
    # Convert UI pixels to Native Image pixels based on scale ratio
    real_tx = tx_ui * scale_ratio
    real_ty = ty_ui * scale_ratio
    
    valid_exts = {'.tif', '.tiff', '.jpg', '.jpeg', '.png', '.bmp'}
    processed = []
    
    try:
        # Process ALL valid images in the folder
        for filepath in dir_path.iterdir():
            if filepath.is_file() and filepath.suffix.lower() in valid_exts:
                if not filepath.name.startswith("ALIGNED_"):
                    
                    if filepath.suffix.lower() in ['.tif', '.tiff']:
                        img = tiff.imread(str(filepath))
                    else:
                        img = cv2.imread(str(filepath), cv2.IMREAD_UNCHANGED)
                        
                    if img is None:
                        continue
                        
                    h, w = img.shape[:2]
                    
                    if flip:
                        img = cv2.flip(img, 1)
                        
                    # Negative rot to mathematically match CSS clockwise rotation
                    # Replaced 1.0 with img_scale to apply the scale transformation
                    M = cv2.getRotationMatrix2D((w / 2, h / 2), -rot, img_scale)
                    M[0, 2] += real_tx
                    M[1, 2] += real_ty
                    
                    # Handle multi-channel TIFFs properly
                    if img.ndim == 3 and img.shape[2] > 4:
                        aligned = np.zeros_like(img)
                        for c in range(img.shape[2]):
                            aligned[:,:,c] = cv2.warpAffine(img[:,:,c], M, (w, h), flags=cv2.INTER_LINEAR)
                    else:
                        aligned = cv2.warpAffine(img, M, (w, h), flags=cv2.INTER_LINEAR)
                        
                    out_path = dir_path / f"ALIGNED_{filepath.name}"
                    
                    # Save in exact original format
                    if filepath.suffix.lower() in ['.tif', '.tiff']:
                        tiff.imwrite(str(out_path), aligned)
                    else:
                        cv2.imwrite(str(out_path), aligned)
                        
                    processed.append(str(out_path.name))
                    
        return jsonify({
            "status": "success",
            "message": f"Successfully aligned {len(processed)} images directly in {dir_path.name}!",
            "files": processed
        })
        
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500


# ==========================================
# FRONTEND: HTML + JS UI 
# ==========================================
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>RAM-FISH Fullstack Aligner</title>
    <style>
        body { margin: 0; padding: 0; display: flex; height: 100vh; font-family: sans-serif; background-color: #1e1e1e; color: #fff; overflow: hidden; }
        #controls { width: 380px; background-color: #2d2d2d; padding: 20px; box-shadow: 2px 0 5px rgba(0,0,0,0.5); overflow-y: auto; z-index: 10; display: flex; flex-direction: column; gap: 10px;}
        #viewport { flex-grow: 1; overflow: hidden; background-color: #000; position: relative; cursor: grab; display: flex; align-items: center; justify-content: center;}
        #viewport:active { cursor: grabbing; }
        
        #canvas-wrapper { position: relative; transform-origin: center center; }
        .img-layer { position: absolute; top: 0; left: 0; transform-origin: center center; width: 100%; height: 100%; pointer-events: none;}
        #ref-img { z-index: 1; }
        #target-img { z-index: 2; opacity: 0.5; }
        
        h2 { margin: 0; border-bottom: 1px solid #555; padding-bottom: 10px; color: #4fc3f7; font-size: 1.2rem;}
        label { display: block; margin-bottom: 3px; font-size: 0.85rem; color: #aaa; font-weight: bold;}
        input[type="text"] { width: 100%; padding: 7px; background: #111; color: white; border: 1px solid #444; box-sizing: border-box; margin-bottom: 5px; border-radius: 3px; font-size: 0.8rem;}
        
        .input-group { display: flex; gap: 5px; margin-bottom: 10px;}
        .input-group button { width: auto; padding: 7px 10px; flex-shrink: 0;}
        
        .slider-container { display: flex; align-items: center; justify-content: space-between; margin-bottom: 10px;}
        input[type="range"] { flex-grow: 1; margin-right: 10px; }
        .num-input { width: 60px; text-align: right; background: #111; color: white; border: 1px solid #444; padding: 4px; border-radius: 3px;}
        
        button { width: 100%; padding: 10px; background-color: #4CAF50; color: white; border: none; cursor: pointer; font-weight: bold; border-radius: 4px; transition: 0.2s;}
        button:hover { background-color: #45a049; }
        button:disabled { background-color: #555; cursor: not-allowed; color: #888;}
        .btn-blue { background-color: #2196F3; }
        .btn-blue:hover { background-color: #1976D2; }
        .btn-gray { background-color: #666; }
        .btn-gray:hover { background-color: #555; }
        
        #status-box { padding: 10px; background: #111; border: 1px solid #444; font-family: monospace; font-size: 0.8rem; display: none; margin-top: 10px;}
        .loader { display: none; text-align: center; font-size: 0.9rem; color: #4fc3f7; margin-top: 5px;}
    </style>
</head>
<body>

    <div id="controls">
        <h2>1. Visual Setup</h2>
        
        <label>Reference DAPI (e.g. Round 1)</label>
        <div class="input-group">
            <input type="text" id="ref-path" placeholder="Path to reference DAPI...">
            <button class="btn-gray browse-btn" data-target="ref-path" data-mode="file">Browse</button>
        </div>
        
        <label>Target / Moving DAPI (e.g. Round 2)</label>
        <div class="input-group">
            <input type="text" id="target-path" placeholder="Path to target DAPI...">
            <button class="btn-gray browse-btn" data-target="target-path" data-mode="file">Browse</button>
        </div>
        
        <button class="btn-blue" id="load-preview-btn">Load Images into UI</button>
        <div id="preview-loader" class="loader">Generating previews...</div>

        <h2 style="margin-top:15px;">2. Alignment Tools</h2>
        <div><label>Target Opacity</label><div class="slider-container"><input type="range" id="opacity" min="0" max="1" step="0.05" value="0.5"><input type="number" id="op-num" class="num-input" value="0.5" step="0.05"></div></div>
        <div><label>Image Scale</label><div class="slider-container"><input type="range" id="img-scale" min="0.1" max="5.0" step="0.01" value="1.0"><input type="number" id="scale-num" class="num-input" value="1.0" step="0.01"></div></div>
        <div><label>Rotation (Deg)</label><div class="slider-container"><input type="range" id="rotation" min="-180" max="180" step="0.05" value="0"><input type="number" id="rot-num" class="num-input" value="0" step="0.05"></div></div>
        <div><label>X Translate</label><div class="slider-container"><input type="range" id="x-trans" min="-1000" max="1000" step="0.5" value="0"><input type="number" id="x-num" class="num-input" value="0" step="0.5"></div></div>
        <div><label>Y Translate</label><div class="slider-container"><input type="range" id="y-trans" min="-1000" max="1000" step="0.5" value="0"><input type="number" id="y-num" class="num-input" value="0" step="0.5"></div></div>
        <div style="display:flex; align-items:center; margin-bottom: 15px;"><input type="checkbox" id="flip-h" style="margin-right:8px;"><label for="flip-h" style="margin:0;">Flip Horizontal</label></div>

        <button id="process-btn" disabled>Apply Rough Alignment to Folder</button>
        <div id="status-box"></div>
    </div>

    <div id="viewport">
        <div id="canvas-wrapper">
            <img id="ref-img" class="img-layer">
            <img id="target-img" class="img-layer">
        </div>
    </div>

    <script>
        let currentScaleRatio = 1.0; 
        let currentTargetFolder = ""; // Dynamically holds target processing folder context
        
        // --- Helper: Extract directory path from target DAPI file path ---
        function updateTargetFolder(filePath) {
            if (!filePath) return;
            const lastSlash = Math.max(filePath.lastIndexOf('/'), filePath.lastIndexOf('\\\\'));
            if (lastSlash !== -1) {
                currentTargetFolder = filePath.substring(0, lastSlash);
            } else {
                currentTargetFolder = ""; 
            }
        }

        // --- Native OS Browse Buttons ---
        document.querySelectorAll('.browse-btn').forEach(btn => {
            btn.addEventListener('click', async () => {
                const targetInput = document.getElementById(btn.dataset.target);
                const mode = btn.dataset.mode;
                
                try {
                    const res = await fetch('/api/browse', { 
                        method: 'POST', 
                        headers: {'Content-Type': 'application/json'}, 
                        body: JSON.stringify({mode}) 
                    });
                    const data = await res.json();
                    
                    if (data.status === 'success') {
                        targetInput.value = data.path;
                        // Dynamically update folder destination if target DAPI path changes
                        if (btn.dataset.target === 'target-path') {
                            updateTargetFolder(data.path);
                        }
                    } else if (data.status === 'error') {
                        alert(data.message);
                    }
                } catch(e) {
                    alert("Error triggering file browser. You can paste the path manually.");
                }
            });
        });

        // Listen for manual keystrokes/pastes to target path input field
        document.getElementById('target-path').addEventListener('input', (e) => {
            updateTargetFolder(e.target.value);
        });

        // --- Load Previews ---
        document.getElementById('load-preview-btn').addEventListener('click', async () => {
            const refPath = document.getElementById('ref-path').value;
            const tgtPath = document.getElementById('target-path').value;
            
            if(!refPath || !tgtPath) { alert("Please provide paths for both Reference and Target DAPI."); return; }
            
            document.getElementById('preview-loader').style.display = 'block';
            document.getElementById('process-btn').disabled = true;

            try {
                // Fetch Ref
                let res1 = await fetch('/api/preview', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({filepath: refPath}) });
                let refData = await res1.json();
                
                // Fetch Target
                let res2 = await fetch('/api/preview', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({filepath: tgtPath}) });
                let tgtData = await res2.json();

                if (refData.status === 'success' && tgtData.status === 'success') {
                    const wrapper = document.getElementById('canvas-wrapper');
                    wrapper.style.width = tgtData.prev_w + 'px';
                    wrapper.style.height = tgtData.prev_h + 'px';
                    
                    document.getElementById('ref-img').src = refData.src;
                    document.getElementById('target-img').src = tgtData.src;
                    
                    // Critical Math: Store the ratio of Native / UI pixels
                    currentScaleRatio = tgtData.orig_w / tgtData.prev_w;
                    
                    // Ensure tracking variable matches latest target image folder state
                    updateTargetFolder(tgtPath);
                    
                    scale = 1; panX = 0; panY = 0;
                    updateViewport();
                    
                    document.getElementById('process-btn').disabled = false;
                } else {
                    alert("Error loading previews: " + (refData.message || tgtData.message));
                }
            } catch(e) {
                alert("Failed to fetch previews.");
            }
            document.getElementById('preview-loader').style.display = 'none';
        });

        // --- UI Sync & Transforms ---
        function syncInputs(rangeId, numId) {
            const r = document.getElementById(rangeId), n = document.getElementById(numId);
            r.addEventListener('input', () => { n.value = r.value; updateAlign(); });
            n.addEventListener('input', () => { r.value = n.value; updateAlign(); });
        }
        syncInputs('opacity', 'op-num');
        syncInputs('rotation', 'rot-num');
        syncInputs('x-trans', 'x-num');
        syncInputs('y-trans', 'y-num');
        syncInputs('img-scale', 'scale-num');
        document.getElementById('flip-h').addEventListener('change', updateAlign);

        function updateAlign() {
            const op = document.getElementById('opacity').value;
            const rot = document.getElementById('rotation').value;
            const x = document.getElementById('x-trans').value;
            const y = document.getElementById('y-trans').value;
            const imgScale = document.getElementById('img-scale').value;
            const flip = document.getElementById('flip-h').checked ? -1 : 1;

            const target = document.getElementById('target-img');
            target.style.opacity = op;
            // Apply flip and scale simultaneously to avoid overwriting properties
            target.style.transform = `translate(${x}px, ${y}px) rotate(${rot}deg) scale(${flip * imgScale}, ${imgScale})`;
        }

        // --- Viewport Panning/Zooming ---
        let scale = 1, panX = 0, panY = 0, dragging = false, sx, sy;
        const v = document.getElementById('viewport'), w = document.getElementById('canvas-wrapper');
        function updateViewport() { w.style.transform = `translate(${panX}px, ${panY}px) scale(${scale})`; }
        
        v.addEventListener('wheel', e => { e.preventDefault(); scale += e.deltaY * -0.001; scale = Math.max(0.1, scale); updateViewport(); });
        v.addEventListener('mousedown', e => { if(e.target.tagName !== 'INPUT' && e.target.tagName !== 'BUTTON') { dragging=true; sx=e.clientX-panX; sy=e.clientY-panY; }});
        v.addEventListener('mousemove', e => { if(dragging){ e.preventDefault(); panX=e.clientX-sx; panY=e.clientY-sy; updateViewport(); }});
        window.addEventListener('mouseup', () => dragging=false);

        // --- Batch Process Folder ---
        document.getElementById('process-btn').addEventListener('click', async () => {
            if(!currentTargetFolder) { 
                alert("Target folder could not be parsed. Please select a valid Target DAPI file path."); 
                return; 
            }
            
            const btn = document.getElementById('process-btn');
            const status = document.getElementById('status-box');
            
            btn.disabled = true;
            btn.innerText = "Processing Folder... Check Terminal";
            status.style.display = 'block';
            status.style.color = '#fff';
            status.innerText = "Aligning all images in folder...";

            const payload = {
                folder: currentTargetFolder,
                scale_ratio: currentScaleRatio,
                tx: document.getElementById('x-trans').value,
                ty: document.getElementById('y-trans').value,
                rotation: document.getElementById('rotation').value,
                img_scale: document.getElementById('img-scale').value,
                flip: document.getElementById('flip-h').checked
            };

            try {
                const res = await fetch('/api/align_folder', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(payload) });
                const result = await res.json();
                
                if (result.status === 'success') {
                    status.style.color = '#4CAF50';
                    status.innerText = result.message;
                } else {
                    status.style.color = '#ff5252';
                    status.innerText = "Error: " + result.message;
                }
            } catch (err) {
                status.style.color = '#ff5252';
                status.innerText = "Network Error.";
            }
            btn.disabled = false;
            btn.innerText = "Apply Rough Alignment to Folder";
        });
    </script>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

def open_browser():
    webbrowser.open_new('http://127.0.0.1:5000')

if __name__ == '__main__':
    print("=====================================================")
    print("  Starting RAM-FISH Aligner Server...                ")
    print("=====================================================")
    Timer(1.0, open_browser).start()
    app.run(host='127.0.0.1', port=5000, threaded=False)