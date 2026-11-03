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
struct ConnectedPlayer;

/** @brief What a LifeKeeper is built from. */
struct LifeKeeperParts {
  /** @brief Where the contacts are published and the losses announced; the
   * keeper subscribes to it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The world the ships and the enemies are destroyed in. */
  std::reference_wrapper<game::World> world;
  /** @brief The players whose lives are kept. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief Gives a player without a ship a new one at its start position and
   * announces it to the players; it may throw NetworkIdExhaustedException.
   * Must not be empty. */
  std::function<void(ConnectedPlayer&)> respawn;
};

}  // namespace rtype::server
