#include "ServerModulesFixture.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionChallenge.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "LaunchOptions.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "StopRequest.hpp"
#include "UdpTestClient.hpp"

namespace {

std::vector<std::string_view> launchArguments(const std::string& portText) {
  return {"-p", portText, rtype::server::SEED_OPTION, "1"};
}

}  // namespace

ServerModulesFixture::ServerModulesFixture()
    : port_(freeUdpPort()),
      previousFolder_(std::filesystem::current_path()),
      portText_(std::to_string(port_)),
      arguments_(launchArguments(portText_)),
      launchOptions_(rtype::server::LaunchOptions::read(arguments_)) {
  std::filesystem::current_path(
      std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER}.parent_path());
}

ServerModulesFixture::~ServerModulesFixture() {
  std::filesystem::current_path(previousFolder_);
}

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
