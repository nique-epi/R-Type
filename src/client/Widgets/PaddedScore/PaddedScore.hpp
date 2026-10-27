#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"

namespace rtype::client {

/**
 * @brief A score written on STATUS_SCORE_DIGITS digits, the zeros that pad it
 * in a dimmer color so a growing score never moves; a higher score stops at
 * LARGEST_SHOWN_SCORE.
 */
struct PaddedScore {
  std::uint32_t value{};
  /** @brief Where the last digit ends, on the top row of the line. */
  sf::Vector2i end;
  sf::Color digitColor;
  sf::Color zeroColor;
  /** @brief Whole factor every pixel of the digits is enlarged by. */
  int scale = NATIVE_SCALE;
};

/** @brief Writes the digits of a score, then the zeros that pad it, at their
 * left. */
void paintPaddedScore(IInterfaceCanvas& canvas, const PaddedScore& score);

}  // namespace rtype::client
