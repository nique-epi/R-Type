#pragma once

#include <optional>
#include <vector>
#include "ActionKey.hpp"

namespace rtype::game {

/**
 * @brief A rectangle that hurts a ship, in logical units, centered at
 * (offsetX, offsetY) from the Position of the enemy (y grows downwards).
 * `activeIn` names the attack and phase it hurts in; empty, it hurts all the
 * time.
 */
struct HurtBox {
  float offsetX{0.0F};
  float offsetY{0.0F};
  float width{0.0F};
  float height{0.0F};
  std::optional<ActionKey> activeIn;
};

/** @brief Component of an enemy: the boxes that hurt a ship on contact,
 * whatever its CollisionBox does. HurtZoneSystem reads it. */
struct HurtZone {
  std::vector<HurtBox> boxes;
};

}  // namespace rtype::game
