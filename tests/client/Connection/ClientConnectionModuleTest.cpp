#include <gtest/gtest.h>
#include "ClientConnectionModuleFixture.hpp"
#include "ClientConstants.hpp"
#include "RunningMaster.hpp"
#include "ServerListPhase.hpp"
#include "SettingLine.hpp"

namespace rtype::client::testing {

/**
 * Given a running master that lists no server, on a port that is not the
 * default one, and a client whose configuration names it with master-url
 * When the server map reads the list of servers
 * Then the master answers an empty list: the client read the master the
 * setting names, not the one on the default port, which no master holds
 */
TEST_F(ClientConnectionModuleFixture,
       TheServerMapReadsTheMasterTheSettingNames) {
  startWith(engine::settingLine(MASTER_URL_SETTING_NAME,
                                RunningMaster::urlOf(masterEndpoint().port)));

  askListing();

  EXPECT_EQ(phaseSoon(ServerListPhase::Empty), ServerListPhase::Empty);
}

/**
 * Given a master-url naming a port no master listens on
 * When the server map reads the list of servers
 * Then the list says the master is unreachable: the client reads no other
 * master than the one the setting names
 */
TEST_F(ClientConnectionModuleFixture, OnlyTheMasterTheSettingNamesIsRead) {
  startWith(engine::settingLine(MASTER_URL_SETTING_NAME, urlWithoutMaster()));

  askListing();

  EXPECT_EQ(phaseSoon(ServerListPhase::MasterUnreachable),
            ServerListPhase::MasterUnreachable);
}

}  // namespace rtype::client::testing
