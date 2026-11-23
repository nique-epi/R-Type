#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace rtype::network {

/** @brief The total of the scores of one account in the leaderboard of the
 * accounts of the master: a row of `GET /api/leaderboard/accounts`. */
struct AccountLeaderboardRow {
  /** @brief From 1; equal totals share a rank, and the next rank skips as
   * many as share it. */
  std::int64_t rank{};
  /** @brief The nickname of the account, whatever nickname it played under. */
  std::string nickname;
  /** @brief The sum of the scores of every game credited to the account. */
  std::int64_t score{};
  /** @brief When the last of those games ended, to the second. */
  std::chrono::sys_seconds lastPlayedAt;

  bool operator==(const AccountLeaderboardRow&) const = default;
};

}  // namespace rtype::network
