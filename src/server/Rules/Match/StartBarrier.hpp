#pragma once

#include <cstddef>
#include <functional>
#include "GameStarted.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Holds the start of a game until every player shows the game screen.
 *
 * open() tells every connected player the GameStarted of the world, the sky
 * its level opens in, and clears whether they are loaded. Each advance() counts
 * a tick, and says the game may start once every connected player that is not
 * away sent GameLoaded (see ConnectedPlayer::loaded), or once
 * START_BARRIER_TICKS ticks went by, so a client that never loads holds the
 * others for that long at most. Every GAME_STARTED_REPEAT_TICKS ticks the
 * players not yet loaded are sent that same GameStarted again, since a lost
 * copy would otherwise cost them the whole wait.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class StartBarrier {
 public:
  /**
   * @param roster The players; it must outlive the barrier.
   * @param send What is called for each datagram to send; it must not be empty.
   */
  StartBarrier(PlayerRoster& roster,
               std::function<void(network::OutgoingDatagram)> send);

  /** @brief Starts the wait: no player is loaded, every connected player is
   * sent @p started, which the wait repeats. */
  void open(network::GameStarted started);

  /**
   * @brief Counts one tick of the wait.
   * @returns Whether the game may start now.
   */
  [[nodiscard]] bool advance();

 private:
  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  logging::Logger logger_;
  /** What open() sent, repeated to the players still loading. */
  network::GameStarted started_;
  std::size_t waitedTicks_{0};
};

}  // namespace rtype::server
