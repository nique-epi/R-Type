#include "SessionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "Heartbeat.hpp"
#include "Heartbeats.hpp"
#include "MessageOrigin.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SessionsTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

network::MessageOrigin originOf(std::size_t index, std::uint16_t sequence) {
  return network::MessageOrigin{
      .sender = playerEndpoint(index),
      .header = network::PacketHeader{.sequence = sequence}};
}

network::PlayerInput inputNumbered(std::uint32_t inputNumber,
                                   std::uint8_t actions) {
  return network::PlayerInput{
      .inputNumber = inputNumber, .actions = actions, .earlierActions = {}};
}

network::PlayerInput inputRepeating(std::uint32_t inputNumber,
                                    std::uint8_t actions,
                                    std::vector<std::uint8_t> earlierActions) {
  return network::PlayerInput{.inputNumber = inputNumber,
                              .actions = actions,
                              .earlierActions = std::move(earlierActions)};
}

ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index) {
  return *roster.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                     SESSION_TOKEN);
}

void addPlayers(PlayerRoster& roster, std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    static_cast<void>(addPlayer(roster, index));
  }
}

void ignoreInput(const ConnectedPlayer& /*player*/,
                 const network::PlayerInput& /*input*/) {}

std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
recorderOf(std::vector<ShownInput>& shown) {
  return [&shown](const ConnectedPlayer& player,
                  const network::PlayerInput& input) {
    shown.push_back(ShownInput{.input = input,
                               .numberKeptBefore = player.newestInputNumber});
  };
}

std::vector<network::OutgoingDatagram> heartbeatsSentAt(PlayerRoster& roster,
                                                        engine::Duration now) {
  std::vector<network::OutgoingDatagram> sent;
  sendHeartbeats(roster, now, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });
  return sent;
}

void readEchoFrom(PlayerRoster& roster, const network::Endpoint& sender) {
  readHeartbeat(
      roster,
      network::MessageOrigin{
          .sender = sender, .header = {}, .receivedAt = ECHO_ARRIVAL},
      network::Heartbeat{.sentAt = PLAYER_HEARTBEAT_SENT_AT,
                         .echo = network::HeartbeatEcho{
                             .sentAt = HEARTBEAT_SENDING_TIME_ON_THE_WIRE,
                             .heldFor = HELD_BY_THE_CLIENT}});
}

}  // namespace rtype::server::testing
