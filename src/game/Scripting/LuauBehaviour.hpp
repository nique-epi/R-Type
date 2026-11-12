#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "EngineException.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauBehaviourState.hpp"

namespace rtype::game {

/**
 * @brief A behaviour written in Luau: each body follows a HeldIntent whose
 * steps the function the script registered runs, in a Luau thread of its own,
 * both kept in the behaviour state of its Pilot, which ends with the body.
 *
 * The function receives the handle `e` of its body (see LuauEntityHandle),
 * which reads the view of the step (see pushViewTable()) and reaches the
 * components of the body. A function that returns gives the intent of that
 * tick (see intentFromTable()), and runs again from its start at the next
 * tick. A function that calls `behaviours.wait(ms, intent)` stops there: the
 * body wants that intent until the wait ends, its one-time requests (an
 * action started or ended, a jump) applying only on the first tick, and the
 * call returns the handle again, holding a fresh view, on the tick the wait
 * ends. A wait costs no step.
 *
 * A step that fails (an error of the script, its memory or its time budget),
 * or a function the body subscribed to an event or its pre_hit function that
 * failed since its last step (see LuauEventBindings and LuauPreHit), stops
 * the script of that body for good, its pre_hit function with it, and is
 * reported once for the behaviour; the body stays still and is removed from the
 * game, so a body frozen out of reach cannot hold a level open. A removal the
 * game refuses (past the removals of a tick) is asked again at the next tick.
 * The other bodies go on. Once the budget of a tick is spent, the steps due
 * wait for the next tick, the bodies wanting what they wanted. Neither copied
 * nor moved: its settings point into its own names.
 */
class LuauBehaviour final : public IBehaviour {
 public:
  explicit LuauBehaviour(LuauBehaviourParts parts);
  LuauBehaviour(const LuauBehaviour&) = delete;
  LuauBehaviour& operator=(const LuauBehaviour&) = delete;
  LuauBehaviour(LuauBehaviour&&) = delete;
  LuauBehaviour& operator=(LuauBehaviour&&) = delete;
  ~LuauBehaviour() override = default;

  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;

 private:
  [[nodiscard]] Intent resumeOrHold(LuauBehaviourState& behaviourState,
                                    const BehaviourView& view) const;
  void stopOnFailure(LuauBehaviourState& behaviourState,
                     const engine::ScriptException& error) const;
  void removeAfterFailure(const BehaviourView& view) const;

  LuauBehaviourParts parts_;
  std::vector<std::string> settingNames_;
  std::vector<BehaviourSetting> settings_;
  mutable bool failureReported_{false};
};

}  // namespace rtype::game
