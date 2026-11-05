#pragma once

#include <functional>
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a ShipSpawnAnnouncer is built from. */
struct ShipSpawnAnnouncerParts {
  /** @brief The world the ships are created in; it must outlive the
   * announcer. */
  std::reference_wrapper<game::World> world;
  /** @brief The players of the game; it must outlive the announcer. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief The sheet every player's ship is made from; it must outlive the
   * announcer. */
  std::reference_wrapper<const game::EntitySheet> ship;
};

}  // namespace rtype::server
