#pragma once

#include <any>
#include <cstdint>
#include <deque>
#include <functional>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::engine {

/**
 * @brief Carries typed events from the code that publishes them to the code
 * that subscribed to their type, without either one knowing the other.
 *
 * publish() only queues; the game loop must call dispatch() after the systems,
 * outside any forEach, so subscribers may add and remove components. Not
 * thread-safe.
 */
class EventBus {
 public:
  template <typename Event>
  using Callback = std::function<void(const Event&)>;

  /**
   * @brief Calls callback with every later event of type Event, until
   * unsubscribe(). The type is written explicitly: subscribe<Collision>(...).
   * @throws EmptyEventCallbackException if callback is empty.
   */
  template <typename Event>
  SubscriptionHandle subscribe(Callback<Event> callback) {
    if (!callback) {
      throw EmptyEventCallbackException();
    }
    const SubscriptionHandle handle{.identifier = nextIdentifier_++};
    subscriptions_.push_back(Subscription{
        .identifier = handle.identifier,
        .eventType = std::type_index(typeid(Event)),
        .callback = [typedCallback =
                         std::move(callback)](const std::any& event) {
          typedCallback(std::any_cast<const Event&>(event));
        }});
    return handle;
  }

  /** @brief Queues the event, a copyable struct, until the next dispatch(). */
  template <typename Event>
  void publish(Event event) {
    pendingEvents_.emplace_back(std::move(event));
  }

  /**
   * @returns true if the subscription was removed, false if the handle is
   * unknown or already unsubscribed. A subscriber may unsubscribe itself.
   */
  bool unsubscribe(SubscriptionHandle handle);

  /**
   * @brief Empties the queue, events published meanwhile included, delivering
   * each in publication order to its subscribers in subscription order.
   * Never call it from a subscriber, and never republish the event received.
   * If a subscriber throws, the events left wait for the next dispatch().
   */
  void dispatch();

 private:
  using AnyEventCallback = std::function<void(const std::any&)>;

  struct Subscription {
    std::uint64_t identifier;
    std::type_index eventType;
    AnyEventCallback callback;
  };

  void deliver(const std::any& event);

  /** Sorted by identifier, which is the order of the subscriptions. */
  std::vector<Subscription> subscriptions_;
  std::deque<std::any> pendingEvents_;
  std::uint64_t nextIdentifier_{0};
};

}  // namespace rtype::engine
