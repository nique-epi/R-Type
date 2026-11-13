#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @returns The color @p color is drawn in when HIGH CONTRAST is on: the
 * replacement HIGH_CONTRAST_COLORS gives its red, green and blue, with
 * @p color's own opacity, or @p color itself when none does. */
[[nodiscard]] sf::Color highContrastOf(sf::Color color);

}  // namespace rtype::client
