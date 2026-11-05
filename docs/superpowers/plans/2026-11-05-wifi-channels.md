# Wi-Fi channel analysis implementation plan

The owner approved a channel analysis screen showing access point counts and strongest RSSI, with manual refresh using the existing scanner.

- Add a fourteen-channel summary in WiFiService. Aggregate all completed scan records before retaining the existing twelve-entry network list. Clear summaries when a scan starts or the radio is shut down.
- Add WiFiChannelsApp as a Home app on both targets. Show cached completed results on entry, select a channel by touch or swipe, and start bounded asynchronous scans only through Refresh or BOOT.
- Reuse ResourceManager ownership and existing scan cleanup on Back. Preserve connected Wi-Fi across exit. Show busy, scanning, failed, empty and unscanned states.
- Test aggregation after record twelve, negative RSSI maxima, invalid channels, stale-result reset, exclusive ownership, refresh, cancellation and twenty host lifecycle cycles. Build the ESP32 and desktop targets and check resources. Record owner-reported Beacon Lab success separately from physical profiling.

Version v0.16.0. No GPIO, dependency, partition or persistence changes. RF results and physical A–E profiling need board evidence.
