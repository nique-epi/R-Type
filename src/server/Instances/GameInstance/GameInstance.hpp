#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <vector>
#include "BydoSpawner.hpp"
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "ScoreKeeper.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "SystemScheduler.hpp"
#include "ThrottledLog.hpp"
#include "World.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server {

/**
 * @brief One game: its world, its players (1 to MAXIMUM_PLAYERS, whose numbers
 * also pick their colors), its state and its tick.
 *
 * A game goes from WaitingForPlayers to InProgress when at least one player is
 * connected and every connected player is ready, from InProgress to Finished
 * when finish() is called, and from Finished back to WaitingForPlayers after
 * FINISHED_STATE_TICKS ticks. Coming back to WaitingForPlayers builds a new
 * world: every player gets a new ship, is no longer ready, and the tick is 0.
 * The players stay connected.
 *
 * The game tells its players where it stands, as the A session section of the
 * protocol page describes. While it waits for players, a LobbyStateAnnouncer
 * sends them the room; a new one comes with each new world, so the room goes
 * out at the first tick of every wait. The tick that starts the game sends
 * every connected player GameStarted, then, on that tick and every tick of the
 * game, the WorldStateSender sends them the world, from tick 0: GameStarted
 * always leaves before the first WorldState. Every datagram to a player
 * carries its session token and its next sequence (see nextSessionHeader).
 *
 * While the game is in progress, each tick first runs the systems of the world
 * (PatternSystem, MovementSystem, then PlayfieldExitSystem, which removes a
 * Bydo that left on the left and announces it on the event bus), then lets a
 * Bydo appear every BYDO_SPAWN_INTERVAL (see BydoSpawner). Each world starts
 * its random draws from a seed of its own, the seed of the game plus the number
 * of worlds built before it, so the seed of the game is enough to replay every
 * world of a session.
 *
 * A datagram of the players is read only when its header carries this
 * protocol and version with no flag set, its messages add up to its end, and
 * its sender is in the roster. Its session token is not checked here: the
 * datagrams onTick() receives are those the DatagramSorter of the network
 * thread kept, and it drops one from the address of a player that carries
 * another token before any queue holds it. Whoever forges the address of a
 * player could otherwise say ready in its place, or send an input numbered
 * 4294967295, above every input the player will ever send, and hold the ship
 * of that player on the forged keys for the rest of the session. A datagram
 * that is not read is logged with its sender and why at the debug level, at
 * most once per REJECTION_LINE_INTERVAL of ticks run, with the number dropped
 * since the previous line (see ThrottledLog), so a flood of them cannot flood
 * the log.
 *
 * A player is heard from each time the game reads one of its datagrams,
 * whatever it holds, a Heartbeat included. A player the game has not heard
 * from for SESSION_TIMEOUT_TICKS ticks crashed or lost the network: its session
 * closes at that tick, after the datagrams are read and before the game moves
 * along its states, so the rest of the tick runs without it. Its ship leaves
 * the world and is announced on the event bus as an EntityDestruction
 * (Removed), a PlayerDeparture (TimedOut) follows, and its seat and number are
 * free for the next player who joins. takeDepartedPlayers() hands the player
 * over to whoever must forget it outside the game. A game counts the ticks it
 * runs, so a stall of the server closes no session.
 *
 * A ReadyState counts only while the game waits for players, and only when its
 * sequence is newer than the newest one accepted from that player, so a late
 * copy never undoes a newer one.
 *
 * A PlayerInput is kept in ConnectedPlayer::newestInput when its inputNumber is
 * higher than the one kept, whatever the state of the game, so an input that
 * arrives late or twice changes nothing. inputNumber is compared as a plain
 * number: a session sending 60 inputs per second would take more than 800 days
 * to wrap it. The input kept outlives a new world, since the client numbers its
 * inputs for the whole session.
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
   * @param seed Where the random draws of the game start from; logged by the
   * spawner each time a world is built.
   * @param events Where the game publishes what happens in the world, such as
   * a Bydo leaving the playfield or a player leaving the game; it must outlive
   * the game.
   */
  GameInstance(Send send, std::uint32_t seed, engine::EventBus& events);

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
   * @brief Runs one tick: applies the ReadyState and keeps the newest
   * PlayerInput among @p datagrams, oldest first, closes the session of every
   * player not heard from for SESSION_TIMEOUT_TICKS ticks, then moves the game
   * along its states. While it waits, the players are told the room; while it
   * is in progress, the tick is counted, the systems run, the Bydos appear and
   * the world is sent. Nothing caused by the content of a datagram makes it
   * throw.
   */
  void onTick(std::span<const network::IncomingDatagram> datagrams);

  /**
   * @returns The players whose session closed since the previous call, as they
   * were when it closed, oldest first; the next call starts afresh. Whoever
   * admitted their address or sends them events must forget them.
   */
  [[nodiscard]] std::vector<ConnectedPlayer> takeDepartedPlayers();

  /**
   * @brief Ends the game in progress. Does nothing in any other state.
   */
  void finish();

 private:
  void readPlayerDatagram(const network::IncomingDatagram& datagram);
  void drop(const network::Endpoint& sender, std::string_view reason);

  /**
   * @returns The time the ticks run since the game was built stand for, the
   * clock of its log lines.
   */
  [[nodiscard]] std::chrono::nanoseconds timeRun() const;

  void applyReadyState(const network::MessageOrigin& origin,
                       const network::ReadyState& message);
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);
  [[nodiscard]] bool everyPlayerIsReady();

  /** @brief Closes the session of every player not heard from for
   * SESSION_TIMEOUT_TICKS ticks, and counts one more tick of silence for the
   * others. */
  void closeSilentSessions();

  /** @brief Frees the seat of the player sending from @p player, removes its
   * ship and announces it, announces the departure, then keeps the player for
   * takeDepartedPlayers(). Everything after the seat is freed reads the copy
   * of the player the roster handed back. */
  void closeSession(const network::Endpoint& player);

  /** @brief Publishes the EntityDestruction (Removed) of a ship that left the
   * world. */
  void announceShipRemoval(std::uint32_t shipEntityId);

  /** @brief Logs that @p player timed out and publishes its PlayerDeparture
   * (TimedOut). */
  void announceTimeout(const ConnectedPlayer& player);

  /** @brief Moves the game to InProgress and sends every connected player
   * GameStarted. */
  void startGame();

  void startNewWorld();

  Send send_;
  engine::EventBus& events_;
  std::unique_ptr<game::World> world_;
  engine::SystemScheduler systems_;
  PlayerRoster roster_;
  std::vector<ConnectedPlayer> departedPlayers_;
  std::unique_ptr<ShipSpawnAnnouncer> shipSpawnAnnouncer_;
  std::unique_ptr<BydoSpawner> bydoSpawner_;
  std::unique_ptr<LobbyStateAnnouncer> lobbyStateAnnouncer_;
  ScoreKeeper scoreKeeper_;
  WorldStateSender worldStateSender_;
  network::MessageDispatcher dispatcher_;
  GameState state_{GameState::WaitingForPlayers};
  std::uint32_t seed_;
  std::uint32_t worldsBuilt_{0};
  std::uint32_t tick_{0};
  std::size_t finishedTicks_{0};
  std::uint64_t ticksRun_{0};
  logging::ThrottledLog rejectionLog_;
  logging::Logger logger_;
};

}  // namespace rtype::server
