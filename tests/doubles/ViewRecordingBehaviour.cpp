#include "ViewRecordingBehaviour.hpp"
#include <optional>
#include <span>
#include <string_view>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"

namespace rtype::game::testing {

namespace {

constexpr std::string_view VIEW_RECORDING_NAME = "view_recording";

}  // namespace

ViewRecordingBehaviour::ViewRecordingBehaviour(
    std::optional<BehaviourView>& seen)
    : seen_(seen) {}

std::string_view ViewRecordingBehaviour::name() const {
  return VIEW_RECORDING_NAME;
}

std::span<const BehaviourSetting> ViewRecordingBehaviour::settings() const {
  return {};
}

Intent ViewRecordingBehaviour::steer(const BehaviourView& view) const {
  seen_.get() = view;
  return Intent{.steered = true};
}

}  // namespace rtype::game::testing
