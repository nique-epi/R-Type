#include <gtest/gtest.h>
#include <optional>
#include "MasterMessageTextTestConstants.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusRule.hpp"
#include "ServerStatusRuleFixtures.hpp"
#include "ServerStatusRuleTestConstants.hpp"
#include "ServersServiceTestConstants.hpp"
#include "StatusEvidence.hpp"

using rtype::master::StatusEvidence;
using rtype::master::statusOf;
using rtype::network::ServerStatus;
using rtype::network::testing::PARIS_LOSS_PERCENT;
using rtype::network::testing::PARIS_TICK_P95;

/**
 * Given a server heard just now, its ticks short and its losses low
 * When its status is computed
 * Then it is online
 */
TEST(ServerStatusRule, AServerHeardJustNowIsOnline) {
  const StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, PARIS_LOSS_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Online);
}

/**
 * Given a server heard 14 s ago
 * When its status is computed
 * Then it is still online: the issue waits for 15 s of silence
 */
TEST(ServerStatusRule, AServerSilentForFourteenSecondsIsOnline) {
  const StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, PARIS_LOSS_PERCENT));

  EXPECT_EQ(
      statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS - ONE_SECOND),
      ServerStatus::Online);
}

/**
 * Given a server heard 15 s ago, 3 heartbeats of 5 s
 * When its status is computed
 * Then it is offline
 */
TEST(ServerStatusRule, AServerSilentForFifteenSecondsIsOffline) {
  const StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, PARIS_LOSS_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS),
            ServerStatus::Offline);
}

/**
 * Given a server whose ticks last a little less than one tick at their 95th
 * percentile
 * When its status is computed
 * Then it is online
 */
TEST(ServerStatusRule, ATickP95UnderOneTickIsOnline) {
  const StatusEvidence evidence = heardWith(
      measuresWith(ONE_TICK_MILLISECONDS - A_LITTLE, PARIS_LOSS_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Online);
}

/**
 * Given a server whose ticks last one tick at their 95th percentile
 * When its status is computed
 * Then it is degraded
 */
TEST(ServerStatusRule, ATickP95OfOneTickIsDegraded) {
  const StatusEvidence evidence =
      heardWith(measuresWith(ONE_TICK_MILLISECONDS, PARIS_LOSS_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Degraded);
}

/**
 * Given a server that loses a little less than 5 % of the datagrams
 * When its status is computed
 * Then it is online
 */
TEST(ServerStatusRule, ALossUnderFivePercentIsOnline) {
  const StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, FIVE_PERCENT - A_LITTLE));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Online);
}

/**
 * Given a server that loses 5 % of the datagrams
 * When its status is computed
 * Then it is degraded
 */
TEST(ServerStatusRule, ALossOfFivePercentIsDegraded) {
  const StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, FIVE_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Degraded);
}

/**
 * Given a server heard just now that measures nothing, its metrics module off
 * When its status is computed
 * Then it is online: a measure not taken is no fault
 */
TEST(ServerStatusRule, AServerThatMeasuresNothingIsOnline) {
  const StatusEvidence evidence =
      heardWith(measuresWith(std::nullopt, std::nullopt));

  EXPECT_EQ(statusOf(evidence, HEARD_AT), ServerStatus::Online);
}

/**
 * Given a server an admin put in maintenance, silent for 15 s and over both
 * bounds
 * When its status is computed
 * Then it is in maintenance
 */
TEST(ServerStatusRule, MaintenanceComesBeforeEverything) {
  StatusEvidence evidence =
      heardWith(measuresWith(ONE_TICK_MILLISECONDS, FIVE_PERCENT));
  evidence.maintenance = true;

  EXPECT_EQ(statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS),
            ServerStatus::Maintenance);
}

/**
 * Given a server silent for 15 s whose last heartbeat was over both bounds
 * When its status is computed
 * Then it is offline: a silent server is not judged on its old measures
 */
TEST(ServerStatusRule, SilenceComesBeforeTheMeasures) {
  const StatusEvidence evidence =
      heardWith(measuresWith(ONE_TICK_MILLISECONDS, FIVE_PERCENT));

  EXPECT_EQ(statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS),
            ServerStatus::Offline);
}

/**
 * Given a server registered 14 s ago that sent no heartbeat since
 * When its status is computed
 * Then it is online: its registration is the last news of it
 */
TEST(ServerStatusRule, ARegistrationIsNewsOfTheServer) {
  StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, PARIS_LOSS_PERCENT));
  evidence.lastHeartbeatAt = std::nullopt;

  EXPECT_EQ(
      statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS - ONE_SECOND),
      ServerStatus::Online);
}

/**
 * Given a server registered a minute ago, whose last heartbeat came 14 s ago
 * When its status is computed
 * Then it is online: the heartbeat, not the registration, is its last news
 */
TEST(ServerStatusRule, AHeartbeatAfterTheRegistrationIsTheLastNews) {
  StatusEvidence evidence =
      heardWith(measuresWith(PARIS_TICK_P95, PARIS_LOSS_PERCENT));
  evidence.registeredAt = HEARD_AT - ONE_MINUTE;

  EXPECT_EQ(
      statusOf(evidence, HEARD_AT + SILENCE_OF_THREE_HEARTBEATS - ONE_SECOND),
      ServerStatus::Online);
}
