#pragma once

#include "Bounds.hpp"
#include "HurtBox.hpp"
#include "Position.hpp"

namespace rtype::game {

/** @returns @p box laid on the enemy standing at @p position. */
constexpr engine::Bounds hurtBoxBounds(const HurtBox& box,
                                       const Position& position) {
  return engine::Bounds{.centerX = position.x + box.offsetX,
                        .centerY = position.y + box.offsetY,
                        .width = box.width,
                        .height = box.height};
}

}  // namespace rtype::game
