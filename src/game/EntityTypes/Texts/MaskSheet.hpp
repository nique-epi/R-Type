#pragma once

#include <memory>
#include "PixelMask.hpp"

namespace rtype::game {

/**
 * @brief The table `[gameplay.mask]` of a sheet: the pixels of its sprite an
 * entity of the type is touched on, and how many pixels of outline the tool
 * that wrote them peeled off, which only the test against the sprite reads.
 *
 * The pixels are shared: every entity of the type collides with the same
 * mask, laid like its sprite, centered on its Position.
 */
struct MaskSheet {
  std::shared_ptr<const engine::PixelMask> pixels;
  int erosion{};
};

}  // namespace rtype::game
