#pragma once
#include "core/App.h"

/**
 * @brief Standard Application Interface for NibruOS.
 *
 * Applications implement the lifecycle methods defined in App / IApp:
 * - title(): Human-readable application title shown in header/tiles.
 * - shortTitle(): 3-4 character abbreviation for compact tiles/badges.
 * - subtitle(): Brief descriptive category or status text.
 * - onEnter(): Acquire resources, validate prerequisites, initialize view state.
 * - onExit(): Release acquired resources/leases, cancel background jobs, close buffers.
 * - onTick(): Non-blocking periodic processing; avoid long synchronous loops.
 * - onTouch(): Handle incoming touch gestures/taps in display coordinates.
 * - onPrimaryAction(): Optional action on short BOOT button press.
 * - onActivate(): Optional action on long BOOT button press (defaults to onPrimaryAction).
 * - onBack(): Optional nested back navigation; return true if consumed, false to exit Home.
 * - render(): Draw active screen content to TFT display when invalidated.
 * - onDisplayRestored(): Handle display waking or overlay dismissal (re-invalidates view).
 */
using IApp = App;
