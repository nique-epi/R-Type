#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/**
 * @brief Colors of the progress bar of the loading screen: its border, and the
 * part already loaded.
 */
constexpr sf::Color LOADING_BAR_OUTLINE_COLOR(200, 205, 230);
constexpr sf::Color LOADING_BAR_FILL_COLOR(110, 125, 220);

}  // namespace rtype::client
