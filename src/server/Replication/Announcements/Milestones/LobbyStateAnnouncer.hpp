#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include "LobbyState.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Tells the players of a game who is in the waiting room, who is
 * ready, and the maps they vote for, as the A session section of the protocol
 * page describes.
 *
 * The game calls onWaitingTick() once per tick while it waits for players.
 * The room is read from the roster each time: every connected player, in
 * order of number, with its ready state, its nickname and its map vote, then
 * the names of the maps of the game. It is sent to every
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
   * @param maps The names of the maps the room offers to the vote, in the
   * order the votes number them; empty when the game offers none.
   */
  LobbyStateAnnouncer(PlayerRoster& roster, Send send,
                      std::vector<std::string> maps);

  /**
   * @brief Lets one tick of the waiting room pass: sends the room when it
   * differs from the room sent last, or when TICKS_PER_LOBBY_STATE ticks
   * passed since the last send.
   */
  void onWaitingTick();

 private:
  PlayerRoster* roster_{nullptr};
  Send send_;
  std::vector<std::string> maps_;
  network::LobbyState lastSent_;
  std::size_t ticksSinceLastSend_{0};
};

}  // namespace rtype::server
