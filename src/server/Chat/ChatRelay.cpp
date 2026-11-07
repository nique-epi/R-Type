#include "ChatRelay.hpp"
#include <cstddef>
#include <functional>
#include <utility>
#include "ChatConstants.hpp"
#include "ChatLine.hpp"
#include "ChatSeat.hpp"
#include "ConnectedPlayer.hpp"
#include "MessageDelivery.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerChatLine.hpp"
#include "PlayerRoster.hpp"
#include "SeatIndex.hpp"
#include "SequenceOrder.hpp"

namespace rtype::server {

namespace {

/** @returns Whether @p line is a new line of the player on @p seat, not a copy
 * of one relayed, sent once the seat may send again at @p tick. */
bool isRelayable(const ChatSeat& seat, const network::ChatLine& line,
                 std::size_t tick) {
  if (tick < seat.nextLineTick) {
    return false;
  }
  if (!seat.newestLineNumber.has_value()) {
    return true;
  }
  return network::isNewerSequence(line.lineNumber, *seat.newestLineNumber);
}

}  // namespace

ChatRelay::ChatRelay(PlayerRoster& roster,
                     std::function<void(network::OutgoingDatagram)> send)
    : roster_(roster), send_(std::move(send)) {}

void ChatRelay::onLine(const ConnectedPlayer& player,
                       const network::ChatLine& line) {
  ChatSeat& seat = seats_.at(seatIndexOf(player));
  if (!isRelayable(seat, line, ticks_)) {
    return;
  }
  seat = ChatSeat{.newestLineNumber = line.lineNumber,
                  .nextLineTick = ticks_ + TICKS_BETWEEN_CHAT_LINES};
  guaranteeToEveryPlayer(
      roster_, send_,
      network::encodedMessage(network::PlayerChatLine{
          .line = line, .playerNumber = player.playerNumber}));
}

void ChatRelay::onPlayerClosing(const ConnectedPlayer& departed) {
  seats_.at(seatIndexOf(departed)) = ChatSeat{};
}

void ChatRelay::onTick() { ++ticks_; }

}  // namespace rtype::server
