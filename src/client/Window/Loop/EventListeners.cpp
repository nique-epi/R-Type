#include "EventListeners.hpp"
#include <SFML/Window/Event.hpp>
#include <ranges>
#include <utility>
#include <vector>
#include "EventListenerSlot.hpp"
#include "EventUse.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::client {

engine::SubscriptionHandle EventListeners::add(EventListener listener) {
  const engine::SubscriptionHandle handle{.identifier = nextIdentifier_};
  ++nextIdentifier_;
  slots_.push_back(
      EventListenerSlot{.handle = handle, .listener = std::move(listener)});
  return handle;
}

bool EventListeners::remove(engine::SubscriptionHandle handle) {
  return std::erase_if(slots_, [handle](const EventListenerSlot& slot) {
           return slot.handle == handle;
         }) > 0;
}

void EventListeners::offer(const sf::Event& event) const {
  for (const EventListenerSlot& slot : std::views::reverse(slots_)) {
    if (slot.listener(event) == EventUse::Kept) {
      return;
    }
  }
}

}  // namespace rtype::client
