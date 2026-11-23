#pragma once

#include "Entity.hpp"

namespace rtype::game {

/**
 * @brief Event published when the ship of a player touches a crystal.
 *
 * It only reports the contact; the weapon keeper decides who wins the
 * crystal.
 */
struct CrystalTouched {
  engine::Entity ship;
  engine::Entity crystal;

  bool operator==(const CrystalTouched&) const = default;
};

}  // namespace rtype::game
