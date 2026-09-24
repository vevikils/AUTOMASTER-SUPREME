import os
import time
import torch
import torch.nn.functional as F
import safetensors.torch
from diffusers import WanPipeline
from diffusers.models import WanTransformer3DModel

p = r"D:\WanVideoStudio_Models\lightx2v--Wan2.1-Distill-Models\wan2.1_t2v_14b_scaled_fp8_e4m3_lightx2v_4step_comfyui.safetensors"
base = r"D:\WanVideoStudio_Models\Wan-AI--Wan2.1-T2V-1.3B-Diffusers"

print("1. Extracting scales from safetensors...")
with safetensors.safe_open(p, framework="pt") as f:
    raw_scales = {k: f.get_tensor(k) for k in f.keys() if "scale_weight" in k}

def comfy_to_diffusers(name):
    name = name.replace(".self_attn.", ".attn1.").replace(".cross_attn.", ".attn2.")
    name = name.replace(".q.", ".to_q.").replace(".k.", ".to_k.").replace(".v.", ".to_v.").replace(".o.", ".to_out.0.")
    name = name.replace(".ffn.0.", ".ffn.net.0.proj.").replace(".ffn.2.", ".ffn.net.2.")
    name = name.replace(".scale_weight", "")
    return name

scales_map = {comfy_to_diffusers(k): v for k, v in raw_scales.items()}
print(f"Loaded {len(scales_map)} layer scales.")

print("2. Loading FP8 transformer...")
t0 = time.time()
transformer = WanTransformer3DModel.from_single_file(p, torch_dtype=torch.float8_e4m3fn)
print(f"Transformer loaded in {time.time()-t0:.2f}s")

print("3. Sanitizing non-FP8 parameters (norms, scale_shift, embeddings, biases)...")
# Rope and buffers in bfloat16
if hasattr(transformer, "rope"):
    transformer.rope = transformer.rope.to(torch.bfloat16)

for name, buf in transformer.named_buffers():
    if buf.dtype == torch.float8_e4m3fn:
        buf.data = buf.data.to(torch.bfloat16)

# Patch embedding and condition embedder in bfloat16
transformer.patch_embedding.to(torch.bfloat16)
transformer.condition_embedder.to(torch.bfloat16)

if hasattr(transformer, "norm_out") and transformer.norm_out is not None:
    transformer.norm_out.to(torch.float32)

for name, mod in transformer.named_modules():
    if "norm_q" in name or "norm_k" in name or "norm_added_k" in name:
        mod.to(torch.bfloat16)
    elif "norm" in name:
        mod.to(torch.float32)

for name, param in transformer.named_parameters():
    if "scale_shift_table" in name:
        param.data = param.data.to(torch.float32)
    elif "bias" in name and param.dtype == torch.float8_e4m3fn:
        param.data = param.data.to(torch.bfloat16)

wrapped_count = 0
for name, module in transformer.named_modules():
    if isinstance(module, torch.nn.Linear) and name in scales_map:
        sc = scales_map[name]
        def make_forward(mod, scale_val):
            def custom_forward(x):
                w = mod.weight
                if w.dtype == torch.float8_e4m3fn:
                    w = w.to(x.dtype) * scale_val.to(device=w.device, dtype=x.dtype)
                bias = mod.bias.to(x.dtype) if mod.bias is not None else None
                return F.linear(x, w, bias)
            return custom_forward
        module.forward = make_forward(module, sc)
        wrapped_count += 1

print(f"Wrapped {wrapped_count} FP8 linear layers with dynamic dequantization!")

print("4. Assembling WanPipeline...")
t0 = time.time()
pipe = WanPipeline.from_pretrained(base, transformer=transformer, torch_dtype=torch.bfloat16)
print(f"Pipeline assembled in {time.time()-t0:.2f}s")

print("5. Enabling sequential cpu offload...")
pipe.enable_sequential_cpu_offload()

if hasattr(pipe, "vae") and pipe.vae is not None:
    if hasattr(pipe.vae, "enable_tiling"):
        pipe.vae.enable_tiling()
    if hasattr(pipe.vae, "enable_slicing"):
        pipe.vae.enable_slicing()

print("6. Testing 1-step inference...")
def step_cb(pipeline, step_index, timestep, callback_kwargs):
    vram = torch.cuda.memory_allocated() / (1024**3)
    peak_vram = torch.cuda.max_memory_allocated() / (1024**3)
    print(f"--> Step {step_index} finished! Current VRAM: {vram:.2f} GB | Peak VRAM: {peak_vram:.2f} GB")
    return callback_kwargs

t0 = time.time()
out = pipe(
    prompt="A cinematic shot of a majestic lion in the savanna at sunset",
    num_frames=9,
    width=480,
    height=480,
    num_inference_steps=1,
    guidance_scale=1.0,
    callback_on_step_end=step_cb
)
print(f"Inference 1-step done in {time.time()-t0:.2f}s!")
print(f"Frames rendered: {len(out.frames[0])}")
print("ALL TESTS PASSED SUCCESSFULLY!")
