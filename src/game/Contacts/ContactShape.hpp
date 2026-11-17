#pragma once

#include <optional>
#include "Bounds.hpp"
#include "PlacedMask.hpp"

namespace rtype::game {

/** @brief What a body is touched on: its box, and the collision mask laid on
 * the grid of the sprite pixels where the body is, when its sheet gives one.
 * A shape without a mask is touched anywhere in its box. */
struct ContactShape {
  engine::Bounds bounds{};
  std::optional<engine::PlacedMask> mask{std::nullopt};
};

}  // namespace rtype::game
