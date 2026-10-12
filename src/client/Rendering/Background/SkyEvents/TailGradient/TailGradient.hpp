#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include "BackgroundConstants.hpp"

namespace rtype::client {

/** @brief Color of a tail up to a fraction of its length. */
struct GradientStop {
  /** Fraction of the tail, from 0 at the head to 1 at its end. */
  float end{};
  sf::Color color;
};

/** @brief Colors along a tail, from its head to its end. */
using TailGradient = std::array<GradientStop, TAIL_GRADIENT_STOP_COUNT>;

/**
 * @returns The color of the first stop that ends beyond the fraction, or the
 * color of the last stop when none does.
 */
[[nodiscard]] sf::Color tailColor(const TailGradient& gradient, float fraction);

}  // namespace rtype::client
