#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace rtype::client {

/** @brief Gives the text a keycap shows for a key. */
using KeyNaming = std::function<std::string(sf::Keyboard::Scancode)>;

/** @returns The name of a key the player may bind, as KEY_NAMES writes it,
 * or nothing for a key it does not list. */
[[nodiscard]] std::optional<std::string_view> keyName(
    sf::Keyboard::Scancode key);

/** @returns The key KEY_NAMES gives this name, or nothing for any other
 * text. */
[[nodiscard]] std::optional<sf::Keyboard::Scancode> keyNamed(
    std::string_view name);

}  // namespace rtype::client
