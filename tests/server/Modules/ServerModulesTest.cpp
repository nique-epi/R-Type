#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
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
#include "PacketHeader.hpp"
#include "RoundTripWire.hpp"
#include "ServerInformation.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerModules.hpp"
#include "ServerModulesFixture.hpp"
#include "TrafficFileTestConstants.hpp"
#include "UdpTestClient.hpp"

using rtype::engine::DATABASE_OPTION;
using rtype::engine::PORT_OPTION;
using rtype::network::MessageType;
using rtype::network::TRAFFIC_CSV_OPTION;
using rtype::server::LaunchOptions;
using rtype::server::serverModules;

/**
 * Given the modules of the server
 * When they are listed
 * Then they are the journal, the network, the storage, the games, the lobby,
 * the information, the metrics and the master, in the order they start
 */
TEST_F(ServerModulesFixture, ListsTheServerModulesWithTheMasterLast) {
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
                       std::string{rtype::server::INFORMATION_MODULE_NAME},
                       std::string{rtype::server::METRICS_MODULE_NAME},
                       std::string{rtype::server::MASTER_MODULE_NAME}}));
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
 * FAREWELL_COPY_INTERVAL apart, under its session token, and the port is
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

/**
 * Given a server whose modules started
 * When a client asks it for its information over the loopback
 * Then it gets one ServerInformation, from which it measures a round trip
 * shorter than its wait
 */
TEST_F(ServerModulesFixture, AnswersAnInformationRequestWhoseRoundTripIsRead) {
  const auto application = serverApplication(launchOptions());
  application->start();
  const rtype::network::Endpoint server{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port()};

  client().send(server,
                rtype::network::DatagramCodec::encode(
                    rtype::network::PacketHeader{},
                    rtype::network::informationRequestAt(
                        std::chrono::steady_clock::now().time_since_epoch())));
  const std::vector<ReceivedAt> received =
      client().waitForDatagrams(1, WAIT_FOR_ANSWER);
  application->stop();

  ASSERT_EQ(received.size(), 1U);
  const std::optional<rtype::network::ServerInformation> answer =
      rtype::network::DatagramCodec::firstMessage<
          rtype::network::ServerInformation>(received.front().payload);
  ASSERT_TRUE(answer.has_value());
  const std::optional<std::chrono::microseconds> roundTrip =
      rtype::network::informationRoundTripOf(
          answer.value_or(rtype::network::ServerInformation{}),
          received.front().time.time_since_epoch());
  ASSERT_TRUE(roundTrip.has_value());
  EXPECT_LT(roundTrip.value_or(std::chrono::microseconds::max()),
            WAIT_FOR_ANSWER);
}
