#pragma once
#include "core/App.h"

/**
 * @brief Standard Application Interface alias for NibruOS.
 *
 * Applications implement the lifecycle methods defined in App / IApp:
 * - onEnter(): Acquire resources, initialize view state.
 * - onExit(): Release resources, cancel background jobs.
 * - onTick(): Non-blocking periodic processing.
 * - onTouch(): Handle incoming touch gestures/taps.
 * - onPrimaryAction(): Handle short BOOT press.
 * - onActivate(): Handle long BOOT press.
 * - onBack(): Optional nested back navigation.
 * - render(): Draw active screen content.
 */
using IApp = App;
