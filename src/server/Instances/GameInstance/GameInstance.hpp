#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameFlow.hpp"
#include "GameSessions.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ScoreKeeper.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief One game: its world, its players (1 to MAXIMUM_PLAYERS, whose numbers
 * also pick their colors), its state and its tick.
 *
 * How the game runs (its states, the tick, the world and the rules that work on
 * it) belongs to GameFlow. Every datagram to a player carries its session token
 * and its next sequence (see nextSessionHeaderFor).
 *
 * The players and their sessions (the datagrams read, who is ready, the newest
 * input, the silence that closes a session, the departures and the Heartbeats)
 * belong to GameSessions. The game gives it three things: a ReadyState counts
 * only while the game waits for players; each PlayerInput newer than the one
 * kept goes through noteTaps before it replaces it, so a direction pressed
 * twice starts a roll or a dash of the ship; and the ship of a player whose
 * session closes leaves the world and is announced on the event bus as an
 * EntityDestruction (Removed). A game in progress whose last player's session
 * closed ends at that same tick, as finish() ends it, so it waits for players
 * again FINISHED_STATE_TICKS ticks later (see GameFlow). takeDepartedPlayers()
 * hands the players whose session closed over to whoever must forget them
 * outside the game.
 *
 * A ScoreKeeper adds the points of each EnemyKilled published on the event bus
 * to the score of the player it names; every score goes back to 0 with each
 * new world.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameInstance {
 public:
  /**
   * @brief What the game calls for each datagram to send.
   */
  using Send = std::function<void(network::OutgoingDatagram)>;

  /**
   * @param send Must not be empty.
   * @param settings The seed the random draws of the game start from, logged
   * by the spawner each time a world is built, and the waves of its
   * difficulty, which must hold a playable curve and at least one enemy.
   * @param events Where the game publishes what happens in the world, such as
   * a Bydo leaving the playfield, a collision or a player leaving the game; it
   * must outlive the game, and its owner dispatches it after each onTick().
   */
  GameInstance(Send send, GameSettings settings, engine::EventBus& events);

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
   * @returns The world of the game. A new one replaces it when the game waits
   * for players again, so do not keep the reference across ticks.
   */
  [[nodiscard]] game::World& world();

  /**
   * @returns The header of the next datagram to the player of the game who
   * sends from @p player, as sessionHeaderFor() gives it: the session token
   * and the next sequence of that player, which then moves on by one, or a
   * header without session when no player of the game sends from there.
   */
  [[nodiscard]] network::PacketHeader nextSessionHeaderFor(
      const network::Endpoint& player);

  /**
   * @brief Gives @p player, who just took a place in roster(), a ship in the
   * world and announces it to the players.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  void onPlayerJoined(ConnectedPlayer& player);

  /**
   * @brief Runs one tick: applies the ReadyState, keeps the newest PlayerInput
   * and closes the session of each player who sent a Disconnection, among
   * @p datagrams, oldest first, closes the session of every player not heard
   * from for SESSION_TIMEOUT_TICKS ticks, then runs the tick of the game (see
   * GameFlow::onTick). Last, a player sent nothing for TICKS_PER_HEARTBEAT
   * ticks is sent a Heartbeat. Nothing caused by the content of a datagram
   * makes it throw.
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
   * only a game that ends by itself does (see finishIfOver). Does nothing in
   * any other state.
   */
  void finish();

 private:
  /** @brief Removes the ship of @p departed from the world, when it has one,
   * and announces it on the event bus as an EntityDestruction (Removed). */
  void removeShipOf(const ConnectedPlayer& departed);

  Send send_;
  engine::EventBus& events_;
  GameSessions sessions_;
  ScoreKeeper scoreKeeper_;
  GameFlow flow_;
};

}  // namespace rtype::server
