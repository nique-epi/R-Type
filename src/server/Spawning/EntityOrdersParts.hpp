#pragma once

#include <functional>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "World.hpp"

namespace rtype::game {

class BehaviourScripts;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief What a EntityOrders is built from. */
struct EntityOrdersParts {
  /** @brief The world the bodies appear in; it must outlive the object. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about each body; it must outlive the object. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief The sheets the scripts name; they must outlive the object. */
  std::reference_wrapper<const game::EntitySheets> types;
  /** @brief The bus that tells the players of a removal, and that the
   * scripts subscribe to and publish on; it must outlive the object. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The scripts of the game, nullptr in a game without any; they must
   * outlive the object. */
  game::BehaviourScripts* scripts{nullptr};
};

}  // namespace rtype::server
