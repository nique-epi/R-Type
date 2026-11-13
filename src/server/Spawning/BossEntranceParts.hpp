#pragma once

#include <functional>
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a BossEntrance is built from. */
struct BossEntranceParts {
  /** @brief The world the boss appears in; it must outlive the entrance. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about the boss; it must outlive the entrance. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief Where the arrival is announced; it must outlive the entrance. */
  std::reference_wrapper<engine::EventBus> events;
};

}  // namespace rtype::server
