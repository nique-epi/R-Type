#pragma once

#include <optional>
#include "HeldIntent.hpp"
#include "IBehaviourState.hpp"
#include "LuauEventSubscriptions.hpp"
#include "LuauThread.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour keeps of one entity between two ticks: the
 * intent its script holds during a wait, the Luau thread that runs it, and
 * the functions it subscribed to events, which end with it. */
struct LuauBehaviourState final : IBehaviourState {
  HeldIntent heldIntent;
  /** @brief Empty before the first step and once its script failed. */
  std::optional<engine::LuauThread> thread;
  /** @brief Whether the thread stopped in a wait, to be resumed there rather
   * than called again from the start of its function. */
  bool waiting{false};
  LuauEventSubscriptions subscriptions;
};

}  // namespace rtype::game
