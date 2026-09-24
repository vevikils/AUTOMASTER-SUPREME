/**
 * Companion Studio - Frontend Controller
 * Manages Personas, Contextual Photo Albums, Chat Stream, Inline Photos, Model Hub & Telemetry.
 */

document.addEventListener('DOMContentLoaded', () => {
  // Global State
  let personas = [];
  let activePersona = null;
  let activeModelId = 'SG161222/Realistic_Vision_V5.1_noVAE';
  let modelsData = { curated: [], local_checkpoints: [] };
  let currentResolution = { width: 512, height: 768 };
  let ws = null;
  let isSending = false;
  let isGeneratingAlbum = false;
  let currentViewingPhotoUrl = null;
  let currentViewingPhotoFilename = null;

  // DOM Elements - Navigation
  const navTabs = document.querySelectorAll('.nav-tab');
  const tabPanes = document.querySelectorAll('.tab-pane');

  // DOM Elements - Telemetry
  const hudVramText = document.getElementById('hud-vram-text');
  const hudVramBar = document.getElementById('hud-vram-bar');
  const hudRamText = document.getElementById('hud-ram-text');
  const hudRamBar = document.getElementById('hud-ram-bar');
  const totalGalleryCount = document.getElementById('total-gallery-count');

  // DOM Elements - Personas Sidebar
  const personasListContainer = document.getElementById('personas-list-container');
  const personasCountBadge = document.getElementById('personas-count-badge');
  const btnOpenCreateModal = document.getElementById('btn-open-create-modal');

  // DOM Elements - View States
  const emptyStateView = document.getElementById('empty-state-view');
  const activeChatContainer = document.getElementById('active-chat-container');
  const sidebarDetails = document.getElementById('sidebar-details');
  const btnHeroCreateModal = document.getElementById('btn-hero-create-modal');

  // DOM Elements - Chat Area
  const chatActiveAvatar = document.getElementById('chat-active-avatar');
  const chatActiveName = document.getElementById('chat-active-name');
  const chatActiveStyle = document.getElementById('chat-active-style');
  const chatActiveTagline = document.getElementById('chat-active-tagline');
  const messagesContainer = document.getElementById('messages-container');
  const chatInputText = document.getElementById('chat-input-text');
  const btnSendMsg = document.getElementById('btn-send-msg');
  const btnQuickAlbum = document.getElementById('btn-quick-album');
  const btnRequestPhoto = document.getElementById('btn-request-photo');
  const btnInlineCamera = document.getElementById('btn-inline-camera');
  const btnClearChat = document.getElementById('btn-clear-chat');
  const genProgressBanner = document.getElementById('gen-progress-banner');
  const gpbTitle = document.getElementById('gpb-title');
  const gpbSub = document.getElementById('gpb-sub');
  const gpbFill = document.getElementById('gpb-fill');
  const gpbPct = document.getElementById('gpb-pct');
  const quickPromptChips = document.querySelectorAll('.chip-prompt');

  // DOM Elements - Right Sidebar
  const selectActiveModel = document.getElementById('select-active-model');
  const activeModelMeta = document.getElementById('active-model-meta');
  const detailArchetype = document.getElementById('detail-archetype');
  const detailHair = document.getElementById('detail-hair');
  const detailEyes = document.getElementById('detail-eyes');
  const detailOutfit = document.getElementById('detail-outfit');
  const detailFeatures = document.getElementById('detail-features');
  const btnTriggerPhotoshoot = document.getElementById('btn-trigger-photoshoot');
  const btnEditActivePersona = document.getElementById('btn-edit-active-persona');
  const inputSteps = document.getElementById('input-steps');
  const valSteps = document.getElementById('val-steps');
  const inputCfg = document.getElementById('input-cfg');
  const valCfg = document.getElementById('val-cfg');
  const resPills = document.querySelectorAll('.res-pill');
  const recentPhotosStrip = document.getElementById('recent-photos-strip');
  const btnSeeAllPhotos = document.getElementById('btn-see-all-photos');

  // DOM Elements - Gallery Tab
  const mainGalleryGrid = document.getElementById('main-gallery-grid');
  const galleryFilterPersona = document.getElementById('gallery-filter-persona');
  const galleryActiveCompanionTitle = document.getElementById('gallery-active-companion-title');
  const btnGalleryNewPhotoshoot = document.getElementById('btn-gallery-new-photoshoot');

  // DOM Elements - Model Hub Tab
  const modelsCatalogGrid = document.getElementById('models-catalog-grid');
  const inputCivitaiModel = document.getElementById('input-civitai-model');
  const btnCivitaiDl = document.getElementById('btn-civitai-dl');
  const civitaiDlStatus = document.getElementById('civitai-download-status-container');
  const btnRefreshLocalModels = document.getElementById('btn-refresh-local-models');

  // DOM Elements - Persona Builder Modal
  const modalPersona = document.getElementById('modal-persona');
  const btnCloseModal = document.getElementById('btn-close-modal');
  const btnCancelModal = document.getElementById('btn-cancel-modal');
  const btnSavePersona = document.getElementById('btn-save-persona');
  const formName = document.getElementById('form-name');
  const formTagline = document.getElementById('form-tagline');
  const formStyle = document.getElementById('form-style');
  const formHair = document.getElementById('form-hair');
  const formEyes = document.getElementById('form-eyes');
  const formOutfit = document.getElementById('form-outfit');
  const formFeatures = document.getElementById('form-features');
  const formArchetype = document.getElementById('form-archetype');
  const formTone = document.getElementById('form-tone');
  const formGreeting = document.getElementById('form-greeting');
  const formBackstory = document.getElementById('form-backstory');
  const chkAutoAlbum = document.getElementById('chk-auto-album');
  const presetChips = document.querySelectorAll('.p-chip');

  // DOM Elements - Album Live Progress Modal
  const modalAlbumProgress = document.getElementById('modal-album-progress');
  const albumProgressTitle = document.getElementById('album-progress-title');
  const albumProgressSub = document.getElementById('album-progress-sub');
  const alpStepText = document.getElementById('alp-step-text');
  const alpPctText = document.getElementById('alp-pct-text');
  const alpFill = document.getElementById('alp-fill');
  const btnAlbumDone = document.getElementById('btn-album-done');
  const albumSlots = [
    document.getElementById('slot-0'),
    document.getElementById('slot-1'),
    document.getElementById('slot-2'),
    document.getElementById('slot-3')
  ];

  // DOM Elements - Lightbox Modal
  const modalLightbox = document.getElementById('modal-lightbox');
  const lightboxImg = document.getElementById('lightbox-img');
  const lbScenarioBadge = document.getElementById('lb-scenario-badge');
  const lbPromptText = document.getElementById('lb-prompt-text');
  const btnCloseLightbox = document.getElementById('btn-close-lightbox');
  const btnSetAvatar = document.getElementById('btn-set-avatar');
  const btnDownloadFull = document.getElementById('btn-download-full');
  const btnDeletePhoto = document.getElementById('btn-delete-photo');

  // ========================================================
  // 1. NAVIGATION
  // ========================================================
  navTabs.forEach(tab => {
    tab.addEventListener('click', () => {
      navTabs.forEach(t => t.classList.remove('active'));
      tabPanes.forEach(p => p.classList.remove('active'));

      tab.classList.add('active');
      const targetId = `pane-${tab.dataset.tab}`;
      const targetPane = document.getElementById(targetId);
      if (targetPane) targetPane.classList.add('active');

      if (tab.dataset.tab === 'gallery') {
        loadGalleryView();
      } else if (tab.dataset.tab === 'models') {
        loadModelsView();
      }
    });
  });

  btnSeeAllPhotos.addEventListener('click', () => {
    navTabs[1].click();
  });

  // Preset Chips Clicking in Persona Builder
  presetChips.forEach(chip => {
    chip.addEventListener('click', () => {
      const targetId = chip.dataset.target;
      const targetInput = document.getElementById(targetId);
      if (targetInput) {
        targetInput.value = chip.dataset.val;
        targetInput.focus();
      }
    });
  });

  // ========================================================
  // 2. WEBSOCKET SETUP
  // ========================================================
  function initWebSocket() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}/ws/stream`;
    
    ws = new WebSocket(wsUrl);

    ws.onmessage = (event) => {
      try {
        const msg = JSON.parse(event.data);
        
        if (msg.type === 'generation_progress') {
          updateGenerationBanner(msg.data);
        } else if (msg.type === 'album_started') {
          handleAlbumStarted(msg);
        } else if (msg.type === 'album_progress') {
          handleAlbumProgress(msg.data);
        } else if (msg.type === 'album_photo_saved') {
          handleAlbumPhotoSaved(msg);
        } else if (msg.type === 'album_completed') {
          handleAlbumCompleted(msg);
        } else if (msg.type === 'avatar_updated') {
          if (activePersona && activePersona.id === msg.persona_id) {
            activePersona.avatar = msg.avatar;
            chatActiveAvatar.src = msg.avatar;
            renderPersonasList();
          }
        } else if (msg.type === 'persona_deleted') {
          loadPersonas();
        }
      } catch (e) {
        console.error('WS Error:', e);
      }
    };

    ws.onclose = () => {
      setTimeout(initWebSocket, 3000);
    };
  }

  function updateGenerationBanner(data) {
    if (data.status === 'iniciando') {
      genProgressBanner.style.display = 'flex';
      gpbTitle.textContent = 'Iniciando renderizado en RTX 4060...';
      gpbSub.textContent = 'Configurando latentes...';
      gpbFill.style.width = '10%';
      gpbPct.textContent = '10%';
    } else if (data.status === 'generando') {
      genProgressBanner.style.display = 'flex';
      const pct = data.progress_percent || 0;
      gpbTitle.textContent = `Renderizando foto (${data.step}/${data.total_steps})...`;
      gpbSub.textContent = `VRAM: ${data.vram_gb || '3.5'} GB • Inferencia local FP16`;
      gpbFill.style.width = `${pct}%`;
      gpbPct.textContent = `${Math.round(pct)}%`;
    } else if (data.status === 'completado') {
      gpbFill.style.width = '100%';
      gpbPct.textContent = '100%';
      gpbTitle.textContent = '¡Foto generada con éxito!';
      setTimeout(() => {
        genProgressBanner.style.display = 'none';
      }, 1200);
    } else if (data.status === 'error') {
      genProgressBanner.style.display = 'none';
    }
  }

  // ========================================================
  // 3. TELEMETRY POLLING
  // ========================================================
  async function fetchTelemetry() {
    try {
      const res = await fetch('/api/system/status');
      if (!res.ok) return;
      const data = await res.json();

      if (data.gpu) {
        hudVramText.textContent = `${data.gpu.vram_used_gb} / ${data.gpu.vram_total_gb} GB`;
        hudVramBar.style.width = `${data.gpu.vram_percent}%`;
      }
      if (data.ram) {
        hudRamText.textContent = `${data.ram.used_gb} / ${data.ram.total_gb} GB`;
        hudRamBar.style.width = `${data.ram.percent}%`;
      }
    } catch (e) {}
  }
  setInterval(fetchTelemetry, 2500);

  // ========================================================
  // 4. PERSONAS & CHAT
  // ========================================================
  async function loadPersonas() {
    try {
      const res = await fetch('/api/personas');
      personas = await res.json();
      
      personasCountBadge.textContent = `${personas.length} ${personas.length === 1 ? 'compañera' : 'compañeras'}`;
      renderPersonasList();
      populateGalleryFilter();

      if (personas.length === 0) {
        activePersona = null;
        emptyStateView.style.display = 'flex';
        activeChatContainer.style.display = 'none';
        sidebarDetails.style.display = 'none';
        totalGalleryCount.textContent = '0';
      } else {
        emptyStateView.style.display = 'none';
        activeChatContainer.style.display = 'flex';
        sidebarDetails.style.display = 'flex';

        // Keep current or select first
        if (!activePersona || !personas.some(p => p.id === activePersona.id)) {
          selectPersona(personas[0]);
        } else {
          const fresh = personas.find(p => p.id === activePersona.id);
          selectPersona(fresh);
        }
      }
    } catch (e) {
      console.error('Error cargando personas:', e);
    }
  }

  function renderPersonasList() {
    personasListContainer.innerHTML = '';
    
    personas.forEach(p => {
      const card = document.createElement('div');
      card.className = `persona-card ${activePersona && activePersona.id === p.id ? 'active' : ''}`;

      card.innerHTML = `
        <div class="persona-avatar-box">
          <img src="${p.avatar || '/ui/avatars/default.jpg'}" class="persona-avatar-img">
          <span class="status-indicator online"></span>
        </div>
        <div class="persona-card-info">
          <div class="persona-card-name">${p.name}</div>
          <div class="persona-card-tagline">${p.tagline}</div>
        </div>
        <div class="persona-card-actions">
          <button class="btn-delete-persona-card" data-id="${p.id}" title="Eliminar a ${p.name}">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="3 6 5 6 21 6"></polyline><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path></svg>
          </button>
        </div>
      `;

      card.addEventListener('click', (e) => {
        if (e.target.closest('.btn-delete-persona-card')) return;
        selectPersona(p);
      });

      const btnDelete = card.querySelector('.btn-delete-persona-card');
      btnDelete.addEventListener('click', async (e) => {
        e.stopPropagation();
        if (confirm(`¿Estás seguro de que quieres eliminar a ${p.name} y todas sus fotos?`)) {
          await deletePersona(p.id);
        }
      });

      personasListContainer.appendChild(card);
    });
  }

  async function deletePersona(personaId) {
    try {
      const res = await fetch(`/api/personas/${personaId}`, { method: 'DELETE' });
      if (res.ok) {
        if (activePersona && activePersona.id === personaId) {
          activePersona = null;
        }
        await loadPersonas();
      }
    } catch (e) {
      alert('Error al eliminar personaje: ' + e);
    }
  }

  async function selectPersona(persona) {
    activePersona = persona;
    renderPersonasList();

    // Update Chat Header
    chatActiveAvatar.src = persona.avatar || '/ui/avatars/default.jpg';
    chatActiveName.textContent = persona.name;
    const styleName = persona.style === 'photorealistic' ? 'Fotorrealista' : (persona.style === 'anime' ? 'Anime' : 'Cinematográfico');
    chatActiveStyle.textContent = styleName;
    chatActiveTagline.textContent = persona.tagline;

    // Update Right Sidebar Traits
    detailArchetype.textContent = persona.personality.archetype || '--';
    detailHair.textContent = persona.appearance.hair || '--';
    detailEyes.textContent = persona.appearance.eyes || '--';
    detailOutfit.textContent = persona.appearance.outfit || '--';
    detailFeatures.textContent = persona.appearance.features || '--';

    // Auto-select recommended model based on style
    if (persona.style === 'anime') {
      selectActiveModel.value = 'cagliostrolab/animagine-xl-3.1';
    } else if (persona.style === 'cinematic') {
      selectActiveModel.value = 'Lykon/dreamshaper-8';
    } else {
      selectActiveModel.value = 'SG161222/Realistic_Vision_V5.1_noVAE';
    }
    activeModelId = selectActiveModel.value;

    // Load Chat History
    await loadChatHistory(persona.id);

    // Load Recent Photos Strip
    await loadRecentPhotosStrip(persona.id);
  }

  async function loadChatHistory(personaId) {
    messagesContainer.innerHTML = '';
    try {
      const res = await fetch(`/api/personas/${personaId}/history`);
      const messages = await res.json();
      messages.forEach(m => appendMessageBubble(m));
      scrollChatToBottom();
    } catch (e) {
      console.error('Error cargando historial:', e);
    }
  }

  function appendMessageBubble(msg) {
    const isCompanion = msg.sender === 'companion';
    const bubbleWrapper = document.createElement('div');
    bubbleWrapper.className = `chat-msg ${isCompanion ? 'companion' : 'user'}`;

    let avatarHtml = '';
    if (isCompanion) {
      const avatarSrc = (activePersona && activePersona.avatar) ? activePersona.avatar : '/ui/avatars/default.jpg';
      avatarHtml = `<img src="${avatarSrc}" class="msg-avatar">`;
    }

    let photoHtml = '';
    if (msg.image_url) {
      const scenarioText = (msg.image_metadata && msg.image_metadata.scenario_title) 
        ? msg.image_metadata.scenario_title 
        : `Foto de ${activePersona ? activePersona.name.split(' ')[0] : 'Chica'}`;
      photoHtml = `
        <div class="msg-photo-card" data-url="${msg.image_url}">
          <span class="msg-photo-badge">📸 ${scenarioText}</span>
          <img src="${msg.image_url}" class="msg-photo-img" loading="lazy">
        </div>
      `;
    }

    bubbleWrapper.innerHTML = `
      ${avatarHtml}
      <div class="msg-bubble-box">
        <div class="msg-bubble">
          ${msg.text}
          ${photoHtml}
        </div>
        <span class="msg-time">${msg.timestamp || ''}</span>
      </div>
    `;

    // Click on photo to view lightbox
    const photoCard = bubbleWrapper.querySelector('.msg-photo-card');
    if (photoCard) {
      photoCard.addEventListener('click', () => {
        openLightbox(msg.image_url, msg.image_metadata);
      });
    }

    messagesContainer.appendChild(bubbleWrapper);
  }

  function scrollChatToBottom() {
    messagesContainer.scrollTop = messagesContainer.scrollHeight;
  }

  // ========================================================
  // 5. SEND MESSAGE & PHOTO REQUEST
  // ========================================================
  async function sendMessage(text, forcePhoto = false) {
    if (!activePersona || isSending) return;
    const msgText = text.trim();
    if (!msgText && !forcePhoto) return;

    isSending = true;
    chatInputText.value = '';

    // Append user message immediately
    const userMsg = {
      sender: 'user',
      text: msgText || '📸 ¡Mándame una foto tuya!',
      timestamp: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
    };
    appendMessageBubble(userMsg);
    scrollChatToBottom();

    if (forcePhoto) {
      genProgressBanner.style.display = 'flex';
      gpbTitle.textContent = 'Tomando foto en RTX 4060...';
      gpbSub.textContent = 'Iniciando difusión...';
      gpbFill.style.width = '10%';
      gpbPct.textContent = '10%';
    }

    try {
      const payload = {
        persona_id: activePersona.id,
        message: msgText || 'Mándame una foto tuya por favor',
        model_id: activeModelId,
        force_photo: forcePhoto,
        width: currentResolution.width,
        height: currentResolution.height,
        steps: parseInt(inputSteps.value),
        cfg: parseFloat(inputCfg.value),
        seed: -1
      };

      const res = await fetch('/api/chat/send', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });

      if (res.ok) {
        const compEntry = await res.json();
        appendMessageBubble(compEntry);
        scrollChatToBottom();

        // Refresh recent photos if image was generated
        if (compEntry.image_url) {
          loadRecentPhotosStrip(activePersona.id);
        }
      }
    } catch (e) {
      console.error('Error enviando mensaje:', e);
    } finally {
      isSending = false;
      genProgressBanner.style.display = 'none';
      chatInputText.focus();
    }
  }

  btnSendMsg.addEventListener('click', () => sendMessage(chatInputText.value, false));

  chatInputText.addEventListener('keydown', (e) => {
    if (e.key === 'Enter' && !e.shiftKey) {
      e.preventDefault();
      sendMessage(chatInputText.value, false);
    }
  });

  btnRequestPhoto.addEventListener('click', () => {
    sendMessage("Mándame una selfie o foto tuya ahora mismo", true);
  });

  btnInlineCamera.addEventListener('click', () => {
    sendMessage("Mándame una foto tuya ahora", true);
  });

  btnClearChat.addEventListener('click', async () => {
    if (!activePersona) return;
    if (confirm(`¿Limpiar la conversación con ${activePersona.name}?`)) {
      await fetch(`/api/personas/${activePersona.id}/history/clear`, { method: 'POST' });
      loadChatHistory(activePersona.id);
    }
  });

  // Quick Prompts
  quickPromptChips.forEach(chip => {
    chip.addEventListener('click', () => {
      const text = chip.dataset.text;
      const isPhoto = text.toLowerCase().includes('foto') || text.toLowerCase().includes('selfie');
      sendMessage(text, isPhoto);
    });
  });

  // ========================================================
  // 6. CONTEXTUAL PHOTO ALBUM GENERATION
  // ========================================================
  async function triggerAlbumGeneration(personaId) {
    if (isGeneratingAlbum) return;
    isGeneratingAlbum = true;

    const p = personas.find(item => item.id === personaId) || activePersona;
    if (!p) return;

    // Reset Slots in Modal
    albumProgressTitle.textContent = `Generando Álbum para ${p.name}`;
    albumProgressSub.textContent = `Renderizando 4 fotos según su descripción en NVIDIA RTX 4060`;
    alpStepText.textContent = `Iniciando motor de difusión...`;
    alpPctText.textContent = `0%`;
    alpFill.style.width = `0%`;
    btnAlbumDone.style.display = 'none';

    albumSlots.forEach((slot, idx) => {
      slot.className = 'album-slot';
      const labels = ['Retrato Oficial', 'Selfie Casual', 'Paseo Atardecer', 'Cafetería'];
      slot.innerHTML = `
        <div class="slot-placeholder">
          <span class="slot-num">${idx + 1}</span>
          <span class="slot-label">${labels[idx] || `Foto ${idx+1}`}</span>
        </div>
      `;
    });

    modalAlbumProgress.style.display = 'flex';

    try {
      const res = await fetch(`/api/personas/${personaId}/generate-album`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          count: 4,
          model_id: activeModelId,
          width: currentResolution.width,
          height: currentResolution.height,
          cfg: parseFloat(inputCfg.value)
        })
      });
    } catch (e) {
      alert('Error iniciando generación de álbum: ' + e);
      modalAlbumProgress.style.display = 'none';
      isGeneratingAlbum = false;
    }
  }

  function handleAlbumStarted(data) {
    alpStepText.textContent = `Iniciando sesión de ${data.total_photos} fotos...`;
    alpFill.style.width = `5%`;
    alpPctText.textContent = `5%`;
  }

  function handleAlbumProgress(data) {
    const pIdx = (data.album_photo_index || 1) - 1;
    const total = data.album_total_photos || 4;
    const scenario = data.scenario_title || 'Foto';
    const step = data.step || 0;
    const totalSteps = data.total_steps || 22;
    const photoPct = data.progress_percent || 0;

    // Overall Progress
    const overallPct = Math.min(100, Math.round(((pIdx + (photoPct / 100)) / total) * 100));
    alpFill.style.width = `${overallPct}%`;
    alpPctText.textContent = `${overallPct}%`;
    alpStepText.textContent = `Foto ${pIdx + 1}/${total}: ${scenario} (Paso ${step}/${totalSteps})`;

    // Highlight current slot
    albumSlots.forEach((s, idx) => {
      if (idx === pIdx && !s.classList.contains('completed')) {
        s.classList.add('rendering');
      } else if (idx !== pIdx) {
        s.classList.remove('rendering');
      }
    });
  }

  function handleAlbumPhotoSaved(data) {
    const slotIdx = (data.current || 1) - 1;
    if (slotIdx >= 0 && slotIdx < albumSlots.length) {
      const slot = albumSlots[slotIdx];
      slot.classList.remove('rendering');
      slot.classList.add('completed');
      slot.innerHTML = `<img src="${data.photo.url}" class="slot-completed-img" alt="Foto">`;
    }

    // Refresh Recent Photos Strip and Gallery if open
    if (activePersona && activePersona.id === data.persona_id) {
      loadRecentPhotosStrip(activePersona.id);
    }
  }

  function handleAlbumCompleted(data) {
    isGeneratingAlbum = false;
    alpFill.style.width = `100%`;
    alpPctText.textContent = `100%`;
    alpStepText.textContent = `¡Sesión completada con éxito! 4 fotos generadas en disco D:`;
    btnAlbumDone.style.display = 'inline-flex';

    // Reload active persona to sync updated avatar
    loadPersonas();
  }

  btnAlbumDone.addEventListener('click', () => {
    modalAlbumProgress.style.display = 'none';
  });

  // Photoshoot triggers from various UI buttons
  btnQuickAlbum.addEventListener('click', () => {
    if (activePersona) triggerAlbumGeneration(activePersona.id);
  });

  btnTriggerPhotoshoot.addEventListener('click', () => {
    if (activePersona) triggerAlbumGeneration(activePersona.id);
  });

  btnGalleryNewPhotoshoot.addEventListener('click', () => {
    if (activePersona) triggerAlbumGeneration(activePersona.id);
  });

  // ========================================================
  // 7. SLIDERS & PARAMETERS
  // ========================================================
  inputSteps.addEventListener('input', (e) => {
    valSteps.textContent = e.target.value;
  });

  inputCfg.addEventListener('input', (e) => {
    valCfg.textContent = parseFloat(e.target.value).toFixed(1);
  });

  resPills.forEach(pill => {
    pill.addEventListener('click', () => {
      resPills.forEach(p => p.classList.remove('active'));
      pill.classList.add('active');
      currentResolution.width = parseInt(pill.dataset.w);
      currentResolution.height = parseInt(pill.dataset.h);
    });
  });

  selectActiveModel.addEventListener('change', () => {
    activeModelId = selectActiveModel.value;
  });

  // ========================================================
  // 8. GALLERY & RECENT PHOTOS
  // ========================================================
  async function loadRecentPhotosStrip(personaId) {
    recentPhotosStrip.innerHTML = '';
    try {
      const res = await fetch(`/api/gallery/${personaId}`);
      const photos = await res.json();
      totalGalleryCount.textContent = photos.length;

      photos.slice(0, 6).forEach(p => {
        const img = document.createElement('img');
        img.src = p.url;
        img.className = 'recent-thumb';
        img.title = p.metadata && p.metadata.scenario_title ? p.metadata.scenario_title : 'Foto';
        img.addEventListener('click', () => openLightbox(p.url, p.metadata, p.filename));
        recentPhotosStrip.appendChild(img);
      });
    } catch (e) {}
  }

  async function loadGalleryView() {
    mainGalleryGrid.innerHTML = '';
    if (!activePersona) {
      mainGalleryGrid.innerHTML = '<p style="color:var(--text-dim); grid-column: 1/-1;">Crea una compañera para ver su galería fotográfica.</p>';
      return;
    }

    galleryActiveCompanionTitle.textContent = activePersona.name;

    try {
      const res = await fetch(`/api/gallery/${activePersona.id}`);
      const photos = await res.json();
      totalGalleryCount.textContent = photos.length;

      if (photos.length === 0) {
        mainGalleryGrid.innerHTML = `
          <div style="grid-column: 1/-1; text-align: center; padding: 50px 20px;">
            <p style="color:var(--text-muted); font-size:1rem; margin-bottom:14px;">Aún no tienes fotos con ${activePersona.name}.</p>
            <button class="btn-primary" id="btn-empty-gallery-album">📸 Generar Sesión Fotográfica (4 fotos)</button>
          </div>
        `;
        const btnEmptyAlbum = document.getElementById('btn-empty-gallery-album');
        if (btnEmptyAlbum) {
          btnEmptyAlbum.addEventListener('click', () => triggerAlbumGeneration(activePersona.id));
        }
        return;
      }

      photos.forEach(p => {
        const card = document.createElement('div');
        card.className = 'gallery-card';
        const dateStr = new Date(p.created_at * 1000).toLocaleDateString([], { month: 'short', day: 'numeric', hour: '2-digit', minute: '2-digit' });
        const scenarioTitle = p.metadata && p.metadata.scenario_title ? p.metadata.scenario_title : 'Retrato';

        card.innerHTML = `
          <img src="${p.url}" class="gallery-card-img" loading="lazy">
          <div class="gallery-card-overlay">
            <span class="gc-scenario-chip">${scenarioTitle}</span>
            <div class="gc-actions-row">
              <button class="gc-btn btn-g-avatar" title="Establecer como Avatar Principal">⭐</button>
              <a href="${p.url}" download="${p.filename}" class="gc-btn" title="Descargar Imagen">💾</a>
              <button class="gc-btn btn-g-delete" title="Eliminar Foto">🗑️</button>
            </div>
          </div>
          <div class="gallery-card-info">
            <div class="gallery-card-title">${scenarioTitle}</div>
            <div class="gallery-card-date">${dateStr} • ${p.size_kb} KB</div>
          </div>
        `;

        // Click card image to open lightbox
        card.querySelector('.gallery-card-img').addEventListener('click', () => {
          openLightbox(p.url, p.metadata, p.filename);
        });

        // Set as avatar
        card.querySelector('.btn-g-avatar').addEventListener('click', async (e) => {
          e.stopPropagation();
          await setCompanionAvatar(p.url);
        });

        // Delete photo
        card.querySelector('.btn-g-delete').addEventListener('click', async (e) => {
          e.stopPropagation();
          if (confirm('¿Eliminar esta foto permanentemente?')) {
            await deleteGalleryPhoto(p.filename);
          }
        });

        mainGalleryGrid.appendChild(card);
      });
    } catch (e) {
      console.error('Error cargando galería:', e);
    }
  }

  async function deleteGalleryPhoto(filename) {
    if (!activePersona) return;
    try {
      const res = await fetch(`/api/gallery/${activePersona.id}/${filename}`, { method: 'DELETE' });
      if (res.ok) {
        await loadGalleryView();
        await loadRecentPhotosStrip(activePersona.id);
        if (modalLightbox.style.display === 'flex') {
          modalLightbox.style.display = 'none';
        }
      }
    } catch (e) {
      alert('Error eliminando foto: ' + e);
    }
  }

  async function setCompanionAvatar(photoUrl) {
    if (!activePersona) return;
    try {
      const res = await fetch(`/api/personas/${activePersona.id}/avatar`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ avatar_url: photoUrl })
      });
      if (res.ok) {
        activePersona.avatar = photoUrl;
        chatActiveAvatar.src = photoUrl;
        renderPersonasList();
        alert('¡Foto establecida como avatar principal!');
      }
    } catch (e) {
      alert('Error asignando avatar: ' + e);
    }
  }

  function populateGalleryFilter() {
    galleryFilterPersona.innerHTML = '';
    personas.forEach(p => {
      const opt = document.createElement('option');
      opt.value = p.id;
      opt.textContent = `📸 ${p.name}`;
      if (activePersona && p.id === activePersona.id) opt.selected = true;
      galleryFilterPersona.appendChild(opt);
    });

    galleryFilterPersona.addEventListener('change', () => {
      const selected = personas.find(p => p.id === galleryFilterPersona.value);
      if (selected) {
        selectPersona(selected);
        loadGalleryView();
      }
    });
  }

  // ========================================================
  // 9. MODEL HUB
  // ========================================================
  async function loadModelsView() {
    modelsCatalogGrid.innerHTML = '';
    try {
      const res = await fetch('/api/models');
      modelsData = await res.json();

      modelsData.curated.forEach(m => {
        const card = document.createElement('div');
        card.className = 'model-catalog-card';
        const isInstalled = m.installed;

        card.innerHTML = `
          <div>
            <div style="display:flex; justify-content:space-between; align-items:center;">
              <span class="civitai-badge" style="background:${m.type === 'sdxl' ? 'var(--accent-violet)' : '#2563eb'};">${m.type.toUpperCase()}</span>
              <span style="font-size:0.75rem; color:var(--text-dim);">${m.size_gb} GB</span>
            </div>
            <h4 class="mcc-title" style="margin-top:8px;">${m.name}</h4>
            <p class="mcc-desc">${m.description}</p>
          </div>
          <div class="mcc-footer">
            <span style="font-size:0.75rem; color:${isInstalled ? 'var(--accent-emerald)' : 'var(--text-dim)'};">
              ${isInstalled ? '🟢 Instalado en D:' : '⚪ Descarga Directa'}
            </span>
            <button class="btn-secondary btn-sm btn-select-model" data-id="${m.id}">
              Seleccionar
            </button>
          </div>
        `;

        card.querySelector('.btn-select-model').addEventListener('click', () => {
          activeModelId = m.id;
          selectActiveModel.value = m.id;
          alert(`Modelo ${m.name} activado para la generación.`);
        });

        modelsCatalogGrid.appendChild(card);
      });

      updateModelDropdown();
    } catch (e) {
      console.error('Error cargando modelos:', e);
    }
  }

  function updateModelDropdown() {
    const curr = selectActiveModel.value;
    
    if (modelsData.local_checkpoints && modelsData.local_checkpoints.length > 0) {
      let optgroup = selectActiveModel.querySelector('optgroup[label="Checkpoints Civitai / Locales"]');
      if (!optgroup) {
        optgroup = document.createElement('optgroup');
        optgroup.label = "Checkpoints Civitai / Locales (.safetensors)";
        selectActiveModel.appendChild(optgroup);
      }
      optgroup.innerHTML = '';
      modelsData.local_checkpoints.forEach(chk => {
        const opt = document.createElement('option');
        opt.value = chk.path;
        opt.textContent = `📁 ${chk.name} (${chk.size_gb} GB)`;
        optgroup.appendChild(opt);
      });
    }

    if (curr) selectActiveModel.value = curr;
  }

  btnCivitaiDl.addEventListener('click', async () => {
    const val = inputCivitaiModel.value.trim();
    if (!val) return;

    btnCivitaiDl.disabled = true;
    civitaiDlStatus.style.display = 'block';
    civitaiDlStatus.innerHTML = '<span style="font-size:0.8rem; color:var(--accent-cyan);">Iniciando stream de descarga a disco D:...</span>';

    try {
      const res = await fetch('/api/models/download/civitai', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ model_id_or_url: val })
      });
      const data = await res.json();
      inputCivitaiModel.value = '';
    } catch (e) {
      alert('Error en descarga: ' + e);
    } finally {
      btnCivitaiDl.disabled = false;
    }
  });

  btnRefreshLocalModels.addEventListener('click', () => {
    loadModelsView();
    alert('Carpeta D:\\CompanionStudio_Models\\checkpoints escaneada.');
  });

  // ========================================================
  // 10. PERSONA BUILDER MODAL
  // ========================================================
  function openCreatePersonaModal() {
    modalPersona.style.display = 'flex';
    formName.value = '';
    formTagline.value = '';
    formHair.value = '';
    formEyes.value = '';
    formOutfit.value = '';
    formFeatures.value = '';
    formArchetype.value = 'Cariñosa y atenta';
    formTone.value = 'cálido, cercano y cariñoso';
    formGreeting.value = '';
    formBackstory.value = '';
    chkAutoAlbum.checked = true;
    formName.focus();
  }

  btnOpenCreateModal.addEventListener('click', openCreatePersonaModal);
  btnHeroCreateModal.addEventListener('click', openCreatePersonaModal);

  btnEditActivePersona.addEventListener('click', () => {
    if (!activePersona) return;
    modalPersona.style.display = 'flex';
    formName.value = activePersona.name;
    formTagline.value = activePersona.tagline;
    formStyle.value = activePersona.style;
    formHair.value = activePersona.appearance.hair || '';
    formEyes.value = activePersona.appearance.eyes || '';
    formOutfit.value = activePersona.appearance.outfit || '';
    formFeatures.value = activePersona.appearance.features || '';
    formArchetype.value = activePersona.personality.archetype || '';
    formTone.value = activePersona.personality.tone || '';
    formGreeting.value = activePersona.personality.greeting || '';
    formBackstory.value = activePersona.personality.backstory || '';
    chkAutoAlbum.checked = false; // Don't auto-album on simple edit
  });

  btnCloseModal.addEventListener('click', () => modalPersona.style.display = 'none');
  btnCancelModal.addEventListener('click', () => modalPersona.style.display = 'none');

  btnSavePersona.addEventListener('click', async () => {
    const name = formName.value.trim();
    if (!name) {
      alert('Por favor introduce un nombre para tu compañera.');
      formName.focus();
      return;
    }

    const payload = {
      name: name,
      tagline: formTagline.value.trim() || 'Compañera virtual personalizada',
      style: formStyle.value,
      avatar: '/ui/avatars/default.jpg',
      appearance: {
        hair: formHair.value.trim() || 'long wavy chestnut hair',
        eyes: formEyes.value.trim() || 'warm amber eyes',
        outfit: formOutfit.value.trim() || 'cozy knit sweater and jeans',
        features: formFeatures.value.trim() || 'sweet natural smile'
      },
      personality: {
        archetype: formArchetype.value.trim() || 'Cariñosa',
        tone: formTone.value.trim() || 'dulce y cercano',
        greeting: formGreeting.value.trim() || `¡Hola! ✨ Me alegro mucho de que estés aquí. Soy ${name}. ¿Qué tal ha ido tu día?`,
        backstory: formBackstory.value.trim(),
        hobbies: ['Fotografía', 'Café', 'Música']
      }
    };

    try {
      const res = await fetch('/api/personas', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });

      if (res.ok) {
        const newPersona = await res.json();
        modalPersona.style.display = 'none';
        await loadPersonas();
        selectPersona(newPersona);

        // Auto-generate album if checked
        if (chkAutoAlbum.checked) {
          triggerAlbumGeneration(newPersona.id);
        }
      }
    } catch (e) {
      alert('Error guardando personaje: ' + e);
    }
  });

  // ========================================================
  // 11. LIGHTBOX VIEWER
  // ========================================================
  function openLightbox(imageUrl, metadata, filename) {
    currentViewingPhotoUrl = imageUrl;
    currentViewingPhotoFilename = filename || imageUrl.split('/').pop();
    lightboxImg.src = imageUrl;
    btnDownloadFull.href = imageUrl;
    btnDownloadFull.download = currentViewingPhotoFilename;
    
    if (metadata && metadata.scenario_title) {
      lbScenarioBadge.textContent = metadata.scenario_title;
      lbScenarioBadge.style.display = 'inline-block';
    } else {
      lbScenarioBadge.style.display = 'none';
    }

    if (metadata && metadata.prompt) {
      lbPromptText.textContent = metadata.prompt;
    } else {
      lbPromptText.textContent = 'Foto tomada en Companion Studio';
    }

    modalLightbox.style.display = 'flex';
  }

  btnCloseLightbox.addEventListener('click', () => {
    modalLightbox.style.display = 'none';
  });

  btnSetAvatar.addEventListener('click', async () => {
    if (currentViewingPhotoUrl) {
      await setCompanionAvatar(currentViewingPhotoUrl);
      modalLightbox.style.display = 'none';
    }
  });

  btnDeletePhoto.addEventListener('click', async () => {
    if (currentViewingPhotoFilename) {
      if (confirm('¿Eliminar esta foto permanentemente?')) {
        await deleteGalleryPhoto(currentViewingPhotoFilename);
      }
    }
  });

  // Initial Boot
  initWebSocket();
  fetchTelemetry();
  loadPersonas();
  loadModelsView();
});
