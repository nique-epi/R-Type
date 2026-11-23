#pragma once

#include <functional>

namespace rtype::engine {

class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

class EntitySheets;
class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief What a WeaponKeeper is built from. */
struct WeaponKeeperParts {
  /** @brief Where the touches are published and the removals announced; the
   * keeper subscribes to it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The world of the ships armed and the crystals removed. */
  std::reference_wrapper<game::World> world;
  /** @brief The players who win the crystals. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief The sheets of the ships and of the crystals. */
  std::reference_wrapper<const game::EntitySheets> types;
};

}  // namespace rtype::server
