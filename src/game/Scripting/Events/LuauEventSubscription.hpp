#pragma once

#include "LuauEventSubscriptionParts.hpp"

namespace rtype::game {

/**
 * @brief A function of a script subscribed to an event of the bus of a game,
 * for as long as this object lives: destroying it unsubscribes the function
 * and lets the collector of Luau take it.
 *
 * It must be destroyed while no script runs. Neither copied nor moved: its
 * destructor gives its handle and its reference back once.
 */
class LuauEventSubscription {
 public:
  explicit LuauEventSubscription(LuauEventSubscriptionParts parts);
  LuauEventSubscription(const LuauEventSubscription&) = delete;
  LuauEventSubscription& operator=(const LuauEventSubscription&) = delete;
  LuauEventSubscription(LuauEventSubscription&&) = delete;
  LuauEventSubscription& operator=(LuauEventSubscription&&) = delete;
  ~LuauEventSubscription();

 private:
  LuauEventSubscriptionParts parts_;
};

}  // namespace rtype::game
