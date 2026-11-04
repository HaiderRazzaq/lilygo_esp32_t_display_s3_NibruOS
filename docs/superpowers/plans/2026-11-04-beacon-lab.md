# Beacon Lab Implementation Plan

**Goal:** Broadcast synthetic Wi-Fi names for a bounded lab session on ESP32-S3.

**Architecture:** BeaconLabApp controls the selector and the session. WiFiService owns ESP32 Wi-Fi calls, and ResourceManager excludes concurrent radio owners. Desktop runs the same UI with no radio transmission.

- [ ] Test all seven selector positions, radio exclusion, stop, timeout and exit in the desktop runner.
- [ ] Add WiFiService start, tick and stop methods for a single channel and a ten-second session. Use a fixed-size frame and send at most one beacon per scheduled tick.
- [ ] Register BeaconLabApp on ESP32 and desktop, preserving the existing app positions. Stop on lock or sleep.
- [ ] Version as v0.13.0 and run desktop, host, documentation, ESP32 build and resource checks. Hardware acceptance requires a second receiver in an isolated lab.
