#include "LuauEventSubscription.hpp"
#include "LuauEventSubscriptionParts.hpp"
#include "lua.h"

namespace rtype::game {

LuauEventSubscription::LuauEventSubscription(LuauEventSubscriptionParts parts)
    : parts_(parts) {}

LuauEventSubscription::~LuauEventSubscription() {
  static_cast<void>(parts_.bus.get().unsubscribe(parts_.handle));
  lua_unref(parts_.machine, parts_.function);
}

}  // namespace rtype::game
