#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief What an obstacle does to a ship that touches it. */
enum class ObstacleContact : std::uint8_t {
  /** @brief Stops it at its side, as an edge of the play area does. */
  Blocking,
  /** @brief Costs it a life. */
  Fatal,
};

}  // namespace rtype::game
