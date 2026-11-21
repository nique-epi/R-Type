#pragma once

#include <functional>

namespace rtype::engine {
class EventBus;
}  // namespace rtype::engine

namespace rtype::game {
class World;
}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief What an ExtraLifeKeeper is built from. */
struct ExtraLifeKeeperParts {
  /** @brief Where the changes of the scores are published and the extra
   * lives announced; the keeper subscribes to it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The world the ships of the players fly in. */
  std::reference_wrapper<game::World> world;
  /** @brief The players whose scores and lives are read and given. */
  std::reference_wrapper<PlayerRoster> roster;
};

}  // namespace rtype::server
