#pragma once

#include <functional>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a WeaponSystem is built from. */
struct WeaponSystemParts {
  /** @brief The world the projectiles are created in; it must outlive the
   * system. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about each projectile; it must outlive the
   * system. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief The sheets the guns name their projectiles in; they must outlive
   * the system. */
  std::reference_wrapper<const game::EntitySheets> types;
  /** @brief Where the removal of a projectile that bursts is announced; it
   * must outlive the system. */
  std::reference_wrapper<engine::EventBus> events;
};

}  // namespace rtype::server
