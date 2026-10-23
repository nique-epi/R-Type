#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Fires the missiles of the players who hold shoot, at most one every
 * SHOT_INTERVAL_TICKS ticks per ship, and tells the players about each one.
 *
 * At each onTick(), every shooter whose ship is in the world fires one missile
 * of its own (see game::World::spawnPlayerMissile), just in front of its ship
 * (see game::missileStartPosition), unless that ship fired less than
 * SHOT_INTERVAL_TICKS ticks ago. A player who holds shoot therefore fires at
 * once, then again every SHOT_INTERVAL_TICKS ticks while it holds it. The wait
 * is counted in the ticks onTick() runs, whether the player holds shoot
 * meanwhile or not, so releasing shoot and pressing it again fires no sooner.
 * The game names the shooters from the newest input of each player, one per
 * tick, so the number of inputs a client sends changes nothing either: a
 * modified client never fires faster.
 *
 * Every connected player is sent an EntitySpawned for each missile, as the
 * Entities section of the protocol page describes. When every network id was
 * given, no missile appears, a warning is logged, and the ship may try again at
 * the next tick.
 *
 * A launcher works on one world: the game builds a new one with each new world,
 * so every ship may fire when a game starts.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class MissileLauncher {
 public:
  /**
   * @param world The world the missiles are created in; it must outlive the
   * launcher.
   * @param roster The players told about each missile; it must outlive the
   * launcher.
   * @param send Must not be empty.
   */
  MissileLauncher(game::World& world, PlayerRoster& roster, SendDatagram send);

  /**
   * @brief Fires a missile from the ship of each of @p shooters whose ship may
   * fire, then counts the tick. Nothing a player sent makes it throw.
   * @param shooters The connected players who hold shoot at this tick.
   */
  void onTick(const ConnectedPlayers& shooters);

 private:
  /**
   * @returns Where the ship of @p shooter is when it fires at this tick: the
   * ship fired no missile in the last SHOT_INTERVAL_TICKS ticks, and it is in
   * the world. Nothing otherwise.
   */
  [[nodiscard]] std::optional<game::Position> firingShipOf(
      const ConnectedPlayer& shooter) const;

  /** @returns Whether the ship @p shipId fired no missile in the last
   * SHOT_INTERVAL_TICKS ticks. */
  [[nodiscard]] bool mayFire(std::uint32_t shipId) const;

  /** @brief Creates the missile of @p shooter in front of @p ship, makes the
   * ship wait SHOT_INTERVAL_TICKS ticks, and tells every connected player. */
  void fire(const ConnectedPlayer& shooter, game::Position ship);

  /**
   * @returns The EntitySpawned of the missile created for @p shooter in front
   * of @p ship, or nothing when every network id was given, which is logged.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnMissile(
      const ConnectedPlayer& shooter, game::Position ship);

  /** @brief Sends @p missile to every connected player. */
  void announce(const network::EntitySpawned& missile);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::size_t ticksRun_{0};
  /** @brief The tick from which each ship that fired may fire again, by the
   * network id of the ship. */
  std::unordered_map<std::uint32_t, std::size_t> nextShotTickByShip_;
};

}  // namespace rtype::server
