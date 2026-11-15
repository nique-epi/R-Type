#pragma once

#include <SFML/Graphics/Color.hpp>
#include "PocketShades.hpp"

namespace rtype::client {

/** @brief The shades of the red pockets, from the darkest, as the cloth and
 * the wheel draw them. */
constexpr sf::Color POCKET_RED_SHADOW(69, 16, 39);
constexpr sf::Color POCKET_RED_DARK(116, 22, 47);
constexpr sf::Color POCKET_RED(168, 32, 53);
constexpr sf::Color POCKET_RED_LIGHT(212, 58, 60);
constexpr PocketShades RED_POCKET_SHADES{.shadow = POCKET_RED_SHADOW,
                                         .dark = POCKET_RED_DARK,
                                         .base = POCKET_RED,
                                         .light = POCKET_RED_LIGHT};

/** @brief The shades of the black pockets, from the darkest. */
constexpr sf::Color POCKET_BLACK_SHADOW(5, 6, 15);
constexpr sf::Color POCKET_BLACK_DARK(11, 13, 31);
constexpr sf::Color POCKET_BLACK(20, 24, 53);
constexpr sf::Color POCKET_BLACK_LIGHT(30, 36, 80);
constexpr PocketShades BLACK_POCKET_SHADES{.shadow = POCKET_BLACK_SHADOW,
                                           .dark = POCKET_BLACK_DARK,
                                           .base = POCKET_BLACK,
                                           .light = POCKET_BLACK_LIGHT};

/** @brief The shades of the green pocket of the zero, from the darkest. */
constexpr sf::Color POCKET_GREEN_SHADOW(15, 58, 54);
constexpr sf::Color POCKET_GREEN_DARK(23, 87, 63);
constexpr sf::Color POCKET_GREEN(35, 122, 74);
constexpr sf::Color POCKET_GREEN_LIGHT(63, 159, 88);
constexpr PocketShades GREEN_POCKET_SHADES{.shadow = POCKET_GREEN_SHADOW,
                                           .dark = POCKET_GREEN_DARK,
                                           .base = POCKET_GREEN,
                                           .light = POCKET_GREEN_LIGHT};

/** @brief The light green a gain of the table is written in. */
constexpr sf::Color GAIN_GREEN(126, 194, 106);

/** @brief The bronze that frames the older numbers drawn, and the palest
 * gold, the lit corner of the plaque of the result. */
constexpr sf::Color CASINO_BRONZE(143, 90, 24);
constexpr sf::Color CASINO_PALE_GOLD(255, 244, 207);

}  // namespace rtype::client
