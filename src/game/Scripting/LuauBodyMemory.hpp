#pragma once

#include <optional>
#include "IBehaviourMemory.hpp"
#include "Routine.hpp"
#include "ScriptThread.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour remembers of one body: its routine, and the
 * Luau thread that runs the steps of that routine. */
struct LuauBodyMemory final : IBehaviourMemory {
  Routine routine;
  /** @brief Empty before the first step and once the routine stopped. */
  std::optional<engine::ScriptThread> thread;
  /** @brief Whether the thread stopped in a wait, to be resumed there rather
   * than called again from the start of its function. */
  bool waiting{false};
};

}  // namespace rtype::game
