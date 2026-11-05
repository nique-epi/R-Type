#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when a player takes a place in the game. The player
 * number is the one of the protocol, 1 to 4, the one PlayerDeparture names
 * when the same player leaves.
 */
struct PlayerArrival {
  std::uint8_t playerNumber{};

  bool operator==(const PlayerArrival&) const = default;
};

}  // namespace rtype::game
