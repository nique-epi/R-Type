#include "LocalizedKeyName.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "InputConstants.hpp"
#include "KeyNames.hpp"

namespace rtype::client {

std::string localizedKeyName(sf::Keyboard::Scancode key) {
  const sf::Keyboard::Key letter = sf::Keyboard::localize(key);
  if (letter >= sf::Keyboard::Key::A && letter <= sf::Keyboard::Key::Z) {
    const int offset =
        static_cast<int>(letter) - static_cast<int>(sf::Keyboard::Key::A);
    return {static_cast<char>(FIRST_LETTER_NAME + offset)};
  }
  return std::string(keyName(key).value_or(UNNAMED_KEY_TEXT));
}

}  // namespace rtype::client
