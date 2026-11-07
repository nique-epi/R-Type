#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "HostedDifficulties.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::server::HostedDifficulties;

/**
 * Given difficulties nothing was kept in
 * When they are asked for "normal"
 * Then they do not hold it: a supervisor that has not started refuses every
 * difficulty
 */
TEST(HostedDifficulties, HoldNoNameBeforeAnyIsKept) {
  const HostedDifficulties difficulties;

  EXPECT_FALSE(difficulties.contains(SHIPPED_DEFAULT_DIFFICULTY));
}

/**
 * Given the names "easy", "hard" and "normal" kept
 * When they are asked for "hard"
 * Then they hold it
 */
TEST(HostedDifficulties, HoldANameTheyKept) {
  HostedDifficulties difficulties;
  difficulties.keep(std::vector<std::string>{SHIPPED_DIFFICULTIES.begin(),
                                             SHIPPED_DIFFICULTIES.end()});

  EXPECT_TRUE(difficulties.contains(CHOSEN_DIFFICULTY));
}

/**
 * Given the names "easy", "hard" and "normal" kept
 * When they are asked for "nightmare"
 * Then they do not hold it
 */
TEST(HostedDifficulties, HoldNoNameTheyDidNotKeep) {
  HostedDifficulties difficulties;
  difficulties.keep(std::vector<std::string>{SHIPPED_DIFFICULTIES.begin(),
                                             SHIPPED_DIFFICULTIES.end()});

  EXPECT_FALSE(difficulties.contains(UNKNOWN_DIFFICULTY));
}
