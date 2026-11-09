#include "TimedBehaviour.hpp"
#include <chrono>
#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

TimedBehaviour::TimedBehaviour(TimedBehaviourParts parts) : parts_(parts) {}

std::string_view TimedBehaviour::name() const {
  return parts_.behaviour.get().name();
}

std::span<const game::BehaviourSetting> TimedBehaviour::settings() const {
  return parts_.behaviour.get().settings();
}

game::Intent TimedBehaviour::steer(const game::BehaviourView& view) const {
  const auto start = std::chrono::steady_clock::now();
  const game::Intent intent = parts_.behaviour.get().steer(view);
  parts_.spent.get() += std::chrono::duration_cast<engine::Duration>(
      std::chrono::steady_clock::now() - start);
  return intent;
}

}  // namespace rtype::tools
