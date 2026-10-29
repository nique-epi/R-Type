#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "GameContext.hpp"
#include "GameInstanceParts.hpp"
#include "GameModules.hpp"
#include "GameSessions.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameModule.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "SessionHooks.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

/**
 * @brief One game: the context its modules work in (its players, its event
 * bus, its timers, its systems and its settings) and its tick, run in stages.
 *
 * A game knows nothing of the rules it plays. Its modules (see IGameModule)
 * carry them out: the game installs them in the order they are listed, then
 * starts them, when it is built, and stops them in the opposite order when it
 * is destroyed. One of them must provide IGameFlow, which tells the game its
 * state and its tick and which the game calls when a player joins, when a
 * session closes and when an input is kept. The rules of R-Type and their
 * replication are two such modules (see RtypeRulesModule and
 * ReplicationModule). Every datagram to a player carries its session token and
 * its next sequence (see nextSessionHeaderFor).
 *
 * The players and their sessions (the datagrams read, who is ready, the newest
 * input, the silence that closes a session, the departures and the Heartbeats)
 * belong to GameSessions. The game gives it three things: a ReadyState counts
 * only while the game waits for players (IGameFlow::state); each PlayerInput
 * newer than the one kept goes to the SessionHooks::newerInput functions of the
 * modules before it replaces it; and a player whose session closes goes to
 * their SessionHooks::closing functions. A player who joins goes to their
 * SessionHooks::joined functions (see onPlayerJoined()). takeDepartedPlayers()
 * hands the players whose session closed over to whoever must forget them
 * outside the game.
 *
 * A tick runs three stages, in order: GAME_INPUT_STAGE reads the datagrams and
 * closes the silent sessions, GAME_SIMULATION_STAGE runs the work the modules
 * added, and GAME_OUTPUT_STAGE sends the Heartbeats. The work of the game
 * itself is added before the modules are installed, so it runs first in the
 * input and output stages, ahead of the work of the modules.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameInstance {
 public:
  /**
   * @param parts What the game sends with, is defined by and publishes on.
   * @param modules The modules of the game, in the order they are installed.
   * Must not hold a null pointer, and one of them must provide IGameFlow,
   * after the modules whose services it requires.
   * @throws engine::MissingServiceException when a module requires a service
   * no module before it provides, or none provides IGameFlow.
   */
  GameInstance(GameInstanceParts parts,
               std::vector<std::unique_ptr<IGameModule>> modules);

  GameInstance(const GameInstance&) = delete;
  GameInstance& operator=(const GameInstance&) = delete;
  GameInstance(GameInstance&&) = delete;
  GameInstance& operator=(GameInstance&&) = delete;
  ~GameInstance() = default;

  [[nodiscard]] GameState state() const;

  /**
   * @returns How many ticks were simulated since the game started: 0 while it
   * waits for players, and again once it waits after a game.
   */
  [[nodiscard]] std::uint32_t tick() const;

  /**
   * @returns The players of the game; it stays at the same address for the
   * life of the game.
   */
  [[nodiscard]] PlayerRoster& roster();

  /**
   * @returns The header of the next datagram to the player of the game who
   * sends from @p player, as sessionHeaderFor() gives it: the session token
   * and the next sequence of that player, which then moves on by one, or a
   * header without session when no player of the game sends from there.
   */
  [[nodiscard]] network::PacketHeader nextSessionHeaderFor(
      const network::Endpoint& player);

  /**
   * @brief Tells the modules that @p player just took a place in roster(), as
   * the SessionHooks::joined functions receive it; with the rules of R-Type,
   * the player gets a ship in the world, announced to the players.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  void onPlayerJoined(ConnectedPlayer& player) const;

  /**
   * @brief Runs one tick: applies the ReadyState, keeps the newest PlayerInput
   * and closes the session of each player who sent a Disconnection, among
   * @p datagrams, oldest first, closes the session of every player not heard
   * from for SESSION_TIMEOUT_TICKS ticks, then runs the work of the modules.
   * Last, a player sent nothing for TICKS_PER_HEARTBEAT ticks is sent a
   * Heartbeat. Nothing caused by the content of a datagram makes it throw.
   * @p datagrams must stay valid until the call returns.
   */
  void onTick(std::span<const network::IncomingDatagram> datagrams);

  /**
   * @returns The players whose session closed since the previous call, as they
   * were when it closed, oldest first; the next call starts afresh. Whoever
   * admitted their address or sends them events must forget them.
   */
  [[nodiscard]] std::vector<ConnectedPlayer> takeDepartedPlayers();

  /**
   * @brief Ends the game in progress without telling its players a result:
   * only a game that ends by itself does. Does nothing in any other state.
   */
  void finish();

 private:
  SessionHooks hooks_;
  engine::SystemScheduler systems_;
  GameSessions sessions_;
  GameContext context_;
  std::span<const network::IncomingDatagram> datagrams_;
  IGameFlow* flow_{nullptr};
  GameModules modules_;
};

}  // namespace rtype::server
