#include "LagometerToggle.hpp"
#include <SFML/Window/Event.hpp>
#include "EventUse.hpp"
#include "LagometerConstants.hpp"

namespace rtype::client {

EventUse LagometerToggle::handle(const sf::Event& event) {
  if (event.is<sf::Event::FocusLost>()) {
    held_ = false;
    return EventUse::Passed;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->scancode == LAGOMETER_KEY) {
    held_ = false;
    return EventUse::Kept;
  }
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed == nullptr || pressed->scancode != LAGOMETER_KEY) {
    return EventUse::Passed;
  }
  if (!held_) {
    shown_ = !shown_;
    held_ = true;
  }
  return EventUse::Kept;
}

bool LagometerToggle::shown() const { return shown_; }

}  // namespace rtype::client
