#pragma once

#include <cstddef>
#include <functional>
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Holds the start of a game until every player shows the game screen.
 *
 * open() tells every connected player GameStarted and clears whether they are
 * loaded. Each advance() counts a tick, and says the game may start once every
 * connected player that is not away sent GameLoaded (see
 * ConnectedPlayer::loaded), or once START_BARRIER_TICKS ticks went by, so a
 * client that never loads holds the others for that long at most. Every
 * GAME_STARTED_REPEAT_TICKS ticks the players not yet loaded are sent
 * GameStarted again, since a lost copy would otherwise cost them the whole
 * wait.
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
   * sent GameStarted. */
  void open();

  /**
   * @brief Counts one tick of the wait.
   * @returns Whether the game may start now.
   */
  [[nodiscard]] bool advance();

 private:
  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  logging::Logger logger_;
  std::size_t waitedTicks_{0};
};

}  // namespace rtype::server
