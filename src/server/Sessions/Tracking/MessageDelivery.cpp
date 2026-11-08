#include "MessageDelivery.hpp"
#include <functional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EncodedMessage.hpp"
#include "GuaranteedDelivery.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
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
    network::markGuaranteed(header);
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

void guaranteeToPlayer(
    ConnectedPlayer& player,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message) {
  player.unacknowledged.add(message);
  sendDueGuaranteedMessages(player, send);
}

void guaranteeToEveryPlayer(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message) {
  for (ConnectedPlayer* const player : roster.connected()) {
    guaranteeToPlayer(*player, send, message);
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
