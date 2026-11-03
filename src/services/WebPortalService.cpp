#include "services/WebPortalService.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include <cstdio>
#include <cstring>

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

const char kIndexHtml[] =
"<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\">"
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">"
"<title>NibruOS Portal</title><style>"
":root{--bg:#0d1117;--card:#161b22;--border:#30363d;--accent:#00e5ff;--accent2:#00b4d8;--text:#f0f6fc;--muted:#8b949e;--success:#2ea043;--danger:#f85149;}"
"*{box-sizing:border-box;margin:0;padding:0;}"
"body{background:var(--bg);color:var(--text);font-family:-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,sans-serif;display:flex;justify-content:center;padding:16px;}"
".container{width:100%;max-width:440px;}"
".header{text-align:center;margin-bottom:16px;}"
".logo{font-size:22px;font-weight:700;color:var(--accent);letter-spacing:1px;}"
".sublogo{font-size:12px;color:var(--muted);margin-top:2px;}"
".card{background:var(--card);border:1px solid var(--border);border-radius:10px;padding:16px;margin-bottom:14px;box-shadow:0 4px 12px rgba(0,0,0,0.4);}"
".title{font-size:14px;font-weight:600;margin-bottom:10px;color:var(--accent);}"
".tabs{display:flex;gap:6px;margin-bottom:14px;}"
".tab{flex:1;padding:10px;text-align:center;font-size:13px;font-weight:600;background:var(--card);border:1px solid var(--border);border-radius:6px;cursor:pointer;color:var(--muted);transition:0.2s;}"
".tab.active{background:var(--border);color:var(--accent);border-color:var(--accent);}"
".form-group{margin-bottom:10px;}"
"label{display:block;font-size:12px;color:var(--muted);margin-bottom:4px;}"
"input{width:100%;padding:9px 12px;background:#090d13;border:1px solid var(--border);border-radius:6px;color:var(--text);font-size:14px;}"
"input:focus{outline:none;border-color:var(--accent);}"
".input-group{display:flex;gap:6px;}"
".btn{width:100%;padding:10px;border:none;border-radius:6px;font-size:14px;font-weight:600;cursor:pointer;transition:0.2s;}"
".btn-primary{background:var(--accent);color:#000;margin-top:6px;}"
".btn-primary:hover{background:var(--accent2);}"
".btn-sec{padding:0 12px;background:var(--border);color:var(--text);font-size:12px;}"
".status{padding:10px;border-radius:6px;font-size:13px;text-align:center;margin-bottom:12px;display:none;}"
".status.success{background:rgba(46,160,67,0.2);color:var(--success);border:1px solid var(--success);display:block;}"
".status.error{background:rgba(248,81,73,0.2);color:var(--danger);border:1px solid var(--danger);display:block;}"
".timer{text-align:center;font-size:12px;color:var(--muted);margin-top:12px;}"
"</style></head><body><div class=\"container\">"
"<div class=\"header\"><div class=\"logo\">NibruOS Portal</div>"
"<div class=\"sublogo\">بوابة التحكم بالأجهزة وإدارة الحسابات</div></div>"
"<div id=\"status\" class=\"status\"></div>"
"<div class=\"card\"><div class=\"form-group\" style=\"margin-bottom:0;\">"
"<label>Web PIN (Shown on device screen / رمز الجلسة):</label>"
"<input type=\"password\" id=\"pin\" maxlength=\"8\" placeholder=\"Enter 4-digit PIN\" style=\"font-weight:700;letter-spacing:4px;text-align:center;\">"
"</div></div>"
"<div class=\"tabs\">"
"<div class=\"tab active\" onclick=\"switchTab('vault')\">🔑 Vault Passwords</div>"
"<div class=\"tab\" onclick=\"switchTab('totp')\">🛡️ 2FA Account</div>"
"</div>"
"<div id=\"tab-vault\" class=\"card\">"
"<div class=\"title\">Add Account to Vault (إضافة كلمة سر للخزنة)</div>"
"<div class=\"form-group\"><label>Title (e.g. Google, GitHub, Bank):</label>"
"<input type=\"text\" id=\"v_title\" maxlength=\"23\" placeholder=\"Account Name\"></div>"
"<div class=\"form-group\"><label>Username / Email:</label>"
"<input type=\"text\" id=\"v_user\" maxlength=\"31\" placeholder=\"user@example.com\"></div>"
"<div class=\"form-group\"><label>Password:</label><div class=\"input-group\">"
"<input type=\"password\" id=\"v_pass\" maxlength=\"47\" placeholder=\"Secret password\">"
"<button type=\"button\" class=\"btn btn-sec\" onclick=\"togglePass()\">👁️</button>"
"<button type=\"button\" class=\"btn btn-sec\" onclick=\"genPass()\">🎲</button></div></div>"
"<div class=\"form-group\"><label>Notes (Optional):</label>"
"<input type=\"text\" id=\"v_notes\" maxlength=\"23\" placeholder=\"Personal account\"></div>"
"<button class=\"btn btn-primary\" onclick=\"submitVault()\">Save to Device Vault</button></div>"
"<div id=\"tab-totp\" class=\"card\" style=\"display:none;\">"
"<div class=\"title\">Add 2FA Account (إضافة مصادقة ثنائية)</div>"
"<div class=\"form-group\"><label>Service Name (e.g. AWS, Discord):</label>"
"<input type=\"text\" id=\"t_name\" maxlength=\"23\" placeholder=\"Service Name\"></div>"
"<div class=\"form-group\"><label>Base32 Secret Key:</label>"
"<input type=\"text\" id=\"t_secret\" maxlength=\"52\" placeholder=\"e.g. JBSWY3DPEHPK3PXP\" style=\"text-transform:uppercase;\"></div>"
"<button class=\"btn btn-primary\" onclick=\"submitTotp()\">Save 2FA to Device</button></div>"
"<div class=\"timer\" id=\"timer\">Device IP: 192.168.4.1 | Session Active</div></div>"
"<script>"
"function switchTab(t){"
"document.querySelectorAll('.tab').forEach((el,i)=>el.classList.toggle('active',(t==='vault'&&i===0)||(t==='totp'&&i===1)));"
"document.getElementById('tab-vault').style.display=t==='vault'?'block':'none';"
"document.getElementById('tab-totp').style.display=t==='totp'?'block':'none';}"
"function togglePass(){const p=document.getElementById('v_pass');p.type=p.type==='password'?'text':'password';}"
"function genPass(){const chars='abcdefghjkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789!@#$%^&*';"
"let s='';for(let i=0;i<16;i++)s+=chars.charAt(Math.floor(Math.random()*chars.length));"
"const p=document.getElementById('v_pass');p.type='text';p.value=s;}"
"function showStatus(msg,isErr){const el=document.getElementById('status');el.textContent=msg;"
"el.className='status '+(isErr?'error':'success');window.scrollTo({top:0,behavior:'smooth'});}"
"async function submitVault(){"
"const pin=document.getElementById('pin').value.trim();"
"const title=document.getElementById('v_title').value.trim();"
"const user=document.getElementById('v_user').value.trim();"
"const pass=document.getElementById('v_pass').value;"
"const notes=document.getElementById('v_notes').value.trim();"
"if(!pin){showStatus('Please enter the Web PIN shown on device',true);return;}"
"if(!title||!pass){showStatus('Title and Password are required',true);return;}"
"try{const res=await fetch('/api/vault/add',{method:'POST',headers:{'Content-Type':'application/json'},"
"body:JSON.stringify({pin,title,username:user,password:pass,notes})});"
"const d=await res.json();if(res.ok&&d.success){"
"showStatus('Account saved to NibruOS Vault successfully! ('+title+')',false);"
"document.getElementById('v_title').value='';"
"document.getElementById('v_user').value='';"
"document.getElementById('v_pass').value='';"
"document.getElementById('v_notes').value='';}"
"else{showStatus(d.error||'Failed to save account',true);}}"
"catch(e){showStatus('Connection error: '+e.message,true);}}"
"async function submitTotp(){"
"const pin=document.getElementById('pin').value.trim();"
"const name=document.getElementById('t_name').value.trim();"
"const secret=document.getElementById('t_secret').value.trim().toUpperCase();"
"if(!pin){showStatus('Please enter the Web PIN shown on device',true);return;}"
"if(!name||!secret){showStatus('Service name and Secret key are required',true);return;}"
"try{const res=await fetch('/api/totp/add',{method:'POST',headers:{'Content-Type':'application/json'},"
"body:JSON.stringify({pin,name,secret})});"
"const d=await res.json();if(res.ok&&d.success){"
"showStatus('2FA account saved to NibruOS successfully! ('+name+')',false);"
"document.getElementById('t_name').value='';"
"document.getElementById('t_secret').value='';}"
"else{showStatus(d.error||'Failed to save 2FA account',true);}}"
"catch(e){showStatus('Connection error: '+e.message,true);}}"
"</script></body></html>";

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
  if (!ctx.resources.acquireWiFi(ResourceOwner::WebPortal)) {
    state_ = WebPortalState::Error;
    return false;
  }
  generateCredentials();
  startedMs_ = nowMs;
  timeoutSec_ = kDefaultTimeoutSec;

#if defined(ARDUINO_ARCH_ESP32)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_, password_);
  IPAddress apIp = WiFi.softAPIP();

  dnsServer.start(53, "*", apIp);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", kIndexHtml);
  });
  server.on("/api/status", HTTP_GET, [this, &ctx]() {
    char resp[128];
    handleStatus(ctx, resp, sizeof(resp));
    server.send(200, "application/json", resp);
  });
  server.on("/api/vault/add", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleAddVault(ctx, body.c_str(), resp, sizeof(resp));
    server.send(code, "application/json", resp);
  });
  server.on("/api/totp/add", HTTP_POST, [this, &ctx]() {
    char resp[128];
    String body = server.arg("plain");
    int code = handleAddTotp(ctx, body.c_str(), resp, sizeof(resp));
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
  (void)ctx;
  snprintf(outResponse, maxLen, "{\"ok\":true,\"device\":\"NibruOS\",\"remaining\":%u}",
           static_cast<unsigned int>(remainingSeconds(startedMs_ + 1000)));
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

int WebPortalService::handleAddTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, name[24]{}, user[32]{}, secret[53]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "name", name, sizeof(name));
  extractJsonString(body, "user", user, sizeof(user));
  extractJsonString(body, "secret", secret, sizeof(secret));

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

  if (!ctx.totp.addAccount(ctx.security.getActiveVaultKey(), name, user[0] ? user : "Account", secret)) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to save 2FA account (invalid Base32 or storage full)\"}");
    return 400;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.totp.accountCount()));
  return 200;
}

int WebPortalService::simulateRequest(AppContext& ctx, const char* method, const char* path, const char* body, char* outResponse, size_t maxLen) {
  if (state_ != WebPortalState::Active) {
    snprintf(outResponse, maxLen, "{\"error\":\"Portal not active\"}");
    return 503;
  }
  if (strcmp(path, "/api/status") == 0) {
    return handleStatus(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddVault(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddTotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/") == 0) {
    snprintf(outResponse, maxLen, "HTML_PORTAL");
    return 200;
  }
  snprintf(outResponse, maxLen, "{\"error\":\"Not found\"}");
  return 404;
}
