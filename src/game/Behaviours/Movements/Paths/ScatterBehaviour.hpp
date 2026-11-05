#pragma once

#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Flies straight away from the point its body steers from, at its full
 * speed: the shards of a burst appear a little away from the center of their
 * projectile (see World::spawnFrom) and each flies on along its own line. A
 * body still on that point flies left, the way of the missiles of an enemy.
 * It has no setting and fires nothing.
 */
class ScatterBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
