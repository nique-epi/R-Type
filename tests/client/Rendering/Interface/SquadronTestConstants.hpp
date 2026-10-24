#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include "EntitySpriteConstants.hpp"

namespace rtype::client::testing {

/** @brief Where the title screen lays the box of its squadron on the mockup:
 * against the right margin, 64 rows down. */
constexpr sf::Vector2i SQUADRON_BOX_ON_THE_TITLE(288, 64);

/** @brief Where the four ships sit once in place on the title, in the order of
 * the player colors: the box plus their places on the mockup, (108, 72),
 * (68, 48), (68, 96) and (28, 72). */
constexpr std::array SHIPS_IN_PLACE_ON_THE_TITLE{
    sf::Vector2i(396, 136), sf::Vector2i(356, 112), sf::Vector2i(356, 160),
    sf::Vector2i(316, 136)};

/** @brief The size of the sprite of a ship, which tells the ships apart from
 * the other images of the entity sheet. */
constexpr sf::Vector2i SHIP_SIZE(PLAYER_SHIP_FRAME.width,
                                 PLAYER_SHIP_FRAME.height);

}  // namespace rtype::client::testing
