#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief The final boss: the Bydo Tyrant.
 *
 * Until it ends its first action it arrives: attack TYRANT_ARRIVAL_ATTACK,
 * sliding in from where it appeared to `rest_x` on its row over `slide_ms`
 * (its signal), then roaring `roar_ms` (its strike), which its
 * `[gameplay.boss]` shields it during. Then, rested `rest_ms`, it lashes its
 * tentacles at a ship within `melee_reach` of its melee point, or takes the
 * next attack of its rotation, calm or enraged (see
 * BydoTyrantBehaviourConstants.hpp). An enraged variant signals
 * `rage_signal_percent` of the calm one. In the phase Enraged it holds still
 * `rage_ms`, then ends that action. In the strike of its charge it crosses
 * the playfield to the left, leaps past the right edge and comes back to
 * `rest_x`. It never shoots: its missiles and Bydos are the volleys of its
 * sheet. Its settings are BYDO_TYRANT_SETTINGS.
 */
class BydoTyrantBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
