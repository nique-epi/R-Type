#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief How the map is painted this frame: whether the corners and the
 * bubble of the chosen server show on the whole map, and how far the planets
 * of the servers have turned, in frames (see PLANET_TURN_FRAME_COUNT). */
struct ServerMapLook {
  bool chosenShown{};
  std::size_t turn{};
};

}  // namespace rtype::client
