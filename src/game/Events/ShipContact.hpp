#pragma once

#include "Entity.hpp"

namespace rtype::game {

/**
 * @brief Event published when the ship of a player touches an enemy.
 *
 * It only reports the contact; the damage rules decide what it costs.
 */
struct ShipContact {
  engine::Entity player;
  engine::Entity enemy;

  bool operator==(const ShipContact&) const = default;
};

}  // namespace rtype::game
