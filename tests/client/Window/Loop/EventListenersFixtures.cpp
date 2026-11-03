#include "EventListenersFixtures.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "EventListenerSlot.hpp"
#include "EventUse.hpp"

rtype::client::EventListener recordingListener(std::vector<int>& seen, int name,
                                               rtype::client::EventUse use) {
  return [&seen, name, use]([[maybe_unused]] const sf::Event& event) {
    seen.push_back(name);
    return use;
  };
}

rtype::client::EventListener keyRecordingListener(
    std::vector<sf::Keyboard::Key>& pressedKeys) {
  return [&pressedKeys](const sf::Event& event) {
    if (const auto* press = event.getIf<sf::Event::KeyPressed>()) {
      pressedKeys.push_back(press->code);
    }
    return rtype::client::EventUse::Kept;
  };
}
