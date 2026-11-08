#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Why a player leaves the game: Left when the player said so, TimedOut
 * when nothing was heard from the player for too long, after a crash or a loss
 * of the network, Expelled when the host of the server expelled the player,
 * Banned when the host banned the player.
 */
enum class DepartureCause : std::uint8_t {
  Left,
  TimedOut,
  Expelled,
  Banned,
};

/**
 * @brief Event published when a player leaves the game. The player number is
 * the one of the protocol, 1 to 4; the next player to join may take it.
 */
struct PlayerDeparture {
  std::uint8_t playerNumber{};
  DepartureCause cause{DepartureCause::Left};

  bool operator==(const PlayerDeparture&) const = default;
};

}  // namespace rtype::game
