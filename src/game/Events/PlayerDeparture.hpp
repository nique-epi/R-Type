#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Why a player leaves the game: Left when the player said so, TimedOut
 * when nothing was heard from the player for too long, after a crash or a loss
 * of the network.
 */
enum class DepartureCause : std::uint8_t {
  Left,
  TimedOut,
};

/**
 * @brief Event published when a player leaves the game. The player number is
 * the one of the protocol, 1 to 4; the next player to join may take it.
 */
struct PlayerDeparture {
  std::uint8_t playerNumber;
  DepartureCause cause;

  bool operator==(const PlayerDeparture&) const = default;
};

}  // namespace rtype::game
