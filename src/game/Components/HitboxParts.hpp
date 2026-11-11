#pragma once

#include <vector>
#include "CollisionBox.hpp"

namespace rtype::game {

/**
 * @brief Component of a body whose sheet lists several boxes
 * (`[[gameplay.hitbox.box]]`): the rectangles it collides with, each laid from
 * its Position as a CollisionBox is.
 *
 * Such a body keeps a CollisionBox too, the rectangle that encloses these
 * boxes: what leaves the playfield and where its gun fires from read that one,
 * and what tests a contact reads these (collidingBoxesOf()). A body without
 * this component collides with its CollisionBox alone.
 */
struct HitboxParts {
  std::vector<CollisionBox> boxes;

  bool operator==(const HitboxParts&) const = default;
};

}  // namespace rtype::game
