#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game::testing {

/**
 * @brief A behaviour that writes the same Intent at every tick, so a test
 * chooses whether it starts an action, ends one, or does neither.
 */
class ActionRequestingBehaviour final : public IBehaviour {
 public:
  explicit ActionRequestingBehaviour(Intent wanted);

  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;

 private:
  Intent wanted_;
};

}  // namespace rtype::game::testing
