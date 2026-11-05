#include "ActionRequestingBehaviour.hpp"
#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"

namespace rtype::game::testing {

namespace {

constexpr std::string_view ACTION_REQUESTING_NAME = "action_requesting";

}  // namespace

ActionRequestingBehaviour::ActionRequestingBehaviour(Intent wanted)
    : wanted_(wanted) {}

std::string_view ActionRequestingBehaviour::name() const {
  return ACTION_REQUESTING_NAME;
}

std::span<const BehaviourSetting> ActionRequestingBehaviour::settings() const {
  return {};
}

Intent ActionRequestingBehaviour::steer(
    [[maybe_unused]] const BehaviourView& view) const {
  return wanted_;
}

}  // namespace rtype::game::testing
