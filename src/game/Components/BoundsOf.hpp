#pragma once

#include "Bounds.hpp"
#include "CollisionBox.hpp"
#include "Position.hpp"

namespace rtype::game {

/** @returns The rectangle of @p box laid on an entity at @p position, centered
 * on it as CollisionBox says. */
constexpr engine::Bounds boundsOf(const Position& position,
                                  const CollisionBox& box) {
  return engine::Bounds{.centerX = position.x,
                        .centerY = position.y,
                        .width = box.width,
                        .height = box.height};
}

}  // namespace rtype::game
