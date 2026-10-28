#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "DatagramCodec.hpp"
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerModules.hpp"
#include "ServerModulesFixture.hpp"
#include "UdpTestClient.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::network::MessageType;
using rtype::server::serverModules;

/**
 * Given the modules of the server
 * When they are listed
 * Then they are the journal, the network, the games and the lobby, in the order
 * they start
 */
TEST_F(ServerModulesFixture, ListsTheLoggerTheNetworkTheGamesAndTheLobby) {
  const auto modules =
      serverModules(journalOptions(), launchOptions(), stopRequest());

  std::vector<std::string> names;
  names.reserve(modules.size());
  for (const auto& listed : modules) {
    names.push_back(listed->name());
  }
  EXPECT_EQ(names, (std::vector<std::string>{
                       std::string{rtype::server::LOGGER_MODULE_NAME},
                       std::string{rtype::server::NETWORK_MODULE_NAME},
                       std::string{rtype::server::GAMES_MODULE_NAME},
                       std::string{rtype::server::LOBBY_MODULE_NAME}}));
}

/**
 * Given a server whose modules started and a client that joined it
 * When the application stops
 * Then the client receives the three copies of a Disconnection, about
 * REPEATED_MESSAGE_INTERVAL apart, under its session token, and the port is
 * free again
 */
TEST_F(ServerModulesFixture, SaysGoodbyeToAJoinedPlayerWhenItStops) {
  Application application(
      LoopStages(rtype::server::SERVER_LOOP_STAGES),
      serverModules(journalOptions(), launchOptions(), stopRequest()));
  application.start();
  const std::uint64_t token = joinWithTheClient();

  application.stop();

  const std::vector<ReceivedAt> everything =
      client().waitForDatagrams(UNREACHABLE_COUNT, GOODBYE_WAIT);
  const std::vector<ReceivedAt> goodbyes =
      holding(everything, MessageType::Disconnection);
  ASSERT_EQ(goodbyes.size(), 3U);
  for (const ReceivedAt& goodbye : goodbyes) {
    EXPECT_EQ(rtype::network::DatagramCodec::decodeHeader(goodbye.payload)
                  .sessionToken,
              token);
  }
  EXPECT_GE(goodbyes[1].time - goodbyes[0].time, MINIMUM_COPY_SPACING);
  EXPECT_GE(goodbyes[2].time - goodbyes[1].time, MINIMUM_COPY_SPACING);
  EXPECT_TRUE(portIsFree(port()));
  EXPECT_EQ(client().failedSendCount(), 0U);
}
