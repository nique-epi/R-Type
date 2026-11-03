#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "DatagramCodec.hpp"
#include "IModule.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerModules.hpp"
#include "ServerModulesFixture.hpp"
#include "TrafficFileTestConstants.hpp"
#include "UdpTestClient.hpp"

using rtype::engine::PORT_OPTION;
using rtype::network::MessageType;
using rtype::network::TRAFFIC_CSV_OPTION;
using rtype::server::DATABASE_OPTION;
using rtype::server::LaunchOptions;
using rtype::server::serverModules;

/**
 * Given the modules of the server
 * When they are listed
 * Then they are the journal, the network, the games, the lobby and the metrics,
 * in the order they start
 */
TEST_F(ServerModulesFixture, ListsTheServerModulesWithTheMetricsLast) {
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
                       std::string{rtype::server::STORAGE_MODULE_NAME},
                       std::string{rtype::server::GAMES_MODULE_NAME},
                       std::string{rtype::server::LOBBY_MODULE_NAME},
                       std::string{rtype::server::METRICS_MODULE_NAME}}));
}

/**
 * Given a server started from an empty folder, its waves file next to its
 * executable
 * When the application starts
 * Then it finds the waves file and starts
 */
TEST_F(ServerModulesFixture,
       FindsItsWavesFromItsExecutableNotFromTheStartFolder) {
  const auto application = serverApplication(launchOptions());

  ASSERT_NO_THROW(application->start());

  application->stop();
}

/**
 * Given a server launched with the traffic option and a relative file
 * When the application starts
 * Then the file exists in the folder the server was started in
 */
TEST_F(ServerModulesFixture, OpensTheTrafficFileOfItsLaunchOption) {
  const std::string portText = std::to_string(port());
  const std::vector<std::string_view> arguments{
      PORT_OPTION,        portText,           DATABASE_OPTION,
      TEST_DATABASE_FILE, TRAFFIC_CSV_OPTION, TRAFFIC_FILE_NAME};
  const auto application = serverApplication(LaunchOptions::read(arguments));

  application->start();

  EXPECT_TRUE(std::filesystem::exists(TRAFFIC_FILE_NAME));
  application->stop();
}

/**
 * Given a server whose modules started and a client that joined it
 * When the application stops
 * Then the client receives the three copies of a Disconnection, about
 * REPEATED_MESSAGE_INTERVAL apart, under its session token, and the port is
 * free again
 */
TEST_F(ServerModulesFixture, SaysGoodbyeToAJoinedPlayerWhenItStops) {
  const auto application = serverApplication(launchOptions());
  application->start();
  const std::uint64_t token = joinWithTheClient();

  application->stop();

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
