#pragma once

#include <cstdint>
#include <functional>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a VolleySystem is built from. */
struct VolleySystemParts {
  /** @brief The world the volleys appear in; it must outlive the system. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about each body; it must outlive the system. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief The sheets of the enemies and of the bodies of their volleys;
   * they must outlive the system. */
  std::reference_wrapper<const game::EntitySheets> types;
  /** @brief What the draws of the volleys that fire by chance start from. */
  std::uint32_t seed{};
};

}  // namespace rtype::server
