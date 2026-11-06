#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How deep the player went into the map: the whole map, the map
 * zoomed on the chosen server, then the panel of its games. */
enum class ServerMapStage : std::uint8_t {
  ZoomedOut,
  ZoomedIn,
  Games,
};

}  // namespace rtype::client
