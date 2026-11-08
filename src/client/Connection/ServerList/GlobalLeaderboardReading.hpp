#pragma once

#include <cstdint>
#include <optional>
#include "GlobalLeaderboard.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for its leaderboard. */
struct GlobalLeaderboardReading {
  /** @brief The number of the ask it answers (see MasterAsks). */
  std::uint64_t ask{};
  /** @brief The rows asked for and the row of the player; nothing when the
   * master did not answer, refused, or answered something it could not read.
   */
  std::optional<network::GlobalLeaderboard> leaderboard;
};

}  // namespace rtype::client
