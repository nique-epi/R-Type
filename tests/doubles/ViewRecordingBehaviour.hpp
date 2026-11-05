#pragma once

#include <functional>
#include <optional>
#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

namespace rtype::game::testing {

/**
 * @brief A behaviour that asks nothing and keeps, in a view the test owns, the
 * last BehaviourView it was given, so a test reads what a behaviour sees.
 */
class ViewRecordingBehaviour final : public IBehaviour {
 public:
  explicit ViewRecordingBehaviour(std::optional<BehaviourView>& seen);

  [[nodiscard]] std::string_view name() const override;
  [[nodiscard]] std::span<const BehaviourSetting> settings() const override;
  [[nodiscard]] Intent steer(const BehaviourView& view) const override;

 private:
  std::reference_wrapper<std::optional<BehaviourView>> seen_;
};

}  // namespace rtype::game::testing
