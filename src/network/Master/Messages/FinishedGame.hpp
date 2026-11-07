#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
#include "FinishedGamePlayer.hpp"

namespace rtype::network {

/**
 * @brief What a game server reports of one game that ended: the body of
 * `PUT /api/servers/me/results/{gameId}`, whose path names the game by the id
 * the server drew.
 */
struct FinishedGame {
  /** @brief When the game ended, to the second. */
  std::chrono::sys_seconds playedAt;
  /** @brief The name of the difficulty it was played at. */
  std::string difficulty;
  /** @brief The players connected when it started, 1 to MAXIMUM_PLAYERS. */
  std::uint8_t playerCount{};
  /** @brief Whether it was vanilla; the master keeps no other game. */
  bool vanilla{false};
  /** @brief A row per player, 1 to MAXIMUM_PLAYERS, those who left before its
   * end included. */
  std::vector<FinishedGamePlayer> players;

  bool operator==(const FinishedGame&) const = default;
};

}  // namespace rtype::network
