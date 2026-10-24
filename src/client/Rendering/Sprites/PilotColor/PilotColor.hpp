#pragma once

namespace rtype::client {

/** @returns Whether a color is one of the colors of ship, 0 to
 * PLAYER_COLOR_COUNT - 1. */
[[nodiscard]] bool isPilotColor(int color);

}  // namespace rtype::client
