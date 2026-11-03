#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace rtype::client {

/**
 * @returns The name the player reads on a key: the letter the keyboard layout
 * prints on a letter key, so the physical Q key reads A on an AZERTY
 * keyboard; the name KEY_NAMES gives any other key; UNNAMED_KEY_TEXT for a key
 * it does not list. Asks the system for the layout at each call.
 */
[[nodiscard]] std::string localizedKeyName(sf::Keyboard::Scancode key);

}  // namespace rtype::client
