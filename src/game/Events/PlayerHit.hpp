#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when a player loses a life. The player number is the
 * one of the protocol, 1 to 4.
 */
struct PlayerHit {
  std::uint8_t playerNumber;

  bool operator==(const PlayerHit&) const = default;
};

}  // namespace rtype::game
