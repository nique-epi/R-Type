#include "KeyPress.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

sf::Event::KeyPressed keyPressOf(sf::Keyboard::Key key,
                                 sf::Keyboard::Scancode scancode) {
  return sf::Event::KeyPressed{.code = key, .scancode = scancode};
}

sf::Event keyPress(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode) {
  return sf::Event{keyPressOf(key, scancode)};
}

sf::Event keyRelease(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode) {
  return sf::Event{sf::Event::KeyReleased{.code = key, .scancode = scancode}};
}

sf::Event shiftedKeyPress(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyPressed{.code = key, .shift = true}};
}

sf::Event typedCharacter(char32_t character) {
  return sf::Event{sf::Event::TextEntered{.unicode = character}};
}
