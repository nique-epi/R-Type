#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The ship a pilot flies: its hull, NO_SHIP_LOOK_ITEM for the
 * standard one, else the number of a hull of the catalog, and its pilot
 * color, 0 to PLAYER_COLOR_COUNT - 1. */
struct PilotHull {
  std::uint8_t hull{};
  int color{};
};

}  // namespace rtype::client
