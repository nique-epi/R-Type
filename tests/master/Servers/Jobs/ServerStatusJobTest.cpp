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
 * Given a job started before any server
 * When a server registers and beats afterwards
 * Then a later refresh gives it online: the job refreshes again, not only
 * when it starts
 */
TEST_F(ServerStatusJobFixture, TheJobRefreshesEveryInterval) {
  startJob(SHORT_REFRESH_INTERVAL);

  const std::int64_t serverId = serverBeatingNow();

  EXPECT_TRUE(statusSoonIs(serverId, ServerStatus::Online));
}

/**
 * Given a job that sleeps an hour between two refreshes
 * When it stops
 * Then it stops within a second, without waiting for the hour
 */
TEST_F(ServerStatusJobFixture, StoppingTheJobDoesNotWaitForItsInterval) {
  startJob(ONE_HOUR_INTERVAL);
  const auto stopAskedAt = std::chrono::steady_clock::now();

  stopJob();

  EXPECT_LT(std::chrono::steady_clock::now() - stopAskedAt, STOP_DEADLINE);
}
