#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief The filter as the player picks it: the order of a choice in
 * LEADERBOARD_DIFFICULTY_LABELS and in LEADERBOARD_PILOT_LABELS. */
struct LeaderboardFilterChoice {
  std::size_t difficulty{};
  std::size_t pilots{};

  bool operator==(const LeaderboardFilterChoice&) const = default;
};

}  // namespace rtype::client
