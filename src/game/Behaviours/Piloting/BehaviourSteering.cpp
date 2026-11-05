#include "BehaviourSteering.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "BehaviourView.hpp"
#include "GameException.hpp"

namespace rtype::game {

float verticalShareTowards(float height, const BehaviourView& view) {
  const float reach =
      view.topSpeed * std::chrono::duration<float>(view.elapsed).count();
  if (reach <= 0.0F) {
    return 0.0F;
  }
  return (height - view.position.y) / reach;
}

std::int64_t settingAt(const BehaviourView& view, std::size_t index) {
  if (index >= view.settings.size()) {
    throw MissingBehaviourSettingException(index);
  }
  return view.settings[index];
}

}  // namespace rtype::game
