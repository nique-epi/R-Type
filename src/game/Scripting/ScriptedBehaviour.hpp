#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "ScriptedBehaviourParts.hpp"

namespace rtype::game {

/**
 * @brief A behaviour written in Luau: steer() hands its view to the function
 * the script registered and reads the intent it returns (see
 * pushBehaviourView() and intentAt()).
 *
 * A call that fails (an error of the script, its memory or its time budget)
 * leaves the body still for that tick, and is reported once through the
 * report of its parts. It keeps no state of its own, as every behaviour: one
 * script pilots every body that names it. Neither copied nor moved: its
 * settings point into its own names.
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
  [[nodiscard]] Intent steerByScript(const BehaviourView& view) const;

  ScriptedBehaviourParts parts_;
  std::vector<std::string> settingNames_;
  std::vector<BehaviourSetting> settings_;
  mutable bool failureReported_{false};
};

}  // namespace rtype::game
