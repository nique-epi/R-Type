#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include "BackgroundColorConstants.hpp"
#include "ColorReplacement.hpp"
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief The three colors HIGH CONTRAST adds: black under everything, white
 * for pale text, and a light grey for what cannot be taken, 9:1 on black. */
constexpr sf::Color HIGH_CONTRAST_BLACK(0, 0, 0);
constexpr sf::Color HIGH_CONTRAST_WHITE(255, 255, 255);
constexpr sf::Color HIGH_CONTRAST_GREY(170, 170, 170);

/**
 * @brief Each color of the menus and the HUD HIGH CONTRAST replaces.
 *
 * The backgrounds turn black: the sky, the ink of fields and keycaps, and
 * the inside of a pressed bar, which is the dark shade of the blue player.
 * Pale text turns white (21:1 on black). The dark blue of the status band
 * turns cyan (14.7:1 on black, where it read 1.9:1), and so does every blue
 * edge, which already reads 7.2:1 on black: cyan leaves it a margin, and
 * black text on a selected bar reads 14.7:1. The blue of what cannot be taken
 * turns grey (9:1). The cyan, gold, orange and white text are left as they are:
 * each reads at 11.9:1 or more on black. Ratios are those of WCAG 2.2, whose
 * high contrast mode asks 7:1.
 */
constexpr std::array HIGH_CONTRAST_COLORS{
    ColorReplacement{.from = SKY_COLOR, .to = HIGH_CONTRAST_BLACK},
    ColorReplacement{.from = INK, .to = HIGH_CONTRAST_BLACK},
    ColorReplacement{.from = PLAYER_DARK_BLUE, .to = HIGH_CONTRAST_BLACK},
    ColorReplacement{.from = PALE_BLUE, .to = HIGH_CONTRAST_WHITE},
    ColorReplacement{.from = EDGE_BLUE, .to = HUD_CYAN},
    ColorReplacement{.from = DEEP_BLUE, .to = HUD_CYAN},
    ColorReplacement{.from = BLUE, .to = HIGH_CONTRAST_GREY},
};

}  // namespace rtype::client
