#pragma once

#include <optional>
#include <vector>
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Fires the missiles of the Bydos: each Bydo fires one straight to the
 * left ENEMY_SHOT_INTERVAL after it appeared, then again every
 * ENEMY_SHOT_INTERVAL, and every player is told about each one.
 *
 * Each Bydo carries its own Gun, which game::World::spawnBydo gives it, so
 * Bydos that appeared at different times fire at different times. At each
 * onTick(), the Gun of every Bydo in the world counts one tick; a Gun that
 * reaches ENEMY_SHOT_INTERVAL_TICKS starts again from 0, and its Bydo fires a
 * missile just in front of it (see game::World::spawnEnemyMissile and
 * game::enemyMissileStartPosition). A Bydo destroyed before its shot fires
 * nothing.
 *
 * Every connected player is sent an EntitySpawned for each missile, as the
 * Entities section of the protocol page describes. When every network id was
 * given, that missile is skipped and a warning is logged; the Bydo fires again
 * an interval later.
 *
 * A gunner works on one world: the game builds a new one with each new world.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class BydoGunner {
 public:
  /**
   * @param world The world whose Bydos fire; it must outlive the gunner.
   * @param roster The players told about each missile; it must outlive the
   * gunner.
   * @param send Must not be empty.
   */
  BydoGunner(game::World& world, PlayerRoster& roster, SendDatagram send);

  /**
   * @brief Counts one tick on the Gun of every Bydo, then fires the missile of
   * each Bydo whose interval is over. Nothing a player sent makes it throw.
   */
  void onTick();

 private:
  /** @brief Counts one tick on the Gun of every Bydo, and keeps in firingBydos_
   * where each Bydo whose interval is over stands. */
  void findTheBydosThatFire();

  /** @brief Creates the missile of the Bydo at @p bydo and tells every
   * connected player. */
  void fire(game::Position bydo);

  /**
   * @returns The EntitySpawned of the missile created in front of @p bydo, or
   * nothing when every network id was given, which is logged.
   */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnMissile(
      game::Position bydo);

  /** @brief Sends @p missile to every connected player. */
  void announce(const network::EntitySpawned& missile);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  /** @brief Where the Bydos that fire at this tick stand. Kept between ticks,
   * so a tick allocates nothing once the most Bydos that fired at once fit in
   * it. */
  std::vector<game::Position> firingBydos_;
};

}  // namespace rtype::server
