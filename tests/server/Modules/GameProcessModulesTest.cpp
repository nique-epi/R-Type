#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "BuiltModuleConstants.hpp"
#include "DatagramCodec.hpp"
#include "GameLaunchOptions.hpp"
#include "GameProcessModules.hpp"
#include "GameProcessModulesFixture.hpp"
#include "GameStatisticsFixtures.hpp"
#include "IModule.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ServerConstants.hpp"
#include "ServerGameStatisticsModuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "SettingLine.hpp"
#include "TrafficFileTestConstants.hpp"
#include "TrafficFixtures.hpp"
#include "UdpTestClient.hpp"

using rtype::engine::METRICS_MODULE_BUILT;
using rtype::network::MessageType;
using rtype::network::TRAFFIC_CSV_OPTION;
using rtype::network::testing::textOf;
using rtype::server::CONFIGURATION_OPTION;
using rtype::server::GameLaunchOptions;
using rtype::server::gameProcessModules;

/**
 * Given the modules of a game process, in a build that compiled the metrics
 * When they are listed
 * Then they are the journal, the network, the storage, the games, the lobby,
 * the metrics and the link to the supervisor, in the order they start, with
 * neither the information nor the master
 */
TEST_F(GameProcessModulesFixture, ListsTheModulesOfAGameWithTheLinkLast) {
  if (!METRICS_MODULE_BUILT) {
    GTEST_SKIP();
  }
  const auto modules = gameProcessModules(launchOptions(), stopRequest());

  std::vector<std::string> names;
  names.reserve(modules.size());
  for (const auto& listed : modules) {
    names.push_back(listed->name());
  }
  EXPECT_EQ(names,
            (std::vector<std::string>{
                std::string{rtype::server::LOGGER_MODULE_NAME},
                std::string{rtype::server::NETWORK_MODULE_NAME},
                std::string{rtype::server::STORAGE_MODULE_NAME},
                std::string{rtype::server::GAMES_MODULE_NAME},
                std::string{rtype::server::LOBBY_MODULE_NAME},
                std::string{rtype::server::METRICS_MODULE_NAME},
                std::string{rtype::server::SUPERVISOR_LINK_MODULE_NAME}}));
}

/**
 * Given the modules of a game process, in a build with or without the metrics
 * When they are listed
 * Then the metrics are among them exactly when the build compiled them
 */
TEST_F(GameProcessModulesFixture, ListsTheMetricsOnlyWhenTheyAreBuilt) {
  const auto modules = gameProcessModules(launchOptions(), stopRequest());

  const bool listed = std::ranges::any_of(
      modules, [](const std::unique_ptr<rtype::engine::IModule>& module) {
        return module->name() == rtype::server::METRICS_MODULE_NAME;
      });

  EXPECT_EQ(listed, METRICS_MODULE_BUILT);
}

/**
 * Given a game process started from an empty folder, its waves file next to
 * its executable
 * When the application starts
 * Then it finds the waves file and starts
 */
TEST_F(GameProcessModulesFixture,
       FindsItsWavesFromItsExecutableNotFromTheStartFolder) {
  const auto application = gameApplication(launchOptions());

  ASSERT_NO_THROW(application->start());

  application->stop();
}

/**
 * Given a game process launched with the traffic option and a relative file,
 * in a build that compiled the metrics, which write it
 * When the application starts
 * Then the file exists in the folder the game was started in
 */
TEST_F(GameProcessModulesFixture, OpensTheTrafficFileOfItsLaunchOption) {
  if (!METRICS_MODULE_BUILT) {
    GTEST_SKIP();
  }
  const auto application = gameApplication(GameLaunchOptions::read(
      gameArguments({TRAFFIC_CSV_OPTION, TRAFFIC_FILE_NAME})));

  application->start();

  EXPECT_TRUE(std::filesystem::exists(TRAFFIC_FILE_NAME));
  application->stop();
}

/**
 * Given a game process given a file of commands that loads the game statistics
 * plugin, writing to a file of the folder it starts in
 * When the application starts
 * Then the file of its port holds the names of its columns: the game read the
 * file it was given, and its modules provide what the plugin needs
 */
TEST_F(GameProcessModulesFixture,
       LoadsTheGameStatisticsPluginOfTheFileItIsGiven) {
  std::vector<std::string> lines = gameStatisticsPluginLines();
  lines.push_back(rtype::engine::settingLine(STATISTICS_FILE_SETTING,
                                             STATISTICS_FILE_NAME));
  writeConfiguration(GAME_CONFIGURATION_FILE, lines);
  const auto application = gameApplication(GameLaunchOptions::read(
      gameArguments({CONFIGURATION_OPTION, GAME_CONFIGURATION_FILE})));

  application->start();

  EXPECT_EQ(textOf(statisticsFileOfPort(port())), STATISTICS_HEADER_LINE);
  application->stop();
}

/**
 * Given a server.cfg in the folder a game process starts in, which loads the
 * game statistics plugin, and no file of commands given
 * When the application starts
 * Then no statistics file is written: a game reads only the file its
 * supervisor gives it
 */
TEST_F(GameProcessModulesFixture, ReadsNoServerConfigurationOfItsFolder) {
  std::vector<std::string> lines = gameStatisticsPluginLines();
  lines.push_back(rtype::engine::settingLine(STATISTICS_FILE_SETTING,
                                             STATISTICS_FILE_NAME));
  writeConfiguration(rtype::server::SERVER_CONFIGURATION_FILE_NAME, lines);
  const auto application = gameApplication(launchOptions());

  application->start();

  EXPECT_FALSE(std::filesystem::exists(statisticsFileOfPort(port())));
  application->stop();
}

/**
 * Given a game process whose modules started and a client that joined it
 * When the application stops
 * Then the client receives the three copies of a Disconnection, about
 * FAREWELL_COPY_INTERVAL apart, under its session token, and the port is
 * free again
 */
TEST_F(GameProcessModulesFixture, SaysGoodbyeToAJoinedPlayerWhenItStops) {
  const auto application = gameApplication(launchOptions());
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
