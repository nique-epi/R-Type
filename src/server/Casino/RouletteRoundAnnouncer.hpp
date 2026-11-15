#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RouletteRound.hpp"

namespace rtype::server {

/**
 * @brief Tells the players the round lists the state of the roulette table:
 * at once when it changes, and every TICKS_PER_ROULETTE_ROUND ticks otherwise,
 * so a lost RouletteRound is made good. The ticks left alone are no change: a
 * client counts them down between two RouletteRound.
 *
 * A table nobody sits at sends nothing; a player who went back to the room
 * learns it from LobbyState. It goes only to players the game admitted, so a
 * forged address gets nothing.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class RouletteRoundAnnouncer {
 public:
  /**
   * @param roster The players of the game; it must outlive the announcer.
   * @param send Called for each datagram to send. Must not be empty.
   */
  RouletteRoundAnnouncer(PlayerRoster& roster,
                         std::function<void(network::OutgoingDatagram)> send);

  /** @brief Sends @p round when it changed or the interval passed. */
  void onTick(const network::RouletteRound& round);

  /** @brief Forgets what was sent, so the next round goes out at once. */
  void forget();

 private:
  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  std::optional<network::RouletteRound> lastSent_;
  std::size_t ticksSinceLastSend_{0};
};

}  // namespace rtype::server
