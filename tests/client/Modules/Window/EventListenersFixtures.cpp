#include "EventListenersFixtures.hpp"
#include <SFML/Window/Event.hpp>
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
