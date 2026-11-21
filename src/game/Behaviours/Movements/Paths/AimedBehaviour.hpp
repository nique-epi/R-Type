#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Flies straight at where the nearest ship was on the first tick of the
 * body, at its full speed: a shot an enemy aims at a player.
 *
 * On its first tick the body takes the heading towards the nearest ship and
 * keeps it in its behaviour state (AimedHeading), so it never follows the
 * ship afterwards. It flies left when it sees no ship on that tick, or one
 * that is not left of it, since an enemy shot never turns back towards its
 * shooter. A view with nowhere to keep the heading (no behaviourState) is
 * aimed again at each tick. It has no setting, and fires as often as its Gun
 * allows, as the straight behaviour does.
 */
class AimedBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
