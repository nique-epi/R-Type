#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Drifts with the sky, shakes, then charges straight ahead: the
 * Drillback.
 *
 * It drifts to the left at `drift_speed` while it is idle, shaking or
 * stunned. Once left of `enter_x` and rested (see hasRested()), it shakes for
 * `signal_ms` (attack CHARGE_ATTACK, its signal), charges to the left at its
 * full speed for `strike_ms` (its strike), then is stunned for `recover_ms`
 * (its recover), when its sheet makes it fragile (see Fragile). Its settings
 * are DRILLBACK_SETTINGS. It fires nothing.
 */
class DrillbackBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
