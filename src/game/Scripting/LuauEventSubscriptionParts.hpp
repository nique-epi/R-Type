#pragma once

#include <functional>
#include "EventBus.hpp"
#include "SubscriptionHandle.hpp"

struct lua_State;

namespace rtype::game {

/** @brief What a LuauEventSubscription releases when it ends: its handle on
 * the bus, and the reference of its function in the registry of the main
 * state of the machine. The bus and the machine must outlive it. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LuauEventSubscriptionParts {
  std::reference_wrapper<engine::EventBus> bus;
  engine::SubscriptionHandle handle{};
  lua_State* machine{nullptr};
  int function{0};
};

}  // namespace rtype::game
