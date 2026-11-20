#include "LevelTransitionScene.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

void LevelTransitionScene::show(std::uint16_t millisecondsLeft) {
  left_ = std::chrono::milliseconds(millisecondsLeft);
}

void LevelTransitionScene::advance(engine::Duration elapsed) {
  left_ = std::max(engine::Duration::zero(), left_ - elapsed);
}

engine::Duration LevelTransitionScene::left() const { return left_; }

bool LevelTransitionScene::holdsTheShips() const {
  return left_ > engine::Duration::zero();
}

}  // namespace rtype::client
