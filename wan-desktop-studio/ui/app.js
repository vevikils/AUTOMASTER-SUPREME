/**
 * Wan 2.1 Video Studio - RTX 4060 Edition
 * Frontend Application Controller
 */

document.addEventListener('DOMContentLoaded', () => {
  // State
  let currentMode = 't2v'; // 't2v' or 'i2v'
  let currentWidth = 832;
  let currentHeight = 480;
  let currentSteps = 25;
  let uploadedImageUrl = null;
  let ws = null;
  let currentVideoUrl = null;

  // DOM Elements - Navigation
  const navTabs = document.querySelectorAll('.nav-tab');
  const tabContents = document.querySelectorAll('.tab-content');

  // DOM Elements - Telemetry HUD
  const hudVram = document.getElementById('hud-vram');
  const hudVramBar = document.getElementById('hud-vram-bar');
  const hudTemp = document.getElementById('hud-temp');
  const hudRam = document.getElementById('hud-ram');
  const hudDiskD = document.getElementById('hud-disk-d');

  // DOM Elements - Studio Controls
  const btnModeT2V = document.getElementById('btn-mode-t2v');
  const btnModeI2V = document.getElementById('btn-mode-i2v');
  const selectModel = document.getElementById('select-model');
  const modelStatusBadge = document.getElementById('model-status-badge');
  const promptInput = document.getElementById('prompt-input');
  const negativePromptInput = document.getElementById('negative-prompt-input');
  const btnEnhance = document.getElementById('btn-enhance');
  const enhanceStyle = document.getElementById('enhance-style');
  const resolutionChips = document.querySelectorAll('.chip');
  const stepPills = document.querySelectorAll('.step-pill');
  
  const inputFrames = document.getElementById('input-frames');
  const valFrames = document.getElementById('val-frames');
  const inputFps = document.getElementById('input-fps');
  const valFps = document.getElementById('val-fps');
  const inputCfg = document.getElementById('input-cfg');
  const valCfg = document.getElementById('val-cfg');
  const inputSeed = document.getElementById('input-seed');
  const btnRandomSeed = document.getElementById('btn-random-seed');
  const btnGenerate = document.getElementById('btn-generate');

  // I2V Elements
  const i2vUploadSection = document.getElementById('i2v-upload-section');
  const dropzone = document.getElementById('dropzone');
  const fileInput = document.getElementById('file-input');
  const dropzoneIdle = document.getElementById('dropzone-idle');
  const dropzonePreview = document.getElementById('dropzone-preview');
  const imgPreview = document.getElementById('img-preview');
  const btnRemoveImg = document.getElementById('btn-remove-img');

  // Accordions
  const btnToggleNegative = document.getElementById('btn-toggle-negative');
  const negativeContent = document.getElementById('negative-content');
  const btnToggleAdvanced = document.getElementById('btn-toggle-advanced');
  const advancedContent = document.getElementById('advanced-content');

  // Viewport Screens
  const screenIdle = document.getElementById('screen-idle');
  const screenGenerating = document.getElementById('screen-generating');
  const screenCompleted = document.getElementById('screen-completed');
  const mainVideoPlayer = document.getElementById('main-video-player');
  const videoMetaCard = document.getElementById('video-meta-card');
  const metaPromptDisplay = document.getElementById('meta-prompt-display');
  const metaTagsFlex = document.getElementById('meta-tags-flex');
  const btnCopyPrompt = document.getElementById('btn-copy-prompt');
  const btnDownloadVideo = document.getElementById('btn-download-video');
  const vfResolution = document.getElementById('vf-resolution');
  const vfDuration = document.getElementById('vf-duration');
  const vfTime = document.getElementById('vf-time');

  // Progress Elements
  const progressCircle = document.getElementById('progress-circle');
  const progressPercentText = document.getElementById('progress-percent-text');
  const genStatusTitle = document.getElementById('gen-status-title');
  const genStatusSub = document.getElementById('gen-status-sub');
  const metricStep = document.getElementById('metric-step');
  const metricEta = document.getElementById('metric-eta');
  const metricSpeed = document.getElementById('metric-speed');
  const metricVram = document.getElementById('metric-vram');

  // Model Hub & Civitai Elements
  const modelsGrid = document.getElementById('models-grid');
  const civitaiInput = document.getElementById('civitai-input');
  const btnCivitaiDownload = document.getElementById('btn-civitai-download');
  const civitaiProgressContainer = document.getElementById('civitai-download-progress-container');
  const modelsDirLabel = document.getElementById('models-dir-label');
  const inputModelDir = document.getElementById('input-model-dir');
  const btnSaveDir = document.getElementById('btn-save-dir');
  const customFilesContainer = document.getElementById('custom-files-container');

  // Gallery Elements
  const galleryGrid = document.getElementById('gallery-grid');
  const galleryCount = document.getElementById('gallery-count');
  const btnRefreshGallery = document.getElementById('btn-refresh-gallery');

  // Hardware Elements
  const hwDetailsList = document.getElementById('hw-details-list');

  // ========================================================
  // 1. NAVIGATION TABS
  // ========================================================
  navTabs.forEach(tab => {
    tab.addEventListener('click', () => {
      navTabs.forEach(t => t.classList.remove('active'));
      tabContents.forEach(c => c.classList.remove('active'));

      tab.classList.add('active');
      const targetId = `tab-${tab.dataset.tab}`;
      const targetContent = document.getElementById(targetId);
      if (targetContent) {
        targetContent.classList.add('active');
      }

      if (tab.dataset.tab === 'gallery') {
        loadGallery();
      } else if (tab.dataset.tab === 'models') {
        loadModels();
      }
    });
  });

  // ========================================================
  // 2. WEBSOCKET SETUP & STREAMING
  // ========================================================
  function initWebSocket() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}/ws/stream`;
    
    ws = new WebSocket(wsUrl);

    ws.onopen = () => {
      console.log('Conexión WebSocket activa para telemetría y renderizado en vivo.');
    };

    ws.onmessage = (event) => {
      try {
        const msg = JSON.parse(event.data);
        handleWsMessage(msg);
      } catch (err) {
        console.error('Error parseando mensaje WS:', err);
      }
    };

    ws.onclose = () => {
      console.warn('WebSocket desconectado. Reintentando en 3s...');
      setTimeout(initWebSocket, 3000);
    };
  }

  function handleWsMessage(msg) {
    if (msg.type === 'generation_progress') {
      const data = msg.data;
      updateGenerationProgress(data);
    } else if (msg.type === 'generation_complete') {
      handleGenerationComplete(msg.data);
    } else if (msg.type === 'generation_error') {
      handleGenerationError(msg.data.error);
    } else if (msg.type === 'download_progress') {
      updateDownloadProgress(msg.data);
    }
  }

  let genPollingInterval = null;

  function stopGenerationPolling() {
    if (genPollingInterval) {
      clearInterval(genPollingInterval);
      genPollingInterval = null;
    }
  }

  function startGenerationPolling() {
    stopGenerationPolling();
    genPollingInterval = setInterval(async () => {
      try {
        const res = await fetch('/api/generate/status');
        if (!res.ok) return;
        const statusData = await res.json();

        if (statusData.progress) {
          updateGenerationProgress(statusData.progress);
        }

        if (!statusData.is_generating || (statusData.progress && statusData.progress.status === 'error')) {
          if (statusData.result) {
            stopGenerationPolling();
            handleGenerationComplete(statusData.result);
          } else if (statusData.error || (statusData.progress && statusData.progress.status === 'error')) {
            stopGenerationPolling();
            handleGenerationError(statusData.error || (statusData.progress && (statusData.progress.error_message || statusData.progress.message)));
          }
        }
      } catch (e) {
        console.warn('Error en sondeo de progreso:', e);
      }
    }, 600);
  }

  function updateGenerationProgress(data) {
    if (!data) return;

    if (data.status === 'error') {
      stopGenerationPolling();
      screenGenerating.style.display = 'none';
      screenIdle.style.display = 'flex';
      btnGenerate.disabled = false;
      updateSelectedModelBadge();
      alert(`Aviso de generación: ${data.message || data.error_message || 'Error en la conexión con el modelo.'}`);
      return;
    }

    screenIdle.style.display = 'none';
    screenCompleted.style.display = 'none';
    screenGenerating.style.display = 'flex';

    const pct = Math.min(100, Math.max(0, Math.round(data.progress_percent || 0)));
    progressPercentText.textContent = `${pct}%`;

    // Update circular SVG stroke-dashoffset (circumference = 2 * PI * 50 = 314.159)
    const circumference = 314.159;
    const offset = circumference - (pct / 100) * circumference;
    progressCircle.style.strokeDashoffset = Math.max(0, offset);

    if (data.status === 'descargando_modelo') {
      genStatusTitle.textContent = 'Descargando Modelo Wan 2.1 en Disco D:...';
      genStatusSub.textContent = data.message || 'Descarga inicial requerida (se almacena en D:\\WanVideoStudio_Models)...';
      metricStep.textContent = data.current_file || 'Descarga';
      metricEta.textContent = data.eta_seconds !== undefined ? `${data.eta_seconds}s` : '--';
      metricSpeed.textContent = data.speed_mb_s ? `${data.speed_mb_s} MB/s` : '--';
      metricVram.textContent = data.downloaded_mb ? `${data.downloaded_mb} / ${data.total_mb} MB` : 'D: 594 GB';
    } else if (data.status === 'cargando_modelo') {
      genStatusTitle.textContent = 'Cargando Pesos de Wan 2.1 (Disco D: ➔ RAM)...';
      genStatusSub.textContent = data.message || 'Cargando pesos en memoria RAM. La primera vez toma 2-3 minutos...';
      metricStep.textContent = data.elapsed_seconds !== undefined ? `${data.elapsed_seconds}s` : 'Carga Disco';
      metricEta.textContent = data.elapsed_seconds !== undefined ? `~${Math.max(15, 160 - data.elapsed_seconds)}s` : '~2 min';
      metricSpeed.textContent = 'NVMe/HDD';
      metricVram.textContent = 'RAM Activa';
    } else if (data.status === 'codificando_prompt') {
      genStatusTitle.textContent = 'Codificando Prompt Semántico (T5-XXL)...';
      genStatusSub.textContent = data.message || 'Procesando texto con T5-XXL en procesador Intel i7 (48 GB RAM)...';
      metricStep.textContent = 'Prompt T5';
      metricEta.textContent = '~1 min';
      metricSpeed.textContent = 'Intel i7';
      metricVram.textContent = '0 GB VRAM';
    } else if (data.status === 'generando' || data.status === 'iniciando_generacion') {
      genStatusTitle.textContent = 'Difundiendo Vídeo en RTX 4060...';
      genStatusSub.textContent = data.message || 'Ejecutando muestreo temporal en núcleos Tensor de la GPU...';
      if (data.step !== undefined && data.total_steps) {
        metricStep.textContent = `${data.step} / ${data.total_steps}`;
      }
      if (data.eta_seconds !== undefined) {
        metricEta.textContent = `${data.eta_seconds}s`;
      }
      if (data.step_time_s) {
        metricSpeed.textContent = `${data.step_time_s}s/it`;
      } else {
        metricSpeed.textContent = '~4s/it';
      }
      if (data.vram_gb) {
        metricVram.textContent = `${data.vram_gb} GB`;
      } else {
        metricVram.textContent = '3.6 GB';
      }
    } else if (data.status === 'codificando_video') {
      genStatusTitle.textContent = 'Ensamblando MP4...';
      genStatusSub.textContent = 'Decodificando latentes con VAE Tiling protegido...';
      metricStep.textContent = 'Exportando';
      metricEta.textContent = 'Pocos seg';
      metricSpeed.textContent = 'VAE Tiling';
      metricVram.textContent = '3.6 GB';
    }
  }

  function updateSelectedModelBadge() {
    const selectedId = selectModel.value;
    const found = cachedModelsList.find(m => m.id === selectedId);
    const btnContent = btnGenerate.querySelector('.btn-content');

    if (found && found.installed) {
      modelStatusBadge.textContent = '🟢 Instalado y Listo';
      modelStatusBadge.style.color = 'var(--accent-emerald)';
      modelStatusBadge.style.background = 'rgba(16, 185, 129, 0.15)';
      if (!btnGenerate.disabled) {
        btnContent.innerHTML = `
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
          GENERAR VÍDEO WAN 2.1
        `;
      }
    } else {
      modelStatusBadge.textContent = '🟡 Requiere Descarga Inicial';
      modelStatusBadge.style.color = 'var(--accent-amber)';
      modelStatusBadge.style.background = 'rgba(245, 158, 11, 0.15)';
      if (!btnGenerate.disabled) {
        btnContent.innerHTML = `
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path><polyline points="7 10 12 15 17 10"></polyline><line x1="12" y1="15" x2="12" y2="3"></line></svg>
          DESCARGAR Y GENERAR VÍDEO
        `;
      }
    }
  }

  selectModel.addEventListener('change', updateSelectedModelBadge);

  function handleGenerationComplete(meta) {
    stopGenerationPolling();
    screenGenerating.style.display = 'none';
    screenCompleted.style.display = 'flex';

    currentVideoUrl = meta.filepath ? `/outputs/${meta.filename}` : meta.video_url;
    mainVideoPlayer.src = currentVideoUrl;
    mainVideoPlayer.load();
    mainVideoPlayer.play().catch(() => {});

    // Update Floating bar
    vfResolution.textContent = `${meta.width}x${meta.height}`;
    vfDuration.textContent = `${meta.duration_seconds}s (${meta.num_frames} frames)`;
    vfTime.textContent = `${meta.total_render_time_s}s render`;

    // Update Meta card
    videoMetaCard.style.display = 'block';
    metaPromptDisplay.textContent = meta.prompt;
    metaTagsFlex.innerHTML = `
      <span class="meta-tag">Modelo: ${meta.model_id}</span>
      <span class="meta-tag">Resolución: ${meta.width}x${meta.height}</span>
      <span class="meta-tag">Pasos: ${meta.num_inference_steps}</span>
      <span class="meta-tag">CFG: ${meta.guidance_scale}</span>
      <span class="meta-tag">Semilla: ${meta.seed}</span>
      <span class="meta-tag">Tiempo: ${meta.total_render_time_s}s</span>
    `;

    btnGenerate.disabled = false;
    btnGenerate.querySelector('.btn-content').innerHTML = `
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
      GENERAR OTRO VÍDEO
    `;

    loadGalleryCount();
  }

  function handleGenerationError(err) {
    stopGenerationPolling();
    screenGenerating.style.display = 'none';
    screenIdle.style.display = 'flex';
    btnGenerate.disabled = false;
    btnGenerate.querySelector('.btn-content').innerHTML = `
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
      GENERAR VÍDEO WAN 2.1
    `;
    alert(`Error en la generación: ${err}`);
  }

  // ========================================================
  // 3. TELEMETRY POLLING
  // ========================================================
  async function fetchTelemetry() {
    try {
      const res = await fetch('/api/system/status');
      if (!res.ok) return;
      const data = await res.json();

      // GPU VRAM
      if (data.gpu && data.gpu.available) {
        const usedGb = (data.gpu.vram_used_mb / 1024).toFixed(1);
        const totalGb = (data.gpu.vram_total_mb / 1024).toFixed(1);
        hudVram.textContent = `${usedGb} / ${totalGb} GB`;
        hudVramBar.style.width = `${data.gpu.vram_used_percent}%`;
        hudTemp.textContent = `${data.gpu.temperature_c}°C`;
        metricVram.textContent = `${usedGb} GB`;
      }

      // RAM
      if (data.ram) {
        hudRam.textContent = `${(data.ram.total_gb - data.ram.available_gb).toFixed(1)} / ${data.ram.total_gb} GB`;
      }

      // Disk D:
      if (data.drives && data.drives.D && data.drives.D.exists) {
        hudDiskD.textContent = `${data.drives.D.free_gb} GB Libres`;
      }

      // Settings HW list
      if (hwDetailsList) {
        hwDetailsList.innerHTML = `
          <div class="profile-item"><span>GPU Detectada:</span><strong>${data.gpu.name || 'NVIDIA RTX 4060'}</strong></div>
          <div class="profile-item"><span>VRAM Total:</span><strong>${(data.gpu.vram_total_mb / 1024).toFixed(2)} GB</strong></div>
          <div class="profile-item"><span>Memoria RAM Sistema:</span><strong>${data.ram.total_gb} GB</strong></div>
          <div class="profile-item"><span>Espacio Disco D: (Modelos):</span><strong>${data.drives.D.free_gb} GB Libres</strong></div>
          <div class="profile-item"><span>Espacio Disco C: (Sistema):</span><strong>${data.drives.C.free_gb} GB Libres</strong></div>
        `;
      }

    } catch (e) {
      console.debug('Error polling telemetry:', e);
    }
  }

  // Poll telemetry every 3 seconds
  setInterval(fetchTelemetry, 3000);
  fetchTelemetry();

  // ========================================================
  // 4. STUDIO CONTROLS & GENERATE LOGIC
  // ========================================================

  // Mode toggling (T2V vs I2V)
  btnModeT2V.addEventListener('click', () => {
    currentMode = 't2v';
    btnModeT2V.classList.add('active');
    btnModeI2V.classList.remove('active');
    i2vUploadSection.style.display = 'none';
  });

  btnModeI2V.addEventListener('click', () => {
    currentMode = 'i2v';
    btnModeI2V.classList.add('active');
    btnModeT2V.classList.remove('active');
    i2vUploadSection.style.display = 'block';
  });

  // =========================================================
  // Contextual AI Prompt Enhancer (Multilingual & Semantic)
  // =========================================================
  function enhancePromptContextual(rawPrompt, style = 'cinematic') {
    const text = (rawPrompt || '').trim();
    if (!text) return { enhanced_prompt: '', suggested_negative: '' };

    const lower = text.toLowerCase();

    // 1. Subject detection
    let subject = 'scene';
    const subjects = [
      [/\b(chica|mujer|muchacha|senorita|señorita|modelo|chicas|mujeres|actriz|dama|princesa|girl|woman|female|lady|model|actress)\b/i, 'woman'],
      [/\b(chico|hombre|muchacho|senor|señor|caballero|actor|guerrero|man|male|boy|guy|gentleman|warrior)\b/i, 'man'],
      [/\b(perro|perrito|cachorro|can|dog|puppy|hound)\b/i, 'dog'],
      [/\b(gato|gatito|felino|cat|kitten|feline)\b/i, 'cat'],
      [/\b(caballo|yegua|potro|horse|stallion|mare)\b/i, 'horse'],
      [/\b(pajaro|pájaro|ave|aguila|águila|halcon|halcón|bird|eagle|hawk)\b/i, 'bird'],
      [/\b(dragon|dragón|dragones|monster|monstruo|creature|criatura|bestia|beast)\b/i, 'fantasy_creature'],
      [/\b(coche|auto|automovil|automóvil|carro|vehiculo|vehículo|car|sports car|supercar|ferrari|porsche|bmw|audi|lamborghini)\b/i, 'sports_car'],
      [/\b(moto|motocicleta|bike|motorcycle|superbike)\b/i, 'motorcycle'],
      [/\b(avion|avión|helicoptero|helicóptero|nave|cohete|airplane|plane|spaceship|rocket|drone)\b/i, 'aircraft'],
      [/\b(robot|cyborg|androide|mecha|cyberpunk|ia|ai|futurista)\b/i, 'robot'],
      [/\b(espacio|galaxia|universo|estrella|estrellas|planeta|planetas|nebulosa|space|galaxy|cosmos|nebula|star|stars|supernova)\b/i, 'cosmic'],
      [/\b(fuego|llama|llamas|explosion|explosión|magma|volcan|volcán|fire|flames|explosion|blast|lava)\b/i, 'fire_elemental'],
      [/\b(agua|rio|río|ola|olas|splash|gota|gotas|water|waves|river|splash|droplets)\b/i, 'water_elemental'],
      [/\b(cafe|café|comida|postre|hamburguesa|pizza|vino|trago|cocktail|coffee|food|drink|beverage|dish)\b/i, 'food_drink'],
      [/\b(playa|costa|mar|oceano|océano|beach|ocean|sea|coast)\b/i, 'beach'],
      [/\b(bosque|selva|arboles|árboles|forest|woods|jungle)\b/i, 'forest'],
      [/\b(montana|montaña|montanas|montañas|alpes|sierra|mountain|mountains|peaks)\b/i, 'mountain'],
      [/\b(cascada|catarata|waterfall|falls)\b/i, 'waterfall'],
      [/\b(ciudad|calle|avenida|rascacielos|city|street|skyline|urban)\b/i, 'city'],
      [/\b(desierto|dunas|desert|sand dunes)\b/i, 'desert']
    ];
    for (const [re, type] of subjects) {
      if (re.test(lower)) { subject = type; break; }
    }

    // 2. Setting detection
    let setting = null;
    const settings = [
      [/\b(playa|costa|arena|orilla|mar|beach|shore|ocean|coast|seaside)\b/i, 'beach'],
      [/\b(bosque|selva|arboleda|forest|jungle|woodland)\b/i, 'forest'],
      [/\b(montana|montaña|nieve|snow|mountain|alps)\b/i, 'mountain'],
      [/\b(ciudad|calle|avenida|callejon|callejón|city|street|downtown|alley)\b/i, 'city'],
      [/\b(habitacion|habitación|dormitorio|cama|casa|salon|salón|room|bedroom|indoor|cozy room)\b/i, 'cozy_interior'],
      [/\b(cafeteria|cafetería|bar|restaurante|cafe|coffee shop|restaurant)\b/i, 'cafe'],
      [/\b(estudio|set de grabacion|studio|photo studio)\b/i, 'studio'],
      [/\b(espacio|galaxia|universo|planeta|space|galaxy|cosmos|alien planet)\b/i, 'space'],
      [/\b(piscina|alberca|pool|swimming pool)\b/i, 'pool'],
      [/\b(carretera|autopista|pista|road|highway|asphalt|track)\b/i, 'highway'],
      [/\b(desierto|duna|dunas|desert|dunes)\b/i, 'desert'],
      [/\b(bajo el agua|submarino|underwater|coral reef|aquarium)\b/i, 'underwater']
    ];
    for (const [re, type] of settings) {
      if (re.test(lower)) { setting = type; break; }
    }
    if (!setting) {
      if (subject === 'beach') setting = 'beach';
      else if (subject === 'sports_car' || subject === 'motorcycle') setting = 'highway';
      else if (subject === 'robot') setting = 'city';
      else if (subject === 'cosmic') setting = 'space';
      else if (subject === 'food_drink') setting = 'cafe';
      else if (subject === 'desert') setting = 'desert';
      else setting = 'outdoor_scenic';
    }

    // 3. Weather / Lighting
    let weatherLight = 'cinematic_soft';
    const lightings = [
      [/\b(lluvia|lloviendo|tormenta|mojado|rain|raining|storm|wet)\b/i, 'rain'],
      [/\b(nieve|nevando|frio|frío|snow|snowing|blizzard|frost)\b/i, 'snow'],
      [/\b(atardecer|puesta de sol|golden hour|ocaso|sunset|dusk)\b/i, 'sunset'],
      [/\b(amanecer|madrugada|alba|sunrise|dawn)\b/i, 'sunrise'],
      [/\b(noche|nocturno|oscuridad|night|nocturnal|darkness)\b/i, 'night'],
      [/\b(soleado|sol|dia|día|sunny|bright sun|sunlight)\b/i, 'sunny'],
      [/\b(niebla|neblina|humo|fog|mist|foggy|hazy|smoky)\b/i, 'fog'],
      [/\b(neon|neones|neon lights|fluorescent)\b/i, 'neon']
    ];
    for (const [re, type] of lightings) {
      if (re.test(lower)) { weatherLight = type; break; }
    }

    // 4. Action
    let action = null;
    const actions = [
      [/\b(caminando|andando|paseando|walking|strolling|striding)\b/i, 'walking'],
      [/\b(corriendo|a toda velocidad|running|sprinting|speeding|fast)\b/i, 'running'],
      [/\b(bailando|baila|dancing|dance)\b/i, 'dancing'],
      [/\b(sonriendo|sonrie|sonríe|smiling|smile|laughing)\b/i, 'smiling'],
      [/\b(mirando|mira|observando|looking|gazing|staring)\b/i, 'gazing'],
      [/\b(nadando|banandose|bañándose|swimming)\b/i, 'swimming'],
      [/\b(conduciendo|pilotando|driving|drifting|cruising)\b/i, 'driving'],
      [/\b(volando|planeando|flying|soaring|gliding)\b/i, 'flying'],
      [/\b(posando|sentada|sentado|resting|posing|sitting|relaxed)\b/i, 'posing'],
      [/\b(explotando|ardiendo|exploding|burning|blazing)\b/i, 'exploding']
    ];
    for (const [re, type] of actions) {
      if (re.test(lower)) { action = type; break; }
    }

    // 5. Clothing
    let clothing = null;
    const clothings = [
      [/\b(bikini|traje de bano|traje de baño|banador|bañador|swimsuit|swimwear)\b/i, 'bikini swimwear'],
      [/\b(vestido|dress|gown)\b/i, 'elegant dress'],
      [/\b(vaqueros|jeans|denim)\b/i, 'casual denim and stylish top'],
      [/\b(chaqueta|abrigo|cuero|leather jacket|coat)\b/i, 'stylish leather jacket'],
      [/\b(ropa deportiva|fitness|gym|sportswear|athletic wear)\b/i, 'form-fitting athletic sportswear'],
      [/\b(armadura|suit|armor|traje espacial|spacesuit)\b/i, 'intricate futuristic armor'],
      [/\b(camisa|shirt|traje|suit|tuxedo|esmoquin)\b/i, 'sharp tailored suit']
    ];
    for (const [re, type] of clothings) {
      if (re.test(lower)) { clothing = type; break; }
    }

    // Dynamic Synthesis
    let subjectDetails = '';
    let dynamicAction = '';

    if (subject === 'woman' || subject === 'man') {
      const isFemale = subject === 'woman';
      const pronoun = isFemale ? 'her' : 'his';
      const subjectNoun = isFemale ? 'a stunning young woman' : 'a charismatic young man';
      const attireStr = clothing ? `wearing ${clothing}` : `dressed in a stylish cinematic outfit`;

      if (action === 'walking') {
        dynamicAction = `confidently walking towards the camera, gentle natural sway, hair lightly dancing in the ambient breeze, authentic lifelike stride`;
      } else if (action === 'running') {
        dynamicAction = `running with dynamic athletic fluidity, subtle motion blur on the limbs, intense expressive focus, wind rushing past`;
      } else if (action === 'smiling' || action === 'gazing') {
        dynamicAction = `slowly turning to meet the viewer's gaze, subtle warm authentic smile blooming across ${pronoun} lips, natural gentle eye blinks, soft micro-expressions`;
      } else if (action === 'posing') {
        dynamicAction = `relaxing with poised elegance, gentle shifting of posture, breathing naturally, hair strands catching subtle air currents`;
      } else if (action === 'swimming') {
        dynamicAction = `gracefully emerging from the crystal water, water droplets glistening on ${pronoun} skin, wet hair slicked back with cinematic clarity`;
      } else if (action === 'dancing') {
        dynamicAction = `performing fluid expressive dance movements, rhythmic body choreography, fabric swirling in sync with motion`;
      } else {
        dynamicAction = `looking towards the horizon then shifting gaze towards camera with an authentic captivating expression, soft wind parting ${pronoun} hair`;
      }
      subjectDetails = `${subjectNoun} ${attireStr}, ultra-detailed skin textures, lifelike facial features, realistic catchlights in the eyes`;

    } else if (subject === 'sports_car' || subject === 'motorcycle') {
      const vType = subject === 'sports_car' ? 'high-performance sports car' : 'custom sleek superbike';
      if (action === 'driving' || action === 'running') {
        dynamicAction = `roaring down the asphalt at high speed, alloy wheels spinning with realistic motion blur, exhaust heat shimmering in the wake`;
      } else {
        dynamicAction = `cruising smoothly with low-slung aggressive stance, aerodynamic body curves gleaming, tires cutting cleanly through the surface`;
      }
      subjectDetails = `a pristine ${vType}, deep glossy automotive reflections, intricate carbon fiber textures, luminous LED headlights`;

    } else if (subject === 'dog' || subject === 'cat' || subject === 'horse' || subject === 'bird' || subject === 'fantasy_creature') {
      const cMap = {
        dog: ['an energetic loyal dog', 'playfully moving with wagging tail and alert curious eyes, silky fur swaying with every step'],
        cat: ['a graceful domestic cat', 'moving with whisper-quiet feline stealth, twitching whiskers and luminous reflective pupils'],
        horse: ['a majestic thoroughbred horse', 'galloping with powerful muscular stride, flowing mane and tail fluttering gracefully in the wind'],
        bird: ['a majestic bird with wings outstretched', 'soaring through the air currents with effortless wingbeats, sharp penetrating gaze'],
        fantasy_creature: ['a formidable mythical dragon', 'slowly unfurling its immense wings, embers and ethereal heat rising from its scales, powerful serpentine motion']
      };
      const [cName, cAct] = cMap[subject] || ['a lifelike creature', 'moving naturally'];
      subjectDetails = `${cName}, ultra-fine fur and feather rendering, authentic anatomy, vibrant lifelike eyes`;
      dynamicAction = cAct;

    } else if (subject === 'aircraft') {
      subjectDetails = `a streamlined aerodynamic aircraft cutting through atmospheric layers, heat exhaust trails shimmering`;
      dynamicAction = `banking smoothly into a sweeping turn, condensation vortices swirling gracefully off the wingtips`;

    } else if (subject === 'robot') {
      subjectDetails = `an advanced futuristic humanoid cyborg, intricate joint hydraulics, glowing internal conduits, weathered metallic alloy plating`;
      dynamicAction = `stepping forward with deliberate mechanical precision, optic sensors refocusing with a subtle luminescent pulse`;

    } else if (subject === 'cosmic') {
      subjectDetails = `a mesmerizing cosmic spectacle, luminous celestial dust clusters, iridescent galactic rings`;
      dynamicAction = `swirling nebular clouds slowly revolving with awe-inspiring astronomical motion, stellar radiance pulsating across deep space`;

    } else if (subject === 'fire_elemental') {
      subjectDetails = `intense kinetic inferno energy, brilliant incandescent orange and crimson flame tendrils`;
      dynamicAction = `roaring upward in a dynamic vortex, glowing embers drifting and sparkling through the air with fluid turbulence`;

    } else if (subject === 'water_elemental') {
      subjectDetails = `pure crystalline water fluid dynamics, microscopic bubbles, pristine refraction and light caustics`;
      dynamicAction = `surging forward in powerful rhythmic fluid motion, delicate splashes bursting into fine airborne droplets`;

    } else if (subject === 'food_drink') {
      subjectDetails = `an artisan gastronomic presentation, tantalizing rich textures, immaculate plating and culinary detail`;
      dynamicAction = `gentle wisps of aromatic steam rising slowly into the air, glistening fresh sauce catching warm specular light highlights`;

    } else {
      subjectDetails = `a breathtaking expansive landscape filled with depth, rich organic textures, and cinematic scope`;
      if (setting === 'beach') {
        dynamicAction = `rhythmic ocean waves cresting and gently breaking onto the glistening wet shoreline, fine sea foam receding over polished stones`;
      } else if (setting === 'waterfall') {
        dynamicAction = `cascading sheets of turquoise water tumbling down the rocky precipice into a mist-shrouded pool, ethereal water spray floating in the air`;
      } else if (setting === 'mountain') {
        dynamicAction = `wisps of alpine cloud sweeping across jagged peaks, golden light shifting across the snow ridges`;
      } else if (setting === 'city') {
        dynamicAction = `smooth urban traffic streams, reflective skyscrapers towering overhead, ambient city life pulsing below`;
      } else if (setting === 'space') {
        dynamicAction = `stellar cosmic dust drifting majestically, vibrant gaseous shockwaves expanding across the stellar void, distant pulsars pulsing`;
      } else if (setting === 'desert') {
        dynamicAction = `wind-swept golden sands rippling across majestic towering dunes, shimmering thermal heat waves on the horizon`;
      } else if (setting === 'underwater') {
        dynamicAction = `sunbeams piercing deep into turquoise water, schooling fish weaving past vibrant swaying corals, floating crystalline air bubbles`;
      } else if (setting === 'cozy_interior' || setting === 'cafe') {
        dynamicAction = `ambient dust motes dancing in warm sunbeams, gentle steam drifting upward, warm relaxing atmosphere`;
      } else {
        dynamicAction = `leaves gently trembling in the breeze, shifting light rays illuminating the foreground with organic motion`;
      }
    }

    const settingDescs = {
      beach: 'on a sun-drenched golden coast, soft ambient sea spray, distant horizon meeting the ocean',
      forest: 'deep within an ancient lush woodland, towering trees with dappled sunbeams filtering through the foliage',
      mountain: 'set against majestic snow-capped alpine peaks, crisp thin mountain air and expansive panoramic grandeur',
      city: 'in a vibrant metropolitan downtown avenue, sleek architecture, reflective glass facades',
      cozy_interior: 'inside an intimate warmly lit architectural interior, soft ambient textures, cozy luxurious vibe',
      cafe: 'at a charming boutique Parisian-style cafe, warm indoor glow, delicate steam rising from a porcelain cup',
      studio: 'in a high-end minimalist film studio, clean infinity backdrop, master key and fill lighting',
      space: 'in the cosmic void overlooking a shimmering nebula, distant celestial stars and planetary curvature',
      pool: 'by the edge of a luxury infinity pool, crystalline turquoise water caressed by warm ambient light',
      highway: 'along a winding scenic coastal highway, pristine asphalt stretching toward the horizon',
      desert: 'in an expansive golden desert expanse under a vast open sky, timeless wind patterns sculpted in the dunes',
      underwater: 'submerged in an enchanting aquatic wonderland, sunlight filtering through clear blue waters'
    };
    const settingText = settingDescs[setting] || 'in an authentic beautifully composed environment';

    const lightMap = {
      sunset: 'bathed in warm golden hour twilight, amber and crimson gradients across the sky, long cinematic shadows',
      sunrise: 'illuminated by gentle crisp dawn light, soft pink and golden hues breaking through morning haze',
      rain: 'captured under atmospheric rainfall, glistening wet surfaces, shimmering puddle reflections, delicate falling rain droplets',
      snow: 'in a gentle winter snowfall, soft flakes drifting through the air, crisp diffused cold illumination',
      night: 'set in moody nocturnal lighting, soft chiaroscuro contrast, subtle cinematic blue rim light',
      neon: 'surrounded by vibrant volumetric neon glow, cyan and magenta rim highlights, rich specular reflections',
      fog: 'wrapped in ethereal morning mist, soft diffused volumetric light beams slicing through the fog',
      sunny: 'illuminated by natural bright sunshine, crisp specular highlights, vibrant saturated color palette',
      cinematic_soft: 'balanced with three-point master studio lighting, subtle rim illumination, soft flattering contrast'
    };
    const lightingText = lightMap[weatherLight] || 'cinematic natural lighting, soft shadow roll-off';

    const cameraMap = {
      cinematic: 'captured on 35mm Arri Alexa camera, 50mm anamorphic prime lens, smooth fluid gimbal tracking shot, shallow depth of field, 24fps filmic cadence',
      photorealistic: 'shot on Sony A7R V with 85mm f/1.4 G-Master lens, pristine 8k optical clarity, authentic bokeh falloff, natural organic color science',
      anime: 'Makoto Shinkai artistic anime movie aesthetic, lush hand-painted background textures, vibrant emotional sky gradients, sparkling dust motes',
      scifi: 'cinematic cyberpunk atmosphere, anamorphic 2.39:1 widescreen framing, subtle chromatic aberration, volumetric fog, Unreal Engine 5 movie grade',
      drone: 'breathtaking sweeping 4K aerial drone shot, steady panoramic orbit, expansive wide-angle field of view, cinematic downward tilt'
    };
    const cameraText = cameraMap[style] || cameraMap.cinematic;

    const cleanRaw = text.replace(/[,.\s]+$/, '');
    const capAction = dynamicAction ? dynamicAction.charAt(0).toUpperCase() + dynamicAction.slice(1) : '';
    const enhanced = `${cleanRaw}. Featuring ${subjectDetails}. ${capAction}. Set ${settingText}, ${lightingText}, ${cameraText}, masterpiece quality, highly detailed fluid motion.`;

    const baseNeg = ['low quality', 'blurry', 'distorted', 'flickering artifacts', 'watermark', 'text', 'grainy', 'jittery motion', 'pixelated', 'overexposed'];
    if (subject === 'woman' || subject === 'man') {
      baseNeg.push('bad anatomy', 'deformed fingers', 'extra hands', 'missing limbs', 'plastic doll skin', 'uncanny valley', 'unnatural eyes', 'wax figure look', 'static portrait');
    } else if (subject === 'sports_car' || subject === 'motorcycle') {
      baseNeg.push('static wheels', 'deformed car body', 'crooked chassis', 'unnatural speed physics', 'toy car look');
    } else if (subject === 'dog' || subject === 'cat' || subject === 'horse' || subject === 'bird' || subject === 'fantasy_creature') {
      baseNeg.push('mutated legs', 'extra tails', 'distorted snout', 'stiff unnatural animation', 'amateur render');
    } else if (subject === 'aircraft' || subject === 'robot') {
      baseNeg.push('deformed mechanics', 'static propellers', 'unrealistic physics', 'amateur 3d model');
    } else {
      baseNeg.push('static still photograph', 'jerky panning', 'muddy textures', 'unrealistic lighting');
    }
    if (style !== 'anime') {
      baseNeg.push('cartoon', '3d animation', 'cgi illustration', 'drawing');
    }
    const uniqueNeg = [...new Set(baseNeg)].join(', ');

    return {
      enhanced_prompt: enhanced,
      suggested_negative: uniqueNeg,
      detected_subject: subject,
      detected_setting: setting
    };
  }

  // Prompt Enhancer Button Event Listener
  btnEnhance.addEventListener('click', async () => {
    const raw = promptInput.value.trim();
    if (!raw) {
      alert('Escribe una idea en el prompt primero para poder mejorarla.');
      return;
    }
    btnEnhance.disabled = true;
    btnEnhance.textContent = 'Mejorando...';
    try {
      const res = await fetch('/api/prompt/enhance', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ prompt: raw, style: enhanceStyle.value })
      });
      const data = await res.json();

      let finalResult = data;
      // If the backend is running the older instance or missing contextual detection,
      // override with our rich contextual enhancer immediately:
      if (!data.detected_subject || (data.enhanced_prompt && data.enhanced_prompt.includes("Hollywood blockbuster grade"))) {
        finalResult = enhancePromptContextual(raw, enhanceStyle.value);
      }

      if (finalResult.enhanced_prompt) {
        promptInput.value = finalResult.enhanced_prompt;
      }
      if (finalResult.suggested_negative && !negativePromptInput.value) {
        negativePromptInput.value = finalResult.suggested_negative;
      }
    } catch (e) {
      console.warn('API error, falling back to local contextual enhancer:', e);
      const localResult = enhancePromptContextual(raw, enhanceStyle.value);
      if (localResult.enhanced_prompt) {
        promptInput.value = localResult.enhanced_prompt;
      }
      if (localResult.suggested_negative && !negativePromptInput.value) {
        negativePromptInput.value = localResult.suggested_negative;
      }
    } finally {
      btnEnhance.disabled = false;
      btnEnhance.innerHTML = `
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="m12 3-1.912 5.813a2 2 0 0 1-1.275 1.275L3 12l5.813 1.912a2 2 0 0 1 1.275 1.275L12 21l1.912-5.813a2 2 0 0 1 1.275-1.275L21 12l-5.813-1.912a2 2 0 0 1-1.275-1.275L12 3Z"></path></svg>
        Mejorar con IA
      `;
    }
  });

  // Accordions
  btnToggleNegative.addEventListener('click', () => {
    btnToggleNegative.classList.toggle('open');
    negativeContent.classList.toggle('open');
  });

  btnToggleAdvanced.addEventListener('click', () => {
    btnToggleAdvanced.classList.toggle('open');
    advancedContent.classList.toggle('open');
  });

  // Resolution Chips
  resolutionChips.forEach(chip => {
    chip.addEventListener('click', () => {
      resolutionChips.forEach(c => c.classList.remove('active'));
      chip.classList.add('active');
      currentWidth = parseInt(chip.dataset.width);
      currentHeight = parseInt(chip.dataset.height);
    });
  });

  // Step Pills
  stepPills.forEach(pill => {
    pill.addEventListener('click', () => {
      stepPills.forEach(p => p.classList.remove('active'));
      pill.classList.add('active');
      currentSteps = parseInt(pill.dataset.steps);
    });
  });

  // Sliders Synchronization
  inputFrames.addEventListener('input', (e) => {
    const val = parseInt(e.target.value);
    const secs = (val / parseInt(inputFps.value)).toFixed(1);
    valFrames.textContent = `${val} frames (~${secs} seg)`;
  });

  inputFps.addEventListener('input', (e) => {
    valFps.textContent = `${e.target.value} fps`;
    const frames = parseInt(inputFrames.value);
    const secs = (frames / parseInt(e.target.value)).toFixed(1);
    valFrames.textContent = `${frames} frames (~${secs} seg)`;
  });

  inputCfg.addEventListener('input', (e) => {
    valCfg.textContent = parseFloat(e.target.value).toFixed(1);
  });

  btnRandomSeed.addEventListener('click', () => {
    inputSeed.value = Math.floor(Math.random() * 2147483647);
  });

  // Image Upload Handling (I2V)
  dropzone.addEventListener('click', () => fileInput.click());

  fileInput.addEventListener('change', async (e) => {
    if (e.target.files && e.target.files[0]) {
      await uploadImageFile(e.target.files[0]);
    }
  });

  dropzone.addEventListener('dragover', (e) => {
    e.preventDefault();
    dropzone.style.borderColor = 'var(--primary)';
  });
  dropzone.addEventListener('dragleave', () => {
    dropzone.style.borderColor = 'var(--border-subtle)';
  });
  dropzone.addEventListener('drop', async (e) => {
    e.preventDefault();
    dropzone.style.borderColor = 'var(--border-subtle)';
    if (e.dataTransfer.files && e.dataTransfer.files[0]) {
      await uploadImageFile(e.dataTransfer.files[0]);
    }
  });

  async function uploadImageFile(file) {
    const formData = new FormData();
    formData.append('file', file);
    try {
      const res = await fetch('/api/upload/image', {
        method: 'POST',
        body: formData
      });
      const data = await res.json();
      if (data.image_url) {
        uploadedImageUrl = data.image_url;
        imgPreview.src = data.image_url;
        dropzoneIdle.style.display = 'none';
        dropzonePreview.style.display = 'block';
      }
    } catch (err) {
      alert('Error al subir la imagen: ' + err);
    }
  }

  btnRemoveImg.addEventListener('click', (e) => {
    e.stopPropagation();
    uploadedImageUrl = null;
    fileInput.value = '';
    dropzonePreview.style.display = 'none';
    dropzoneIdle.style.display = 'flex';
  });

  // Copy Prompt
  btnCopyPrompt.addEventListener('click', () => {
    navigator.clipboard.writeText(metaPromptDisplay.textContent).then(() => {
      btnCopyPrompt.textContent = '¡Copiado!';
      setTimeout(() => btnCopyPrompt.textContent = 'Copiar Prompt', 2000);
    });
  });

  // Download Video
  btnDownloadVideo.addEventListener('click', () => {
    if (currentVideoUrl) {
      const a = document.createElement('a');
      a.href = currentVideoUrl;
      a.download = currentVideoUrl.split('/').pop();
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
    }
  });

  // ========================================================
  // 5. GENERATE VIDEO TRIGGER
  // ========================================================
  btnGenerate.addEventListener('click', async () => {
    const prompt = promptInput.value.trim();
    if (!prompt) {
      alert('Por favor, escribe un prompt describiendo el vídeo que deseas generar.');
      promptInput.focus();
      return;
    }

    if (currentMode === 'i2v' && !uploadedImageUrl) {
      alert('En modo Image-to-Video debes seleccionar o arrastrar una imagen primero.');
      return;
    }

    const payload = {
      prompt: prompt,
      negative_prompt: negativePromptInput.value.trim(),
      image_url: currentMode === 'i2v' ? uploadedImageUrl : null,
      model_id: selectModel.value,
      pipeline_type: currentMode,
      height: currentHeight,
      width: currentWidth,
      num_frames: parseInt(inputFrames.value),
      fps: parseInt(inputFps.value),
      num_inference_steps: currentSteps,
      guidance_scale: parseFloat(inputCfg.value),
      seed: inputSeed.value ? parseInt(inputSeed.value) : -1,
      quantization: null // RTX 4060 uses bfloat16 + CPU offload
    };

    btnGenerate.disabled = true;
    btnGenerate.querySelector('.btn-content').innerHTML = `
      <span class="btn-glow"></span>
      Renderizando en RTX 4060...
    `;

    // Switch view to generating
    updateGenerationProgress({
      status: 'cargando_modelo',
      progress_percent: 2,
      step: 0,
      total_steps: currentSteps,
      eta_seconds: currentSteps * 4,
      message: 'Iniciando pipeline acelerado para RTX 4060...'
    });

    try {
      const res = await fetch('/api/generate/video', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      if (!res.ok) {
        const err = await res.json();
        throw new Error(err.detail || 'Error en la petición de generación');
      }
      startGenerationPolling();
    } catch (e) {
      handleGenerationError(e.message);
    }
  });

  // ========================================================
  // 6. MODEL HUB & CIVITAI FUNCTIONS
  // ========================================================
  async function loadModels() {
    try {
      const res = await fetch('/api/models/list');
      const data = await res.json();
      cachedModelsList = data.curated || [];
      updateSelectedModelBadge();
      renderModelsGrid(cachedModelsList);
      renderCustomFiles(data.custom_files || []);
      if (data.models_dir) {
        modelsDirLabel.textContent = data.models_dir;
        inputModelDir.value = data.models_dir;
      }
    } catch (e) {
      console.error('Error cargando modelos:', e);
    }
  }

  function renderModelsGrid(models) {
    modelsGrid.innerHTML = '';
    models.forEach(m => {
      const card = document.createElement('div');
      card.className = 'model-card';
      
      const isInstalled = m.installed;
      const downloadInfo = m.download_info;

      let actionHtml = '';
      if (downloadInfo && downloadInfo.status === 'descargando') {
        actionHtml = `
          <div style="width: 100%;">
            <div style="display:flex; justify-content:space-between; font-size:0.75rem; margin-bottom:4px;">
              <span>Descargando... ${downloadInfo.progress_percent}%</span>
              <span>${downloadInfo.speed_mb_s} MB/s</span>
            </div>
            <div class="hud-bar"><div class="hud-bar-fill" style="width:${downloadInfo.progress_percent}%;"></div></div>
          </div>
        `;
      } else if (isInstalled) {
        actionHtml = `<span class="status-pill-installed">✓ Instalado y Listo</span>`;
      } else {
        actionHtml = `<button class="btn-download-model" data-repoid="${m.id}">⬇ Descargar</button>`;
      }

      card.innerHTML = `
        <div class="mc-header">
          <div class="mc-title">${m.name}</div>
          <div class="mc-type-badge">${m.type.toUpperCase()}</div>
        </div>
        <div class="mc-desc">${m.description}</div>
        <div class="mc-footer">
          <div class="mc-size">${m.size_approx}</div>
          ${actionHtml}
        </div>
      `;
      modelsGrid.appendChild(card);
    });

    // Attach download listeners
    document.querySelectorAll('.btn-download-model').forEach(btn => {
      btn.addEventListener('click', async (e) => {
        const repoId = e.target.dataset.repoid;
        e.target.disabled = true;
        e.target.textContent = 'Iniciando...';
        try {
          await fetch('/api/models/download/huggingface', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ repo_id: repoId })
          });
          loadModels();
        } catch (err) {
          alert('Error iniciando descarga: ' + err);
          e.target.disabled = false;
        }
      });
    });
  }

  function renderCustomFiles(files) {
    if (!files || files.length === 0) {
      customFilesContainer.innerHTML = '<p class="empty-state">No hay modelos personalizados ni LoRAs adicionales en la carpeta.</p>';
      return;
    }
    let html = `
      <table style="width: 100%; border-collapse: collapse; font-size: 0.82rem;">
        <thead>
          <tr style="border-bottom: 1px solid var(--border-subtle); text-align: left; color: var(--text-dim);">
            <th style="padding: 8px;">Nombre de Archivo</th>
            <th style="padding: 8px;">Tipo</th>
            <th style="padding: 8px;">Tamaño</th>
          </tr>
        </thead>
        <tbody>
    `;
    files.forEach(f => {
      html += `
        <tr style="border-bottom: 1px solid rgba(255,255,255,0.04);">
          <td style="padding: 8px; color: var(--text-main);">${f.name}</td>
          <td style="padding: 8px; color: var(--accent-cyan);">${f.is_lora ? 'LoRA' : 'Checkpoint'}</td>
          <td style="padding: 8px; font-family: var(--font-mono);">${f.size_mb} MB</td>
        </tr>
      `;
    });
    html += `</tbody></table>`;
    customFilesContainer.innerHTML = html;
  }

  // Civitai Downloader
  btnCivitaiDownload.addEventListener('click', async () => {
    const val = civitaiInput.value.trim();
    if (!val) {
      alert('Introduce un enlace o ID de modelo de Civitai.');
      return;
    }
    btnCivitaiDownload.disabled = true;
    btnCivitaiDownload.textContent = 'Conectando con Civitai...';

    try {
      const res = await fetch('/api/models/download/civitai', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ model_id_or_url: val })
      });
      const data = await res.json();
      civitaiProgressContainer.style.display = 'block';
      civitaiProgressContainer.innerHTML = `
        <div style="background: rgba(0,0,0,0.4); padding: 12px; border-radius: 8px; border: 1px solid var(--border-subtle);">
          <div style="font-size: 0.85rem; font-weight: 600; margin-bottom: 6px;">Descarga iniciada desde Civitai</div>
          <div id="civitai-prog-bar" class="hud-bar"><div class="hud-bar-fill" style="width: 5%;"></div></div>
          <div id="civitai-prog-label" style="font-family: var(--font-mono); font-size: 0.72rem; color: var(--text-muted); margin-top: 4px;">Iniciando stream a disco D:...</div>
        </div>
      `;
      civitaiInput.value = '';
    } catch (e) {
      alert('Error en descarga de Civitai: ' + e);
    } finally {
      btnCivitaiDownload.disabled = false;
      btnCivitaiDownload.innerHTML = `
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path><polyline points="7 10 12 15 17 10"></polyline><line x1="12" y1="15" x2="12" y2="3"></line></svg>
        Descargar de Civitai
      `;
    }
  });

  function updateDownloadProgress(data) {
    // If it's civitai
    if (data.task_id && data.task_id.startsWith('civitai')) {
      const bar = document.getElementById('civitai-prog-bar');
      const label = document.getElementById('civitai-prog-label');
      if (bar && label) {
        bar.querySelector('.hud-bar-fill').style.width = `${data.progress_percent}%`;
        label.textContent = `${data.downloaded_mb} / ${data.total_mb} MB (${data.progress_percent}%) | Velocidad: ${data.speed_mb_s} MB/s | ETA: ${data.eta_seconds}s`;
        if (data.status === 'completado') {
          label.textContent = '¡Descarga de Civitai completada!';
          loadModels();
        }
      }
    } else {
      // It's Hugging Face
      loadModels();
    }
  }

  // Save Model Dir
  btnSaveDir.addEventListener('click', async () => {
    const newDir = inputModelDir.value.trim();
    if (!newDir) return;
    try {
      const res = await fetch('/api/settings/update', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ models_dir: newDir })
      });
      if (res.ok) {
        alert('Ruta de almacenamiento de modelos actualizada.');
        modelsDirLabel.textContent = newDir;
        loadModels();
      }
    } catch (e) {
      alert('Error al guardar la ruta: ' + e);
    }
  });

  // ========================================================
  // 7. GALLERY FUNCTIONS
  // ========================================================
  async function loadGalleryCount() {
    try {
      const res = await fetch('/api/gallery');
      const data = await res.json();
      galleryCount.textContent = data.length || 0;
    } catch (e) {}
  }

  async function loadGallery() {
    try {
      const res = await fetch('/api/gallery');
      const videos = await res.json();
      galleryCount.textContent = videos.length || 0;
      galleryGrid.innerHTML = '';

      if (videos.length === 0) {
        galleryGrid.innerHTML = '<p class="empty-state">Aún no has generado ningún vídeo. Ve a la pestaña Studio para crear tu primer vídeo Wan 2.1.</p>';
        return;
      }

      videos.forEach(v => {
        const card = document.createElement('div');
        card.className = 'gallery-card';
        const prompt = v.metadata ? v.metadata.prompt : v.filename;
        const res = v.metadata ? `${v.metadata.width}x${v.metadata.height}` : '480p';
        const renderTime = v.metadata ? `${v.metadata.total_render_time_s}s` : `${v.size_mb} MB`;

        card.innerHTML = `
          <video class="gc-video-preview" src="${v.url}" loop muted onmouseover="this.play()" onmouseout="this.pause()"></video>
          <div class="gc-info">
            <div class="gc-prompt" title="${prompt}">${prompt}</div>
            <div class="gc-meta-row">
              <span>${res}</span>
              <span>${renderTime}</span>
            </div>
          </div>
        `;

        card.addEventListener('click', () => {
          // Play in Studio tab
          navTabs[0].click();
          handleGenerationComplete(v.metadata || {
            filename: v.filename,
            filepath: v.url,
            prompt: prompt,
            width: 832,
            height: 480,
            num_frames: 49,
            num_inference_steps: 25,
            guidance_scale: 5.0,
            seed: 0,
            total_render_time_s: 0,
            duration_seconds: 3.0,
            model_id: 'Wan 2.1'
          });
        });

        galleryGrid.appendChild(card);
      });
    } catch (e) {
      console.error('Error cargando galería:', e);
    }
  }

  btnRefreshGallery.addEventListener('click', loadGallery);

  // Initialize
  initWebSocket();
  loadGalleryCount();
  loadModels();
});
