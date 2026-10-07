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
 * that subscribed to them, without either one knowing the other.
 *
 * An event is a copyable struct, and its type is its kind: a subscriber only
 * receives the events of the type it subscribed to.
 *
 * publish() only queues the event. dispatch() delivers the queued events in
 * the order they were published, each one to the subscribers of its type in
 * the order they subscribed. An event published by a subscriber during
 * dispatch() is delivered by that same dispatch(), so a subscriber that
 * publishes the event it receives makes dispatch() run forever. An event that
 * has no subscriber when it is delivered is dropped.
 *
 * The game loop calls dispatch() after the systems of a tick, outside any
 * ComponentRegistry::forEach, so subscribers may add and remove components.
 *
 * A subscriber may unsubscribe itself, or subscribe another callback, while it
 * is called: a callback subscribed during a delivery receives the next events,
 * not the one being delivered. dispatch() must not be called from a
 * subscriber. When a subscriber throws, the exception leaves dispatch() and
 * the events not delivered yet wait for the next dispatch().
 *
 * Not thread-safe: every thread that runs a game or a frame loop owns its bus.
 */
class EventBus {
 public:
  template <typename Event>
  using Callback = std::function<void(const Event&)>;

  /**
   * @brief Calls callback with every event of type Event delivered from now
   * on, until unsubscribe(). The event type is always written explicitly:
   * subscribe<Collision>(callback).
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

  /** @brief Queues the event until the next dispatch(). */
  template <typename Event>
  void publish(Event event) {
    pendingEvents_.emplace_back(std::move(event));
  }

  /**
   * @returns true if the subscription was removed, false if the handle is
   * unknown or already unsubscribed.
   */
  bool unsubscribe(SubscriptionHandle handle);

  /** @brief Delivers the queued events until the queue is empty. */
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
