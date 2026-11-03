#include <gtest/gtest.h>
#include <vector>
#include "FrameLoopFixture.hpp"
#include "FrameLoopTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;

/**
 * Given a frame loop whose work stops it at its third pass
 * When it runs
 * Then it ran exactly three passes
 */
TEST_F(FrameLoopFixture, RunsPassesUntilAWorkStopsIt) {
  runPasses(THREE_PASSES);

  EXPECT_EQ(elapsedByPass().size(), THREE_PASSES.size());
}

/**
 * Given a frame loop whose passes last FIRST_PASS_DURATION, then
 * SECOND_PASS_DURATION, by the clock
 * When it runs three passes
 * Then the first gets no time, as the clock did not move since run() began,
 * and each next one gets the time the pass before it lasted
 */
TEST_F(FrameLoopFixture, MovesEachPassByTheTimeSinceThePreviousOne) {
  runPasses(THREE_PASSES);

  EXPECT_EQ(elapsedByPass(),
            (std::vector<Duration>{Duration{}, FIRST_PASS_DURATION,
                                   SECOND_PASS_DURATION}));
}

/**
 * Given a frame loop stopped before it runs
 * When it runs
 * Then it runs no pass
 */
TEST_F(FrameLoopFixture, RunsNoPassOnceStoppedBeforeRunning) {
  stopBeforeRunning();

  runPasses(ONE_PASS);

  EXPECT_TRUE(elapsedByPass().empty());
}
