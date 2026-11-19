#include "KeyDown.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>

namespace rtype::client {

std::optional<bool> keyDownOf(const sf::Event& event,
                              sf::Keyboard::Scancode key) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>();
      pressed != nullptr && pressed->scancode == key) {
    return true;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->scancode == key) {
    return false;
  }
  return std::nullopt;
}

}  // namespace rtype::client
