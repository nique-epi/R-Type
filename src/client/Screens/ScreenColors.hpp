#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/**
 * @brief Colors of the progress bar of the loading screen: its border, and the
 * part already loaded.
 */
constexpr sf::Color LOADING_BAR_OUTLINE_COLOR(200, 205, 230);
constexpr sf::Color LOADING_BAR_FILL_COLOR(110, 125, 220);

/**
 * @brief Translucent layer the options screen lays over the game, which stays
 * visible behind it.
 */
constexpr sf::Color OPTIONS_BACKDROP_COLOR(0, 0, 0, 160);

}  // namespace rtype::client
