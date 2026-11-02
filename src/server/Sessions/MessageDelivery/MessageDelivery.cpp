#include "MessageDelivery.hpp"
#include <cstdint>
#include <functional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EncodedMessage.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Sends @p player every guaranteed message it has due, in as few
 * datagrams as they fit, each under the next session header of the player
 * with GUARANTEED_FLAG set.
 */
void sendDueGuaranteedMessages(
    ConnectedPlayer& player,
    const std::function<void(network::OutgoingDatagram)>& send) {
  while (player.unacknowledged.hasDue()) {
    network::PacketHeader header = nextSessionHeader(player);
    header.flags =
        static_cast<std::uint8_t>(header.flags | network::GUARANTEED_FLAG);
    const std::vector<network::EncodedMessage> due =
        player.unacknowledged.takeDue(header.sequence);
    send(network::OutgoingDatagram{
        .destination = player.endpoint,
        .payload = network::DatagramCodec::encode(header, due)});
  }
}

}  // namespace

void sendToEveryPlayer(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message) {
  for (ConnectedPlayer* const player : roster.connected()) {
    send(sessionDatagram(*player, message.type, message.payload));
  }
}

void guaranteeToEveryPlayer(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message) {
  for (ConnectedPlayer* const player : roster.connected()) {
    player->unacknowledged.add(message);
    sendDueGuaranteedMessages(*player, send);
  }
}

void ageGuaranteedMessages(PlayerRoster& roster, engine::Duration elapsed) {
  for (ConnectedPlayer* const player : roster.connected()) {
    player->unacknowledged.advance(elapsed);
  }
}

void resendGuaranteedMessages(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send) {
  for (ConnectedPlayer* const player : roster.connected()) {
    sendDueGuaranteedMessages(*player, send);
  }
}

}  // namespace rtype::server
