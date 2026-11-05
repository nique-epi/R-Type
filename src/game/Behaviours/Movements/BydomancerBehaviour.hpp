#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Stands at the back of the playfield and casts: the Bydomancer.
 *
 * It flies left until `stand_x`, then stays, bobbing `bob` units above and
 * below its row every `bob_period_ms`. Its row is the one it appeared on until
 * it is wounded, then the row `jump` units towards the middle of the area of
 * play, where it leaps at once (Intent::jumpsTo), a single time.
 *
 * Idle and rested at its column, it casts an orb: attack ORB_ATTACK, a signal
 * of `signal_ms`, a strike of `strike_ms` during which it wants to fire its
 * Gun, then a recover of `recover_ms`. A ship closer than `nova_reach`, while
 * it is idle, makes it cast its nova instead: attack NOVA_ATTACK, a strike of
 * `nova_ms` only, whose hurt boxes its sheet times frame by frame. Its
 * settings are BYDOMANCER_SETTINGS.
 */
class BydomancerBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
