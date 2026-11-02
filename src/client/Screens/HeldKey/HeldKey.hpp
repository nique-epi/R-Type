#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>

namespace rtype::client {

/** @returns Whether @p event lets go of @p heldKey: its release, or the
 * window losing the focus, which hides that release. A screen on which a key
 * held down acts once keeps the key pressed last, and forgets it then. */
[[nodiscard]] bool releasesHeldKey(const sf::Event& event,
                                   std::optional<sf::Keyboard::Key> heldKey);

}  // namespace rtype::client
