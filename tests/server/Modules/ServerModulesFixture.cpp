#include "ServerModulesFixture.hpp"
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
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOptions.hpp"
#include "LoopStages.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerModules.hpp"
#include "StopRequest.hpp"
#include "UdpTestClient.hpp"

namespace {

std::vector<std::string_view> launchArguments(const std::string& portText,
                                              const std::string& databaseText) {
  return {"-p",
          portText,
          rtype::server::SEED_OPTION,
          "1",
          rtype::engine::DATABASE_OPTION,
          databaseText};
}

}  // namespace

ServerModulesFixture::ServerModulesFixture()
    : port_(freeUdpPort()),
      workingFolderChange_(workingFolder_.path()),
      portText_(std::to_string(port_)),
      databaseText_((workingFolder_.path() / TEST_DATABASE_FILE).string()),
      arguments_(launchArguments(portText_, databaseText_)),
      launchOptions_(rtype::server::LaunchOptions::read(arguments_)) {}

std::uint64_t ServerModulesFixture::joinWithTheClient() {
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

std::vector<ReceivedAt> ServerModulesFixture::holding(
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

std::uint16_t ServerModulesFixture::port() const { return port_; }

rtype::server::StopRequest& ServerModulesFixture::stopRequest() {
  return stopRequest_;
}

UdpTestClient& ServerModulesFixture::client() { return client_; }

const rtype::server::LaunchOptions& ServerModulesFixture::launchOptions()
    const {
  return launchOptions_;
}

std::unique_ptr<rtype::engine::Application>
ServerModulesFixture::serverApplication(
    const rtype::server::LaunchOptions& options) {
  return std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      rtype::server::serverModules(journalOptions(), options, stopRequest_),
      rtype::engine::configurationStep(
          std::filesystem::path{rtype::server::SERVER_CONFIGURATION_FILE_NAME},
          options.forcedLines(), configurationReport_));
}

void ServerModulesFixture::writeConfiguration(
    const std::vector<std::string>& lines) const {
  std::string text;
  for (const std::string& line : lines) {
    text += line + '\n';
  }
  workingFolder_.write(rtype::server::SERVER_CONFIGURATION_FILE_NAME, text);
}
