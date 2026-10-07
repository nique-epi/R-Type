#include "KeyPress.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

sf::Event keyPress(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyPressed{.code = key}};
}

sf::Event keyRelease(sf::Keyboard::Key key) {
  return sf::Event{sf::Event::KeyReleased{.code = key}};
}
