#include <gtest/gtest.h>
#include "ClientConnectionModuleFixture.hpp"
#include "ClientConstants.hpp"
#include "PlayerSettings.hpp"
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
                                RunningMaster::urlOf(masterEndpoint().port)),
            PlayerSettings{});

  askListing();

  EXPECT_EQ(phaseSoon(ServerListPhase::Empty), ServerListPhase::Empty);
}

/**
 * Given a player who chose the running master, and a master-url naming a port
 * no master listens on
 * When the server map reads the list of servers
 * Then the master the player chose answers: the choice wins over the setting
 */
TEST_F(ClientConnectionModuleFixture, AChosenMasterWinsOverTheSetting) {
  PlayerSettings settings;
  settings.master = masterEndpoint();
  startWith(engine::settingLine(MASTER_URL_SETTING_NAME, urlWithoutMaster()),
            settings);

  askListing();

  EXPECT_EQ(phaseSoon(ServerListPhase::Empty), ServerListPhase::Empty);
}

/**
 * Given a client reading a master that does not answer
 * When the player chooses the running master while the client runs
 * Then the server map reads that master, asked again without another ask of
 * the map
 */
TEST_F(ClientConnectionModuleFixture, AChosenMasterIsReadWhileTheClientRuns) {
  startWith(engine::settingLine(MASTER_URL_SETTING_NAME, urlWithoutMaster()),
            PlayerSettings{});
  askListing();
  ASSERT_EQ(phaseSoon(ServerListPhase::MasterUnreachable),
            ServerListPhase::MasterUnreachable);

  chooseMaster(masterEndpoint());

  EXPECT_EQ(phaseSoon(ServerListPhase::Empty), ServerListPhase::Empty);
}

}  // namespace rtype::client::testing
