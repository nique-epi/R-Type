#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>

namespace rtype::client {

/** @returns Whether @p event presses (true) or releases (false) the key at
 * the place @p key, as a binding names it; nothing for any other event. The
 * waiting room latches READY on it, so a key held down acts once. */
[[nodiscard]] std::optional<bool> keyDownOf(const sf::Event& event,
                                            sf::Keyboard::Scancode key);

/** @returns Whether @p event presses (true) or releases (false) the key the
 * layout of the keyboard names @p key, wherever it lies: a fixed key whose
 * keycap shows a letter (MAP_KEY) is that letter on an AZERTY keyboard too;
 * nothing for any other event. */
[[nodiscard]] std::optional<bool> keyDownOf(const sf::Event& event,
                                            sf::Keyboard::Key key);

}  // namespace rtype::client
