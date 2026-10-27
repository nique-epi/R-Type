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
 * rows. The orange one is one step of blue above STAR_GOLD, the flame of every
 * ship: a frame of player 4 changes its light shade and keeps its flame. */
constexpr sf::Color PLAYER_LIGHT_BLUE(127, 230, 255);
constexpr sf::Color PLAYER_LIGHT_PINK(255, 163, 207);
constexpr sf::Color PLAYER_LIGHT_GREEN(210, 255, 138);
constexpr sf::Color PLAYER_LIGHT_ORANGE(255, 211, 139);

/** @brief Dark shade of the ship of each player color, read from the same
 * rows. */
constexpr sf::Color PLAYER_DARK_BLUE(15, 90, 115);
constexpr sf::Color PLAYER_DARK_PINK(122, 31, 77);
constexpr sf::Color PLAYER_DARK_GREEN(61, 107, 26);
constexpr sf::Color PLAYER_DARK_ORANGE(122, 74, 16);

/** @brief The three shades of each player color as the sheets draw them. */
constexpr PlayerColors PLAYER_BLUE_SHADES{
    .dark = PLAYER_DARK_BLUE, .ship = PLAYER_BLUE, .light = PLAYER_LIGHT_BLUE};
constexpr PlayerColors PLAYER_PINK_SHADES{
    .dark = PLAYER_DARK_PINK, .ship = PLAYER_PINK, .light = PLAYER_LIGHT_PINK};
constexpr PlayerColors PLAYER_GREEN_SHADES{.dark = PLAYER_DARK_GREEN,
                                           .ship = PLAYER_GREEN,
                                           .light = PLAYER_LIGHT_GREEN};
constexpr PlayerColors PLAYER_ORANGE_SHADES{.dark = PLAYER_DARK_ORANGE,
                                            .ship = PLAYER_ORANGE,
                                            .light = PLAYER_LIGHT_ORANGE};

/** @brief The shades of each player color as the sheets draw them, in the
 * order of their rows: player 1 first. */
constexpr std::array PLAYER_COLORS{PLAYER_BLUE_SHADES, PLAYER_PINK_SHADES,
                                   PLAYER_GREEN_SHADES, PLAYER_ORANGE_SHADES};

static_assert(PLAYER_LIGHT_ORANGE != STAR_GOLD,
              "the light shade of player 4 must not be the gold of the flames");

/** @brief Light orange of the enemy missile: the warnings of the status band,
 * LAST LIFE and OUT, always written as words. */
constexpr sf::Color WARNING_ORANGE(255, 179, 107);

/** @brief Layer a dialog lays over the screens below: the sky at 72 %
 * opacity. */
constexpr std::uint8_t DIM_LAYER_ALPHA = 184;
constexpr sf::Color DIM_LAYER_COLOR(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b,
                                    DIM_LAYER_ALPHA);

}  // namespace rtype::client
