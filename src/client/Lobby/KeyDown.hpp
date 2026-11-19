#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>

namespace rtype::client {

/** @returns Whether @p event presses (true) or releases (false) @p key;
 * nothing for any other event. The waiting room latches READY and MAP on it,
 * so a key held down acts once. */
[[nodiscard]] std::optional<bool> keyDownOf(const sf::Event& event,
                                            sf::Keyboard::Scancode key);

}  // namespace rtype::client
