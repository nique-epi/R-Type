#include "HeldKey.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>

namespace rtype::client {

bool releasesHeldKey(const sf::Event& event,
                     std::optional<sf::Keyboard::Key> heldKey) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  return event.is<sf::Event::FocusLost>() ||
         (released != nullptr && heldKey == released->code);
}

}  // namespace rtype::client
