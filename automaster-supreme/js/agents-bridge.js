/**
 * AUTOMASTER SUPREME - Multi-Agent Bridge & Telemetry
 * Interconnects with the 4 installed agents:
 * - agentes-personalizados.md (Director)
 * - arquitecto-documentador.md (Acoustic Architect)
 * - revisor-codigo.md (Safety Reviewer)
 * - generador-pruebas.md (QA & Maximizer Tester)
 */

export class AgentsBridge {
  constructor() {
    this.container = document.getElementById('agents-terminal');
    this.statusPill = document.getElementById('agents-status-pill');
    this.agentAvatars = {
      'agentes-personalizados': document.getElementById('avatar-orchestrator'),
      'arquitecto-documentador': document.getElementById('avatar-architect'),
      'revisor-codigo': document.getElementById('avatar-reviewer'),
      'generador-pruebas': document.getElementById('avatar-qa')
    };
    this.logHistory = [];
  }

  log(agentId, message, details = null) {
    const timestamp = new Date().toLocaleTimeString();
    const entry = {
      agentId,
      message,
      details,
      timestamp
    };
    this.logHistory.push(entry);

    // Highlight active avatar
    Object.keys(this.agentAvatars).forEach(id => {
      const avatar = this.agentAvatars[id];
      if (avatar) {
        avatar.classList.toggle('active', id === agentId);
      }
    });

    // Append to UI terminal
    if (this.container) {
      const line = document.createElement('div');
      line.className = `agent-log-line agent-${agentId}`;

      let detailsHtml = '';
      if (details) {
        detailsHtml = Object.entries(details)
          .map(([k, v]) => `<span class="detail-badge">${k}: <strong>${v}</strong></span>`)
          .join(' ');
      }

      line.innerHTML = `
        <span class="log-time">[${timestamp}]</span>
        <span class="log-msg">${message}</span>
        ${detailsHtml ? `<div class="log-details">${detailsHtml}</div>` : ''}
      `;

      this.container.appendChild(line);
      this.container.scrollTop = this.container.scrollHeight;
    }

    if (this.statusPill) {
      this.statusPill.textContent = 'AGENTES ACTIVOS - IA COOPERATIVA';
      this.statusPill.classList.add('pulse');
    }
  }

  clear() {
    if (this.container) {
      this.container.innerHTML = '';
    }
  }
}
