#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "EngineException.hpp"
#include "GameEventConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameListSettings.hpp"
#include "GamesModule.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "LoopStages.hpp"
#include "MessageType.hpp"
#include "RecordingServerLink.hpp"
#include "SentDatagrams.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModulesFixture.hpp"
#include "ServerTrafficModule.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::engine::ModuleStartFailedException;
using rtype::network::IncomingDatagram;
using rtype::network::MessageType;
using rtype::server::GameListSettings;
using rtype::server::GamesModule;
using rtype::server::REPEATED_MESSAGE_COPIES;
using rtype::server::SERVER_LOOP_STAGES;
using rtype::server::ServerApplication;
using rtype::server::testing::MISSING_FILE;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::ServerModulesFixture;
using rtype::server::testing::SOME_SEED;

/**
 * Given the games and the lobby started, and a player seated in the first game
 * When the modules stop
 * Then the player is sent a Disconnection REPEATED_MESSAGE_COPIES times once
 * the loop stopped, and nothing else
 */
TEST_F(ServerModulesFixture, TellsEveryPlayerTheServerStops) {
  seatPlayer();

  application().stop();

  EXPECT_EQ(messageTypesSentAfterRunTo(playerEndpoint(0)),
            std::vector<MessageType>(REPEATED_MESSAGE_COPIES,
                                     MessageType::Disconnection));
}

/**
 * Given the games and the lobby started, and a player seated in the first game
 * When a tick reads a Disconnection from that player
 * Then the network forgets the session it admitted for that player
 */
TEST_F(ServerModulesFixture, ForgetsThePlayersWhoLeft) {
  seatPlayer();
  ASSERT_EQ(link().admitted().size(), 1U);

  tick({IncomingDatagram{.sender = playerEndpoint(0),
                         .payload = disconnectionDatagram()}},
       {});

  EXPECT_EQ(link().forgotten(), link().admitted());
}

/**
 * Given the games and the lobby started, and two players seated in the first
 * game
 * When the modules stop
 * Then each player is sent a Disconnection REPEATED_MESSAGE_COPIES times once
 * the loop stopped
 */
TEST_F(ServerModulesFixture, TellsEachOfTwoPlayersTheServerStops) {
  seatPlayer(0);
  seatPlayer(1);

  application().stop();

  const std::vector<MessageType> expected(REPEATED_MESSAGE_COPIES,
                                          MessageType::Disconnection);
  EXPECT_EQ(messageTypesSentAfterRunTo(playerEndpoint(0)), expected);
  EXPECT_EQ(messageTypesSentAfterRunTo(playerEndpoint(1)), expected);
}

/**
 * Given the games and the lobby started, and no player connected
 * When the modules stop
 * Then nothing is sent once the loop stopped
 */
TEST_F(ServerModulesFixture, SendsNothingWhenNoPlayerIsConnectedAtStop) {
  application().stop();

  EXPECT_TRUE(link().sentAfterRun().empty());
}

/**
 * Given the games module told to read a waves file that does not exist
 * When the application starts its modules
 * Then the start fails with a ModuleStartFailedException
 */
TEST(GamesModule, FailsToStartWhenTheWavesFileIsMissing) {
  RecordingServerLink link;
  ServerApplication::ReceivedDatagrams received{};
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerTrafficModule>(link, received));
  modules.push_back(std::make_unique<GamesModule>(GameListSettings{
      .seed = SOME_SEED, .waveFile = std::string{MISSING_FILE}}));
  Application application(LoopStages(SERVER_LOOP_STAGES), std::move(modules));

  EXPECT_THROW(application.start(), ModuleStartFailedException);
}
