const encoder = new TextEncoder();
const AAD = encoder.encode('NibruOS/account-import/v1');

export function normalizeSecret(value) {
  let secret = value.toUpperCase().replace(/[\s-]/g, '');
  if (!/^[A-Z2-7]+={0,6}$/.test(secret)) throw new Error('مفتاح Base32 غير صالح. انسخ مفتاح الإعداد، وليس رمز الدخول المؤقت.');
  secret = secret.replace(/=+$/, '');
  if (secret.length > 52 || [1, 3, 6].includes(secret.length % 8)) throw new Error('طول المفتاح غير مدعوم (حتى 32 بايت).');
  let buffer = 0, bits = 0, bytes = 0;
  for (const c of secret) {
    buffer = ((buffer << 5) | 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567'.indexOf(c)) >>> 0;
    bits += 5;
    if (bits >= 8) { bits -= 8; bytes++; }
  }
  if (!bytes || bytes > 32 || (bits && (buffer & ((1 << bits) - 1)))) throw new Error('المفتاح ناقص أو يحتوي نهاية غير صحيحة.');
  return secret;
}
export function parseAccount(name, value) {
  let secret = value.trim(), digits = 6, period = 30;
  if (/^otpauth:/i.test(secret)) {
    let url;
    try { url = new URL(secret); } catch { throw new Error('رابط إعداد المصادقة غير صالح.'); }
    if (url.protocol !== 'otpauth:' || url.hostname !== 'totp' || url.hash || url.username || url.password) throw new Error('يدعم هذا الإصدار روابط TOTP فقط.');
    for (const key of ['secret', 'digits', 'period', 'algorithm']) {
      if (url.searchParams.getAll(key).length > 1) throw new Error('الرابط يحتوي إعدادات مكررة.');
    }
    if ((url.searchParams.get('algorithm') || 'SHA1').toUpperCase() !== 'SHA1') throw new Error('الخوارزمية المدعومة حاليًا SHA1 فقط؛ لم يتم تغيير إعداد حسابك.');
    const d = url.searchParams.get('digits') || '6', p = url.searchParams.get('period') || '30';
    if (!['6', '8'].includes(d) || !['30', '60'].includes(p)) throw new Error('يدعم هذا الإصدار 6 أو 8 أرقام وفترة 30 أو 60 ثانية.');
    digits = Number(d); period = Number(p);
    if (!name.trim()) {
      try { name = decodeURIComponent(url.pathname.slice(1)); } catch { throw new Error('اسم الحساب داخل الرابط غير صالح.'); }
    }
    secret = url.searchParams.get('secret') || '';
  }
  name = name.trim();
  if (!/^[\x20-\x7e]{1,23}$/.test(name)) throw new Error('اكتب اسمًا إنجليزيًا للحساب، حتى 23 حرفًا.');
  return {name, secret: normalizeSecret(secret), digits, period};
}
export async function makeFrame(account, pairing, nonce = globalThis.crypto.getRandomValues(new Uint8Array(12))) {
  pairing = pairing.toUpperCase().replace(/[\s-]/g, '');
  if (!/^[A-Z2-7]{16}$/.test(pairing)) throw new Error('اكتب رمز الاقتران المؤقت الظاهر على شاشة الجهاز (16 حرفًا).');
  if (nonce.length !== 12) throw new Error('Invalid nonce length');
  const material = await crypto.subtle.digest('SHA-256', encoder.encode(pairing));
  const key = await crypto.subtle.importKey('raw', material, 'AES-GCM', false, ['encrypt']);
  new Uint8Array(material).fill(0);
  const plain = encoder.encode(account.name + '\t' + account.secret + '\t' + account.digits + '\t' + account.period);
  let encrypted;
  try { encrypted = new Uint8Array(await crypto.subtle.encrypt({name: 'AES-GCM', iv: nonce, additionalData: AAD, tagLength: 128}, key, plain)); }
  finally { plain.fill(0); }
  const hex = bytes => Array.from(bytes, b => b.toString(16).padStart(2, '0')).join('');
  return 'NI1:' + hex(nonce) + ':' + hex(encrypted.slice(0, -16)) + ':' + hex(encrypted.slice(-16)) + '\n';
}

if (typeof document !== 'undefined') {
  const element = id => document.getElementById(id);
  const status = element('status');
  let port = null;
  const clearFields = () => { element('secret').value = element('pairing').value = ''; element('secret').type = 'password'; };
  element('mode').addEventListener('change', () => { element('wifiFields').hidden = element('mode').value !== 'wifi'; });
  element('reveal').addEventListener('click', () => { element('secret').type = element('secret').type === 'password' ? 'text' : 'password'; });
  element('close').addEventListener('click', async () => {
    clearFields();
    const closing = port; port = null;
    try {
      if (closing) await closing.close();
      status.textContent = 'تم إغلاق الاتصال ومسح الحقول. ارجع من شاشة الاستيراد على الجهاز.';
    } catch { status.textContent = 'تم مسح الحقول. تعذر إغلاق المنفذ؛ افصل الكابل عند الحاجة وألغِ الاستيراد من الجهاز.'; }
  });
  element('send').addEventListener('click', async () => {
    element('send').disabled = true;
    element('close').disabled = true;
    try {
      const account = parseAccount(element('name').value, element('secret').value);
      let frame;
      if (element('mode').value === 'usb') {
        if (!('serial' in navigator)) throw new Error('افتح الصفحة في Chrome أو Edge على الكمبيوتر لاستخدام USB.');
        if (!port) {
          const selected = await navigator.serial.requestPort({filters: [{usbVendorId: 0x303a}]});
          await selected.open({baudRate: 115200});
          port = selected;
        }
        // Opening a port can reset some devices. Recheck the pairing screen if
        // the device resets; a new session deliberately rejects this envelope.
        frame = await makeFrame(account, element('pairing').value);
        const writer = port.writable.getWriter();
        try { await writer.write(encoder.encode(frame)); } finally { writer.releaseLock(); }
      } else {
        frame = await makeFrame(account, element('pairing').value);
        const response = await fetch('/relay', {method: 'POST', headers: {'Content-Type': 'application/json', 'X-Nibru-CSRF': document.querySelector('meta[name=csrf]').content},
          body: JSON.stringify({ip: element('ip').value.trim(), frame})});
        if (!response.ok) throw new Error('تعذر الإرسال. تحقق من عنوان الجهاز واتصال الشبكة المحلية.');
      }
      clearFields();
      status.textContent = 'أُرسلت البيانات المشفّرة. تحقق من اسم الحساب على شاشة NibruOS واضغط SAVE هناك. الإرسال لا يعني أن الحساب حُفظ.';
    } catch (error) {
      if (port && element('mode').value === 'usb') {
        const failed = port; port = null;
        try { await failed.close(); } catch { /* Clear action remains available. */ }
      }
      status.textContent = error.message || 'تعذر الإرسال؛ تحقق من الاتصال وأعد المحاولة.';
    }
    finally { element('send').disabled = false; element('close').disabled = false; }
  });
}
