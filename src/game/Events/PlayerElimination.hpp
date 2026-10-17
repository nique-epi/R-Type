#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when a player has no life left and now watches. The
 * player number is the one of the protocol, 1 to 4.
 */
struct PlayerElimination {
  std::uint8_t playerNumber;

  bool operator==(const PlayerElimination&) const = default;
};

}  // namespace rtype::game
