#include "ServerDatagrams.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "Heartbeat.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ReadyState.hpp"
#include "ServerLobbyTestConstants.hpp"

std::string senderText(const rtype::network::Endpoint& sender) {
  return "from " + sender.address + " port " + std::to_string(sender.port);
}

std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header) {
  return requestDatagram(0, header);
}

std::vector<std::byte> requestDatagram(
    std::uint64_t cookie, const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header,
      rtype::network::ConnectionRequest{
          .cookie = cookie,
          .nickname = std::string{rtype::server::testing::PLAYER_NICKNAME}});
}

std::vector<std::byte> requestDatagramOf(const std::string& nickname,
                                         std::uint64_t cookie) {
  return rtype::network::DatagramCodec::encode(
      rtype::network::PacketHeader{},
      rtype::network::ConnectionRequest{.cookie = cookie,
                                        .nickname = nickname});
}

std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(
      header, rtype::network::ReadyState{.ready = true});
}

std::vector<std::byte> heartbeatDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(header,
                                               rtype::network::Heartbeat{});
}

std::vector<std::byte> disconnectionDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(header,
                                               rtype::network::Disconnection{});
}

std::vector<std::byte> inputDatagram(
    const rtype::network::PlayerInput& input,
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(header, input);
}
