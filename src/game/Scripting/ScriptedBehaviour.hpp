#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "IBehaviourMemory.hpp"
#include "Intent.hpp"
#include "ScriptedBehaviourParts.hpp"
#include "ScriptedRoutine.hpp"

namespace rtype::game {

/**
 * @brief A behaviour written in Luau: each body runs the function the script
 * registered in a thread of its own, kept in the memory of its Pilot, which
 * ends with the body.
 *
 * A function that returns gives the intent of that tick (see
 * pushBehaviourView() and intentAt()), and runs again from its start at the
 * next tick. A function that calls `behaviours.wait(ms, intent)` stops there:
 * the body wants that intent until the wait ends, its one-time requests (an
 * action started or ended, a jump) applying only on the first tick, and the
 * call returns a fresh view on the tick the wait ends. A wait costs no step.
 *
 * A step that fails (an error of the script, its memory or its time budget)
 * stops the script of that body for good and is reported once for the
 * behaviour; the body stays still and is removed from the game, so a body
 * frozen out of reach cannot hold a level open. A removal the game refuses
 * (past the removals of a tick) is asked again at the next tick. The other
 * bodies go on. Once the budget of
 * a tick is spent, the steps due wait for the next tick, the bodies wanting
 * what they wanted. Neither copied nor moved: its settings point into its own
 * names.
 */
class ScriptedBehaviour final : public IBehaviour {
 public:
  explicit ScriptedBehaviour(ScriptedBehaviourParts parts);
  ScriptedBehaviour(const ScriptedBehaviour&) = delete;
  ScriptedBehaviour& operator=(const ScriptedBehaviour&) = delete;
  ScriptedBehaviour(ScriptedBehaviour&&) = delete;
  ScriptedBehaviour& operator=(ScriptedBehaviour&&) = delete;
  ~ScriptedBehaviour() override = default;

  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;

 private:
  [[nodiscard]] Intent stepOf(ScriptedRoutine& routine,
                              const BehaviourView& view) const;
  void removeStopped(const BehaviourView& view) const;

  ScriptedBehaviourParts parts_;
  std::vector<std::string> settingNames_;
  std::vector<BehaviourSetting> settings_;
  mutable bool failureReported_{false};
};

}  // namespace rtype::game
