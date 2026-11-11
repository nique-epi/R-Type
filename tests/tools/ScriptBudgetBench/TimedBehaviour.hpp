#pragma once

#include <functional>
#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What a TimedBehaviour wraps, and where it adds the time spent. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct TimedBehaviourParts {
  std::reference_wrapper<const game::IBehaviour> behaviour;
  std::reference_wrapper<engine::Duration> spent;
};

/**
 * @brief A behaviour that steers as the one it wraps, and adds the time each
 * call takes to a total its owner reads: the share of the scripts in a tick
 * is what the bodies they pilot spend in it, garbage collection included.
 */
class TimedBehaviour final : public game::IBehaviour {
 public:
  explicit TimedBehaviour(TimedBehaviourParts parts);

  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const game::BehaviourSetting> settings()
      const override;
  [[nodiscard]] game::Intent steer(
      const game::BehaviourView& view) const override;

 private:
  TimedBehaviourParts parts_;
};

}  // namespace rtype::tools
