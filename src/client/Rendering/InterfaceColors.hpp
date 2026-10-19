#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstdint>
#include "BackgroundColors.hpp"

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

/** @brief Main color of the ship of each player color, in the order of the
 * rows of the entity sheet, read from the file: what the interface paints in
 * the color of a player. */
constexpr std::array PLAYER_SHIP_COLORS{
    sf::Color(31, 163, 201), sf::Color(217, 74, 140), sf::Color(126, 194, 58),
    sf::Color(224, 154, 42)};

/** @brief Layer a dialog lays over the screens below: the sky at 72 %
 * opacity. */
constexpr std::uint8_t DIM_LAYER_ALPHA = 184;
constexpr sf::Color DIM_LAYER_COLOR(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b,
                                    DIM_LAYER_ALPHA);

}  // namespace rtype::client
