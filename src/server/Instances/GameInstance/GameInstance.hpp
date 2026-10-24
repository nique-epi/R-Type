#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "BydoSpawner.hpp"
#include "CollisionRules.hpp"
#include "ConnectedPlayer.hpp"
#include "DamageRules.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MissileLauncher.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerDeparture.hpp"
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
 * when finish() is called or once no player is left in it, and from Finished
 * back to WaitingForPlayers after FINISHED_STATE_TICKS ticks. Coming back to
 * WaitingForPlayers builds a new world: every player gets a new ship, is no
 * longer ready, and the tick is 0. The players stay connected.
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
 * While the game is in progress, each tick first moves the ship of every player
 * who sent an input, by the directions its newest input holds (see
 * game::shipPositionAfter), then runs the systems of the world
 * (PatternSystem, MovementSystem, PlayfieldExitSystem, which removes a Bydo
 * that left on the left or a missile that left on the right and announces it
 * on the event bus, then CollisionSystem), then lets the ship of every player
 * whose newest input holds shoot fire a missile, at most one every
 * SHOT_INTERVAL_TICKS ticks (see MissileLauncher), then lets a Bydo appear
 * every BYDO_SPAWN_INTERVAL (see BydoSpawner). The ships move and fire in no
 * other state. Each world starts its random draws from a seed of its own, the
 * seed of the game plus the number of worlds built before it, so the seed of
 * the game is enough to replay every world of a session.
 *
 * The collisions CollisionSystem finds are judged when the event bus delivers
 * them: CollisionRules turns a missile of a player that touches a Bydo into a
 * MissileHit, and DamageRules destroys the missile, takes MISSILE_DAMAGE from
 * the health of the Bydo and destroys the Bydo once it has none left, an
 * EnemyKilled crediting its points to the player who fired. Both work on the
 * world they were built with, and a new pair comes with each new world, so the
 * events of a tick must be delivered before the world is replaced: the server
 * dispatches the bus at the end of every tick.
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
 * A player the server sent nothing for TICKS_PER_HEARTBEAT ticks, whatever
 * sent it, is sent a Heartbeat at the end of the tick, so its client knows the
 * server is still there in a state that repeats no message.
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
 * runs, so a stall of the server closes no session. A game in progress whose
 * last player's session closed ends at that same tick, as finish() ends it, so
 * it waits for players again FINISHED_STATE_TICKS ticks later.
 *
 * A Disconnection from a player closes its session at once, in the tick that
 * reads it and in the same way, with a PlayerDeparture (Left) instead: a player
 * who leaves does not wait for the timeout. The copies of that Disconnection
 * the client sends after it come from an address the roster no longer holds,
 * so they change nothing.
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
   * a Bydo leaving the playfield, a collision or a player leaving the game; it
   * must outlive the game, and its owner dispatches it after each onTick().
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
   * @brief Runs one tick: applies the ReadyState, keeps the newest PlayerInput
   * and closes the session of each player who sent a Disconnection, among
   * @p datagrams, oldest first, closes the session of every player not heard
   * from for SESSION_TIMEOUT_TICKS ticks, ends a game in progress that no
   * player is left in, then moves the game along its states. While it waits,
   * the players are told the room; while it is in progress, the tick is
   * counted, the ships move, the systems run, the ships whose players hold
   * shoot fire, the Bydos appear and the world is sent. Last, a player sent
   * nothing for TICKS_PER_HEARTBEAT ticks is sent a Heartbeat. Nothing caused
   * by the content of a datagram makes it throw.
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
  /** @brief A datagram of a player of the game that passed every check of
   * onTick(), with what reading it needs. */
  struct PlayerDatagram {
    ConnectedPlayer* player{};
    network::MessageOrigin origin;
    std::vector<network::MessageFrame> messages;
  };

  /** @brief Notes that the sender of @p datagram was heard from, then hands
   * its messages to the dispatcher, when it passes the checks of
   * playerDatagramOf(). */
  void readPlayerDatagram(const network::IncomingDatagram& datagram);

  /**
   * @returns @p datagram split into its messages, with the player who sent it,
   * when its header is of this protocol and version, its messages add up and
   * its sender is a player of the game; nothing otherwise, the datagram being
   * dropped and logged.
   */
  [[nodiscard]] std::optional<PlayerDatagram> playerDatagramOf(
      const network::IncomingDatagram& datagram);

  void drop(const network::Endpoint& sender, std::string_view reason);

  /** @brief Moves the game along its states by one tick: the start of the
   * game, its systems, its missiles and its Bydos, or the end of the wait after
   * it. */
  void advanceState();

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

  /** @brief Moves the ship of every player who sent an input by one tick of
   * what that input holds. */
  void moveShips();

  /** @brief Closes the session of every player not heard from for
   * SESSION_TIMEOUT_TICKS ticks, and counts one more tick of silence for the
   * others. The players to close are listed first, then closed, so no seat is
   * freed while the roster is being walked. */
  void closeSilentSessions();

  /** @returns The addresses of the players not heard from for
   * SESSION_TIMEOUT_TICKS ticks; counts one more tick of silence for every
   * other player. */
  [[nodiscard]] std::vector<network::Endpoint> countSilentTicks();

  /** @brief Frees the seat of the player sending from @p player, removes its
   * ship and announces it, announces that the player left for @p cause, then
   * keeps the player for takeDepartedPlayers(). Does nothing when no player of
   * the game sends from there. Everything after the seat is freed reads the
   * copy of the player the roster handed back. */
  void closeSession(const network::Endpoint& player,
                    game::DepartureCause cause);

  /** @brief Publishes the EntityDestruction (Removed) of a ship that left the
   * world. */
  void announceShipRemoval(std::uint32_t shipEntityId);

  /** @brief Logs that @p player left or timed out, as @p cause says, and
   * publishes its PlayerDeparture. */
  void announceDeparture(const ConnectedPlayer& player,
                         game::DepartureCause cause);

  /** @brief Writes the info line of a departure: the address of a player who
   * left, or how long a player who timed out stayed silent. */
  void logDeparture(const ConnectedPlayer& player,
                    game::DepartureCause cause) const;

  /** @returns Whether the game is in progress with no player left in it. */
  [[nodiscard]] bool isAbandoned();

  /** @brief Logs and ends the game when it is in progress with no player left
   * in it, as finish() ends it. */
  void finishIfAbandoned();

  /** @brief Sends a Heartbeat to every player sent nothing for
   * TICKS_PER_HEARTBEAT ticks, then counts the tick that ends for every
   * player. */
  void sendHeartbeats();

  /** @brief Moves the game to InProgress and sends every connected player
   * GameStarted. */
  void startGame();

  /** @brief Builds a new world, and every part of the game that works on one
   * world: the rules that judge its collisions, the announcer of its ships, the
   * spawner of its Bydos, the launcher of its missiles and the announcer of the
   * room. */
  void buildWorld();

  void startNewWorld();

  Send send_;
  engine::EventBus& events_;
  std::unique_ptr<game::World> world_;
  engine::SystemScheduler systems_;
  PlayerRoster roster_;
  std::vector<ConnectedPlayer> departedPlayers_;
  std::unique_ptr<ShipSpawnAnnouncer> shipSpawnAnnouncer_;
  std::unique_ptr<BydoSpawner> bydoSpawner_;
  std::unique_ptr<MissileLauncher> missileLauncher_;
  std::unique_ptr<game::CollisionRules> collisionRules_;
  std::unique_ptr<game::DamageRules> damageRules_;
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
