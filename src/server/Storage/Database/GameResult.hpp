#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief What one player leaves of a game. */
struct PlayerResult {
  std::string nickname;
  std::uint32_t score{};
  /** @brief The lives the player lost in the game. */
  std::uint32_t deaths{};

  bool operator==(const PlayerResult&) const = default;
};

/**
 * @brief What one finished game leaves: a row per player, the players who left
 * before its end included.
 */
struct GameResult {
  std::string difficulty;
  /** @brief The players connected when the game started. */
  std::uint8_t playerCount{};
  /** @brief When the game ended, to the second. */
  std::chrono::sys_seconds endedAt;
  std::vector<PlayerResult> players;
  /** @brief Whether the game was vanilla (see GameSettings::vanilla). */
  bool vanilla{false};
};

}  // namespace rtype::server
