#include <gtest/gtest.h>
#include <optional>
#include "EventBus.hpp"
#include "LuauEventSubscription.hpp"
#include "LuauEventSubscriptionParts.hpp"
#include "ScriptEvent.hpp"
#include "ScriptLimits.hpp"
#include "ScriptMachine.hpp"
#include "SubscriptionHandle.hpp"
#include "lua.h"

using rtype::engine::EventBus;
using rtype::engine::ScriptLimits;
using rtype::engine::ScriptMachine;
using rtype::engine::SubscriptionHandle;
using rtype::game::LuauEventSubscription;
using rtype::game::LuauEventSubscriptionParts;
using rtype::game::ScriptEvent;

/**
 * Given a subscription to a bus held by a LuauEventSubscription
 * When that object is destroyed
 * Then the bus no longer knows the subscription
 */
TEST(LuauEventSubscription, UnsubscribesWhenItEnds) {
  const ScriptMachine machine{ScriptLimits{}};
  EventBus bus;
  const SubscriptionHandle handle =
      bus.subscribe<ScriptEvent>([](const ScriptEvent&) {});
  lua_createtable(machine.state(), 0, 0);
  const int function = lua_ref(machine.state(), -1);
  std::optional<LuauEventSubscription> subscription;
  subscription.emplace(LuauEventSubscriptionParts{.bus = bus,
                                                  .handle = handle,
                                                  .machine = machine.state(),
                                                  .function = function});

  subscription.reset();

  EXPECT_FALSE(bus.unsubscribe(handle));
}
