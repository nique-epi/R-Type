#pragma once

#include <cstdint>
#include <optional>
#include "IBehaviourMemory.hpp"
#include "Intent.hpp"
#include "ScriptThread.hpp"

namespace rtype::game {

/** @brief What a scripted behaviour remembers of one body: the thread its
 * function runs in, how long it still waits and what it wants meanwhile. */
struct ScriptedRoutine final : IBehaviourMemory {
  /** @brief Empty before the first step and once the routine stopped. */
  std::optional<engine::ScriptThread> thread;
  /** @brief The ticks until the thread is resumed, this one included; 0 while
   * it does not wait. */
  std::uint32_t ticksLeft{0};
  /** @brief What the body wants while no step of its script runs. */
  Intent held{.steered = true};
  /** @brief Whether its script failed: it is never called again. */
  bool stopped{false};
};

}  // namespace rtype::game
