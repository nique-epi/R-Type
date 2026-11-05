#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Hunts the nearest ship and swings a scythe at it: the Reaper.
 *
 * Idle, it flies towards the point `reach` logical units to the right of the
 * nearest ship, at its height; with no ship in play it crosses the playfield
 * to the left. Once within `align` units of that point on each axis, and
 * rested (see hasRested()), it stops and swings: attack SCYTHE_ATTACK, a
 * signal of `signal_ms`, a strike of `strike_ms` and a recover of
 * `recover_ms` (see ActionCycle), still the whole time. The hurt boxes of its
 * sheet follow the frames of the strike. Its settings are REAPER_SETTINGS. It
 * fires nothing.
 */
class ReaperBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
