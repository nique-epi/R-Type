#include "ServerPingProbeFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationRequest.hpp"
#include "ServerPingProbe.hpp"
#include "ServerPingProbeTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

ServerPingProbeFixture::ServerPingProbeFixture() {
  clock_.advance(ROUND_SENT_AT);
}

network::Endpoint ServerPingProbeFixture::paris() {
  return network::Endpoint{.address = std::string{PARIS_ADDRESS},
                           .port = GAME_PORT};
}

network::Endpoint ServerPingProbeFixture::lyon() {
  return network::Endpoint{.address = std::string{LYON_ADDRESS},
                           .port = GAME_PORT};
}

network::Endpoint ServerPingProbeFixture::stranger() {
  return network::Endpoint{.address = std::string{STRANGER_ADDRESS},
                           .port = GAME_PORT};
}

void ServerPingProbeFixture::measureParisAndLyon() {
  probe_.measure({paris(), lyon()});
}

std::uint32_t ServerPingProbeFixture::sentAtOfTheRound() const {
  const std::optional<network::ServerInformationRequest> request =
      network::DatagramCodec::firstMessage<network::ServerInformationRequest>(
          link_.sent().front().payload);
  return request.value_or(network::ServerInformationRequest{}).sentAt;
}

void ServerPingProbeFixture::deliverAnswer(const network::Endpoint& server,
                                           std::uint32_t sentAt,
                                           engine::Duration after) {
  const network::ServerInformation information{
      .sentAt = sentAt,
      .heldFor = HELD_FOR_MICROSECONDS,
      .protocolVersion = network::PROTOCOL_VERSION,
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .name = std::string{SERVER_NAME},
      .playerCount = 0,
      .gameCount = 0,
      .freeSeatCount = 0};
  link_.deliver(network::IncomingDatagram{
      .sender = server,
      .payload =
          network::DatagramCodec::encode(network::PacketHeader{}, information),
      .receivedAt = ROUND_SENT_AT + after});
}

void ServerPingProbeFixture::deliverGarbageFromParis() {
  link_.deliver(network::IncomingDatagram{
      .sender = paris(),
      .payload = std::vector<std::byte>(GARBAGE_SIZE, GARBAGE_BYTE),
      .receivedAt = ROUND_SENT_AT + ANSWER_ARRIVES_AFTER});
}

ServerPingProbe& ServerPingProbeFixture::probe() { return probe_; }

const RecordingDatagramLink& ServerPingProbeFixture::link() const {
  return link_;
}

}  // namespace rtype::client::testing
