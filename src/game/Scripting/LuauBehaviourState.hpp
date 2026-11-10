#pragma once

#include <optional>
#include "HeldIntent.hpp"
#include "IBehaviourState.hpp"
#include "LuauThread.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour keeps of one entity between two ticks: the
 * intent its script holds during a wait, and the Luau thread that runs it. */
struct LuauBehaviourState final : IBehaviourState {
  HeldIntent heldIntent;
  /** @brief Empty before the first step and once its script failed. */
  std::optional<engine::LuauThread> thread;
  /** @brief Whether the thread stopped in a wait, to be resumed there rather
   * than called again from the start of its function. */
  bool waiting{false};
};

}  // namespace rtype::game
