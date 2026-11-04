# 1. Update docs/CURRENT_STATE.md
with open('docs/CURRENT_STATE.md', 'r', encoding='utf-8') as f:
    cs = f.read()

target_cs = '"BUG-0015"\n  ],'
if target_cs in cs:
    cs = cs.replace(target_cs, '"BUG-0015",\n    "BUG-0016"\n  ],')
elif '"BUG-0015"\r\n  ],' in cs:
    cs = cs.replace('"BUG-0015"\r\n  ],', '"BUG-0015",\r\n    "BUG-0016"\r\n  ],')

with open('docs/CURRENT_STATE.md', 'w', encoding='utf-8') as f:
    f.write(cs)
print("Updated docs/CURRENT_STATE.md with BUG-0016")

# 2. Append BUG-0016 to docs/KNOWN_ISSUES.md
with open('docs/KNOWN_ISSUES.md', 'r', encoding='utf-8') as f:
    ki = f.read()

new_bug = """
```json
{
  "record_type": "issue",
  "id": "BUG-0016",
  "title": "2FA Authenticator screen components disappear on cancelling ADD or DELETE",
  "status": "FIXED — UNVERIFIED",
  "description": "In 2FA Authenticator (TotpApp), opening the ADD or DELETE views and subsequently cancelling or pressing Back returned the user to an empty screen showing only the OTP code and seconds timer, with all other UI elements (account name, navigation arrows, bottom bar, ADD and DELETE buttons) missing.",
  "symptoms": "Upon cancelling ADD or DELETE, the screen background was wiped black and only the 6-digit TOTP code and countdown timer appeared in the center of the display.",
  "reproduction": "Open 2FA Authenticator, tap ADD (Methods view) or DELETE (ConfirmDelete view), then tap CANCEL or press Back. Observe the screen content.",
  "affected_versions": "v0.1.0 through v0.9.2",
  "suspected_cause": "Desynchronization between fullRedraw_ screen clearing and cardDirty_ component drawing.",
  "root_cause": "TotpApp::onBack() set fullRedraw_ = true but omitted cardDirty_ = true. When render() was invoked, screen.clear() wiped the entire display to black, but the card drawing block was skipped because cardDirty_ was false, leaving only the tick-driven OTP numbers and timer labels drawn.",
  "files": [
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "tests/host/test_authenticator.cpp"
  ],
  "attempts": [
    "VER-0113 HOST PASS",
    "VER-0114 SIMULATOR PASS",
    "VER-0115 BUILD PASS",
    "HWT-0035 COM5 PASS"
  ],
  "fix": "1. Enforced cardDirty_ = true in TotpApp::onBack() and onDisplayRestored. 2. Set cardDirty_ = true unconditionally inside render() whenever fullRedraw_ triggers screen.clear(). 3. Styled ConfirmDelete buttons using TextKeyboard::button. 4. Added regression assertions to test_authenticator.cpp.",
  "verification": [
    "VER-0113",
    "VER-0114",
    "VER-0115",
    "HWT-0035"
  ],
  "risks": "Minimal; dirty flag synchronization between screen clearing and view rendering."
}
```
"""

if "BUG-0016" not in ki:
    ki = ki.rstrip() + "\n" + new_bug
    with open('docs/KNOWN_ISSUES.md', 'w', encoding='utf-8') as f:
        f.write(ki)
    print("Updated docs/KNOWN_ISSUES.md with BUG-0016")
