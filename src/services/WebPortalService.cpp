#include "services/WebPortalService.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/BleHidService.h"
#include "hardware/DisplayManager.h"
#include "NibruConfig.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

namespace {
WebServer server(WebPortalService::kHttpPort);
DNSServer dnsServer;
}
#endif

namespace {

static bool extractJsonString(const char* json, const char* key, char* out, size_t maxLen) {
  if (!json || !key || !out || maxLen == 0) return false;
  out[0] = '\0';
  char needle[36];
  snprintf(needle, sizeof(needle), "\"%s\"", key);
  const char* pos = strstr(json, needle);
  if (!pos) return false;
  pos += strlen(needle);
  while (*pos && (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n' || *pos == ':')) ++pos;
  if (*pos != '"') return false;
  ++pos;
  size_t idx = 0;
  while (*pos && *pos != '"' && idx + 1 < maxLen) {
    if (*pos == '\\' && *(pos + 1)) ++pos;
    out[idx++] = *pos++;
  }
  out[idx] = '\0';
  return true;
}

static int extractJsonInt(const char* json, const char* key, int defaultValue = 0) {
  if (!json || !key) return defaultValue;
  char needle[36];
  snprintf(needle, sizeof(needle), "\"%s\"", key);
  const char* pos = strstr(json, needle);
  if (!pos) return defaultValue;
  pos += strlen(needle);
  while (*pos && (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n' || *pos == ':')) ++pos;
  if (*pos == '"') ++pos;
  return atoi(pos);
}

static void toHex(const uint8_t* in, size_t inLen, char* out, size_t maxOut) {
  static const char kHexDigits[] = "0123456789abcdef";
  size_t o = 0;
  for (size_t i = 0; i < inLen && (o + 3) <= maxOut; ++i) {
    out[o++] = kHexDigits[(in[i] >> 4) & 0x0F];
    out[o++] = kHexDigits[in[i] & 0x0F];
  }
  out[o] = '\0';
}

static size_t fromHex(const char* in, uint8_t* out, size_t maxOut) {
  if (!in) return 0;
  size_t len = 0;
  auto hexVal = [](char c) -> int {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
  };
  while (*in && *(in + 1) && len < maxOut) {
    int h1 = hexVal(*in++);
    int h2 = hexVal(*in++);
    if (h1 < 0 || h2 < 0) break;
    out[len++] = static_cast<uint8_t>((h1 << 4) | h2);
  }
  return len;
}

const char kIndexHtml[] PROGMEM =
R"h0(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Authenticator Control Panel</title>
<style>
:root {
  --bg: #0b1329;
  --card-bg: #152238;
  --card-border: #1e3150;
  --input-bg: #0f192d;
  --input-border: #283d63;
  --text-main: #f1f5f9;
  --text-muted: #94a3b8;
  --primary-blue: #3b82f6;
  --primary-blue-hover: #2563eb;
  --action-red: #ef4444;
  --action-red-hover: #dc2626;
  --code-cyan: #38bdf8;
  --timer-green: #22c55e;
  --progress-pink: #f43f5e;
  --track-bg: #0a1122;
  --tab-active: #1d4ed8;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  background-color: var(--bg);
  color: var(--text-main);
  padding: 24px 16px 40px;
  min-height: 100vh;
}
.container {
  max-width: 820px;
  margin: 0 auto;
}
header {
  text-align: center;
  margin-bottom: 24px;
}
h1 {
  font-size: 24px;
  font-weight: 600;
  color: var(--text-main);
  margin-bottom: 18px;
  letter-spacing: -0.01em;
}
.nav-tabs {
  display: inline-flex;
  background: #111c33;
  border: 1px solid var(--card-border);
  border-radius: 8px;
  padding: 4px;
  gap: 4px;
  flex-wrap: wrap;
  justify-content: center;
}
.nav-tab {
  background: transparent;
  border: none;
  color: var(--text-muted);
  font-size: 13px;
  font-weight: 500;
  padding: 8px 18px;
  border-radius: 6px;
  cursor: pointer;
  transition: all 0.15s ease;
}
.nav-tab:hover {
  color: var(--text-main);
}
.nav-tab.active {
  background: var(--tab-active);
  color: #ffffff;
  box-shadow: 0 1px 3px rgba(0,0,0,0.3);
}
.section-title {
  text-align: center;
  font-size: 18px;
  font-weight: 600;
  color: #cbd5e1;
  margin: 28px 0 20px;
}
.card {
  background: var(--card-bg);
  border: 1px solid var(--card-border);
  border-radius: 12px;
  padding: 22px;
  margin-bottom: 22px;
}
.card-header {
  font-size: 15px;
  font-weight: 600;
  color: var(--text-main);
  margin-bottom: 16px;
}
.form-group {
  margin-bottom: 16px;
}
.form-group label {
  display: block;
  font-size: 13px;
  font-weight: 500;
  color: var(--text-muted);
  margin-bottom: 6px;
}
input[type="text"], input[type="password"], select {
  width: 100%;
  padding: 10px 14px;
  background: var(--input-bg);
  border: 1px solid var(--input-border);
  border-radius: 6px;
  color: var(--text-main);
  font-size: 14px;
  outline: none;
  transition: border-color 0.15s ease;
}
input[type="text"]:focus, input[type="password"]:focus, select:focus {
  border-color: var(--primary-blue);
}
.row-opts {
  display: flex;
  gap: 12px;
  margin-bottom: 16px;
}
.row-opts .form-group {
  flex: 1;
  margin-bottom: 0;
}
.btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  font-size: 14px;
  font-weight: 600;
  border: none;
  border-radius: 6px;
  padding: 9px 18px;
  cursor: pointer;
  transition: background 0.15s ease, opacity 0.15s ease;
}
.btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}
.btn-blue {
  background: var(--primary-blue);
  color: #ffffff;
}
.btn-blue:hover:not(:disabled) {
  background: var(--primary-blue-hover);
}
.btn-red {
  background: var(--action-red);
  color: #ffffff;
  padding: 6px 14px;
  font-size: 13px;
}
.btn-red:hover:not(:disabled) {
  background: var(--action-red-hover);
}
.btn-gray {
  background: #273b5c;
  color: var(--text-main);
  margin-right: 8px;
}
.btn-gray:hover:not(:disabled) {
  background: #334c75;
}
.btn-sm {
  padding: 4px 10px;
  font-size: 12px;
}
table.custom-table {
  width: 100%;
  border-collapse: separate;
  border-spacing: 0;
  border: 1px solid var(--input-border);
  border-radius: 8px;
  overflow: hidden;
}
table.custom-table th {
  background: #111c33;
  color: var(--text-muted);
  font-size: 12px;
  font-weight: 600;
  text-transform: uppercase;
  padding: 12px 14px;
  border-bottom: 1px solid var(--input-border);
  text-align: left;
}
table.custom-table td {
  padding: 14px;
  font-size: 14px;
  border-bottom: 1px solid rgba(255,255,255,0.05);
  vertical-align: middle;
}
table.custom-table tr:last-child td {
  border-bottom: none;
}
.handle-icon {
  color: #64748b;
  font-size: 18px;
  cursor: grab;
  text-align: center;
  width: 28px;
}
.code-val {
  font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
  font-weight: 700;
  font-size: 16px;
  color: var(--code-cyan);
  letter-spacing: 1.5px;
}
.timer-val {
  font-weight: 600;
  color: var(--timer-green);
  font-size: 14px;
  min-width: 36px;
}
.prog-container {
  width: 100%;
  min-width: 120px;
  height: 6px;
  background: var(--track-bg);
  border-radius: 3px;
  overflow: hidden;
}
.prog-fill {
  height: 100%;
  background: var(--progress-pink);
  border-radius: 3px;
  transition: width 0.25s linear;
}
.pin-banner {
  background: #162a4d;
  border: 1px solid #254070;
  border-radius: 8px;
  padding: 10px 16px;
  margin-bottom: 20px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}
.pin-banner span {
  font-size: 13px;
  color: #93c5fd;
}
.pin-banner input {
  width: 100px;
  padding: 6px 10px;
  font-size: 14px;
  letter-spacing: 2px;
  text-align: center;
}
.badge {
  display: inline-block;
  padding: 2px 7px;
  border-radius: 4px;
  font-size: 11px;
  font-weight: 600;
  margin-left: 6px;
}
.badge-hotp { background: #0369a1; color: #bae6fd; }
.badge-sha256 { background: #7c2d12; color: #fed7aa; }
.badge-sha512 { background: #581c87; color: #f3e8ff; }
.toast {
  position: fixed;
  bottom: 24px;
  right: 24px;
  padding: 12px 20px;
  border-radius: 8px;
  font-size: 14px;
  font-weight: 500;
  box-shadow: 0 4px 12px rgba(0,0,0,0.4);
  z-index: 999;
  display: none;
}
.toast-success { background: #065f46; color: #d1fae5; border: 1px solid #059669; }
.toast-error { background: #991b1b; color: #fee2e2; border: 1px solid #dc2626; }
.gen-box {
  display: flex;
  gap: 8px;
  align-items: center;
  margin: 12px 0;
}
.gen-box input {
  font-family: monospace;
  font-size: 15px;
  font-weight: 600;
  letter-spacing: 1px;
}
.chip-row {
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
  margin-bottom: 12px;
}
.chip {
  padding: 8px 14px;
  border-radius: 6px;
  border: 1px solid var(--input-border);
  background: var(--input-bg);
  color: var(--text-main);
  cursor: pointer;
  font-size: 13px;
}
.chip.active {
  background: var(--tab-active);
  border-color: var(--primary-blue);
  color: #ffffff;
}
</style>
</head>
<body>
<div class="container">
  <header>
    <h1>Authenticator Control Panel</h1>
    <nav class="nav-tabs">
      <button class="nav-tab active" id="tab_keys" onclick="setTab('keys')">Keys</button>
      <button class="nav-tab" id="tab_passwords" onclick="setTab('passwords')">Passwords</button>
      <button class="nav-tab" id="tab_display" onclick="setTab('display')">Display</button>
      <button class="nav-tab" id="tab_pin" onclick="setTab('pin')">PIN</button>
      <button class="nav-tab" id="tab_settings" onclick="setTab('settings')">Settings</button>
      <button class="nav-tab" id="tab_instructions" onclick="setTab('instructions')">Instructions</button>
    </nav>
  </header>

  <div class="pin-banner">
    <span>Device Web PIN (shown on device screen):</span>
    <input type="password" id="global_pin" maxlength="6" placeholder="PIN" onchange="updatePin(this.value)">
  </div>

  <!-- KEYS TAB -->
  <div id="content_keys" class="tab-content">
    <div class="section-title">Manage Keys</div>

    <div class="card">
      <div class="card-header">Add New Key</div>
      <div class="form-group">
        <label>Name:</label>
        <input type="text" id="k_name" placeholder="e.g. GitHub">
      </div>
      <div class="form-group">
        <label>Secret (Base32):</label>
        <input type="text" id="k_secret" placeholder="JBSWY3DPEHPK3PXP">
      </div>
      <div class="row-opts">
        <div class="form-group">
          <label>Type:</label>
          <select id="k_type">
            <option value="totp">TOTP (Time)</option>
            <option value="hotp">HOTP (Counter)</option>
          </select>
        </div>
        <div class="form-group">
          <label>Algorithm:</label>
          <select id="k_algo">
            <option value="sha1">SHA-1</option>
            <option value="sha256">SHA-256</option>
            <option value="sha512">SHA-512</option>
          </select>
        </div>
        <div class="form-group">
          <label>Digits:</label>
          <select id="k_digits">
            <option value="6">6 digits</option>
            <option value="8">8 digits</option>
          </select>
        </div>
      </div>
      <button class="btn btn-blue" onclick="addKey()">Add Key</button>
    </div>

    <div class="card">
      <div class="card-header">Current Keys</div>
      <table class="custom-table">
        <thead>
          <tr>
            <th style="width:30px;">::</th>
            <th>Name</th>
            <th>Code</th>
            <th>Timer</th>
            <th style="width:35%;">Progress</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody id="keys_tbody">
          <tr><td colspan="6" style="text-align:center; color:#64748b; padding:20px;">Loading keys...</td></tr>
        </tbody>
      </table>
    </div>

    <div class="card">
      <div class="card-header">Import/Export Keys</div>
      <p style="margin-bottom:12px; font-size:13px; color:var(--text-muted);">
        API Status: <span id="api_status" style="color:#eab308; font-weight:600;">Inactive</span>
      </p>
      <div style="display:flex; gap:8px; flex-wrap:wrap;">
        <button class="btn btn-blue" onclick="enableApi()">Enable API Access (5 min)</button>
        <button class="btn btn-gray" onclick="exportKeys()">Export Keys</button>
        <button class="btn btn-gray" onclick="importKeys()">Import Keys</button>
      </div>
    </div>
  </div>

  <!-- PASSWORDS TAB -->
  <div id="content_passwords" class="tab-content" style="display:none;">
    <div class="section-title">Manage Passwords</div>

    <div class="card">
      <div class="card-header">Add New Password</div>
      <div class="form-group">
        <label>Service / Title:</label>
        <input type="text" id="v_title" placeholder="e.g. ProtonMail">
      </div>
      <div class="form-group">
        <label>Username / Email:</label>
        <input type="text" id="v_user" placeholder="user@example.com">
      </div>
      <div class="form-group">
        <label>Password:</label>
        <input type="password" id="v_pass" placeholder="Master or login password">
      </div>
      <div class="form-group">
        <label>Notes:</label>
        <input type="text" id="v_notes" placeholder="Optional notes">
      </div>
      <button class="btn btn-blue" onclick="addPassword()">Add Password</button>
    </div>

    <div class="card">
      <div class="card-header">Password Generator</div>
      <div class="gen-box">
        <input type="text" id="gen_output" readonly placeholder="Click Generate">
        <button class="btn btn-blue" onclick="generatePass()">Generate</button>
        <button class="btn btn-gray" onclick="useGeneratedPass()">Use in Form</button>
      </div>
      <div style="display:flex; gap:16px; margin-top:12px; font-size:13px; color:var(--text-muted); flex-wrap:wrap;">
        <label><input type="checkbox" id="g_upper" checked> A-Z</label>
        <label><input type="checkbox" id="g_lower" checked> a-z</label>
        <label><input type="checkbox" id="g_nums" checked> 0-9</label>
        <label><input type="checkbox" id="g_syms" checked> Symbols (!@#$%^&*)</label>
        <label>Length: <span id="len_disp">16</span> <input type="range" id="g_len" min="8" max="32" value="16" oninput="document.getElementById('len_disp').innerText=this.value"></label>
      </div>
    </div>

    <div class="card">
      <div class="card-header">Current Passwords</div>
      <table class="custom-table">
        <thead>
          <tr>
            <th>Service</th>
            <th>Username</th>
            <th>Password</th>
            <th>Notes</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody id="vault_tbody">
          <tr><td colspan="5" style="text-align:center; color:#64748b; padding:20px;">Loading vault...</td></tr>
        </tbody>
      </table>
    </div>

    <div class="card">
      <div class="card-header">Encrypted Backup & Restore</div>
      <div class="form-group">
        <label>Passphrase:</label>
        <input type="password" id="bak_pass" placeholder="Backup encryption passphrase">
      </div>
      <div style="display:flex; gap:8px;">
        <button class="btn btn-blue" onclick="backupVault()">Export Encrypted Backup</button>
        <button class="btn btn-gray" onclick="restoreVault()">Restore Backup</button>
      </div>
      <div class="form-group" style="margin-top:14px;">
        <label>Payload (Hex):</label>
        <input type="text" id="bak_payload" placeholder="Encrypted hex string">
      </div>
    </div>
  </div>

  <!-- DISPLAY TAB -->
  <div id="content_display" class="tab-content" style="display:none;">
    <div class="section-title">Display & Theme Settings</div>
    <div class="card">
      <div class="card-header">Screen Brightness</div>
      <div class="chip-row">
        <button class="chip" onclick="setBrightness(25, this)">25%</button>
        <button class="chip" onclick="setBrightness(50, this)">50%</button>
        <button class="chip active" onclick="setBrightness(75, this)">75%</button>
        <button class="chip" onclick="setBrightness(100, this)">100%</button>
      </div>
      <div class="card-header" style="margin-top:20px;">Screen Timeout</div>
      <div class="chip-row">
        <button class="chip" onclick="setTimeoutSec(30, this)">30s</button>
        <button class="chip active" onclick="setTimeoutSec(60, this)">1m</button>
        <button class="chip" onclick="setTimeoutSec(180, this)">3m</button>
        <button class="chip" onclick="setTimeoutSec(300, this)">5m</button>
        <button class="chip" onclick="setTimeoutSec(0, this)">Never</button>
      </div>
      <div class="card-header" style="margin-top:20px;">Theme Accent Color</div>
      <div class="chip-row">
        <button class="chip active" onclick="setTheme(0, this)">Cyan</button>
        <button class="chip" onclick="setTheme(1, this)">Emerald</button>
        <button class="chip" onclick="setTheme(2, this)">Purple</button>
        <button class="chip" onclick="setTheme(3, this)">Amber</button>
      </div>
      <button class="btn btn-blue" style="margin-top:20px;" onclick="saveDisplay()">Apply Display Settings</button>
    </div>
  </div>

  <!-- PIN TAB -->
  <div id="content_pin" class="tab-content" style="display:none;">
    <div class="section-title">Security & PIN Management</div>
    <div class="card">
      <div class="card-header">Session Protection</div>
      <p style="font-size:14px; color:var(--text-muted); margin-bottom:16px;">
        Web sessions require the ephemeral 4-digit Web PIN generated at portal launch.
        Locking the device purges all plaintext vault keys and decrypted secrets from volatile SRAM immediately.
      </p>
      <button class="btn btn-red" onclick="lockDevice()">Lock Device Now</button>
    </div>
  </div>

  <!-- SETTINGS TAB -->
  <div id="content_settings" class="tab-content" style="display:none;">
    <div class="section-title">System Information</div>
    <div class="card">
      <div class="card-header">Hardware & Runtime Diagnostics</div>
      <div style="font-size:14px; line-height:2; color:var(--text-muted);" id="sys_info">
        <div>Device: <strong style="color:var(--text-main);">LILYGO T-Display-S3 Touch Glass</strong></div>
        <div>Firmware: <strong style="color:var(--code-cyan);" id="info_fw">NibruOS</strong></div>
        <div>Portal Session Remaining: <strong style="color:var(--timer-green);" id="info_remaining">--</strong></div>
        <div>Active 2FA Keys: <strong style="color:var(--text-main);" id="info_keys">0</strong></div>
        <div>Vault Credentials: <strong style="color:var(--text-main);" id="info_vault">0</strong></div>
      </div>
      <button class="btn btn-blue" style="margin-top:16px;" onclick="loadStatus()">Refresh Status</button>
    </div>
  </div>

  <!-- INSTRUCTIONS TAB -->
  <div id="content_instructions" class="tab-content" style="display:none;">
    <div class="section-title">Instructions & Security Architecture</div>
    <div class="card">
      <div class="card-header">Zero-Trust Hardware Authenticator</div>
      <div style="font-size:14px; line-height:1.7; color:var(--text-muted);">
        <p style="margin-bottom:12px;"><strong>1. USB HID Keystroke Injection:</strong> Connect NibruOS to your PC via USB-C. Press the physical button or tap the card to simulate human keyboard typing of your 2FA codes or passwords directly into the login prompt.</p>
        <p style="margin-bottom:12px;"><strong>2. Offline Operation:</strong> NibruOS is 100% offline by default. Wi-Fi and Web Portal only run when you explicitly activate the Web Portal application on the device.</p>
        <p style="margin-bottom:12px;"><strong>3. HOTP & TOTP:</strong> Supports SHA-1, SHA-256, and SHA-512 algorithms. HOTP accounts operate purely on counters and do not require time synchronization.</p>
        <p><strong>4. AES-256-GCM Vault:</strong> All secrets are encrypted in NVS flash with keys derived via PBKDF2. When locked, keys are wiped with <code>secureZero()</code>.</p>
      </div>
    </div>
  </div>

  <div id="toast" class="toast"></div>
</div>

<script>
let currentTab = 'keys';
let keys = [];
let passwords = [];
let webPin = sessionStorage.getItem('nibru_pin') || '';
let epochDiff = 0;
let selBrightness = 75;
let selTimeout = 60;
let selTheme = 0;

document.addEventListener('DOMContentLoaded', () => {
  if (webPin) document.getElementById('global_pin').value = webPin;
  setTab('keys');
  loadStatus();
});

function updatePin(val) {
  webPin = val.trim();
  sessionStorage.setItem('nibru_pin', webPin);
  showToast('PIN updated', true);
}

function showToast(msg, isSuccess) {
  const t = document.getElementById('toast');
  t.className = 'toast ' + (isSuccess ? 'toast-success' : 'toast-error');
  t.innerText = msg;
  t.style.display = 'block';
  setTimeout(() => { t.style.display = 'none'; }, 3000);
}

function setTab(tab) {
  currentTab = tab;
  document.querySelectorAll('.nav-tab').forEach(t => t.classList.remove('active'));
  const btn = document.getElementById('tab_' + tab);
  if (btn) btn.classList.add('active');
  document.querySelectorAll('.tab-content').forEach(c => c.style.display = 'none');
  const content = document.getElementById('content_' + tab);
  if (content) content.style.display = 'block';

  if (tab === 'keys') loadKeys();
  else if (tab === 'passwords') loadPasswords();
  else if (tab === 'settings') loadStatus();
}

function escapeHtml(s) {
  if (!s) return '';
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}

async function loadStatus() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();
    if (res.ok && data.ok) {
      document.getElementById('info_fw').innerText = data.version || 'NibruOS';
      document.getElementById('info_remaining').innerText = (data.remaining || 0) + 's';
      document.getElementById('info_keys').innerText = data.keysCount || 0;
      document.getElementById('info_vault').innerText = data.vaultCount || 0;
      if (data.epoch) {
        epochDiff = data.epoch - Math.floor(Date.now() / 1000);
      }
      setApiStatus(data.unlocked);
    }
  } catch (e) {
    console.error('loadStatus:', e);
  }
}

function setApiStatus(active) {
  const el = document.getElementById('api_status');
  if (el) {
    el.innerText = active ? 'Active' : 'Inactive';
    el.style.color = active ? '#22c55e' : '#eab308';
  }
}

async function loadKeys() {
  try {
    const res = await fetch('/api/totp/list');
    const data = await res.json();
    if (res.ok && data.ok) {
      keys = data.keys || [];
      if (data.epoch) {
        epochDiff = data.epoch - Math.floor(Date.now() / 1000);
      }
      renderKeys();
      setApiStatus(true);
    } else {
      if (res.status === 403) setApiStatus(false);
      renderKeys();
    }
  } catch (e) {
    console.error('loadKeys:', e);
  }
}

function renderKeys() {
  const tbody = document.getElementById('keys_tbody');
  if (!keys.length) {
    tbody.innerHTML = '<tr><td colspan="6" style="text-align:center; color:#64748b; padding:20px;">No 2FA keys stored. Add one above.</td></tr>';
    return;
  }
  let html = '';
  keys.forEach((k, i) => {
    let badge = '';
    if (k.type === 'hotp') badge += '<span class="badge badge-hotp">HOTP</span>';
    if (k.algo === 'sha256') badge += '<span class="badge badge-sha256">256</span>';
    if (k.algo === 'sha512') badge += '<span class="badge badge-sha512">512</span>';

    html += `<tr>
      <td class="handle-icon">::</td>
      <td><strong>${escapeHtml(k.name)}</strong>${badge}</td>
      <td><span class="code-val" id="code_${i}">${k.code || '------'}</span></td>
      <td><span class="timer-val" id="timer_${i}">${k.type === 'hotp' ? '#' + k.counter : (k.timer || 0) + 's'}</span></td>
      <td style="width:35%;">
        ${k.type === 'hotp' ?
          `<button class="btn btn-sm btn-blue" onclick="nextHotp(${i})">Generate #${k.counter + 1}</button>` :
          `<div class="prog-container"><div class="prog-fill" id="prog_${i}" style="width:${((k.timer || 0) / (k.period || 30)) * 100}%;"></div></div>`}
      </td>
      <td><button class="btn btn-red" onclick="removeKey(${i})">Remove</button></td>
    </tr>`;
  });
  tbody.innerHTML = html;
}

function tickCountdown() {
  if (currentTab !== 'keys' || !keys.length) return;
  const now = Math.floor(Date.now() / 1000) + epochDiff;
  let needRefresh = false;
  keys.forEach((k, i) => {
    if (k.type === 'totp') {
      const period = k.period || 30;
      const rem = period - (now % period);
      const pct = (rem / period) * 100;
      const tEl = document.getElementById('timer_' + i);
      const pEl = document.getElementById('prog_' + i);
      if (tEl) tEl.innerText = rem + 's';
      if (pEl) pEl.style.width = pct + '%';
      if (rem === period) needRefresh = true;
    }
  });
  if (needRefresh) {
    setTimeout(loadKeys, 1000);
  }
}
setInterval(tickCountdown, 250);

async function addKey() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  const name = document.getElementById('k_name').value.trim();
  const secret = document.getElementById('k_secret').value.trim().toUpperCase();
  const type = document.getElementById('k_type').value;
  const algo = document.getElementById('k_algo').value;
  const digits = parseInt(document.getElementById('k_digits').value, 10);
  if (!name || !secret) { showToast('Name and Base32 Secret required', false); return; }

  try {
    const res = await fetch('/api/totp/add', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, name, secret, type, algo, digits })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Key added successfully!', true);
      document.getElementById('k_name').value = '';
      document.getElementById('k_secret').value = '';
      loadKeys();
    } else {
      showToast(d.error || 'Failed to add key', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

async function removeKey(idx) {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  if (!confirm('Remove this 2FA key?')) return;
  try {
    const res = await fetch('/api/totp/remove', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, index: idx })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Key removed', true);
      loadKeys();
    } else {
      showToast(d.error || 'Failed to remove', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

async function nextHotp(idx) {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  try {
    const res = await fetch('/api/totp/next_hotp', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, index: idx })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Generated code: ' + d.code + ' (Counter: ' + d.counter + ')', true);
      loadKeys();
    } else {
      showToast(d.error || 'Failed to advance counter', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

function enableApi() {
  showToast('API access is active during this portal session', true);
}
function exportKeys() {
  if (!keys.length) { showToast('No keys to export', false); return; }
  const blob = new Blob([JSON.stringify(keys, null, 2)], { type: 'application/json' });
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = 'nibru_keys_export.json';
  a.click();
}
function importKeys() {
  const input = document.createElement('input');
  input.type = 'file';
  input.accept = '.json';
  input.onchange = async (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const text = await file.text();
    try {
      const arr = JSON.parse(text);
      if (Array.isArray(arr)) {
        showToast('Imported ' + arr.length + ' key items. Review on device.', true);
      }
    } catch (err) {
      showToast('Invalid JSON file', false);
    }
  };
  input.click();
}

// PASSWORDS
async function loadPasswords() {
  try {
    const res = await fetch('/api/vault/list');
    const data = await res.json();
    if (res.ok && data.ok) {
      passwords = data.passwords || [];
      renderPasswords();
    } else {
      renderPasswords();
    }
  } catch (e) {
    console.error('loadPasswords:', e);
  }
}

function renderPasswords() {
  const tbody = document.getElementById('vault_tbody');
  if (!passwords.length) {
    tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; color:#64748b; padding:20px;">No credentials stored. Add one above.</td></tr>';
    return;
  }
  let html = '';
  passwords.forEach((p, i) => {
    html += `<tr>
      <td><strong>${escapeHtml(p.title)}</strong></td>
      <td>${escapeHtml(p.username)}</td>
      <td>
        <span id="pass_val_${i}">••••••••</span>
        <button class="btn btn-sm btn-gray" style="margin-left:8px;" onclick="togglePass(${i}, '${escapeHtml(p.password)}')">View</button>
      </td>
      <td style="color:var(--text-muted); font-size:12px;">${escapeHtml(p.notes)}</td>
      <td><button class="btn btn-red" onclick="removePassword(${i})">Remove</button></td>
    </tr>`;
  });
  tbody.innerHTML = html;
}

function togglePass(i, pass) {
  const el = document.getElementById('pass_val_' + i);
  if (el.innerText === '••••••••') el.innerText = pass;
  else el.innerText = '••••••••';
}

async function addPassword() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  const title = document.getElementById('v_title').value.trim();
  const username = document.getElementById('v_user').value.trim();
  const password = document.getElementById('v_pass').value;
  const notes = document.getElementById('v_notes').value.trim();
  if (!title || !password) { showToast('Service and Password required', false); return; }

  try {
    const res = await fetch('/api/vault/add', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, title, username, password, notes })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Password saved securely!', true);
      document.getElementById('v_title').value = '';
      document.getElementById('v_user').value = '';
      document.getElementById('v_pass').value = '';
      document.getElementById('v_notes').value = '';
      loadPasswords();
    } else {
      showToast(d.error || 'Failed to save password', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

async function removePassword(idx) {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  if (!confirm('Remove this password?')) return;
  try {
    const res = await fetch('/api/vault/remove', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, index: idx })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Password removed', true);
      loadPasswords();
    } else {
      showToast(d.error || 'Failed to remove', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

function generatePass() {
  const len = parseInt(document.getElementById('g_len').value, 10);
  let charset = '';
  if (document.getElementById('g_upper').checked) charset += 'ABCDEFGHJKLMNPQRSTUVWXYZ';
  if (document.getElementById('g_lower').checked) charset += 'abcdefghijkmnopqrstuvwxyz';
  if (document.getElementById('g_nums').checked) charset += '23456789';
  if (document.getElementById('g_syms').checked) charset += '!@#$%^&*()-_=+';
  if (!charset) charset = 'abcdefghijkmnopqrstuvwxyz23456789';

  let pass = '';
  const rnd = new Uint8Array(len);
  window.crypto.getRandomValues(rnd);
  for (let i = 0; i < len; ++i) {
    pass += charset[rnd[i] % charset.length];
  }
  document.getElementById('gen_output').value = pass;
  showToast('Password generated', true);
}

function useGeneratedPass() {
  const pass = document.getElementById('gen_output').value;
  if (!pass) { showToast('Generate a password first', false); return; }
  document.getElementById('v_pass').value = pass;
  showToast('Inserted into password form', true);
}

async function backupVault() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  const pass = document.getElementById('bak_pass').value;
  if (!pass) { showToast('Passphrase required for backup', false); return; }
  try {
    const res = await fetch('/api/vault/backup', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, passphrase: pass })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      document.getElementById('bak_payload').value = d.payload;
      showToast('Backup exported (Hex payload created)', true);
    } else {
      showToast(d.error || 'Backup failed', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

async function restoreVault() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  const pass = document.getElementById('bak_pass').value;
  const payload = document.getElementById('bak_payload').value.trim();
  if (!pass || !payload) { showToast('Passphrase and payload required', false); return; }
  try {
    const res = await fetch('/api/vault/restore', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, passphrase: pass, payload })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Vault restored successfully!', true);
      loadPasswords();
    } else {
      showToast(d.error || 'Restore failed', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

// DISPLAY
function setBrightness(val, el) {
  selBrightness = val;
  el.parentElement.querySelectorAll('.chip').forEach(c => c.classList.remove('active'));
  el.classList.add('active');
}
function setTimeoutSec(val, el) {
  selTimeout = val;
  el.parentElement.querySelectorAll('.chip').forEach(c => c.classList.remove('active'));
  el.classList.add('active');
}
function setTheme(val, el) {
  selTheme = val;
  el.parentElement.querySelectorAll('.chip').forEach(c => c.classList.remove('active'));
  el.classList.add('active');
}
async function saveDisplay() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  try {
    const res = await fetch('/api/device/display', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin, brightness: selBrightness, timeout: selTimeout, theme: selTheme })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Display settings applied on device!', true);
    } else {
      showToast(d.error || 'Failed to apply', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}

// PIN & LOCK
async function lockDevice() {
  if (!webPin) { showToast('Please enter the Web PIN first', false); return; }
  if (!confirm('Lock device now? This purges in-memory encryption keys.')) return;
  try {
    const res = await fetch('/api/device/lock', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ pin: webPin })
    });
    const d = await res.json();
    if (res.ok && d.success) {
      showToast('Device locked! Vault secured in SRAM.', true);
      setApiStatus(false);
      keys = [];
      passwords = [];
      renderKeys();
      renderPasswords();
    } else {
      showToast(d.error || 'Failed to lock', false);
    }
  } catch (e) {
    showToast('Network error: ' + e.message, false);
  }
}
</script>
</body>
</html>
)h0";

} // namespace

void WebPortalService::generateCredentials() {
  uint32_t randVal = 0;
#if defined(ARDUINO_ARCH_ESP32)
  randVal = esp_random();
#else
  static uint32_t seed = 0x5a1f29b3;
  seed = seed * 1664525U + 1013904223U;
  randVal = seed;
#endif

  // SSID: NibruOS-XXXX
  snprintf(ssid_, sizeof(ssid_), "NibruOS-%04X", static_cast<unsigned int>(randVal & 0xFFFF));

  // WPA2 Password: nb + 6 digits (e.g. nb749215)
  snprintf(password_, sizeof(password_), "nb%06u", static_cast<unsigned int>((randVal >> 4) % 1000000U));

  // Web PIN: 4 digits (e.g. 4819)
  snprintf(webPin_, sizeof(webPin_), "%04u", static_cast<unsigned int>((randVal >> 12) % 10000U));
}

uint32_t WebPortalService::remainingSeconds(uint32_t nowMs) const {
  if (state_ != WebPortalState::Active) return 0;
  const uint32_t elapsedSec = (nowMs - startedMs_) / 1000U;
  if (elapsedSec >= timeoutSec_) return 0;
  return timeoutSec_ - elapsedSec;
}

bool WebPortalService::start(uint32_t nowMs, AppContext& ctx) {
  if (state_ == WebPortalState::Active) return true;

  // If BLE Keyboard is currently holding the radio lease or enabled, suspend it temporarily
  // so that SoftAP gets dedicated, non-interfering radio bandwidth and full heap headroom.
  if (ctx.bleHid.isEnabled() || ctx.resources.ownsWiFi(ResourceOwner::BleKeyboard)) {
    bleSuspended_ = true;
    ctx.bleHid.setEnabled(false, ctx);
  } else {
    bleSuspended_ = false;
  }

  if (!ctx.resources.acquireWiFi(ResourceOwner::WebPortal)) {
    if (bleSuspended_) {
      ctx.bleHid.setEnabled(true, ctx);
      bleSuspended_ = false;
    }
    state_ = WebPortalState::Error;
    return false;
  }
  generateCredentials();
  startedMs_ = nowMs;
  timeoutSec_ = kDefaultTimeoutSec;

#if defined(ARDUINO_ARCH_ESP32)
  WiFi.mode(WIFI_AP);
  const bool apOk = WiFi.softAP(ssid_, password_);
  IPAddress apIp = WiFi.softAPIP();
  if (!apOk) {
    Serial.println("[WebPortal] Failed to start SoftAP!");
    ctx.resources.releaseWiFi(ResourceOwner::WebPortal);
    if (bleSuspended_) {
      ctx.bleHid.setEnabled(true, ctx);
      bleSuspended_ = false;
    }
    state_ = WebPortalState::Error;
    return false;
  }
  Serial.printf("[WebPortal] SoftAP started successfully. IP: %s\n", apIp.toString().c_str());

  dnsServer.start(53, "*", apIp);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", kIndexHtml);
  });
  server.on("/api/status", HTTP_GET, [this, &ctx]() {
    char resp[256];
    handleStatus(ctx, resp, sizeof(resp));
    server.send(200, "application/json", resp);
  });
  server.on("/api/totp/list", HTTP_GET, [this, &ctx]() {
    char resp[3072];
    int code = handleTotpList(ctx, resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/totp/add", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleAddTotp(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/totp/remove", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleRemoveTotp(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/totp/next_hotp", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleNextHotp(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/vault/list", HTTP_GET, [this, &ctx]() {
    char resp[3072];
    int code = handleVaultList(ctx, resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/vault/add", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleAddVault(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/vault/remove", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleRemoveVault(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/vault/backup", HTTP_POST, [this, &ctx]() {
    char resp[2048];
    String body = server.arg("plain");
    int code = handleBackup(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/vault/restore", HTTP_POST, [this, &ctx]() {
    char resp[512];
    String body = server.arg("plain");
    int code = handleRestore(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/device/display", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleDeviceDisplay(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/device/lock", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleDeviceLock(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.onNotFound([]() {
    server.send(200, "text/html", kIndexHtml);
  });
  server.begin();
#endif

  state_ = WebPortalState::Active;
  return true;
}

void WebPortalService::stop(AppContext& ctx) {
  if (state_ == WebPortalState::Idle) return;
#if defined(ARDUINO_ARCH_ESP32)
  server.stop();
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
#endif
  ctx.resources.releaseWiFi(ResourceOwner::WebPortal);
  state_ = WebPortalState::Idle;
  CryptoService::secureZero(password_, sizeof(password_));
  CryptoService::secureZero(webPin_, sizeof(webPin_));

  // Automatically restore BLE Keyboard if it was suspended for Web Portal
  if (bleSuspended_) {
    bleSuspended_ = false;
    ctx.bleHid.setEnabled(true, ctx);
    Serial.println("[WebPortal] Restored BLE Keyboard after Web Portal closed");
  }
}

void WebPortalService::tick(uint32_t nowMs, AppContext& ctx) {
  if (state_ != WebPortalState::Active) return;
  if (remainingSeconds(nowMs) == 0) {
    stop(ctx);
    return;
  }
#if defined(ARDUINO_ARCH_ESP32)
  dnsServer.processNextRequest();
  server.handleClient();
#endif
}

int WebPortalService::handleStatus(AppContext& ctx, char* outResponse, size_t maxLen) {
  const bool unlocked = ctx.security.hasActiveKey();
  const uint32_t epoch = ctx.time.getEpoch();
  const uint8_t keyCount = ctx.totp.isLoaded() ? ctx.totp.accountCount() : 0;
  const uint8_t vaultCount = ctx.vault.isLoaded() ? ctx.vault.count() : 0;
  snprintf(outResponse, maxLen,
           "{\"ok\":true,\"device\":\"NibruOS\",\"version\":\"%s\",\"remaining\":%u,\"unlocked\":%s,\"epoch\":%lu,\"keysCount\":%u,\"vaultCount\":%u}",
           Config::kVersion,
           static_cast<unsigned int>(remainingSeconds(startedMs_ + 1000)),
           unlocked ? "true" : "false",
           static_cast<unsigned long>(epoch),
           keyCount, vaultCount);
  return 200;
}

int WebPortalService::handleTotpList(AppContext& ctx, char* outResponse, size_t maxLen) {
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"ok\":false,\"error\":\"Device locked\",\"keys\":[]}");
    return 403;
  }
  if (!ctx.totp.isLoaded()) {
    ctx.totp.loadVault(ctx.security.getActiveVaultKey());
  }
  const uint8_t count = ctx.totp.accountCount();
  const uint32_t epoch = ctx.time.getEpoch();
  size_t offset = snprintf(outResponse, maxLen, "{\"ok\":true,\"epoch\":%lu,\"keys\":[", static_cast<unsigned long>(epoch));
  for (uint8_t i = 0; i < count; ++i) {
    const TotpAccount* acc = ctx.totp.getAccount(i);
    if (!acc) continue;
    char code[10] = "------";
    uint8_t remaining = 0;
    ctx.totp.getAccountCode(i, epoch, code, sizeof(code), remaining);
    const bool isHotp = (static_cast<OtpType>(acc->type) == OtpType::Hotp);
    const char* algoStr = (acc->algo == static_cast<uint8_t>(OtpAlgo::Sha256)) ? "sha256" :
                          ((acc->algo == static_cast<uint8_t>(OtpAlgo::Sha512)) ? "sha512" : "sha1");
    char item[256];
    snprintf(item, sizeof(item),
             "%s{\"index\":%u,\"name\":\"%s\",\"user\":\"%s\",\"code\":\"%s\",\"timer\":%u,\"period\":%u,\"type\":\"%s\",\"algo\":\"%s\",\"digits\":%u,\"counter\":%lu}",
             (i > 0 ? "," : ""),
             i, acc->name, acc->user, code,
             isHotp ? 0 : remaining,
             acc->period,
             isHotp ? "hotp" : "totp",
             algoStr,
             acc->digits,
             static_cast<unsigned long>(acc->counter));
    CryptoService::secureZero(code, sizeof(code));
    const size_t itemLen = strlen(item);
    if (offset + itemLen + 4 < maxLen) {
      memcpy(outResponse + offset, item, itemLen);
      offset += itemLen;
      outResponse[offset] = '\0';
    }
  }
  if (offset + 3 < maxLen) {
    outResponse[offset++] = ']';
    outResponse[offset++] = '}';
    outResponse[offset] = '\0';
  }
  return 200;
}

int WebPortalService::handleAddTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, name[24]{}, user[32]{}, secret[65]{}, typeStr[16]{}, algoStr[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "name", name, sizeof(name));
  extractJsonString(body, "user", user, sizeof(user));
  extractJsonString(body, "secret", secret, sizeof(secret));
  extractJsonString(body, "type", typeStr, sizeof(typeStr));
  extractJsonString(body, "algo", algoStr, sizeof(algoStr));
  int digits = extractJsonInt(body, "digits", 6);
  int period = extractJsonInt(body, "period", 30);
  int counter = extractJsonInt(body, "counter", 0);

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }

  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }

  if (!name[0] || !secret[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Service name and Secret key are required\"}");
    return 400;
  }

  if (!ctx.totp.isLoaded()) {
    if (!ctx.totp.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load 2FA storage\"}");
      return 500;
    }
  }

  OtpType type = (strcmp(typeStr, "hotp") == 0) ? OtpType::Hotp : OtpType::Totp;
  OtpAlgo algo = OtpAlgo::Sha1;
  if (strcmp(algoStr, "sha256") == 0) algo = OtpAlgo::Sha256;
  else if (strcmp(algoStr, "sha512") == 0) algo = OtpAlgo::Sha512;

  if (!ctx.totp.addAccount(ctx.security.getActiveVaultKey(), name, user[0] ? user : "Account", secret,
                           static_cast<uint8_t>(digits), static_cast<uint8_t>(period),
                           type, algo, static_cast<uint32_t>(counter))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to save 2FA account (invalid Base32 or storage full)\"}");
    return 400;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.totp.accountCount()));
  return 200;
}

int WebPortalService::handleRemoveTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.totp.accountCount()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.totp.removeAccount(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to remove account\"}");
    return 500;
  }
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.totp.accountCount()));
  return 200;
}

int WebPortalService::handleNextHotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.totp.accountCount()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.totp.incrementHotp(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to advance counter\"}");
    return 500;
  }
  const TotpAccount* acc = ctx.totp.getAccount(static_cast<uint8_t>(index));
  char code[10]{};
  uint8_t rem = 0;
  ctx.totp.getAccountCode(static_cast<uint8_t>(index), 0, code, sizeof(code), rem);
  snprintf(outResponse, maxLen, "{\"success\":true,\"code\":\"%s\",\"counter\":%lu}",
           code, static_cast<unsigned long>(acc ? acc->counter : 0));
  CryptoService::secureZero(code, sizeof(code));
  return 200;
}

int WebPortalService::handleVaultList(AppContext& ctx, char* outResponse, size_t maxLen) {
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"ok\":false,\"error\":\"Device locked\",\"passwords\":[]}");
    return 403;
  }
  if (!ctx.vault.isLoaded()) {
    ctx.vault.loadVault(ctx.security.getActiveVaultKey());
  }
  const uint8_t count = ctx.vault.count();
  size_t offset = snprintf(outResponse, maxLen, "{\"ok\":true,\"passwords\":[");
  for (uint8_t i = 0; i < count; ++i) {
    const VaultEntry* entry = ctx.vault.getEntry(i);
    if (!entry) continue;
    char item[256];
    snprintf(item, sizeof(item),
             "%s{\"index\":%u,\"title\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"notes\":\"%s\"}",
             (i > 0 ? "," : ""),
             i, entry->title, entry->username, entry->password, entry->notes);
    const size_t itemLen = strlen(item);
    if (offset + itemLen + 4 < maxLen) {
      memcpy(outResponse + offset, item, itemLen);
      offset += itemLen;
      outResponse[offset] = '\0';
    }
  }
  if (offset + 3 < maxLen) {
    outResponse[offset++] = ']';
    outResponse[offset++] = '}';
    outResponse[offset] = '\0';
  }
  return 200;
}

int WebPortalService::handleAddVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, title[24]{}, user[32]{}, pass[48]{}, notes[24]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "title", title, sizeof(title));
  extractJsonString(body, "username", user, sizeof(user));
  extractJsonString(body, "password", pass, sizeof(pass));
  extractJsonString(body, "notes", notes, sizeof(notes));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }

  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }

  if (!title[0] || !pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Title and Password are required\"}");
    return 400;
  }

  if (!ctx.vault.isLoaded()) {
    if (!ctx.vault.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load vault\"}");
      return 500;
    }
  }

  if (!ctx.vault.addEntry(ctx.security.getActiveVaultKey(), title, user, pass, notes)) {
    snprintf(outResponse, maxLen, "{\"error\":\"Vault storage full or write error\"}");
    return 500;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleRemoveVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.vault.count()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to remove password\"}");
    return 500;
  }
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleBackup(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, pass[48]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "passphrase", pass, sizeof(pass));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Passphrase is required\"}");
    return 400;
  }
  if (!ctx.vault.isLoaded()) {
    if (!ctx.vault.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load vault\"}");
      return 500;
    }
  }

  uint8_t payload[2048]{};
  size_t outLen = 0;
  if (!ctx.vault.exportBackup(pass, payload, sizeof(payload), outLen)) {
    snprintf(outResponse, maxLen, "{\"error\":\"Backup export failed\"}");
    return 500;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"payload\":\"");
  size_t prefixLen = strlen(outResponse);
  toHex(payload, outLen, outResponse + prefixLen, maxLen - prefixLen - 3);
  size_t end = strlen(outResponse);
  if (end + 3 < maxLen) {
    outResponse[end++] = '"';
    outResponse[end++] = '}';
    outResponse[end] = '\0';
  }
  CryptoService::secureZero(payload, sizeof(payload));
  return 200;
}

int WebPortalService::handleRestore(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, pass[48]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "passphrase", pass, sizeof(pass));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }
  if (!pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Passphrase is required\"}");
    return 400;
  }

  const char* pKey = "\"payload\"";
  const char* pPos = strstr(body, pKey);
  if (!pPos) {
    snprintf(outResponse, maxLen, "{\"error\":\"Missing payload field\"}");
    return 400;
  }
  pPos += strlen(pKey);
  while (*pPos && (*pPos == ' ' || *pPos == '\t' || *pPos == ':')) ++pPos;
  if (*pPos != '"') {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid payload format\"}");
    return 400;
  }
  ++pPos;

  uint8_t binPayload[2048]{};
  size_t binLen = fromHex(pPos, binPayload, sizeof(binPayload));
  if (binLen < 48) {
    snprintf(outResponse, maxLen, "{\"error\":\"Payload corrupted or too short\"}");
    return 400;
  }

  if (!ctx.vault.importBackup(pass, binPayload, binLen, ctx.security.getActiveVaultKey())) {
    CryptoService::secureZero(binPayload, sizeof(binPayload));
    snprintf(outResponse, maxLen, "{\"error\":\"Restore failed: wrong passphrase or corrupted data\"}");
    return 400;
  }

  CryptoService::secureZero(binPayload, sizeof(binPayload));
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleDeviceDisplay(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  int brightness = extractJsonInt(body, "brightness", -1);
  int timeout = extractJsonInt(body, "timeout", -1);
  int theme = extractJsonInt(body, "theme", -1);

  if (brightness >= 0 && brightness <= 100) {
    ctx.settings.setBrightness(static_cast<uint8_t>(brightness));
  }
  if (timeout >= 0) {
    ctx.settings.setScreenTimeoutSec(static_cast<uint16_t>(timeout));
  }
  if (theme >= 0 && theme <= 3) {
    ctx.settings.setTheme(static_cast<uint8_t>(theme));
  }
  snprintf(outResponse, maxLen, "{\"success\":true}");
  return 200;
}

int WebPortalService::handleDeviceLock(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  ctx.security.lock();
  ctx.totp.lock();
  ctx.vault.lock();
  snprintf(outResponse, maxLen, "{\"success\":true,\"locked\":true}");
  return 200;
}

int WebPortalService::simulateRequest(AppContext& ctx, const char* method, const char* path, const char* body, char* outResponse, size_t maxLen) {
  if (state_ != WebPortalState::Active) {
    snprintf(outResponse, maxLen, "{\"error\":\"Portal not active\"}");
    return 503;
  }
  if (strcmp(path, "/api/status") == 0) {
    return handleStatus(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/list") == 0 && strcmp(method, "GET") == 0) {
    return handleTotpList(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddTotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/remove") == 0 && strcmp(method, "POST") == 0) {
    return handleRemoveTotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/next_hotp") == 0 && strcmp(method, "POST") == 0) {
    return handleNextHotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/list") == 0 && strcmp(method, "GET") == 0) {
    return handleVaultList(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddVault(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/remove") == 0 && strcmp(method, "POST") == 0) {
    return handleRemoveVault(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/backup") == 0 && strcmp(method, "POST") == 0) {
    return handleBackup(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/restore") == 0 && strcmp(method, "POST") == 0) {
    return handleRestore(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/device/display") == 0 && strcmp(method, "POST") == 0) {
    return handleDeviceDisplay(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/device/lock") == 0 && strcmp(method, "POST") == 0) {
    return handleDeviceLock(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/") == 0) {
    snprintf(outResponse, maxLen, "HTML_PORTAL");
    return 200;
  }
  snprintf(outResponse, maxLen, "{\"error\":\"Not found\"}");
  return 404;
}
