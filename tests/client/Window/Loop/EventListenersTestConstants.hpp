#pragma once

#include <SFML/Window/Keyboard.hpp>

/** @brief The names the tests give their listeners, to read in which order
 * they saw an event. */
constexpr int SCREENS_LISTENER = 1;
constexpr int GLOBAL_KEY_LISTENER = 2;

/** @brief The key the listener of a global key keeps. */
constexpr sf::Keyboard::Key GLOBAL_KEY = sf::Keyboard::Key::F3;
