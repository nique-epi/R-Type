#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client::testing {

/** @brief A color of text or of an edge, and the color it is drawn on. */
struct TextOnBackground {
  sf::Color text;
  sf::Color background;
};

/** @returns The contrast ratio of two opaque colors, from 1 to 21, as WCAG
 * 2.2 defines it from their relative luminance. */
[[nodiscard]] double contrastRatioOf(sf::Color first, sf::Color second);

/** @returns The ratio of @p pair once both its colors are drawn as HIGH
 * CONTRAST draws them. */
[[nodiscard]] double highContrastRatioOf(TextOnBackground pair);

}  // namespace rtype::client::testing
