#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "EncodedMessage.hpp"
#include "LookSeat.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::server {

/**
 * @brief Shares the look of every player of a game with every player, as a
 * guaranteed PlayerLooks.
 *
 * A changed look goes to every player; a look equal to the one kept is a copy
 * its client sent while it waited for the answer, answered to its sender
 * alone. A player gets at most one choice taken every
 * TICKS_BETWEEN_LOOK_CHOICES ticks; a choice sooner is ignored. Every player
 * gets the looks when a player arrives. The look belongs to the seat until its
 * player leaves. Not thread-safe: use it from the simulation thread only.
 */
class LookRelay {
 public:
  /**
   * @param roster The players of the game; it must outlive the relay.
   * @param send Called for each datagram to send. Must not be empty.
   */
  LookRelay(PlayerRoster& roster,
            std::function<void(network::OutgoingDatagram)> send);

  /** @brief Keeps and shares @p choice of @p player, or answers it to
   * @p player alone when it changes nothing; ignored before the wait of the
   * player is over. */
  void onChoice(ConnectedPlayer& player, const network::ShipLookChoice& choice);

  /** @brief Sends the looks of every player to every player, the newcomer
   * included. */
  void onPlayerJoined();

  /** @brief Forgets the look and the wait of @p departed, whose seat is
   * freed. */
  void onPlayerClosing(const ConnectedPlayer& departed);

  /** @brief Counts a tick. */
  void onTick();

 private:
  [[nodiscard]] network::EncodedMessage playerLooks();

  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  std::array<LookSeat, network::MAXIMUM_PLAYERS> seats_{};
  std::size_t ticks_{0};
};

}  // namespace rtype::server
