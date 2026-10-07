#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief Color filling the playfield during a game. */
constexpr sf::Color PLAYFIELD_BACKGROUND_COLOR(10, 12, 40);

/** @brief Border and filled part of the progress bar of the loading screen. */
constexpr sf::Color LOADING_BAR_OUTLINE_COLOR(200, 205, 230);
constexpr sf::Color LOADING_BAR_FILL_COLOR(110, 125, 220);

/** @brief Translucent layer the options screen lays over the game. */
constexpr sf::Color OPTIONS_BACKDROP_COLOR(0, 0, 0, 160);

}  // namespace rtype::client
