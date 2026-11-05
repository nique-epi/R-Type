#include "BehaviourSteering.hpp"
#include <chrono>
#include "BehaviourView.hpp"

namespace rtype::game {

float verticalShareTowards(float height, const BehaviourView& view) {
  const float reach =
      view.topSpeed * std::chrono::duration<float>(view.elapsed).count();
  if (reach <= 0.0F) {
    return 0.0F;
  }
  return (height - view.position.y) / reach;
}

}  // namespace rtype::game
