#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string_view>
#include "BarState.hpp"

namespace rtype::client {

/** @returns How wide a line of UTF-8 text is in the regular font, in pixels:
 * REGULAR_FONT_ADVANCE per character. */
[[nodiscard]] int regularTextWidth(std::string_view text);

/** @returns The left of a line of text in the regular font centered in a
 * box, in pixels. */
[[nodiscard]] int centeredTextLeft(sf::IntRect box, std::string_view text);

/** @returns The top of a line of text centered in a box: its middle row, in
 * pixels. */
[[nodiscard]] int centeredTextTop(sf::IntRect box);

/** @returns The color of the label of a bar in a state, also the color of
 * text laid on a row outside any box. */
[[nodiscard]] sf::Color barTextColor(BarState state);

}  // namespace rtype::client
