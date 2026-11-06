#include "BehaviourSteering.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "BehaviourView.hpp"
#include "GameException.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns How far the body of @p view moves on one axis during this tick at
 * a full share; 0 when it cannot move. */
float reachOf(const BehaviourView& view) {
  return view.topSpeed * std::chrono::duration<float>(view.elapsed).count();
}

}  // namespace

float verticalShareTowards(float height, const BehaviourView& view) {
  const float reach = reachOf(view);
  if (reach <= 0.0F) {
    return 0.0F;
  }
  return (height - view.position.y) / reach;
}

float horizontalShareTowards(float column, const BehaviourView& view) {
  const float reach = reachOf(view);
  if (reach <= 0.0F) {
    return 0.0F;
  }
  return (column - view.position.x) / reach;
}

std::int64_t settingAt(const BehaviourView& view, std::size_t index) {
  if (index >= view.settings.size()) {
    throw MissingBehaviourSettingException(index);
  }
  return view.settings[index];
}

engine::Duration millisecondsSettingAt(const BehaviourView& view,
                                       std::size_t index) {
  return std::chrono::milliseconds{settingAt(view, index)};
}

}  // namespace rtype::game
