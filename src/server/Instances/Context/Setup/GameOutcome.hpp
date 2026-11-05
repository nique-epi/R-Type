#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief How a game that ends by itself ended: its players beat every wave of
 * the level, or none of them has a life left.
 */
enum class GameOutcome : std::uint8_t {
  Victory,
  Defeat,
};

}  // namespace rtype::server
