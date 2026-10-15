#include "KeyPress.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

sf::Event keyPress(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyPressed{.code = key}};
}

sf::Event keyRelease(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyReleased{.code = key}};
}

sf::Event shiftedKeyPress(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyPressed{.code = key, .shift = true}};
}

sf::Event typedCharacter(char32_t character) {
  return sf::Event{sf::Event::TextEntered{.unicode = character}};
}
