#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "PixelSize.hpp"

constexpr int LOWEST_VOLUME = 0;
constexpr int HIGHEST_VOLUME = 100;
constexpr int BELOW_LOWEST_VOLUME = -1;
constexpr int ABOVE_HIGHEST_VOLUME = 101;
constexpr int A_VOLUME = 35;
constexpr int ANOTHER_VOLUME = 60;
constexpr rtype::client::PixelSize A_WINDOW_SIZE{.width = 1280, .height = 720};
constexpr sf::Keyboard::Scancode A_KEY = sf::Keyboard::Scancode::J;
