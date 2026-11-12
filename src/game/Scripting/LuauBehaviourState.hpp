#pragma once

#include <optional>
#include <string>
#include "HeldIntent.hpp"
#include "IBehaviourState.hpp"
#include "IncomingHit.hpp"
#include "LuauEntityHandle.hpp"
#include "LuauEventSubscriptions.hpp"
#include "LuauPreHit.hpp"
#include "LuauThread.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour keeps of one entity between two ticks: the
 * intent its script holds during a wait, the Luau thread that runs it, the
 * functions it subscribed to events, the function that judges its hits, and
 * the handle `e` its script receives, with its `data`, all of which end with
 * it. */
struct LuauBehaviourState final : IBehaviourState {
  /**
   * @returns Whether @p hit applies, as the pre_hit function decides; true
   * when there is none. A function that fails lets the hit apply, is
   * forgotten, and keeps why in preHitFailure.
   */
  [[nodiscard]] bool letsHit(const IncomingHit& hit) override;

  HeldIntent heldIntent;
  /** @brief Empty before the first step and once its script failed. */
  std::optional<engine::LuauThread> thread;
  /** @brief Whether the thread stopped in a wait, to be resumed there rather
   * than called again from the start of its function. */
  bool waiting{false};
  LuauEventSubscriptions subscriptions;
  /** @brief Empty before the first step. */
  std::optional<LuauEntityHandle> handle;
  /** @brief The function `behaviours.pre_hit` declared; empty when none was,
   * once it failed, and once the script of the entity stopped. */
  std::optional<LuauPreHit> preHit;
  /** @brief Why the pre_hit function failed, for the next step of the entity
   * to stop its script; nothing while it did not. */
  std::optional<std::string> preHitFailure;
};

}  // namespace rtype::game
