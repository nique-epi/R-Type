#pragma once

#include <span>
#include "CollisionBox.hpp"
#include "HitboxParts.hpp"

namespace rtype::game {

/**
 * @returns The rectangles a body collides with: the boxes of @p parts when it
 * has some, @p box alone for a body without HitboxParts (@p parts null). The
 * span points into @p box or @p parts, and lasts as long as they do.
 */
constexpr std::span<const CollisionBox> collidingBoxesOf(
    const CollisionBox& box, const HitboxParts* parts) {
  if (parts == nullptr) {
    return std::span<const CollisionBox>{&box, 1};
  }
  return std::span<const CollisionBox>{parts->boxes};
}

}  // namespace rtype::game
