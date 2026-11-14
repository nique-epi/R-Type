#pragma once

#include <memory>
#include "PixelMask.hpp"

namespace rtype::game {

/**
 * @brief Component of a body whose sheet gives a collision mask
 * (`[gameplay.mask]`): the pixels it is touched on, laid like the idle frame
 * of its sprite and centered on its Position (placedMaskOf()).
 *
 * The mask is shared by every body of the sheet, and outlives the body. Its
 * CollisionBox stays the first test of a contact: the mask only decides
 * between the pairs whose boxes overlap. A body without this component, or
 * whose mask a script emptied by resizing its box (forgetMaskOf()), is
 * touched anywhere in its box.
 */
struct CollisionMask {
  std::shared_ptr<const engine::PixelMask> pixels;
};

}  // namespace rtype::game
