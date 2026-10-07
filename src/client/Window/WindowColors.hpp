#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/**
 * @brief Color filling the playfield, so it stands out from the black bars.
 *
 * Provisional: it stands in for the scrolling background until that one
 * exists.
 */
constexpr sf::Color PLAYFIELD_BACKGROUND_COLOR(10, 12, 40);

/**
 * @brief Colors of a window size button and of its label, after the state of
 * its size: selected, available, or too large for the desktop.
 *
 * Provisional, like the buttons themselves.
 */
constexpr sf::Color SELECTED_WINDOW_SIZE_COLOR(235, 238, 255);
constexpr sf::Color SELECTED_WINDOW_SIZE_LABEL_COLOR(10, 12, 40);
constexpr sf::Color AVAILABLE_WINDOW_SIZE_COLOR(72, 84, 160);
constexpr sf::Color AVAILABLE_WINDOW_SIZE_LABEL_COLOR(235, 238, 255);
constexpr sf::Color UNAVAILABLE_WINDOW_SIZE_COLOR(30, 33, 58);
constexpr sf::Color UNAVAILABLE_WINDOW_SIZE_LABEL_COLOR(92, 98, 132);

}  // namespace rtype::client
