#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstdint>
#include "EntitySpriteConstants.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"

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

constexpr std::uint32_t ANY_SEED = 7;
constexpr sf::IntRect BOX({288, 64}, SQUADRON_BOX_SIZE);
constexpr engine::Duration FRAME = std::chrono::milliseconds(10);
/**
 * @brief Moments of the arrival, from the moment the squadron is built, each
 * well inside one phase of SquadronConstants.hpp so a rounding of the float
 * time never moves it to the next one.
 */
constexpr engine::Duration WHILE_THE_LINE_GROWS = std::chrono::milliseconds(90);
constexpr engine::Duration ONCE_THE_BOX_IS_OPEN =
    std::chrono::milliseconds(500);
constexpr engine::Duration THREE_LETTERS_IN = std::chrono::milliseconds(575);
constexpr engine::Duration JUST_AFTER_THE_FIRST_ARRIVAL =
    std::chrono::milliseconds(610);
constexpr engine::Duration ONCE_LANDED_BEFORE_THE_DRIFT =
    std::chrono::milliseconds(1850);
constexpr engine::Duration JUST_BEFORE_THE_FIRST_SHOT =
    std::chrono::milliseconds(2390);
constexpr engine::Duration JUST_AFTER_THE_FIRST_SHOT =
    std::chrono::milliseconds(2410);
constexpr engine::Duration FIRST_MISSILE_OUT_NEXT_NOT_FIRED =
    std::chrono::milliseconds(3300);
constexpr engine::Duration LONG_WATCH = std::chrono::seconds(30);
constexpr engine::Duration A_WHILE = std::chrono::seconds(10);
constexpr engine::Duration A_STALL = std::chrono::seconds(1);
constexpr sf::Vector2i MISSILE_SIZE(16, 6);
constexpr int MIDDLE_ROW = 72;
constexpr int BOTTOM_ROW = 143;
constexpr sf::Vector2i CORNER_STROKE(8, 1);
constexpr sf::Vector2i TITLE_POSITION(296, 72);

}  // namespace rtype::client::testing
