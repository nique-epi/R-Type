#pragma once

#include <SFML/Graphics/Color.hpp>
#include "PlayerColors.hpp"
#include "ShadeSwap.hpp"

namespace rtype::client::testing {

/** @brief The shades of player 3 in the sheets, read from the third row of
 * entities.png: dark, main and light green. */
constexpr sf::Color SHEET_DARK_GREEN(61, 107, 26);
constexpr sf::Color SHEET_GREEN(126, 194, 58);
constexpr sf::Color SHEET_LIGHT_GREEN(210, 255, 138);
constexpr PlayerColors SHEET_GREEN_SHADES{
    .dark = SHEET_DARK_GREEN, .ship = SHEET_GREEN, .light = SHEET_LIGHT_GREEN};

/** @brief The shades of player 3 in the palette for deuteranopia, as the
 * picture the team validated shows them: dark, main and light bluish
 * green. */
constexpr sf::Color DARK_BLUISH_GREEN(0, 78, 52);
constexpr sf::Color BLUISH_GREEN(0, 158, 115);
constexpr sf::Color LIGHT_BLUISH_GREEN(126, 213, 177);
constexpr PlayerColors BLUISH_GREEN_SHADES{.dark = DARK_BLUISH_GREEN,
                                           .ship = BLUISH_GREEN,
                                           .light = LIGHT_BLUISH_GREEN};

/** @brief The swap a frame of player 3 needs in the palette for
 * deuteranopia: the greens of the sheets for the bluish greens. */
constexpr ShadeSwap SHEET_GREEN_TO_BLUISH_GREEN{.from = SHEET_GREEN_SHADES,
                                                .to = BLUISH_GREEN_SHADES};

/** @brief The main shade of player 2 in the palettes for protanopia (magenta)
 * and tritanopia (deep green), and of player 4 in every palette for color
 * blindness (yellow), as the validated picture shows them. */
constexpr sf::Color MAGENTA(238, 51, 119);
constexpr sf::Color DEEP_GREEN(34, 136, 51);
constexpr sf::Color COLOR_BLIND_YELLOW(240, 228, 66);

}  // namespace rtype::client::testing
