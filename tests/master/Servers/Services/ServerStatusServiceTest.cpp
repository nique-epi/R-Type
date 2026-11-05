#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "DatedStatus.hpp"
#include "KnownServer.hpp"
#include "ListedServer.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "PageRequest.hpp"
#include "ServerCreation.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusServiceFixture.hpp"
#include "ServerStatusServiceTestConstants.hpp"
#include "ServersFixtures.hpp"
#include "ServersServiceTestConstants.hpp"
#include "StatusChangeRecord.hpp"

using rtype::master::CreationAttempt;
using rtype::master::KnownServer;
using rtype::master::PageRequest;
using rtype::master::StatusChangeRecord;
using rtype::network::CreatedServer;
using rtype::network::DatedStatus;
using rtype::network::ListedServer;
using rtype::network::ServerCreation;
using rtype::network::ServerStatus;
using rtype::network::testing::PARIS_TICK_P95;

/**
 * Given a server just registered, which the list gives offline until the
 * first refresh
 * When the statuses are refreshed at its registration
 * Then it is online, and its history holds that one change
 */
TEST_F(ServerStatusServiceFixture,
       ARegisteredServerTurnsOnlineAtTheFirstRefresh) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  ASSERT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Offline);

  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));

  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Online);
  EXPECT_EQ(
      historyOf(server.id),
      (std::vector<DatedStatus>{DatedStatus{.since = FIRST_HEARTBEAT_DATE,
                                            .status = ServerStatus::Online}}));
}

/**
 * Given an online server that keeps beating
 * When the statuses are refreshed again 5 s later
 * Then nothing changes and nothing is kept
 */
TEST_F(ServerStatusServiceFixture, ARefreshWithNothingNewKeepsNothing) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  beat(server, PARIS_TICK_P95, FIRST_HEARTBEAT_DATE);
  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));
  beat(server, PARIS_TICK_P95, FIRST_HEARTBEAT_DATE + FIVE_SECONDS);

  const std::vector<StatusChangeRecord> changes =
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE + FIVE_SECONDS);

  EXPECT_TRUE(changes.empty());
  EXPECT_EQ(historyOf(server.id).size(), 1U);
}

/**
 * Given an online server whose last heartbeat is 15 s old
 * When the statuses are refreshed
 * Then it turns offline at that refresh, newest first in its history
 */
TEST_F(ServerStatusServiceFixture, AServerSilentForFifteenSecondsTurnsOffline) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  beat(server, PARIS_TICK_P95, FIRST_HEARTBEAT_DATE);
  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));

  static_cast<void>(
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE + FIFTEEN_SECONDS));

  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Offline);
  EXPECT_EQ(historyOf(server.id),
            (std::vector<DatedStatus>{
                DatedStatus{.since = FIRST_HEARTBEAT_DATE + FIFTEEN_SECONDS,
                            .status = ServerStatus::Offline},
                DatedStatus{.since = FIRST_HEARTBEAT_DATE,
                            .status = ServerStatus::Online}}));
}

/**
 * Given a server whose last heartbeat gives a tick p95 of 20 ms
 * When the statuses are refreshed
 * Then it is degraded
 */
TEST_F(ServerStatusServiceFixture, AServerOverItsBoundsTurnsDegraded) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  beat(server, SLOW_TICK_P95, FIRST_HEARTBEAT_DATE);

  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));

  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Degraded);
}

/**
 * Given an online server an admin puts in maintenance, then takes out of it
 * When the statuses are refreshed after each
 * Then it is in maintenance, then online again
 */
TEST_F(ServerStatusServiceFixture, MaintenanceIsPutAndTakenOut) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);

  ASSERT_TRUE(statuses().setMaintenance(server.id, true));
  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));
  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Maintenance);

  ASSERT_TRUE(statuses().setMaintenance(server.id, false));
  static_cast<void>(
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE + FIVE_SECONDS));
  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Online);
}

/**
 * Given no server of id 999
 * When an admin puts it in maintenance
 * Then nothing is found
 */
TEST_F(ServerStatusServiceFixture, MaintenanceOfAnUnknownServerFindsNothing) {
  EXPECT_FALSE(statuses().setMaintenance(UNKNOWN_SERVER_ID, true));
}

/**
 * Given a server created on the web site and never started
 * When the statuses are refreshed
 * Then nothing changes, and the list leaves it out
 */
TEST_F(ServerStatusServiceFixture, AServerThatNeverRegisteredIsNotWatched) {
  const CreatedServer created =
      servers()
          .createServer(CreationAttempt{
              .creation = ServerCreation{.name = std::string{TEST_SERVER_NAME}},
              .address = std::string{NEVER_REGISTERED_CREATOR},
              .requestedAt = FIRST_HEARTBEAT_DATE})
          .value_or(CreatedServer{});

  const std::vector<StatusChangeRecord> changes =
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE);

  EXPECT_TRUE(changes.empty());
  EXPECT_EQ(listedStatusOf(servers(), created.id), std::nullopt);
}

/**
 * Given a server that turned online at a refresh, then was put in
 * maintenance in the same second
 * When the statuses are refreshed in that second again
 * Then its history keeps the later change only
 */
TEST_F(ServerStatusServiceFixture, TwoChangesInOneSecondKeepTheLater) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));
  ASSERT_TRUE(statuses().setMaintenance(server.id, true));

  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));

  EXPECT_EQ(historyOf(server.id), (std::vector<DatedStatus>{DatedStatus{
                                      .since = FIRST_HEARTBEAT_DATE,
                                      .status = ServerStatus::Maintenance}}));
}

/**
 * Given a server that turned online, then nothing for 8 days
 * When the statuses are refreshed then
 * Then it turns offline, and the change of 8 days ago is erased
 */
TEST_F(ServerStatusServiceFixture, AChangeOlderThanSevenDaysIsErased) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  static_cast<void>(statuses().refreshStatuses(FIRST_HEARTBEAT_DATE));

  static_cast<void>(
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE + EIGHT_DAYS));

  EXPECT_EQ(historyOf(server.id),
            (std::vector<DatedStatus>{
                DatedStatus{.since = FIRST_HEARTBEAT_DATE + EIGHT_DAYS,
                            .status = ServerStatus::Offline}}));
}

/**
 * Given a server whose last heartbeat was slow, which then registers again, as
 * it does when it restarts
 * When the statuses are refreshed
 * Then it is online: a registration forgets the measures of the heartbeat
 * before it
 */
TEST_F(ServerStatusServiceFixture, ARegistrationForgetsTheLastMeasures) {
  const KnownServer server = registeredAt(FIRST_HEARTBEAT_DATE);
  beat(server, SLOW_TICK_P95, FIRST_HEARTBEAT_DATE);
  registerAgain(server.id, FIRST_HEARTBEAT_DATE + FIVE_SECONDS);

  static_cast<void>(
      statuses().refreshStatuses(FIRST_HEARTBEAT_DATE + FIVE_SECONDS));

  EXPECT_EQ(listedStatusOf(servers(), server.id), ServerStatus::Online);
}

/**
 * Given three registered servers
 * When the list is read one server at a time, after the first
 * Then it holds the second server by id, and only it
 */
TEST_F(ServerStatusServiceFixture, TheListIsReadAPageAtATime) {
  std::vector<std::int64_t> ids;
  ids.reserve(THREE_SERVERS);
  for (int index = 0; index < THREE_SERVERS; ++index) {
    ids.push_back(registeredAt(FIRST_HEARTBEAT_DATE).id);
  }

  const std::vector<ListedServer> page =
      servers()
          .serverListingOf(
              PageRequest{.limit = ONE_ITEM, .offset = AFTER_THE_FIRST})
          .servers;

  ASSERT_EQ(page.size(), 1U);
  EXPECT_EQ(page.front().id, ids.at(SECOND_SERVER));
}
