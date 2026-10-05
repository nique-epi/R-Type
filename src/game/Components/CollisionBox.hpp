#pragma once

namespace rtype::game {

/**
 * @brief Size of the rectangle an entity collides with, in logical units.
 *
 * The rectangle is centered on the Position of the entity: it extends by half
 * its width on each side and by half its height above and below.
 */
struct CollisionBox {
  float width;
  float height;
};

}  // namespace rtype::game
