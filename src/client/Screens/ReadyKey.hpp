#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace rtype::client {

/** @brief The key bound to BoundAction::Ready, and the name its keycap
 * shows. */
struct ReadyKey {
  sf::Keyboard::Scancode key{};
  std::string name;
};

}  // namespace rtype::client
