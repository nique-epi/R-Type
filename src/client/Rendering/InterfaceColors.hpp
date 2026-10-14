#pragma once

#include <SFML/Graphics/Color.hpp>
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

/** @brief Layer a dialog lays over the screens below: the sky at 72 %
 * opacity. */
constexpr std::uint8_t DIM_LAYER_ALPHA = 184;
constexpr sf::Color DIM_LAYER_COLOR(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b,
                                    DIM_LAYER_ALPHA);

}  // namespace rtype::client
