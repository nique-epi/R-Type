#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include "MetalShades.hpp"

namespace rtype::client {

/** @brief A face of metal to texture: its area, its shades, and the seed of
 * its strokes, so the same face looks the same in every frame. */
struct MetalPatch {
  sf::IntRect area;
  MetalShades shades;
  std::uint32_t seed{};
};

}  // namespace rtype::client
