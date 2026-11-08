#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace rtype::network {

/** @brief The best score of one player in a leaderboard of the master. */
struct GlobalLeaderboardRow {
  /** @brief From 1; equal scores share a rank, and the next rank skips as
   * many as share it. */
  std::int64_t rank{};
  /** @brief The nickname the player had in the game of that score. */
  std::string nickname;
  std::uint32_t score{};
  /** @brief When that game ended, to the second. */
  std::chrono::sys_seconds playedAt;
  /** @brief The id and the name of the server the game was played on. */
  std::int64_t serverId{};
  std::string serverName;

  bool operator==(const GlobalLeaderboardRow&) const = default;
};

}  // namespace rtype::network
