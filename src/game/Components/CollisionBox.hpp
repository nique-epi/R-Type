#pragma once

namespace rtype::game {

/**
 * @brief The rectangle an entity collides with, in logical units.
 *
 * It is centered (offsetX, offsetY) away from the Position of the entity, y
 * growing downwards, and extends by half its width on each side of that
 * center and by half its height above and below it. Every sheet but a boss
 * leaves the offset at 0, so the rectangle is centered on the Position; the
 * Bydo Tyrant lays it on its eye, its only weak point.
 */
struct CollisionBox {
  float width{};
  float height{};
  float offsetX{0.0F};
  float offsetY{0.0F};

  bool operator==(const CollisionBox&) const = default;
};

}  // namespace rtype::game
