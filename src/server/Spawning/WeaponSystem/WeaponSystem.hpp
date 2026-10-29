#pragma once

#include <optional>
#include <vector>
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "WeaponShot.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Fires the Gun of every body whose Intent wants to shoot, ships and
 * Bydos alike, and tells the players about each projectile.
 *
 * At each onTick(), every Gun that is not ready yet counts one tick down.
 * Every body that has a Gun, an Intent and a Position, whose Intent wants to
 * shoot and whose Gun is ready, fires one projectile of the type of its Gun
 * (see game::World::spawnPlayerMissile and game::World::spawnEnemyMissile),
 * just in front of the body (see game::missileStartPosition and
 * game::enemyMissileStartPosition), and its Gun starts counting its interval
 * again. The wait is counted in the ticks onTick() runs, whether the Intent
 * wants to shoot meanwhile or not, so releasing shoot and pressing it again
 * fires no sooner. Which controller wrote the Intent changes nothing: the
 * number of inputs a client sends changes nothing either, so a modified client
 * never fires faster.
 *
 * A missile of a player belongs to the player whose ship fired it, so a kill is
 * credited to it; a ship that belongs to no connected player fires nothing.
 * Every connected player is sent an EntitySpawned for each projectile, as the
 * Entities section of the protocol page describes. When every network id was
 * given, no projectile appears, a warning is logged, and the Gun counts its
 * interval all the same.
 *
 * A weapon system works on one world: the game builds a new one with each new
 * world.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class WeaponSystem {
 public:
  /**
   * @param world The world the projectiles are created in; it must outlive
   * the system.
   * @param roster The players told about each projectile; it must outlive the
   * system.
   * @param send Must not be empty.
   */
  WeaponSystem(game::World& world, PlayerRoster& roster, SendDatagram send);

  /**
   * @brief Counts the tick on every Gun, then fires each Gun that is ready
   * and wanted. Nothing a player sent makes it throw.
   */
  void onTick();

 private:
  /** @brief Counts the tick on every Gun, and lists in shots_ the ones that
   * fire. */
  void collectShots();

  /** @brief Creates the projectile of @p shot and tells every connected
   * player. */
  void fire(const WeaponShot& shot);

  /**
   * @returns The EntitySpawned of the projectile created for @p shot, or
   * nothing when every network id was given, which is logged, or when the
   * shooter belongs to no connected player.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnProjectile(
      const WeaponShot& shot);

  /** @brief Sends @p projectile to every connected player. */
  void announce(const network::EntitySpawned& projectile);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::vector<WeaponShot> shots_;
};

}  // namespace rtype::server
