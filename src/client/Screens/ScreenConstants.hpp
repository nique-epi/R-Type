#pragma once

#include <SFML/Window/Keyboard.hpp>

namespace rtype::client {

/** @brief Asset id of the font the screens write with. */
constexpr const char* INTERFACE_FONT_ID = "fonts/tuffy.ttf";

/** @brief Key whose release opens the options during a game, and closes
 * them. */
constexpr sf::Keyboard::Key OPTIONS_KEY = sf::Keyboard::Key::Escape;

/**
 * @brief Size of the progress bar of the loading screen, centered in the
 * playfield, and thickness of its border, in playfield units.
 */
constexpr float LOADING_BAR_WIDTH = 960.0F;
constexpr float LOADING_BAR_HEIGHT = 24.0F;
constexpr float LOADING_BAR_OUTLINE_THICKNESS = 3.0F;

}  // namespace rtype::client
