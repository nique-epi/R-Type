#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

/** @brief The window event of a key being pressed, with no modifier held. */
[[nodiscard]] sf::Event keyPress(sf::Keyboard::Key key);
