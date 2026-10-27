#include "SessionOpening.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "FloodConstants.hpp"
#include "IncomingDatagram.hpp"
#include "LoopbackPeer.hpp"
#include "MalformedDatagramConstants.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::tools {

namespace {

std::vector<std::byte> requestDatagram(std::uint64_t cookie) {
  return network::DatagramCodec::encode(
      network::PacketHeader{},
      network::ConnectionRequest{.cookie = cookie,
                                 .nickname = std::string{TOOL_NICKNAME}});
}

std::vector<std::byte> acceptanceDatagram() {
  return network::DatagramCodec::encode(
      network::PacketHeader{.sessionToken = TOOL_SESSION_TOKEN,
                            .sequence = SEQUENCE_BEFORE_THE_FLOOD},
      network::ConnectionAccepted{.playerNumber =
                                      network::FIRST_PLAYER_NUMBER});
}

/**
 * @returns The first message of type Message that @p datagram holds, or
 * nothing when it holds none or cannot be read.
 */
template <network::DecodableMessage Message>
std::optional<Message> firstMessageOf(
    const network::IncomingDatagram& datagram) {
  try {
    for (const network::MessageFrame& message :
         network::DatagramCodec::splitMessages(datagram.payload)) {
      if (message.type == static_cast<std::uint8_t>(Message::TYPE)) {
        return network::decodeMessage<Message>(message.payload);
      }
    }
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
  return std::nullopt;
}

/**
 * @returns The ConnectionChallenge @p peer receives within ANSWER_TIMEOUT, or
 * nothing when the answer is another message or does not come.
 */
std::optional<network::ConnectionChallenge> challengeTo(LoopbackPeer& peer) {
  const std::optional<network::IncomingDatagram> answer =
      peer.receive(ANSWER_TIMEOUT);
  if (!answer.has_value()) {
    return std::nullopt;
  }
  return firstMessageOf<network::ConnectionChallenge>(*answer);
}

/**
 * @returns The session token of the ConnectionAccepted @p peer receives
 * within ANSWER_TIMEOUT, or nothing when the answer is another message or
 * does not come.
 */
std::optional<std::uint64_t> acceptedToken(LoopbackPeer& peer) {
  const std::optional<network::IncomingDatagram> answer =
      peer.receive(ANSWER_TIMEOUT);
  if (!answer.has_value() ||
      !firstMessageOf<network::ConnectionAccepted>(*answer).has_value()) {
    return std::nullopt;
  }
  return network::DatagramCodec::decodeHeader(answer->payload).sessionToken;
}

/**
 * @returns The port of the first sender whose datagram to @p peer holds a
 * ConnectionRequest within CLIENT_WAIT_TIMEOUT, or nothing.
 */
std::optional<std::uint16_t> firstRequesterOf(LoopbackPeer& peer) {
  const auto deadline = std::chrono::steady_clock::now() + CLIENT_WAIT_TIMEOUT;
  while (std::chrono::steady_clock::now() < deadline) {
    const std::optional<network::IncomingDatagram> datagram =
        peer.receive(ANSWER_TIMEOUT);
    if (datagram.has_value() &&
        firstMessageOf<network::ConnectionRequest>(*datagram).has_value()) {
      return datagram->sender.port;
    }
  }
  return std::nullopt;
}

}  // namespace

std::optional<std::uint64_t> joinServer(LoopbackPeer& peer,
                                        std::uint16_t serverPort) {
  peer.send(serverPort, requestDatagram(NO_COOKIE));
  const std::optional<network::ConnectionChallenge> challenge =
      challengeTo(peer);
  if (!challenge.has_value()) {
    return std::nullopt;
  }
  peer.send(serverPort, requestDatagram(challenge->cookie));
  return acceptedToken(peer);
}

bool serverAnswers(std::uint16_t serverPort) {
  LoopbackPeer stranger{network::ANY_PORT};
  stranger.send(serverPort, requestDatagram(NO_COOKIE));
  return stranger.receive(ANSWER_TIMEOUT).has_value();
}

std::optional<std::uint16_t> acceptClient(LoopbackPeer& peer) {
  const std::optional<std::uint16_t> clientPort = firstRequesterOf(peer);
  if (clientPort.has_value()) {
    peer.send(*clientPort, acceptanceDatagram());
  }
  return clientPort;
}

}  // namespace rtype::tools
