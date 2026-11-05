#include <gtest/gtest.h>
#include <vector>
#include "ListedServer.hpp"
#include "MasterDatabaseFixture.hpp"
#include "MasterDatabaseTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusService.hpp"
#include "ServerStatusServiceTestConstants.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "ServersService.hpp"
#include "StatusChangeRecord.hpp"

using rtype::master::ServersService;
using rtype::master::ServerStatusService;
using rtype::master::StatusChangeRecord;
using rtype::network::heartbeatText;
using rtype::network::ListedServer;
using rtype::network::ServerHeartbeat;
using rtype::network::ServerStatus;
using rtype::network::testing::parisHeartbeat;

/**
 * Given a file at version 1 holding a server whose last heartbeat lists the
 * two games and the three players of "Paris 1"
 * When a master of this version opens it
 * Then the list counts those games and players
 */
TEST_F(MasterDatabaseFixture, AnUpgradeCountsTheGamesOfTheLastHeartbeat) {
  keepVersionOneServer(heartbeatText(parisHeartbeat()));

  ServersService servers{upgradedDatabase()};
  const std::vector<ListedServer> listed =
      servers.serverListingOf(WHOLE_PAGE).servers;

  ASSERT_EQ(listed.size(), 1U);
  EXPECT_EQ(listed.front().gameCount, PARIS_GAME_COUNT);
  EXPECT_EQ(listed.front().playerCount, PARIS_PLAYER_COUNT);
}

/**
 * Given a file at version 1 holding a server whose last heartbeat had a slow
 * tick p95
 * When a master of this version opens it and refreshes the statuses
 * Then the server is degraded, judged on that heartbeat
 */
TEST_F(MasterDatabaseFixture, AnUpgradeKeepsTheMeasuresOfTheLastHeartbeat) {
  ServerHeartbeat slow = parisHeartbeat();
  slow.measures.tickP95Milliseconds = SLOW_TICK_P95;
  keepVersionOneServer(heartbeatText(slow));

  ServerStatusService statuses{upgradedDatabase()};
  const std::vector<StatusChangeRecord> changes =
      statuses.refreshStatuses(VERSION_ONE_DATE);

  ASSERT_EQ(changes.size(), 1U);
  EXPECT_EQ(changes.front().change.status, ServerStatus::Degraded);
}

/**
 * Given a file at version 1 holding a server whose last heartbeat is no JSON
 * When a master of this version opens it
 * Then the upgrade goes through, and the list gives the server no game
 */
TEST_F(MasterDatabaseFixture, AnUnreadableHeartbeatDoesNotStopTheUpgrade) {
  keepVersionOneServer(UNREADABLE_HEARTBEAT);

  ServersService servers{upgradedDatabase()};
  const std::vector<ListedServer> listed =
      servers.serverListingOf(WHOLE_PAGE).servers;

  ASSERT_EQ(listed.size(), 1U);
  EXPECT_EQ(listed.front().gameCount, 0);
}
