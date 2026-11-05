#include <gtest/gtest.h>
#include <thread>
#include "HeartbeatJobFixture.hpp"
#include "HeartbeatJobTestConstants.hpp"

using rtype::server::testing::FAST_TIMES;
using rtype::server::testing::FIRST_PILOT_NAME;
using rtype::server::testing::HeartbeatJobFixture;
using rtype::server::testing::MASTER_DOWN_TIME;
using rtype::server::testing::SECOND_PILOT_NAME;
using rtype::server::testing::SLOW_INTERVAL_TIMES;

/**
 * Given a master that created the server, and a game with a pilot
 * When the job starts
 * Then the server registers and the master keeps a heartbeat naming the pilot
 */
TEST_F(HeartbeatJobFixture, RegistersThenTellsTheGames) {
  publishGameOf(FIRST_PILOT_NAME);

  startJob(FAST_TIMES);

  EXPECT_TRUE(heartbeatSoonNames(FIRST_PILOT_NAME));
}

/**
 * Given a job told of a game, whose interval is far longer than the test
 * When a second pilot arrives
 * Then the master is told at once, without waiting for the interval
 */
TEST_F(HeartbeatJobFixture, TellsAChangeWithoutWaitingForTheInterval) {
  publishGameOf(FIRST_PILOT_NAME);
  startJob(SLOW_INTERVAL_TIMES);
  ASSERT_TRUE(heartbeatSoonNames(FIRST_PILOT_NAME));

  publishGameOf(SECOND_PILOT_NAME);

  EXPECT_TRUE(heartbeatSoonNames(SECOND_PILOT_NAME));
}

/**
 * Given a job that told the master of a game
 * When the master stops, the game changes, then the master comes back
 * Then the master is told of the change once it is back
 */
TEST_F(HeartbeatJobFixture, TellsTheMasterAgainOnceItComesBack) {
  publishGameOf(FIRST_PILOT_NAME);
  startJob(FAST_TIMES);
  ASSERT_TRUE(heartbeatSoonNames(FIRST_PILOT_NAME));

  master().stop();
  publishGameOf(SECOND_PILOT_NAME);
  std::this_thread::sleep_for(MASTER_DOWN_TIME);
  master().restart();

  EXPECT_TRUE(heartbeatSoonNames(SECOND_PILOT_NAME));
}
