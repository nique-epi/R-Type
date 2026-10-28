#pragma once

#include <functional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Closes the sessions of the players who leave a game, and keeps them
 * for whoever must forget them outside the game.
 *
 * A player is heard from each time its datagram is read, whatever it holds. A
 * player not heard from for SESSION_TIMEOUT_TICKS ticks crashed or lost the
 * network: closeSilentSessions() closes its session. A game counts the ticks
 * it runs, so a stall of the server closes no session by itself; a stall long
 * enough for the clients to take the server for lost closes each of their
 * sessions, SESSION_TIMEOUT_TICKS ticks after the last datagram the server read
 * from its client, since those clients send nothing more.
 *
 * Closing a session frees the seat and the number of the player, calls the
 * function given at construction with the player, logs the departure,
 * publishes it on the event bus as a PlayerDeparture, and keeps the player for
 * takeDeparted(). Everything after the seat is freed reads the copy of the
 * player the roster handed back.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class Departures {
 public:
  /**
   * @param roster The players; it must outlive the object.
   * @param events Where the departures are published; it must outlive the
   * object.
   * @param onClosing Called for each player whose session closes; must not be
   * empty.
   */
  Departures(PlayerRoster& roster, engine::EventBus& events,
             std::function<void(const ConnectedPlayer&)> onClosing);

  /**
   * @brief Closes the session of the player sending from @p player, for
   * @p cause. Does nothing when no player of the game sends from there.
   */
  void close(const network::Endpoint& player, game::DepartureCause cause);

  /**
   * @brief Closes the session of every player not heard from for
   * SESSION_TIMEOUT_TICKS ticks, and counts one more tick of silence for the
   * others. The players to close are listed first, then closed, so no seat is
   * freed while the roster is being walked.
   */
  void closeSilentSessions();

  /**
   * @returns The players whose session closed since the previous call, as they
   * were when it closed, oldest first; the next call starts afresh.
   */
  [[nodiscard]] std::vector<ConnectedPlayer> takeDeparted();

 private:
  /** @returns The addresses of the players not heard from for
   * SESSION_TIMEOUT_TICKS ticks; counts one more tick of silence for every
   * other player. */
  [[nodiscard]] std::vector<network::Endpoint> countSilentTicks();

  /** @brief Logs that @p player left or timed out, as @p cause says, and
   * publishes its PlayerDeparture. */
  void announce(const ConnectedPlayer& player, game::DepartureCause cause);

  /** @brief Writes the info line of a departure: the address of a player who
   * left, or how long a player who timed out stayed silent. */
  void logDeparture(const ConnectedPlayer& player,
                    game::DepartureCause cause) const;

  PlayerRoster& roster_;
  engine::EventBus& events_;
  std::function<void(const ConnectedPlayer&)> onClosing_;
  std::vector<ConnectedPlayer> departed_;
  logging::Logger logger_;
};

}  // namespace rtype::server
