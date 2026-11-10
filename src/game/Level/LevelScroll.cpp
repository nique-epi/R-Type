#include "LevelScroll.hpp"
#include <chrono>
#include "LevelConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

void LevelScroll::advance(engine::Duration elapsed) {
  if (paused_ || elapsed <= engine::Duration::zero()) {
    return;
  }
  travelled_ += elapsed;
}

void LevelScroll::pause() { paused_ = true; }

void LevelScroll::resume() { paused_ = false; }

void LevelScroll::returnTo(engine::Duration travelled) {
  travelled_ = travelled;
}

engine::Duration LevelScroll::travelled() const { return travelled_; }

float LevelScroll::position() const {
  return LEVEL_SCROLL_SPEED * std::chrono::duration<float>(travelled_).count();
}

float LevelScroll::speed() const { return paused_ ? 0.0F : LEVEL_SCROLL_SPEED; }

}  // namespace rtype::game
