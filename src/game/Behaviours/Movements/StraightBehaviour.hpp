#pragma once

#include <string_view>
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game {

/**
 * @brief Crosses the playfield towards the ships on the row the body appeared
 * on, at its full speed, and fires as often as its Gun allows.
 */
class StraightBehaviour final : public IBehaviour {
 public:
  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;
};

}  // namespace rtype::game
