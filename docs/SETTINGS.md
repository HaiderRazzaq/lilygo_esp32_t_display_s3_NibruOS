# Essential Settings v0.1

Implemented as the existing SettingsApp in Home (runtime firmware version stays
v0.3). Seven menu rows, three visible at a time: Display, Accent, System/memory,
Wi-Fi diagnostics, Device, existing Security and Touch Test. Swipe vertically
scrolls. BOOT short selects next, hold activates; GPIO14/header Back cancels a
confirmation, then returns to menu, then Home. Existing Security/PIN screens are
retained, not newly introduced by this scope; their keypad remains touch-based.

Display supports 25/50/75/100%, 30s/1m/3m/5m/Never timeout and immediate sleep.
BOOT cycles the 10 controls; focused border and active fill are distinct.
Theme provides four accents, applied immediately to shared headers and selected
Home card. System/memory has two pages (swipe or BOOT), sampled at 1 Hz with
bounded formatting; chip, CPU, image/flash, uptime, reset, internal heap and PSRAM
values come from real target APIs. Host page is explicitly unavailable/mock.
Wi-Fi diagnostics reads the single WiFiService; it starts no scans, acquires no
lease, and shows scan/link states, duration, attempt, count, owner and error.
Earlier scan timeout remains physically unverified (BUG-0001).

## Persistence and reset scope

SettingsService owns brightness, timeout and accent in NVS `nibru_settings`.
Schema 1 is a single 5-byte `config` blob: version, brightness, timeout little
endian (2 bytes), theme. This avoids mixed records after interrupted multi-key
updates. Values are validated. The older same-namespace keys `bright`, `timeout`,
`theme`, or `nibru_cfg` keys `bright`, `sleep_sec` are imported when no current
record exists; source legacy keys remain untouched. Unknown schema or malformed
record loads visible defaults, refuses ordinary writes and displays NVS error;
only a confirmed explicit reset may replace it. A failed current NVS open is visible and write-protected until reload/reset; an empty destination does not shadow legacy preferences. Invalid schema-1 values fall
back to defaults per field. Failed writes preserve previous runtime values and
show an error. Unchanged selections do not write. Preferences handles always
close; no app opens its own preferences handle for these controls.

Reset confirms and writes only this user preference blob. It preserves all
Wi-Fi credentials, security/PIN state, application/vault secrets, firmware,
partition table, unrelated NVS keys/namespaces and MicroSD contents. No erase,
format, Preferences.clear or factory reset is used. Confirmation defaults to
Cancel; Back cancels. Touch confirmation requires a fresh contact identity whose press starts at least 500ms after opening, so held/late SINGLE_CLICK reports and raw double taps cannot confirm. Reboot confirms, shuts down
shared Wi-Fi, releases the scanner token, locks an enabled security session,
flushes serial and calls ESP.restart(), with no artificial UI delay.

## Inactivity and wake

Unsigned millis arithmetic handles timeout rollover. Sleep turns backlight off;
no Deep Sleep, power rail change or driver reinitialization. Wake restores the
configured brightness. A wake input is consumed, including held-button release
and subsequent gestures, until the touch contact and all buttons are released and inputs are quiet
for 350ms; the wake cannot directly activate reboot/reset or other app actions.

## Backlight implementation decision

GPIO38 default remains the repository's AW9364 pulse driver. Existing changelog
reports PWM incompatibility, but current physical step behavior still requires
new evidence. To satisfy the requested Arduino-ESP32 2.x LEDC implementation
without replacing that path, `lilygo-t-display-s3-ledc` opts into channel0,
5kHz/8-bit, `ledcSetup/ledcAttachPin/ledcWrite`. Failed LEDC setup falls back to
visible HIGH (LOW while sleeping). GPIO15 remains HIGH. This alternative is
build/mock checked only and **not accepted for deployment on AW9364** without
physical electrical/brightness verification. No LEDC 3.x API or dependency
upgrade is used. See the official [LEDC migration notes](https://github.com/espressif/arduino-esp32/blob/master/docs/en/migration_guides/2.x_to_3.0.rst).

## Deferred scope

No new BLE settings, SD/file manager, battery percentage, deep sleep, OTA/update,
PIN/lock feature, network manager, gesture/orientation setting, animation engine,
backup or advanced time/zone feature is added. Some security/network/time code
predates this task and is retained. No nonfunctional placeholder rows were added.

Run [profiling](MEMORY_PROFILING.md), [hardware procedure](HARDWARE_TEST_LOG.md)
and [resource report](performance/SETTINGS_RESOURCE_IMPACT.md) before hardware
acceptance. Build/host PASS never establishes physical dimming, reception,
latency, heap headroom or power consumption.

## Touch-repeat and repaint repair — BUG-0008

A contact emits at most one semantic gesture. NONE/Down waits for gesture
classification or release, so swipes are not mistaken for menu taps. Repeated
SINGLE_CLICK/Contact reports are suppressed; Up may carry the final gesture.
A fresh Down phase recovers from a missed/coalesced Up. Raw contact still wakes
the screen immediately, with wake action consumption preserved. Duplicate/orphan
release frames cannot create a fresh tap; controller timing needs device recheck.
Touch events carry start time, so a confirm press begun during the500ms rejection
window stays rejected even if its classified event arrives later.

Same-page controls redraw only changed values/focus; identical selections draw
nothing. View transitions and restoration after the PIN overlay repaint the full
page once using the App display-restoration hook. Diagnostic text is padded and
painted opaquely instead of blanking whole panels. Existing PIN keypad is painted
once per view. No sprite/framebuffer or graphics framework was introduced.
The owner's original physical FAIL is retained as HWT-0010; software tests/builds
pass, but repaired runtime acceptance is HWT-0012, NOT RUN. See
[resource impact](performance/TOUCH_FIX_RESOURCE_IMPACT.md).
