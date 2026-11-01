# Account entry: manual, USB and Wi-Fi

FEAT-0014 / BUG-0011 / DEV-0025. This extends the existing authenticator;
there is no separate app, new GPIO, network, task or firmware dependency.
[Software evidence](VERIFICATION_REPORT.md) is separate from pending
[physical acceptance HWT-0018](HARDWARE_TEST_LOG.md) and the
[resource report](performance/ACCOUNT_ENTRY_RESOURCE_IMPACT.md).

## Use

The Base32 secret is the setup key provided by the account's website, often
under "Can't scan QR?". It is not the six-digit rotating login code. Keep its
full value; do not shorten it or put real keys into chat or diagnostic logs.

1. Configure/unlock the device PIN, then open 2FA > ADD.
2. TYPE ON DEVICE uses the standard number/QWERTY rows, Shift, symbols,
   Delete and Space. Names are up to 23 printable ASCII characters; Base32
   accepts A–Z/2–7 and remains masked. Manual defaults are SHA1/6 digits/30 s.
3. To paste, run `tools\open_account_setup.bat`, or run
   `python tools/account_setup.py`. The local page opens on this computer;
   keep the helper running. Python 3 and a browser with WebCrypto are required.
   `--no-browser` and `--port 0` are supported. No package installation needed.
4. Choose USB or Wi-Fi both on the device and in the page. Paste a setup key
   or `otpauth://totp/...` URI, supply a short English account label, and type
   the temporary 16-character pairing code displayed by the device. This code
   is separate from the device PIN and expires after two minutes.
5. USB: use Chrome or Edge with Web Serial, a data cable and the ESP32 serial
   port. Close serial monitors first. Grant the browser port selection yourself.
   If opening the port resets the board, reopen ADD > USB and use its new code.
   Serial import works in the default Serial/JTAG firmware; optional keyboard
   HID firmware is not needed. Busy/disconnected ports can be retried.
6. Wi-Fi: connect the device first through Wi-Fi Hub. Computer and device must
   share a reachable private local network. Copy the displayed device IPv4
   address into the page. Guest isolation/firewalls may block UDP port 8766.
   The helper relays encrypted packets; it never receives the plaintext key.
7. Send, then review the account name and metadata on the physical device and
   tap SAVE. Transmission success is not a saved-account acknowledgement.
   Only SAVE commits to the existing encrypted vault. Back cancels. Failed
   storage keeps the candidate for retry until the session deadline.

The helper accepts SHA1 TOTP with 6/8 digits and 30/60-second periods, up to
32 decoded secret bytes and 8 total accounts. Unsupported URI algorithms,
HOTP, repeated critical parameters, malformed/noncanonical Base32 and long
names are rejected. Pasted whitespace/hyphens and optional trailing padding
are normalized. Camera/QR scanning, Google migration QR and backup/export are
not implemented. Synchronize time through Wi-Fi for actual codes afterward.

## Session and protocol boundaries

`AccountImportService` is shared through AppContext on both targets. TotpApp
opens a receive channel only during an explicit, unlocked ADD session. Wi-Fi
requires the existing connected WiFiService and the AuthenticatorImport lease;
it never starts an AP, scan, or connection. A single native WiFiUDP socket
exists while receiving. Serial is read only during explicit USB receive.

The one-use pairing code has 80 random bits (16 Base32 characters). SHA256
derives an ephemeral AES256-GCM key; the browser creates a fresh 96-bit nonce
and a 128-bit authentication tag, with AAD `NibruOS/account-import/v1`.
The ASCII envelope is `NI1:<nonce hex>:<ciphertext hex>:<tag hex>\n`.
Authenticated plaintext is `name\tsecret\tdigits\tperiod` (96 bytes maximum).
Names, canonical Base32, decoded capacity and metadata are validated again by
the firmware. Frame storage is bounded at 256 bytes, receive work at 64 bytes
per tick, and eight malformed/authentication failures terminate the session.
An overflowing serial frame is drained to newline before another is parsed.

Only one authenticated candidate is accepted. The socket closes and pairing
key/code/frame are wiped at Review; the candidate is never automatically saved.
SAVE requires a later physical contact (350 ms guard) and checks the absolute
120 s deadline even when touch dispatch precedes the regular service tick.
Cancel, Back, app exit, lock, display sleep and expiry wipe session/candidate
buffers and release the lease. Wi-Fi disconnection aborts receive. The explicit
session keeps the display awake for its bounded duration. No network response
contains a key or token, and there is no remote account-read/delete API.

The trusted first-party companion is served only on 127.0.0.1. It validates
Host/Origin and a random CSRF token, bounds request bodies, restricts the relay
to private IPv4 destinations and encrypted frames, and serves an exact static
route allowlist. The page has a restrictive CSP and no CDN, analytics, cloud,
localStorage or account-data files. The Python helper logs neither request
bodies nor pasted secrets. The browser clears secret/pairing fields after send
or explicit Clear, including a failing USB close. Typed plaintext byte buffers
are zeroed; JavaScript strings/garbage collection cannot promise complete RAM
erasure. The user still must trust their computer/browser and protect the
pairing code. This is not TLS or a security certification.

## Reproducible checks

- Host: `CXX=... python tools/run_host_tests.py` on Windows uses real CNG for
  crypto interoperability and tests bounds, tag/key rejection, rollover,
  timeout-before-SAVE, storage faults, sleep and at least 20 import cycles.
- Desktop: `tools\run_desktop_tests.bat --smoke` exercises the same QWERTY UI
  and paired receive/physical-confirmation logic with injected transports.
- Helper: `python -m unittest discover -s tests/tools -v` and
  `node --test tests/tools/account_setup.test.mjs tests/tools/account_setup_ui.test.mjs`.
  Run the host suite first to generate the public CNG envelope fixture.
- Physical HWT-0018: use a disposable public test fixture only; verify manual
  touch, USB/Web Serial, Wi-Fi, cancel/expiry/lock/sleep/disconnect, persistence
  after restart and all profiling scenarios A–E (>=20 cycles). Native USB/UDP
  and heap/stack/PSRAM/latency/power remain NOT VERIFIED / NOT MEASURED.
