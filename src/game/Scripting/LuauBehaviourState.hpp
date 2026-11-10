#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "HeldIntent.hpp"
#include "IBehaviourState.hpp"
#include "LuauEventSubscription.hpp"
#include "LuauThread.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour keeps of one entity between two ticks: the
 * intent its script holds during a wait, the Luau thread that runs it, and
 * the functions it subscribed to events, which end with it. */
struct LuauBehaviourState final : IBehaviourState {
  /** @brief Stops the script of the entity for good: its thread and its
   * subscriptions end, and the entity stands still. Only while no script
   * runs. */
  void stopScript();

  HeldIntent heldIntent;
  /** @brief Empty before the first step and once its script failed. */
  std::optional<engine::LuauThread> thread;
  /** @brief Whether the thread stopped in a wait, to be resumed there rather
   * than called again from the start of its function. */
  bool waiting{false};
  std::vector<std::unique_ptr<LuauEventSubscription>> subscriptions;
};

}  // namespace rtype::game
