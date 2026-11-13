#pragma once

#include <SFML/Graphics/Color.hpp>
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief The mark that says a grid holds more rows than it shows: a track
 * in ink, and on it a thumb in the cyan of the chosen cell, never shorter
 * than SCROLL_THUMB_MINIMUM_LENGTH. */
constexpr sf::Color SCROLL_TRACK_COLOR = INK;
constexpr sf::Color SCROLL_THUMB_COLOR = HUD_CYAN;
constexpr int SCROLL_THUMB_MINIMUM_LENGTH = 8;

}  // namespace rtype::client
