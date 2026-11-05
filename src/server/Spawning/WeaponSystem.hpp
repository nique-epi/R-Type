#pragma once

#include <optional>
#include <vector>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "WeaponShot.hpp"
#include "WeaponSystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Fires the Gun of every body whose Intent wants to shoot, ships and
 * enemies alike, and tells the players about each projectile.
 *
 * At each onTick(), every Gun of a body that also has an Intent and is not
 * ready yet counts one tick down. Every such body that has a Position, a
 * CollisionBox, an EntityType and a NetworkIdentity too, whose Intent wants to
 * shoot and whose Gun is ready, fires one projectile of the sheet its Gun
 * names (see game::World::spawn), just in front of the body (see
 * game::projectileStartPosition), and its Gun starts counting its interval
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
   * @param parts The world, the players, how to send, and the sheets the guns
   * name.
   */
  explicit WeaponSystem(WeaponSystemParts parts);

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
   * projectile is a missile of a player whose shooter belongs to no connected
   * player.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnProjectile(
      const WeaponShot& shot);

  /** @brief Sends @p projectile to every connected player. */
  void announce(const network::EntitySpawned& projectile);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  const game::EntitySheets* types_;
  logging::Logger logger_;
  std::vector<WeaponShot> shots_;
};

}  // namespace rtype::server
