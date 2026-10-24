#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "PlayerColors.hpp"

namespace rtype::client {

/** @brief Cyan of the heads-up display: text laid on the sky, and the titles
 * of dialogs. */
constexpr sf::Color HUD_CYAN(127, 230, 255);

/**
 * @brief Blue of the edges: the border of bars and panels, the rule under a
 * heading, the corners of the squadron box.
 */
constexpr sf::Color EDGE_BLUE(31, 163, 201);

/**
 * @brief Near black of text on a light fill, and of the outline of text laid
 * on the sky, so a star, the planet or a comet behind never touches a letter.
 */
constexpr sf::Color INK(10, 11, 26);

/** @brief Main color of the ship of each player color, read from the rows
 * of the entity sheet: blue for player 1, pink for player 2, green for player
 * 3, orange for player 4. */
constexpr sf::Color PLAYER_BLUE(31, 163, 201);
constexpr sf::Color PLAYER_PINK(217, 74, 140);
constexpr sf::Color PLAYER_GREEN(126, 194, 58);
constexpr sf::Color PLAYER_ORANGE(224, 154, 42);

/** @brief Light shade of the ship of each player color, read from the same
 * rows. */
constexpr sf::Color PLAYER_LIGHT_BLUE(127, 230, 255);
constexpr sf::Color PLAYER_LIGHT_PINK(255, 163, 207);
constexpr sf::Color PLAYER_LIGHT_GREEN(210, 255, 138);
constexpr sf::Color PLAYER_LIGHT_ORANGE(255, 211, 138);

/** @brief The shades of each player color, in the order of the rows of the
 * entity sheet: player 1 first. */
constexpr std::array PLAYER_COLORS{
    PlayerColors{.ship = PLAYER_BLUE, .light = PLAYER_LIGHT_BLUE},
    PlayerColors{.ship = PLAYER_PINK, .light = PLAYER_LIGHT_PINK},
    PlayerColors{.ship = PLAYER_GREEN, .light = PLAYER_LIGHT_GREEN},
    PlayerColors{.ship = PLAYER_ORANGE, .light = PLAYER_LIGHT_ORANGE}};

/** @brief Light orange of the enemy missile: the warnings of the status band,
 * LAST LIFE and OUT, always written as words. */
constexpr sf::Color WARNING_ORANGE(255, 179, 107);

/** @brief Layer a dialog lays over the screens below: the sky at 72 %
 * opacity. */
constexpr std::uint8_t DIM_LAYER_ALPHA = 184;
constexpr sf::Color DIM_LAYER_COLOR(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b,
                                    DIM_LAYER_ALPHA);

}  // namespace rtype::client
