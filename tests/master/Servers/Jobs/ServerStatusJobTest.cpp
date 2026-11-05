#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include "ServerStatus.hpp"
#include "ServerStatusJobFixture.hpp"
#include "ServerStatusJobTestConstants.hpp"

using rtype::network::ServerStatus;

/**
 * Given a server that registered and beat just now
 * When the job starts
 * Then the list soon gives it online
 */
TEST_F(ServerStatusJobFixture, TheJobTurnsABeatingServerOnline) {
  const std::int64_t serverId = serverBeatingNow();

  startJob(SHORT_REFRESH_INTERVAL);

  EXPECT_TRUE(statusSoonIs(serverId, ServerStatus::Online));
}

/**
 * Given a job whose first refresh turned a first server online
 * When a second server registers and beats afterwards
 * Then a later refresh turns it online too: the job refreshes again, not only
 * when it starts
 */
TEST_F(ServerStatusJobFixture, TheJobRefreshesEveryInterval) {
  const std::int64_t firstServer = serverBeatingNow();
  startJob(SHORT_REFRESH_INTERVAL);
  ASSERT_TRUE(statusSoonIs(firstServer, ServerStatus::Online));

  const std::int64_t secondServer = serverBeatingNow();

  EXPECT_TRUE(statusSoonIs(secondServer, ServerStatus::Online));
}

/**
 * Given a job that refreshed once, then sleeps an hour before the next refresh
 * When it stops
 * Then it stops within a second, without waiting for the hour
 */
TEST_F(ServerStatusJobFixture, StoppingTheJobDoesNotWaitForItsInterval) {
  const std::int64_t serverId = serverBeatingNow();
  startJob(ONE_HOUR_INTERVAL);
  ASSERT_TRUE(statusSoonIs(serverId, ServerStatus::Online));
  const auto stopAskedAt = std::chrono::steady_clock::now();

  stopJob();

  EXPECT_LT(std::chrono::steady_clock::now() - stopAskedAt, STOP_DEADLINE);
}
