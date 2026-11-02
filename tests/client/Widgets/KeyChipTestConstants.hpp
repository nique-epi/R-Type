#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "PointerEvents.hpp"

namespace rtype::client::testing {

constexpr sf::IntRect CHIP_AREA({50, 60}, {100, 16});
constexpr sf::Keyboard::Key CHIP_KEY = sf::Keyboard::Key::F;
constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::G;
constexpr sf::Vector2i ON_THE_CHIP = sf::Vector2i(100, 68) * INTERFACE_PIXEL;
constexpr sf::Vector2i OFF_THE_CHIP = sf::Vector2i(200, 68) * INTERFACE_PIXEL;

}  // namespace rtype::client::testing
