#pragma once

#include <functional>
#include "EventBus.hpp"
#include "SubscriptionHandle.hpp"

struct lua_State;

namespace rtype::game {

/** @brief What a LuauEventSubscription is built from: the bus and the handle
 * of the subscription on it, and the main state of the machine with the
 * reference of the subscribed function in its registry. The bus and the
 * machine must outlive it. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LuauEventSubscriptionParts {
  std::reference_wrapper<engine::EventBus> bus;
  engine::SubscriptionHandle handle{};
  lua_State* machine{nullptr};
  int function{0};
};

}  // namespace rtype::game
