#include "KeyNames.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string_view>
#include "InputConstants.hpp"

namespace rtype::client {

std::optional<std::string_view> keyName(sf::Keyboard::Scancode key) {
  for (const NamedKey& named : KEY_NAMES) {
    if (named.key == key) {
      return named.name;
    }
  }
  return std::nullopt;
}

std::optional<sf::Keyboard::Scancode> keyNamed(std::string_view name) {
  for (const NamedKey& named : KEY_NAMES) {
    if (named.name == name) {
      return named.key;
    }
  }
  return std::nullopt;
}

}  // namespace rtype::client
