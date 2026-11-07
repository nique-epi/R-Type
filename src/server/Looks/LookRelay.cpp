#include "LookRelay.hpp"
#include <functional>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "EncodedMessage.hpp"
#include "LookSeat.hpp"
#include "LooksConstants.hpp"
#include "MessageDelivery.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerLooks.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SeatIndex.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::server {

LookRelay::LookRelay(PlayerRoster& roster,
                     std::function<void(network::OutgoingDatagram)> send)
    : roster_(roster), send_(std::move(send)) {}

void LookRelay::onChoice(ConnectedPlayer& player,
                         const network::ShipLookChoice& choice) {
  LookSeat& seat = seats_.at(seatIndexOf(player));
  if (ticks_ < seat.nextChoiceTick) {
    return;
  }
  seat.nextChoiceTick = ticks_ + TICKS_BETWEEN_LOOK_CHOICES;
  if (seat.look == choice.look) {
    guaranteeToPlayer(player, send_, playerLooks());
    return;
  }
  seat.look = choice.look;
  guaranteeToEveryPlayer(roster_, send_, playerLooks());
}

void LookRelay::onPlayerJoined() {
  guaranteeToEveryPlayer(roster_, send_, playerLooks());
}

void LookRelay::onPlayerClosing(const ConnectedPlayer& departed) {
  seats_.at(seatIndexOf(departed)) = LookSeat{};
}

void LookRelay::onTick() { ++ticks_; }

network::EncodedMessage LookRelay::playerLooks() {
  network::PlayerLooks message;
  message.players.reserve(network::MAXIMUM_PLAYERS);
  for (const ConnectedPlayer* const player : roster_.connected()) {
    message.players.push_back(
        network::PlayerLook{.playerNumber = player->playerNumber,
                            .look = seats_.at(seatIndexOf(*player)).look});
  }
  return network::encodedMessage(message);
}

}  // namespace rtype::server
