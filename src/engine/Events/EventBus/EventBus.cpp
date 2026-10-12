#include "EventBus.hpp"
#include <algorithm>
#include <any>
#include <cstdint>
#include <typeindex>
#include <utility>
#include "SubscriptionHandle.hpp"

namespace rtype::engine {

bool EventBus::unsubscribe(SubscriptionHandle handle) {
  const auto found = std::ranges::find(subscriptions_, handle.identifier,
                                       &Subscription::identifier);
  if (found == subscriptions_.end()) {
    return false;
  }
  subscriptions_.erase(found);
  return true;
}

void EventBus::dispatch() {
  while (!pendingEvents_.empty()) {
    const std::any event = std::move(pendingEvents_.front());
    pendingEvents_.pop_front();
    deliver(event);
  }
}

void EventBus::deliver(const std::any& event) {
  if (subscriptions_.empty()) {
    return;
  }
  const std::type_index eventType(event.type());
  const std::uint64_t lastIdentifier = subscriptions_.back().identifier;
  auto subscription = subscriptions_.begin();
  while (subscription != subscriptions_.end() &&
         subscription->identifier <= lastIdentifier) {
    const std::uint64_t identifier = subscription->identifier;
    if (subscription->eventType == eventType) {
      const AnyEventCallback callback = subscription->callback;
      callback(event);
    }
    subscription = std::ranges::upper_bound(subscriptions_, identifier, {},
                                            &Subscription::identifier);
  }
}

}  // namespace rtype::engine
