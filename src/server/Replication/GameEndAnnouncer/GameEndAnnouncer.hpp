#pragma once

#include <cstddef>
#include "EntitySpawnSender.hpp"
#include "GameEnded.hpp"
#include "PlayerRoster.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

/**
 * @brief Tells the players of a game that it is over, with the score of each
 * of them, as a GameEnded (see the A session section of the protocol page).
 *
 * The game builds one when it ends, and the result is read from the roster
 * then: every connected player, in order of number, with its score. It never
 * changes after, so every copy says the same, whoever leaves or scores
 * meanwhile. The game calls onFinishedTick() once per tick while it stays
 * finished: the first call sends the result to every connected player, then
 * it goes again to the players still connected every TICKS_PER_GAME_ENDED
 * ticks, so a lost GameEnded is corrected within GAME_ENDED_INTERVAL. Each
 * datagram carries the session token and the next sequence of its recipient
 * (see nextSessionHeader).
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameEndAnnouncer {
 public:
  /**
   * @param roster The players of the game, whose result is read now; it must
   * outlive the announcer.
   * @param send Must not be empty.
   */
  GameEndAnnouncer(PlayerRoster& roster, SendDatagram send);

  /**
   * @brief Lets one tick of the finished game pass: sends the result at the
   * first one, then once TICKS_PER_GAME_ENDED ticks passed since the last
   * send.
   */
  void onFinishedTick();

 private:
  PlayerRoster* roster_{nullptr};
  SendDatagram send_;
  network::GameEnded result_;
  /** @brief Starts a whole interval in, so the first tick sends. */
  std::size_t ticksSinceLastSend_{TICKS_PER_GAME_ENDED};
};

}  // namespace rtype::server
