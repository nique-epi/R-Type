#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
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
#include "ShipSpawnAnnouncer.hpp"
#include "SystemScheduler.hpp"
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
 * protocol and version with no flag set, its messages add up to its end, its
 * sender is in the roster, and it carries the session token of that player. A
 * datagram with another token is dropped and counted: whoever forges the
 * address of a player could otherwise say ready in its place, or send an input
 * numbered 4294967295, above every input the player will ever send, and hold
 * the ship of that player on the forged keys for the rest of the session. Such
 * a drop is logged with its sender at most once per TICKS_PER_REPORT ticks,
 * with the number dropped since the previous line, so a flood of forged
 * datagrams cannot flood the log.
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
   * a Bydo leaving the playfield; it must outlive the game.
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
   * @returns How many datagrams of the players were dropped because they carry
   * another session token than the one of the player they come from.
   */
  [[nodiscard]] std::size_t foreignTokenCount() const;

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
   * PlayerInput among @p datagrams, oldest first, then moves the game along its
   * states. While it waits, the players are told the room; while it is in
   * progress, the tick is counted, the systems run, the Bydos appear and the
   * world is sent. Nothing caused by the content of a datagram makes it throw.
   */
  void onTick(std::span<const network::IncomingDatagram> datagrams);

  /**
   * @brief Ends the game in progress. Does nothing in any other state.
   */
  void finish();

 private:
  void readPlayerDatagram(const network::IncomingDatagram& datagram);
  void dropForeignToken(const network::Endpoint& sender);
  void advanceLogWindow();
  void applyReadyState(const network::MessageOrigin& origin,
                       const network::ReadyState& message);
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);
  [[nodiscard]] bool everyPlayerIsReady();

  /** @brief Moves the game to InProgress and sends every connected player
   * GameStarted. */
  void startGame();

  void startNewWorld();

  Send send_;
  std::unique_ptr<game::World> world_;
  engine::SystemScheduler systems_;
  PlayerRoster roster_;
  std::unique_ptr<ShipSpawnAnnouncer> shipSpawnAnnouncer_;
  std::unique_ptr<BydoSpawner> bydoSpawner_;
  std::unique_ptr<LobbyStateAnnouncer> lobbyStateAnnouncer_;
  WorldStateSender worldStateSender_;
  network::MessageDispatcher dispatcher_;
  GameState state_{GameState::WaitingForPlayers};
  std::uint32_t seed_;
  std::uint32_t worldsBuilt_{0};
  std::uint32_t tick_{0};
  std::size_t finishedTicks_{0};
  std::size_t foreignTokenCount_{0};
  std::size_t foreignTokensSinceLastLine_{0};
  std::size_t ticksInLogWindow_{0};
  bool foreignTokenLoggedInWindow_{false};
  logging::Logger logger_;
};

}  // namespace rtype::server
