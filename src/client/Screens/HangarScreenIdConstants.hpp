#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The hangar, pushed over the server map or the waiting room: the
 * locker of items and the ship that wears them. */
constexpr ScreenId HANGAR_SCREEN{"hangar"};

/** @brief The name the map, the waiting room and the hangar itself show,
 * the key that opens the hangar from the map and the waiting room, and
 * what its keycap says. */
constexpr const char* HANGAR_LABEL = "HANGAR";
constexpr sf::Keyboard::Key HANGAR_KEY = sf::Keyboard::Key::G;
constexpr const char* HANGAR_KEY_NAME = "G";

}  // namespace rtype::client
