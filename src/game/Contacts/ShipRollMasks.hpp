#pragma once

#include <memory>
#include "RolledMasks.hpp"

namespace rtype::game {

/** @brief Component of a ship whose sheet gives a collision mask: the masks
 * it shows through its barrel roll, which bodyMaskAt() lays in place of its
 * CollisionMask while its ShipMotion rolls. */
struct ShipRollMasks {
  std::shared_ptr<const RolledMasks> masks;
};

}  // namespace rtype::game
