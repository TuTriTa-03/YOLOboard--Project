'use strict';

const $ = (id) => document.getElementById(id);

function refreshIcons() {
  if (window.lucide) window.lucide.createIcons();
}
document.addEventListener('DOMContentLoaded', refreshIcons);

// -------------------------
// CLOCK
// -------------------------
function updateClock() {
  const now = new Date();
  const time = now.toLocaleTimeString('vi-VN', { hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: false });
  const shortTime = now.toLocaleTimeString('vi-VN', { hour: '2-digit', minute: '2-digit', hour12: false });
  const date = now.toLocaleDateString('vi-VN', { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' });
  if ($('topClock')) $('topClock').textContent = time;
  if ($('exactTime')) $('exactTime').textContent = time;
  if ($('digitalClockMain')) $('digitalClockMain').textContent = shortTime;
  if ($('digitalDateMain')) $('digitalDateMain').textContent = date;
  if ($('calendarDate')) $('calendarDate').textContent = `${String(now.getDate()).padStart(2,'0')}/${String(now.getMonth()+1).padStart(2,'0')}/${now.getFullYear()}`;
  if ($('calendarDay')) $('calendarDay').textContent = new Intl.DateTimeFormat('vi-VN', { weekday: 'long' }).format(now);
  document.querySelectorAll('.statusTime').forEach(el => el.textContent = shortTime);
}
updateClock();
window.setInterval(updateClock, 1000);

// -------------------------
// LIVE ESP32 ENVIRONMENT API
// -------------------------
let lastSensorData = null;

const hostname = window.location.hostname;

const USE_DEMO_SENSOR_DATA =
  hostname.endsWith('github.io') ||
  hostname === 'localhost' ||
  hostname === '127.0.0.1' ||
  window.location.protocol === 'file:' ||
  new URLSearchParams(window.location.search).get('demo') === '1';

function setConnectionState(state, message) {
  const dot = $('sensorConnectionDot');
  const label = $('sensorConnectionStatus');
  const topDot = $('connectionDot');
  const topLabel = $('connectionStatus');

  const cls =
    state === 'online' ? 'online' :
    state === 'error' ? 'error' : 'offline';

  if (dot) dot.className = `status-dot ${cls}`;
  if (topDot) topDot.className = `status-dot ${cls}`;
  if (label) label.textContent = message;
  if (topLabel) topLabel.textContent = message;
}

function renderEnvironmentData(data) {
  const temperature = Number(data.temperature);
  const humidity = Number(data.humidity);
  const status = String(
    data.environmentStatus || data.status || 'UNKNOWN'
  ).toUpperCase();

  if ($('temperatureValue')) {
    $('temperatureValue').textContent =
      Number.isFinite(temperature) ? temperature.toFixed(2) : '--';
  }

  if ($('humidityValue')) {
    $('humidityValue').textContent =
      Number.isFinite(humidity) ? humidity.toFixed(2) : '--';
  }

  if ($('miniTemperature')) {
    $('miniTemperature').textContent =
      Number.isFinite(temperature) ? `${temperature.toFixed(1)}°C` : '--°C';
  }

  if ($('miniHumidity')) {
    $('miniHumidity').textContent =
      Number.isFinite(humidity) ? `${humidity.toFixed(1)}%` : '--%';
  }

  if ($('miniSensorStatus')) {
    $('miniSensorStatus').textContent = data.demo
      ? 'DEMO • Dữ liệu cảm biến mô phỏng'
      : `Trạng thái: ${status}`;
  }

  const statusElement = $('environmentStatus');

  if (statusElement) {
    statusElement.textContent = status;
    statusElement.className = 'environment-value';

    if (status === 'NORMAL') {
      statusElement.classList.add('normal');
    } else if (status.includes('ERROR')) {
      statusElement.classList.add('error');
    } else {
      statusElement.classList.add('warning');
    }
  }

  if ($('sensorUpdatedAt')) {
    const currentTime = new Date().toLocaleTimeString('vi-VN');

    $('sensorUpdatedAt').textContent = data.demo
      ? `Dữ liệu DEMO • Cập nhật lúc ${currentTime}`
      : `Cập nhật lúc ${currentTime}`;
  }

  if ($('apiErrorMessage')) {
    $('apiErrorMessage').textContent = '';
    $('apiErrorMessage').classList.remove('visible');
  }
}

/*
 * GitHub Pages cannot directly access the ESP32's local API.
 * Use clearly labelled simulated data for the public demo.
 */
function updateDemoEnvironmentData() {
  const now = Date.now() / 1000;

  const temperature = 28.4 + Math.sin(now / 12) * 0.5;
  const humidity = 62.0 + Math.sin(now / 15) * 2.0;

  const data = {
    available: true,
    sensorAvailable: true,
    wifiConnected: false,
    temperature,
    humidity,
    status: 'NORMAL',
    environmentStatus: 'NORMAL',
    demo: true
  };

  lastSensorData = data;

  setConnectionState('online', 'GitHub Pages • Chế độ DEMO');

  if ($('livePing')) {
    $('livePing').textContent = 'DEMO';
  }

  renderEnvironmentData(data);
}

/*
 * Real ESP32 API.
 * This branch runs when the dashboard is hosted on the board,
 * for example at http://192.168.x.x/.
 */
async function updateEnvironmentData() {
  if (USE_DEMO_SENSOR_DATA) {
    updateDemoEnvironmentData();
    return;
  }

  const startedAt = performance.now();

  try {
    const response = await fetch('/api/status', {
      method: 'GET',
      cache: 'no-store',
      headers: { Accept: 'application/json' }
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }

    const data = await response.json();
    const elapsed = Math.round(performance.now() - startedAt);

    if ($('livePing')) {
      $('livePing').textContent = `${elapsed} ms`;
    }

    if (!data.available || !data.sensorAvailable) {
      setConnectionState(
        data.wifiConnected ? 'online' : 'offline',
        data.wifiConnected
          ? 'ESP32 online • Sensor lỗi'
          : 'ESP32 • Wi-Fi chưa kết nối'
      );

      if ($('temperatureValue')) $('temperatureValue').textContent = '--';
      if ($('humidityValue')) $('humidityValue').textContent = '--';

      if ($('environmentStatus')) {
        $('environmentStatus').textContent =
          data.environmentStatus || data.status || 'SENSOR ERROR';
        $('environmentStatus').className = 'environment-value error';
      }

      if ($('sensorUpdatedAt')) {
        $('sensorUpdatedAt').textContent =
          'Chưa nhận được số đo hợp lệ từ DHT20';
      }

      if ($('apiErrorMessage')) {
        $('apiErrorMessage').textContent =
          'API đã phản hồi nhưng cảm biến chưa có dữ liệu hợp lệ.';
        $('apiErrorMessage').classList.add('visible');
      }

      if ($('miniSensorStatus')) {
        $('miniSensorStatus').textContent = 'Cảm biến chưa sẵn sàng';
      }

      return;
    }

    lastSensorData = data;

    setConnectionState('online', 'Đã nhận dữ liệu từ ESP32');

    renderEnvironmentData(data);

  } catch (error) {
    console.error('Environment API error:', error);

    setConnectionState('error', 'Không kết nối được API');

    if ($('apiErrorMessage')) {
      $('apiErrorMessage').textContent =
        'Không gọi được /api/status. Kiểm tra Wi-Fi, IP ESP32, WebServer và LittleFS.';
      $('apiErrorMessage').classList.add('visible');
    }

    if ($('sensorUpdatedAt')) {
      $('sensorUpdatedAt').textContent = 'Đang chờ kết nối lại...';
    }
  }
}

updateEnvironmentData();
window.setInterval(updateEnvironmentData, 2000);

// -------------------------
// ROBOT FACE / EXPRESSIONS
// -------------------------
const emotionSVGs = {
  happy: `<path d="M60 95 Q75 75 90 95" stroke="#38bdf8" stroke-width="7" stroke-linecap="round" fill="none"/><path d="M110 95 Q125 75 140 95" stroke="#38bdf8" stroke-width="7" stroke-linecap="round" fill="none"/>`,
  sad: `<path d="M60 85 Q75 105 90 85" stroke="#38bdf8" stroke-width="7" stroke-linecap="round" fill="none"/><path d="M110 85 Q125 105 140 85" stroke="#38bdf8" stroke-width="7" stroke-linecap="round" fill="none"/><circle cx="135" cy="108" r="4" fill="#60a5fa"/>`,
  angry: `<line x1="60" y1="80" x2="88" y2="95" stroke="#f43f5e" stroke-width="7" stroke-linecap="round"/><line x1="140" y1="80" x2="112" y2="95" stroke="#f43f5e" stroke-width="7" stroke-linecap="round"/>`,
  surprised: `<circle cx="75" cy="90" r="12" fill="#38bdf8"/><circle cx="125" cy="90" r="12" fill="#38bdf8"/><ellipse cx="100" cy="115" rx="6" ry="10" fill="#38bdf8"/>`,
  sleep: `<line x1="60" y1="92" x2="88" y2="92" stroke="#38bdf8" stroke-width="7" stroke-linecap="round"/><line x1="112" y1="92" x2="140" y2="92" stroke="#38bdf8" stroke-width="7" stroke-linecap="round"/><text x="135" y="70" fill="#93c5fd" font-size="14" font-weight="bold">Z</text>`,
  wink: `<ellipse cx="75" cy="92" rx="10" ry="14" fill="#38bdf8"/><circle cx="78" cy="88" r="4" fill="#fff"/><path d="M112 92 Q126 80 140 92" stroke="#38bdf8" stroke-width="7" stroke-linecap="round" fill="none"/>`,
  love: `<text x="58" y="105" font-size="34" fill="#f43f5e">♥</text><text x="108" y="105" font-size="34" fill="#f43f5e">♥</text>`,
  confused: `<circle cx="75" cy="88" r="9" fill="#38bdf8"/><circle cx="125" cy="95" r="14" fill="#38bdf8"/><path d="M88 116 Q100 108 112 116" stroke="#38bdf8" stroke-width="4" stroke-linecap="round" fill="none"/>`,
  tired: `<line x1="60" y1="95" x2="88" y2="95" stroke="#64748b" stroke-width="6" stroke-linecap="round"/><line x1="112" y1="95" x2="140" y2="95" stroke="#64748b" stroke-width="6" stroke-linecap="round"/>`
};

const jpEmotionReactions = {
  happy: { label: 'Vui vẻ', jp: 'えへへ、うれしいな！', vi: 'Ehehe, mình vui quá!' },
  sad: { label: 'Buồn', jp: 'どうしたの…？かなしいな…', vi: 'Có chuyện gì thế? Mình hơi buồn rồi…' },
  angry: { label: 'Tức giận', jp: 'もう！ぷんぷんだよ！', vi: 'Thật là! Mình giận rồi đó!' },
  surprised: { label: 'Ngạc nhiên', jp: 'わぁ！びっくりした！', vi: 'Oa! Giật cả mình!' },
  sleep: { label: 'Buồn ngủ', jp: 'ふわぁ…おやすみなさい…', vi: 'Ngáp… Chúc bạn ngủ ngon nha…' },
  wink: { label: 'Nháy mắt', jp: 'だいすきだよ！', vi: 'Mình quý bạn lắm đó!' },
  love: { label: 'Yêu thích', jp: 'きみのことが、だいすきです！', vi: 'Mình rất quý bạn luôn đó!' },
  confused: { label: 'Bối rối', jp: 'えっと…よくわからないなぁ…', vi: 'Ưm… mình chưa hiểu lắm…' },
  tired: { label: 'Mệt mỏi', jp: 'ちょっとつかれちゃった…', vi: 'Mình hơi mệt một xíu rồi…' }
};

function updateEmotionLabel(key) {
  const reaction = jpEmotionReactions[key];
  if ($('currentEmotionLabel')) $('currentEmotionLabel').textContent = `Biểu cảm: ${reaction ? reaction.label : key}`;
  document.querySelectorAll('.emotion-button').forEach(button => button.classList.toggle('active', button.getAttribute('onclick')?.includes(`'${key}'`)));
}

function changeRobotFace(key) {
  if ($('heroEyesGroup')) $('heroEyesGroup').innerHTML = emotionSVGs[key] || emotionSVGs.happy;
  if ($('robotHeroSvg')) $('robotHeroSvg').style.filter = key === 'angry' ? 'drop-shadow(0 0 8px rgba(244,63,94,.45))' : 'none';
}
function selectExpression(key) {
  const reaction = jpEmotionReactions[key];
  if (!reaction) return;
  changeRobotFace(key);
  updateEmotionLabel(key);
  speakSawakoAnime(reaction.jp, reaction.vi, key);
}
function cycleRandomEmotion() {
  const keys = Object.keys(jpEmotionReactions);
  const current = $('currentEmotionLabel')?.textContent || '';
  const candidates = keys.filter(key => !current.toLowerCase().includes(jpEmotionReactions[key].label.toLowerCase()));
  const key = (candidates.length ? candidates : keys)[Math.floor(Math.random() * (candidates.length || keys.length))];
  selectExpression(key);
}
changeRobotFace('happy');

// -------------------------
// JAPANESE SPEECH + SUBTITLES
// -------------------------
let selectedJpVoice = null;
let subtitleTimeout = null;
let currentUtterance = null;
let speechRequestId = 0;

function populateVoices() {
  if (!('speechSynthesis' in window)) return;
  const voices = window.speechSynthesis.getVoices();
  const japaneseVoices = voices.filter(voice => /^ja(?:-|$)/i.test(voice.lang));
  selectedJpVoice = japaneseVoices.find(voice => /nanami|kyoko|haruka|sayaka|google.*japanese|japanese.*female/i.test(voice.name))
    || japaneseVoices.find(voice => voice.localService) || japaneseVoices[0] || null;
  if ($('voiceStatus') && !currentUtterance) {
    $('voiceStatus').textContent = selectedJpVoice
      ? `Giọng Nhật khả dụng: ${selectedJpVoice.name} (${selectedJpVoice.lang})`
      : 'Chưa tìm thấy voice tiếng Nhật; hãy kiểm tra giọng đọc trên thiết bị.';
  }
}
if ('speechSynthesis' in window) {
  window.speechSynthesis.onvoiceschanged = populateVoices;
  window.setTimeout(populateVoices, 500);
}
function showSubtitles(jp, vi) {
  if ($('subJapanese')) $('subJapanese').textContent = jp || '';
  if ($('subVietnamese')) $('subVietnamese').textContent = vi || '';
  const banner = $('liveSubtitleOverlay');
  if (!banner) return;
  banner.classList.remove('subtitle-hidden');
  banner.classList.add('subtitle-visible');
  if (subtitleTimeout) { clearTimeout(subtitleTimeout); subtitleTimeout = null; }
}
function hideSubtitles(delay = 1200) {
  if (subtitleTimeout) clearTimeout(subtitleTimeout);
  subtitleTimeout = window.setTimeout(() => {
    $('liveSubtitleOverlay')?.classList.remove('subtitle-visible');
    $('liveSubtitleOverlay')?.classList.add('subtitle-hidden');
    subtitleTimeout = null;
  }, delay);
}
function stopSpeech() {
  speechRequestId++;
  currentUtterance = null;
  if ('speechSynthesis' in window) window.speechSynthesis.cancel();
  hideSubtitles(0);
  if ($('voiceStatus')) $('voiceStatus').textContent = 'Đã dừng giọng nói.';
}
function speakSawakoAnime(jp, vi, emotion = 'happy') {
  const requestId = ++speechRequestId;
  changeRobotFace(emotion);
  updateEmotionLabel(emotion);
  showSubtitles(jp, vi);
  if (!('speechSynthesis' in window) || typeof SpeechSynthesisUtterance === 'undefined') {
    if ($('voiceStatus')) $('voiceStatus').textContent = 'Trình duyệt chưa hỗ trợ đọc giọng nói. Phụ đề vẫn hiển thị.';
    hideSubtitles(5000);
    return;
  }
  const synth = window.speechSynthesis;
  synth.cancel();
  currentUtterance = null;
  const rate = Number.parseFloat($('voiceRateSlider')?.value ?? '0.90');
  const pitch = Number.parseFloat($('voicePitchSlider')?.value ?? '1.20');
  const volume = Number.parseFloat($('voiceVolSlider')?.value ?? '90') / 100;
  const utterance = new SpeechSynthesisUtterance(jp);
  utterance.lang = 'ja-JP';
  utterance.rate = Number.isFinite(rate) ? Math.min(1.4, Math.max(.6, rate)) : .9;
  utterance.pitch = Number.isFinite(pitch) ? Math.min(2, Math.max(.8, pitch)) : 1.2;
  utterance.volume = Number.isFinite(volume) ? Math.min(1, Math.max(0, volume)) : .9;
  populateVoices();
  if (selectedJpVoice) { utterance.voice = selectedJpVoice; utterance.lang = selectedJpVoice.lang || 'ja-JP'; }
  currentUtterance = utterance;
  utterance.onstart = () => {
    if (requestId !== speechRequestId) return;
    if ($('voiceStatus')) $('voiceStatus').textContent = selectedJpVoice ? `Đang đọc bằng ${selectedJpVoice.name}...` : 'Đang đọc bằng voice mặc định...';
  };
  utterance.onend = () => {
    if (requestId !== speechRequestId) return;
    currentUtterance = null;
    hideSubtitles(1800);
    if ($('voiceStatus')) $('voiceStatus').textContent = 'Đã đọc xong.';
  };
  utterance.onerror = event => {
    if (requestId !== speechRequestId) return;
    currentUtterance = null;
    hideSubtitles(3000);
    if ($('voiceStatus')) $('voiceStatus').textContent = `Không thể phát giọng nói (${event.error || 'unknown'}).`;
  };
  window.setTimeout(() => {
    if (requestId === speechRequestId) {
      try { synth.speak(utterance); }
      catch (error) { console.error(error); }
    }
  }, 100);
}
function playGreetingVoice() {
  speakSawakoAnime('はじめまして！えいりくです。いつでもそばにいますね！', 'Rất vui được gặp bạn! Mình là Eilik. Mình sẽ luôn ở bên bạn nhé!', 'love');
}
function speakJapanesePreset(type) {
  if (type === 'ganbatte') speakSawakoAnime('きょうもいっしょにがんばろうね！えいりくが応援してるよ！', 'Hôm nay chúng mình cùng cố gắng nhé! Eilik luôn cổ vũ bạn đó!', 'happy');
}

// -------------------------
// CHAT DEMO
// -------------------------
function getDemoReply(message) {
  const lower = message.toLocaleLowerCase('vi');
  if (lower.includes('nhiệt độ') || lower.includes('độ ẩm') || lower.includes('cảm biến')) {
    if (lastSensorData && lastSensorData.sensorAvailable) {
      return { jp: 'センサーのデータだよ！', vi: `Dữ liệu hiện tại: ${Number(lastSensorData.temperature).toFixed(1)}°C, độ ẩm ${Number(lastSensorData.humidity).toFixed(1)}%. Trạng thái: ${lastSensorData.environmentStatus || lastSensorData.status}.`, emotion: 'happy' };
    }
    return { jp: 'センサーを確認してね！', vi: 'Mình chưa nhận được dữ liệu cảm biến từ ESP32. Hãy kiểm tra kết nối nhé.', emotion: 'confused' };
  }
  if (lower.includes('thứ mấy') || lower.includes('ngày mai') || lower.includes('ngày mấy') || lower.includes('mấy giờ') || lower.includes('bây giờ')) {
    const now = new Date();
    if (lower.includes('ngày mai')) {
      const tomorrow = new Date(now); tomorrow.setDate(now.getDate() + 1);
      return { jp: 'あしたもいいひになりますように！', vi: `Ngày mai là ${tomorrow.toLocaleDateString('vi-VN')}. Chúc bạn một ngày vui vẻ nhé!`, emotion: 'happy' };
    }
    return { jp: `いまは ${now.getHours()}じ ${now.getMinutes()}ふん だよ！`, vi: `Bây giờ là ${now.getHours()} giờ ${now.getMinutes()} phút.`, emotion: 'happy' };
  }
  if (lower.includes('lập trình') || lower.includes('code') || lower.includes('esp32') || lower.includes('stm32')) {
    return { jp: 'プログラミングっておもしろいね！', vi: 'Lập trình thật thú vị! Dashboard này đang dùng ESP32 và HTTP API để hiển thị dữ liệu cảm biến.', emotion: 'love' };
  }
  if (lower.includes('chuyện cười') || lower.includes('cười')) return { jp: 'ロボットもわらっちゃうね！えへへ！', vi: 'Ehehe! Nếu robot có thể cười thì tiếng cười của bạn là nguồn năng lượng của mình đó!', emotion: 'love' };
  if (lower.includes('bạn làm được gì') || lower.includes('tính năng')) return { jp: 'いろいろなことをいっしょにためしてみよう！', vi: 'Mình có thể đổi biểu cảm, đọc tiếng Nhật, hiển thị giờ và theo dõi nhiệt độ/độ ẩm nếu ESP32 đang kết nối.', emotion: 'happy' };
  return { jp: 'うん、きこえてるよ！おはなししてくれてありがとう！', vi: 'Mình nghe bạn rồi! Đây là câu trả lời demo phía trình duyệt; AI thật cần backend riêng.', emotion: 'happy' };
}
function appendMessage(sender, text) {
  const container = $('chatDialogue');
  if (!container) return;
  const row = document.createElement('div');
  row.className = sender === 'user' ? 'chat-bubble user-bubble' : 'chat-bubble';
  row.textContent = text;
  container.appendChild(row);
  container.scrollTop = container.scrollHeight;
}
function sendChatMessage() {
  const input = $('chatInputText');
  if (!input) return;
  const message = input.value.trim();
  if (!message) return;
  input.value = '';
  appendMessage('user', message);
  const reply = getDemoReply(message);
  window.setTimeout(() => {
    appendMessage('bot', `${reply.jp}\n${reply.vi}`);
    speakSawakoAnime(reply.jp, reply.vi, reply.emotion);
  }, 300);
}
function sendQuickPrompt(text) {
  if (!$('chatInputText')) return;
  $('chatInputText').value = text;
  sendChatMessage();
}
function askBotFromSuggestion(text) { showScreenTab('screenDetailedChat'); sendQuickPrompt(text); }
$('chatForm')?.addEventListener('submit', event => { event.preventDefault(); sendChatMessage(); });

// -------------------------
// VOICE INPUT SIMULATION
// -------------------------
function simulateVoiceRecord(button) {
  if (!button) return;
  button.classList.add('recording');
  button.disabled = true;
  const oldLabel = button.getAttribute('aria-label');
  button.setAttribute('aria-label', 'Đang mô phỏng ghi âm');
  window.setTimeout(() => {
    button.classList.remove('recording');
    button.disabled = false;
    if (oldLabel) button.setAttribute('aria-label', oldLabel);
    sendQuickPrompt('Chào Eilik, bạn có khỏe không?');
  }, 900);
}
function toggleVoiceSim() { showScreenTab('screenSettings'); }
function focusChatInput() { showScreenTab('screenDetailedChat'); window.setTimeout(() => $('chatInputText')?.focus(), 350); }

// -------------------------
// SCREEN NAVIGATION / VIEW
// -------------------------
function showScreenTab(screenId) {
  const element = $(screenId);
  if (element) element.scrollIntoView({ behavior: 'smooth', block: 'nearest', inline: 'center' });
}
function switchView(mode) {
  const container = $('mockupScreens');
  const slider = $('viewSliderBtn');
  const grid = $('viewFullBtn');
  if (!container || !slider || !grid) return;
  const isGrid = mode === 'grid';
  container.classList.toggle('grid-mode', isGrid);
  slider.classList.toggle('active', !isGrid);
  grid.classList.toggle('active', isGrid);
}

// -------------------------
// VOICE SETTINGS LABELS
// -------------------------
function updateVoiceSettingLabels() {
  if ($('voiceRateSlider') && $('rateValue')) $('rateValue').textContent = Number($('voiceRateSlider').value).toFixed(2);
  if ($('voicePitchSlider') && $('pitchValue')) $('pitchValue').textContent = Number($('voicePitchSlider').value).toFixed(2);
  if ($('voiceVolSlider') && $('volumeValue')) $('volumeValue').textContent = `${$('voiceVolSlider').value}%`;
}
['voiceRateSlider','voicePitchSlider','voiceVolSlider'].forEach(id => $(id)?.addEventListener('input', updateVoiceSettingLabels));
updateVoiceSettingLabels();
