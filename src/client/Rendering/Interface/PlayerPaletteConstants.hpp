#pragma once

#include <SFML/Graphics/Color.hpp>
#include "InterfaceColorConstants.hpp"
#include "PlayerColors.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

/**
 * @brief The colors the palettes for color blindness give players 2 to 4.
 *
 * Their main shades are Okabe and Ito's yellow, bluish green and reddish
 * purple (jfly.uni-koeln.de/color), Paul Tol's magenta and teal of his
 * vibrant scheme and green of his bright one (sronpersonalpages.nl/~pault).
 * Their dark and light shades move them in OKLab as the sheets move theirs:
 * lightness down 0.25 and chroma times 0.675 for the dark one, lightness up
 * 0.188 and chroma times 0.768 for the light one.
 */
constexpr sf::Color PLAYER_YELLOW(240, 228, 66);
constexpr sf::Color PLAYER_DARK_YELLOW(154, 147, 51);
constexpr sf::Color PLAYER_LIGHT_YELLOW(255, 255, 181);
constexpr sf::Color PLAYER_MAGENTA(238, 51, 119);
constexpr sf::Color PLAYER_DARK_MAGENTA(125, 3, 56);
constexpr sf::Color PLAYER_LIGHT_MAGENTA(255, 144, 181);
constexpr sf::Color PLAYER_TEAL(0, 153, 136);
constexpr sf::Color PLAYER_DARK_TEAL(0, 74, 64);
constexpr sf::Color PLAYER_LIGHT_TEAL(125, 208, 193);
constexpr sf::Color PLAYER_REDDISH_PURPLE(204, 121, 167);
constexpr sf::Color PLAYER_DARK_REDDISH_PURPLE(111, 61, 89);
constexpr sf::Color PLAYER_LIGHT_REDDISH_PURPLE(255, 188, 224);
constexpr sf::Color PLAYER_BLUISH_GREEN(0, 158, 115);
constexpr sf::Color PLAYER_DARK_BLUISH_GREEN(0, 78, 52);
constexpr sf::Color PLAYER_LIGHT_BLUISH_GREEN(126, 213, 177);
constexpr sf::Color PLAYER_DEEP_GREEN(34, 136, 51);
constexpr sf::Color PLAYER_DARK_DEEP_GREEN(0, 58, 5);
constexpr sf::Color PLAYER_LIGHT_DEEP_GREEN(120, 190, 124);

/** @brief The three shades of each of those colors, dark, main then light. */
constexpr PlayerColors PLAYER_YELLOW_SHADES{.dark = PLAYER_DARK_YELLOW,
                                            .ship = PLAYER_YELLOW,
                                            .light = PLAYER_LIGHT_YELLOW};
constexpr PlayerColors PLAYER_MAGENTA_SHADES{.dark = PLAYER_DARK_MAGENTA,
                                             .ship = PLAYER_MAGENTA,
                                             .light = PLAYER_LIGHT_MAGENTA};
constexpr PlayerColors PLAYER_TEAL_SHADES{
    .dark = PLAYER_DARK_TEAL, .ship = PLAYER_TEAL, .light = PLAYER_LIGHT_TEAL};
constexpr PlayerColors PLAYER_REDDISH_PURPLE_SHADES{
    .dark = PLAYER_DARK_REDDISH_PURPLE,
    .ship = PLAYER_REDDISH_PURPLE,
    .light = PLAYER_LIGHT_REDDISH_PURPLE};
constexpr PlayerColors PLAYER_BLUISH_GREEN_SHADES{
    .dark = PLAYER_DARK_BLUISH_GREEN,
    .ship = PLAYER_BLUISH_GREEN,
    .light = PLAYER_LIGHT_BLUISH_GREEN};
constexpr PlayerColors PLAYER_DEEP_GREEN_SHADES{
    .dark = PLAYER_DARK_DEEP_GREEN,
    .ship = PLAYER_DEEP_GREEN,
    .light = PLAYER_LIGHT_DEEP_GREEN};

/**
 * @brief The palettes for protanopia, deuteranopia and tritanopia. Player 1
 * keeps its blue, the color of the interface. Simulated as a person with that
 * color blindness sees them (Viénot, Brettel and Mollon 1999 for protanopia
 * and deuteranopia, Brettel, Viénot and Mollon 1997 for tritanopia), the main
 * shades of two players stay at least 22.9, 14.3 and 13.1 apart in CIEDE2000,
 * where those of the sheets fall to 6.9, 2.8 and 13.1.
 */
constexpr PlayerPalette PROTANOPIA_PLAYER_PALETTE{
    PLAYER_BLUE_SHADES, PLAYER_MAGENTA_SHADES, PLAYER_TEAL_SHADES,
    PLAYER_YELLOW_SHADES};
constexpr PlayerPalette DEUTERANOPIA_PLAYER_PALETTE{
    PLAYER_BLUE_SHADES, PLAYER_REDDISH_PURPLE_SHADES,
    PLAYER_BLUISH_GREEN_SHADES, PLAYER_YELLOW_SHADES};
constexpr PlayerPalette TRITANOPIA_PLAYER_PALETTE{
    PLAYER_BLUE_SHADES, PLAYER_DEEP_GREEN_SHADES, PLAYER_GREEN_SHADES,
    PLAYER_YELLOW_SHADES};

}  // namespace rtype::client
