#include "GameProcessModulesFixture.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "ConnectionChallenge.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameLaunchOptions.hpp"
#include "GameProcessModules.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "StopRequest.hpp"
#include "UdpTestClient.hpp"

GameProcessModulesFixture::GameProcessModulesFixture()
    : port_(freeUdpPort()),
      workingFolderChange_(workingFolder_.path()),
      portText_(std::to_string(port_)),
      supervisorPortText_(std::to_string(supervisor_.localEndpoint().port)),
      databaseText_((workingFolder_.path() / TEST_DATABASE_FILE).string()),
      journalText_(journal().path().string()),
      launchOptions_(
          rtype::server::GameLaunchOptions::read(gameArguments({}))) {}

std::vector<std::string_view> GameProcessModulesFixture::gameArguments(
    const std::vector<std::string_view>& more) const {
  std::vector<std::string_view> arguments{
      rtype::engine::PORT_OPTION,       portText_,
      rtype::server::SUPERVISOR_OPTION, supervisorPortText_,
      rtype::server::LINK_TOKEN_OPTION, GAME_PROCESS_LINK_TOKEN,
      rtype::engine::DATABASE_OPTION,   databaseText_,
      rtype::logging::FILE_OPTION,      journalText_,
      rtype::logging::NO_STDERR_OPTION};
  arguments.insert(arguments.end(), more.begin(), more.end());
  return arguments;
}

std::uint64_t GameProcessModulesFixture::joinWithTheClient() {
  const rtype::network::Endpoint server{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port_};
  client().send(server, requestDatagram());
  const std::vector<ReceivedAt> challenge =
      client().waitForDatagrams(1, WAIT_FOR_ANSWER);
  const auto frames =
      rtype::network::DatagramCodec::splitMessages(challenge.at(0).payload);
  const std::uint64_t cookie =
      rtype::network::decodeMessage<rtype::network::ConnectionChallenge>(
          frames.at(0).payload)
          .cookie;
  client().send(server, requestDatagram(cookie));
  const std::vector<ReceivedAt> accepted =
      client().waitForDatagrams(2, WAIT_FOR_ANSWER);
  return rtype::network::DatagramCodec::decodeHeader(accepted.at(1).payload)
      .sessionToken;
}

std::vector<ReceivedAt> GameProcessModulesFixture::holding(
    const std::vector<ReceivedAt>& received, rtype::network::MessageType type) {
  std::vector<ReceivedAt> matching;
  for (const ReceivedAt& datagram : received) {
    for (const auto& frame :
         rtype::network::DatagramCodec::splitMessages(datagram.payload)) {
      if (frame.type == static_cast<std::uint8_t>(type)) {
        matching.push_back(datagram);
        break;
      }
    }
  }
  return matching;
}

std::uint16_t GameProcessModulesFixture::port() const { return port_; }

rtype::server::StopRequest& GameProcessModulesFixture::stopRequest() {
  return stopRequest_;
}

UdpTestClient& GameProcessModulesFixture::client() { return client_; }

const rtype::server::GameLaunchOptions&
GameProcessModulesFixture::launchOptions() const {
  return launchOptions_;
}

std::unique_ptr<rtype::engine::Application>
GameProcessModulesFixture::gameApplication(
    const rtype::server::GameLaunchOptions& options) {
  return std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      rtype::server::gameProcessModules(options, stopRequest_),
      rtype::engine::configurationStep(
          options.configurationFile().value_or(std::filesystem::path{}),
          options.forcedLines(), configurationReport_));
}

void GameProcessModulesFixture::writeConfiguration(
    std::string_view file, const std::vector<std::string>& lines) const {
  workingFolder_.writeLines(file, lines);
}
