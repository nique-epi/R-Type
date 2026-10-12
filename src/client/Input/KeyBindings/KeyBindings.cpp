#include "KeyBindings.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "PlayerAction.hpp"

namespace rtype::client {

std::optional<PlayerAction> KeyBindings::actionFor(sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::Key::Up:
      return PlayerAction::Up;
    case sf::Keyboard::Key::Down:
      return PlayerAction::Down;
    case sf::Keyboard::Key::Left:
      return PlayerAction::Left;
    case sf::Keyboard::Key::Right:
      return PlayerAction::Right;
    case sf::Keyboard::Key::Space:
      return PlayerAction::Shoot;
    default:
      return std::nullopt;
  }
}

}  // namespace rtype::client
