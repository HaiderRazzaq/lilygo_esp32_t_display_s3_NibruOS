# Verification evidence

Store small sanitized logs needed to support meaningful outcomes. No credentials,
tokens, Wi-Fi passwords, real SSIDs/BSSIDs, account data or TOTP seeds. Keep
generated binaries, build folders and giant routine logs out of Git.

- [2026-11-01 target build](2026-11-01-target-build.txt): captured complete fresh
  build output; local account paths replaced with `<USER>`. Supports VER-0001.
- [2026-11-01 environment](2026-11-01-environment.txt): observed host-compiler
  failure and cleanup-lock excerpts transcribed from command results. Supports
  VER-0002 and VER-0004; not a firmware or device failure log.
- [Documentation validation](2026-11-01-documentation.txt): recorded validation
  command output once run; supports VER-0003.

Physical evidence must identify the image, board/modules/pins, power, procedure,
observation, operator/provenance and limitations. Imported owner reports have
less provenance than captured current-image serial logs; never hide this gap.
Do not edit a saved observation to turn failure into success. Append a new record
and evidence file for retests or factual corrections.
