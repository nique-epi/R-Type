#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief How the server writes the world state it sends a player during a
 * game.
 */
enum class StateEncoding : std::uint8_t {
  /** @brief As the difference from the newest state the player acknowledged,
   * or in full when it acknowledged none within MAXIMUM_REFERENCE_DISTANCE
   * ticks. */
  Delta,
  /** @brief In full every time, every entity listed: kept to measure what the
   * differences save. */
  Full
};

}  // namespace rtype::server
