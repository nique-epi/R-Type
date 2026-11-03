#pragma once

#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <vector>
#include "EventListenerSlot.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::client {

/**
 * @brief The modules that listen to the window events: each event is shown
 * to them in turn, the last added first, until one keeps it.
 *
 * A module adds its listener in its start() and removes it in its stop(), so
 * a module started later sees an event first. A module listed after the
 * screens module (a debug display, a console) sees a key before the screen on
 * top, and keeps it when it is its own, whatever screen is shown. It is the
 * order of the screen stack: the last drawn, on top, hears first.
 *
 * A listener adds or removes no listener while it is shown an event.
 */
class EventListeners {
 public:
  /** @returns The handle that removes @p listener, never one returned
   * before. */
  [[nodiscard]] engine::SubscriptionHandle add(EventListener listener);

  /**
   * @brief Removes the listener added under @p handle.
   * @returns false, changing nothing, when no listener has this handle, as
   * once it was removed.
   */
  bool remove(engine::SubscriptionHandle handle);

  /** @brief Shows @p event to the listeners, the last added first, until one
   * keeps it. */
  void offer(const sf::Event& event) const;

 private:
  std::vector<EventListenerSlot> slots_;
  std::uint64_t nextIdentifier_{0};
};

}  // namespace rtype::client
