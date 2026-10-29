#pragma once

#include <SFML/Window/Event.hpp>
#include <functional>
#include "EventUse.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::client {

/** @brief Shows a window event to a module, which keeps it or passes it on. */
using EventListener = std::function<EventUse(const sf::Event&)>;

/** @brief A listener of the window events, and the handle that removes it. */
struct EventListenerSlot {
  engine::SubscriptionHandle handle{};
  EventListener listener;
};

}  // namespace rtype::client
