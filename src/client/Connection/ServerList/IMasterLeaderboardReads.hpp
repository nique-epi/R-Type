#pragma once

#include <cstdint>
#include <optional>
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"

namespace rtype::client {

/**
 * @brief The read of the leaderboard of the master, without the frame ever
 * waiting on HTTP: an ask returns at once, and its reading shows up on a later
 * frame. A newer ask replaces the one before it.
 */
class IMasterLeaderboardReads {
 public:
  virtual ~IMasterLeaderboardReads() = default;

  /** @returns The number of this ask for the leaderboard @p query names. */
  virtual std::uint64_t askLeaderboard(
      const network::GlobalLeaderboardQuery& query) = 0;

  /** @returns The newest leaderboard read, nothing before the first. */
  [[nodiscard]] virtual std::optional<GlobalLeaderboardReading> leaderboard()
      const = 0;
};

}  // namespace rtype::client
