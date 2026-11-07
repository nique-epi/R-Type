#pragma once

#include "Entity.hpp"

namespace rtype::game {

/**
 * @brief Event published when the ship of a player touches a look item.
 *
 * It only reports the contact; the look item keeper decides who wins the item.
 */
struct LookItemTouched {
  engine::Entity ship;
  engine::Entity item;

  bool operator==(const LookItemTouched&) const = default;
};

}  // namespace rtype::game
