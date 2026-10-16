#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

/** @brief A key being pressed, as a control receives it: with no modifier
 * held, at a place of the keyboard, unknown unless given. */
[[nodiscard]] sf::Event::KeyPressed keyPressOf(
    sf::Keyboard::Key key,
    sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown);

/** @brief The window event of a key being pressed, with no modifier held, at
 * a place of the keyboard, unknown unless given. */
[[nodiscard]] sf::Event keyPress(
    sf::Keyboard::Key key,
    sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown);

/** @brief The window event of a key being released, with no modifier held,
 * at a place of the keyboard, unknown unless given. */
[[nodiscard]] sf::Event keyRelease(
    sf::Keyboard::Key key,
    sf::Keyboard::Scancode scancode = sf::Keyboard::Scan::Unknown);

/** @brief The window event of a key being pressed with Shift held. */
[[nodiscard]] sf::Event shiftedKeyPress(sf::Keyboard::Key key);

/** @brief The window event of a character typed on the keyboard. */
[[nodiscard]] sf::Event typedCharacter(char32_t character);
