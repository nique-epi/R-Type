#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BodyPlacement.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EventBus.hpp"
#include "Logger.hpp"
#include "PendingBurst.hpp"
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
 * Entities section of the protocol page describes. The one of a missile of a
 * player names that player and the inputNumber of the newest input applied to
 * its ship, so that player's client can tell the missile it predicted, and is
 * guaranteed (see the Own missiles section of the protocol page). When every
 * network id was given, no projectile appears, a warning is logged, and the Gun
 * counts its interval all the same.
 *
 * Then every projectile with a ShardBurst counts one tick down; one whose time
 * came is destroyed, announced on the event bus with an EntityDestruction
 * Removed, and replaced by its shards (see game::World::spawnFrom), which
 * appear SHARD_START_DISTANCE from its center along directions spread evenly
 * from FIRST_SHARD_ANGLE_RADIANS, and steer from that center; each is told to
 * every connected player as a projectile is.
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
   * @returns The EntitySpawned of the projectile created for @p shot, naming
   * its owner and its firing input for a missile of a player, or
   * nothing when every network id was given, which is logged, or when the
   * projectile is a missile of a player whose shooter belongs to no connected
   * player.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnProjectile(
      const WeaponShot& shot);

  /**
   * @returns The EntitySpawned of a body of @p sheet made at @p placement,
   * belonging to @p owner when it is a missile of a player; nothing when
   * every network id was given, which is logged.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnAt(
      const game::EntitySheet& sheet, game::BodyPlacement placement,
      std::optional<std::uint8_t> owner);

  /** @brief Sends @p projectile to every connected player: once, or as a
   * guaranteed message for a missile of a player. */
  void announce(const network::EntitySpawned& projectile);

  /** @brief Counts the tick on every ShardBurst, and lists in bursts_ the
   * projectiles whose time to burst came. */
  void collectBursts();

  /** @brief Removes the projectile of @p pending, announces its removal, and
   * makes its shards appear and tells every connected player. */
  void burst(const PendingBurst& pending);

  /** @returns The EntitySpawned of the shard @p index of @p pending, or
   * nothing when its sheet is unknown or every network id was given, which
   * is logged. */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnShard(
      const PendingBurst& pending, std::size_t index);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  const game::EntitySheets* types_;
  engine::EventBus* events_;
  logging::Logger logger_;
  std::vector<WeaponShot> shots_;
  std::vector<PendingBurst> bursts_;
};

}  // namespace rtype::server
