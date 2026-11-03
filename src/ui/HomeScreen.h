#pragma once
#include <stdint.h>

struct AppContext;
class App;

/**
 * @brief Home Launcher UI rendering component.
 *
 * Handles 2-card and 3-card paginated app tile rendering, badges, selection
 * indicators, and header/footer decoration for the Home screen.
 */
class HomeScreen {
public:
  static void draw(AppContext& ctx, App* const* apps, uint8_t count, uint8_t selected, bool armed = false);
};
