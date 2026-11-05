#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Goes along the row the body appeared on, at its full speed, the way
 * its `heading` setting says (STRAIGHT_SETTINGS), and fires as often as its Gun
 * allows: a Drifter and a missile of an enemy head left, a missile of a player
 * right.
 */
class StraightBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
