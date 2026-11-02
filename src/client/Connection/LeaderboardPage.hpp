#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPhase.hpp"

namespace rtype::client {

/** @brief What a leaderboard shows now. The rows and the own row are empty
 * until the phase is Answered. */
struct LeaderboardPage {
  LeaderboardPhase phase{};
  /** @brief The best scores, the best first. */
  // NOLINTNEXTLINE(readability-redundant-member-init)
  std::vector<network::LeaderboardRow> rows{};
  /** @brief The row of the player, whether or not it is among them. */
  std::optional<network::LeaderboardRow> ownRow = std::nullopt;
  /** @brief Changes whenever anything else here changes, so a reader rebuilds
   * what it shows only then. */
  std::uint32_t revision{};
};

}  // namespace rtype::client
