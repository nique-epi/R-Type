#pragma once

#include <gtest/gtest.h>
#include "GlobalLeaderboard.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "MasterLeaderboard.hpp"
#include "RecordingMasterLeaderboardReads.hpp"

namespace rtype::client::testing {

/** @brief The leaderboard of the master, over reads that answer only when the
 * test says so. */
class MasterLeaderboardFixture : public ::testing::Test {
 protected:
  /** @returns The filter the tests ask for. */
  [[nodiscard]] static network::LeaderboardRequest askedRequest();

  /** @returns What the master answers: KIRA first, NOVA second and named. */
  [[nodiscard]] static network::GlobalLeaderboard masterAnswer();

  /** @returns The rows of masterAnswer() as the screen shows them. */
  [[nodiscard]] static network::LeaderboardRow kiraShown();
  [[nodiscard]] static network::LeaderboardRow novaShown();

  [[nodiscard]] RecordingMasterLeaderboardReads& reads();
  [[nodiscard]] MasterLeaderboard& leaderboard();

 private:
  RecordingMasterLeaderboardReads reads_;
  MasterLeaderboard leaderboard_{reads_};
};

}  // namespace rtype::client::testing
