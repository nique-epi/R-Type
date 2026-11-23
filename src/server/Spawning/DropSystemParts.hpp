#pragma once

#include <functional>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a DropSystem is built from. */
struct DropSystemParts {
  /** @brief Where the drops are published; the system subscribes to it. It
   * must outlive the system. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The world the crystals appear in; it must outlive the system. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about each crystal; it must outlive the system.
   */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief The sheets the drops name; they must outlive the system. */
  std::reference_wrapper<const game::EntitySheets> types;
};

}  // namespace rtype::server
