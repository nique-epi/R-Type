#pragma once

#include <cstddef>
#include <functional>
#include "LobbyState.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Tells the players of a game who is in the waiting room and who is
 * ready, as the A session section of the protocol page describes.
 *
 * The game calls onWaitingTick() once per tick while it waits for players.
 * The room is read from the roster each time: every connected player, in
 * order of number, with its ready state and its nickname. It is sent to every
 * connected player at once when it differs from the room sent last, which is
 * empty for a new announcer, and otherwise TICKS_PER_LOBBY_STATE ticks after
 * the last send, so a lost LobbyState is corrected within LOBBY_STATE_INTERVAL.
 * Each datagram carries the session token and the next sequence of its
 * recipient (see nextSessionHeader). A room without players reaches no one.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class LobbyStateAnnouncer {
 public:
  /**
   * @brief What the announcer calls for each datagram to send.
   */
  using Send = std::function<void(network::OutgoingDatagram)>;

  /**
   * @param roster The players of the game; it must outlive the announcer.
   * @param send Must not be empty.
   */
  LobbyStateAnnouncer(PlayerRoster& roster, Send send);

  /**
   * @brief Lets one tick of the waiting room pass: sends the room when it
   * differs from the room sent last, or when TICKS_PER_LOBBY_STATE ticks
   * passed since the last send.
   */
  void onWaitingTick();

 private:
  PlayerRoster* roster_{nullptr};
  Send send_;
  network::LobbyState lastSent_;
  std::size_t ticksSinceLastSend_{0};
};

}  // namespace rtype::server
