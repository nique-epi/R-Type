#include "ServerDatagrams.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "EncodedMessage.hpp"
#include "Endpoint.hpp"
#include "Heartbeat.hpp"
#include "MasterProbe.hpp"
#include "MessagePayload.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ReadyState.hpp"
#include "ServerInformationRequest.hpp"
#include "ServerLobbyTestConstants.hpp"

rtype::network::PacketHeader playerHeader(std::uint64_t sessionToken) {
  rtype::network::PacketHeader header;
  header.sessionToken = sessionToken;
  return header;
}

std::vector<std::byte> playerReady(std::uint64_t sessionToken) {
  return readyDatagram(playerHeader(sessionToken));
}

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

std::vector<std::byte> requestDatagramFor(
    const rtype::network::GameChoice& game, std::uint64_t cookie) {
  return rtype::network::DatagramCodec::encode(
      rtype::network::PacketHeader{},
      rtype::network::ConnectionRequest{
          .cookie = cookie,
          .nickname = std::string{rtype::server::testing::PLAYER_NICKNAME},
          .game = game});
}

std::vector<std::byte> requestDatagramOf(const std::string& nickname,
                                         std::uint64_t cookie) {
  return rtype::network::DatagramCodec::encode(
      rtype::network::PacketHeader{},
      rtype::network::ConnectionRequest{.cookie = cookie,
                                        .nickname = nickname});
}

std::vector<std::byte> informationRequestDatagram(
    const rtype::network::PacketHeader& header, std::uint32_t sentAt) {
  return rtype::network::DatagramCodec::encode(
      header, rtype::network::ServerInformationRequest{.sentAt = sentAt});
}

std::vector<std::byte> requestAndInformationRequestDatagram() {
  const std::vector<rtype::network::EncodedMessage> messages{
      rtype::network::encodedMessage(rtype::network::ConnectionRequest{
          .cookie = 0,
          .nickname = std::string{rtype::server::testing::PLAYER_NICKNAME}}),
      rtype::network::encodedMessage(
          rtype::network::ServerInformationRequest{.sentAt = 0})};
  return rtype::network::DatagramCodec::encode(rtype::network::PacketHeader{},
                                               messages);
}

std::vector<std::byte> probeDatagram(
    const rtype::network::PacketHeader& header) {
  return rtype::network::DatagramCodec::encode(header,
                                               rtype::network::MasterProbe{});
}

std::vector<std::byte> informationRequestAndProbeDatagram() {
  const std::vector<rtype::network::EncodedMessage> messages{
      rtype::network::encodedMessage(
          rtype::network::ServerInformationRequest{.sentAt = 0}),
      rtype::network::encodedMessage(rtype::network::MasterProbe{})};
  return rtype::network::DatagramCodec::encode(rtype::network::PacketHeader{},
                                               messages);
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

std::vector<std::byte> heartbeatDatagram(
    const rtype::network::PacketHeader& header,
    const rtype::network::Heartbeat& heartbeat) {
  return rtype::network::DatagramCodec::encode(header, heartbeat);
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
