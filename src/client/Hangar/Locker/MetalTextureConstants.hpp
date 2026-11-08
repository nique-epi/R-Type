#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief The strokes: the share of rows that get one, their shortest length,
 * their longest as a share of the width, the share of light ones. */
constexpr float BRUSH_STROKE_CHANCE = 0.45F;
constexpr int BRUSH_STROKE_SHORTEST = 3;
constexpr float BRUSH_STROKE_LONGEST_SHARE = 0.5F;
constexpr float BRUSH_LIGHT_CHANCE = 0.55F;

/** @brief The wear: the chance of a light pixel on the top and left sides,
 * on the bottom and right sides; scratches and grime pixels per pixel of the
 * area; the longest scratch. */
constexpr float TOP_EDGE_WEAR_CHANCE = 0.18F;
constexpr float BOTTOM_EDGE_WEAR_CHANCE = 0.08F;
constexpr float SCRATCHES_PER_PIXEL = 0.0006F;
constexpr int LONGEST_SCRATCH = 5;
constexpr float GRIME_PER_PIXEL = 0.04F;

/** @brief Where the four rivets of a plate sit: RIVET_OFFSET from its
 * top-left corner, RIVET_INSET from its bottom-right one. */
constexpr sf::Vector2i RIVET_OFFSET{2, 2};
constexpr sf::Vector2i RIVET_INSET{4, 4};

}  // namespace rtype::client
