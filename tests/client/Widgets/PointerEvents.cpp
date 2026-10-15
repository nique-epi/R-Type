#include "PointerEvents.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

sf::Vector2f samePlayfieldPoint(sf::Vector2i pixel) {
  return sf::Vector2f(pixel);
}

sf::Event pointerMove(sf::Vector2i position) {
  return sf::Event{sf::Event::MouseMoved{.position = position}};
}

sf::Event leftPress(sf::Vector2i position) {
  return sf::Event{sf::Event::MouseButtonPressed{
      .button = sf::Mouse::Button::Left, .position = position}};
}

sf::Event leftRelease(sf::Vector2i position) {
  return sf::Event{sf::Event::MouseButtonReleased{
      .button = sf::Mouse::Button::Left, .position = position}};
}
