#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include "ConnectedPlayer.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ReadyState.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"

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
 * A ReadyState counts only while the game waits for players, from a connected
 * player whose session token the datagram carries, and only when its sequence
 * is newer than the newest one accepted from that player, so a late copy never
 * undoes a newer one.
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
   */
  explicit GameInstance(Send send);

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
   * @brief Gives @p player, who just took a place in roster(), a ship in the
   * world and announces it to the players.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  void onPlayerJoined(ConnectedPlayer& player);

  /**
   * @brief Runs one tick: applies the ReadyState among @p datagrams, then
   * moves the game along its states and counts the tick while it is in
   * progress. Nothing caused by the content of a datagram makes it throw.
   */
  void onTick(std::span<const network::IncomingDatagram> datagrams);

  /**
   * @brief Ends the game in progress. Does nothing in any other state.
   */
  void finish();

 private:
  void readReadyStates(std::span<const network::IncomingDatagram> datagrams);
  void applyReadyState(const network::MessageOrigin& origin,
                       const network::ReadyState& message);
  [[nodiscard]] bool everyPlayerIsReady();
  void startNewWorld();

  Send send_;
  std::unique_ptr<game::World> world_;
  PlayerRoster roster_;
  std::unique_ptr<ShipSpawnAnnouncer> shipSpawnAnnouncer_;
  network::MessageDispatcher dispatcher_;
  GameState state_{GameState::WaitingForPlayers};
  std::uint32_t tick_{0};
  std::size_t finishedTicks_{0};
};

}  // namespace rtype::server
