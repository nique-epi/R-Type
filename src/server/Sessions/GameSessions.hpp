#pragma once

#include <functional>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramReader.hpp"
#include "Departures.hpp"
#include "Endpoint.hpp"
#include "GameSessionsParts.hpp"
#include "IClock.hpp"
#include "IncomingDatagram.hpp"
#include "InputKeeper.hpp"
#include "MessageDispatcher.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The players of one game and the sessions the server keeps with them:
 * who they are, whether they are ready, their newest input, the silence that
 * closes a session, their departure and the Heartbeats that keep it open.
 * Nothing in it belongs to one game: the game gives it the things that do (see
 * GameSessionsParts).
 *
 * A datagram of the players is read when its header carries this protocol and
 * version with no flag set, its messages add up to its end, and its sender is
 * in the roster (see DatagramReader). Reading one resets the silence of its
 * sender, whatever it holds. Its ReadyState is kept by a ReadyKeeper, its
 * PlayerInput by an InputKeeper, and a Disconnection closes the session of its
 * sender at once, with a PlayerDeparture (Left): the copies of that
 * Disconnection the client sends after it come from an address the roster no
 * longer holds, so they change nothing.
 *
 * The silence of a player is counted in the ticks the game runs, and the time
 * since the server last sent a player a datagram in real time, read on the
 * clock at the start of each tick: during a stall of the server, the datagrams
 * of the players wait in their queue and are read at the next tick, while their
 * clients go on waiting, so the first tick after a stall of HEARTBEAT_INTERVAL
 * or more sends a Heartbeat to each player it sent nothing else.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameSessions {
 public:
  explicit GameSessions(GameSessionsParts parts);

  GameSessions(const GameSessions&) = delete;
  GameSessions& operator=(const GameSessions&) = delete;
  GameSessions(GameSessions&&) = delete;
  GameSessions& operator=(GameSessions&&) = delete;
  ~GameSessions() = default;

  /**
   * @returns The players; it stays at the same address for the life of the
   * sessions.
   */
  [[nodiscard]] PlayerRoster& roster();

  /**
   * @brief Starts the tick, before anything is sent in it: adds the time on the
   * clock since the start of the previous tick, or since the sessions were
   * built, to ConnectedPlayer::sinceSent of each player.
   */
  void startTick();

  /**
   * @brief Reads @p datagrams, oldest first: applies each ReadyState, keeps the
   * newest PlayerInput and closes the session of each player who sent a
   * Disconnection. Nothing caused by the content of a datagram makes it throw.
   */
  void readDatagrams(std::span<const network::IncomingDatagram> datagrams);

  /** @brief Closes the session of every player not heard from for
   * SESSION_TIMEOUT_TICKS ticks, and counts one more tick of silence for each
   * player it does not close. */
  void closeSilentSessions();

  /** @returns Whether at least one player is connected and every connected
   * player is ready. */
  [[nodiscard]] bool everyPlayerIsReady();

  /**
   * @brief Ends the tick: sends a Heartbeat to every player sent nothing for
   * SILENCE_BEFORE_HEARTBEAT, then counts the tick.
   */
  void endTick();

  /**
   * @returns The header of the next datagram to the player who sends from
   * @p player, as sessionHeaderFor() gives it: the session token and the next
   * sequence of that player, which then moves on by one, or a header without
   * session when no player sends from there.
   */
  [[nodiscard]] network::PacketHeader nextSessionHeaderFor(
      const network::Endpoint& player);

  /**
   * @returns The players whose session closed since the previous call, as they
   * were when it closed, oldest first; the next call starts afresh. Whoever
   * admitted their address or sends them events must forget them.
   */
  [[nodiscard]] std::vector<ConnectedPlayer> takeDepartedPlayers();

 private:
  void readDatagram(const network::IncomingDatagram& datagram);

  std::function<void(network::OutgoingDatagram)> send_;
  const engine::IClock* clock_{nullptr};
  engine::Duration previousTickStart_{};
  PlayerRoster roster_;
  DatagramReader reader_;
  ReadyKeeper readyKeeper_;
  InputKeeper inputKeeper_;
  Departures departures_;
  network::MessageDispatcher dispatcher_;
};

}  // namespace rtype::server
