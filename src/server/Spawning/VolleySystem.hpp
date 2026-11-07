#pragma once

#include <optional>
#include <vector>
#include "BodyPlacement.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "VolleyShot.hpp"
#include "VolleySystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Fires the volleys of the enemies: the bodies the sheet of an enemy
 * sends out at once as it enters an action, missiles or Bydos, and tells the
 * players about each one.
 *
 * At each onTick(), every body whose Intent starts an action (the
 * Intent::startsAction PilotSystem has just recorded) and whose sheet has a
 * `[[gameplay.volley]]` on that action fires it: each body appears where
 * game::volleyPlacement() lays it (see game::World::spawnFrom), belonging to
 * no player, and every connected player is sent its EntitySpawned, as the
 * Entities section of the protocol page describes. A Bydo of a volley is an
 * enemy like any other: the waves and the boss stage count it, a missile of a
 * player hits it. When every network id was given, the rest of the volley is
 * skipped and a warning is logged.
 *
 * A volley system works on one world: the game builds a new one with each
 * new world. Not thread-safe: use it from the simulation thread only.
 */
class VolleySystem {
 public:
  explicit VolleySystem(VolleySystemParts parts);

  /** @brief Fires every volley whose action starts at this tick. */
  void onTick();

 private:
  /** @brief Lists in shots_ the volleys whose action starts at this tick. */
  void collectShots();

  /** @brief Makes the bodies of @p shot appear and tells every player. */
  void fire(const VolleyShot& shot);

  /** @returns The EntitySpawned of a body of @p sheet made at @p placement;
   * nothing when every network id was given, which is logged. */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnAt(
      const game::EntitySheet& sheet, game::BodyPlacement placement);

  /** @brief Sends @p body to every connected player. */
  void announce(const network::EntitySpawned& body);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  const game::EntitySheets* types_;
  logging::Logger logger_;
  std::vector<VolleyShot> shots_;
};

}  // namespace rtype::server
