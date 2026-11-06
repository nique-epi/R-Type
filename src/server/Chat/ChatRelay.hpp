#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include "ChatLine.hpp"
#include "ChatSeat.hpp"
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief Relays the chat lines of the players of a game to every player of the
 * game, its writer included, as a guaranteed PlayerChatLine.
 *
 * A line is relayed once: its client sends it again until the relay comes back,
 * and a line whose number is not newer than the newest one relayed for its
 * player is such a copy, ignored. A player gets at most one line relayed per
 * TICKS_BETWEEN_CHAT_LINES ticks; a line sooner is ignored, not recorded, so
 * its next copy goes through once the wait is over. A copy or a flood costs a
 * lookup and a comparison, and writes no line. The wait and the number belong
 * to the player, not to its seat: the next player on a seat freed by a
 * departure starts afresh. The ticks are counted here because the tick of the
 * game stays 0 while it waits for players, when they chat too.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class ChatRelay {
 public:
  /**
   * @param roster The players of the game; it must outlive the relay.
   * @param send Called for each datagram to send. Must not be empty.
   */
  ChatRelay(PlayerRoster& roster,
            std::function<void(network::OutgoingDatagram)> send);

  /** @brief Relays @p line of @p player to every player, unless it is a copy of
   * a line relayed or comes before the wait of the player is over. */
  void onLine(const ConnectedPlayer& player, const network::ChatLine& line);

  /** @brief Forgets the number and the wait of @p departed, whose seat is
   * freed. */
  void onPlayerClosing(const ConnectedPlayer& departed);

  /** @brief Counts a tick. */
  void onTick();

 private:
  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  std::array<ChatSeat, network::MAXIMUM_PLAYERS> seats_{};
  std::size_t ticks_{0};
};

}  // namespace rtype::server
