#pragma once

#include <functional>
#include "OutgoingDatagram.hpp"

namespace rtype::engine {

class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief What a LookItemKeeper is built from. */
struct LookItemKeeperParts {
  /** @brief Where the touches are published and the removals announced; the
   * keeper subscribes to it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The world the items are removed from. */
  std::reference_wrapper<game::World> world;
  /** @brief The players who win the items. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief Called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
};

}  // namespace rtype::server
