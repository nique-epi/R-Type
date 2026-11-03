#pragma once

#include <string>
#include "ILeaderboardSource.hpp"

namespace rtype::client {

/** @brief A tab of the leaderboard: its label, and where it reads its rows. */
struct LeaderboardTab {
  std::string label;
  /** @brief Must outlive the screen; null for a tab that cannot read yet, which
   * says its source is out of reach. */
  ILeaderboardSource* source{};
};

}  // namespace rtype::client
