#include "FrameLoop.hpp"
#include "Application.hpp"
#include "IClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

FrameLoop::FrameLoop(const engine::IClock& clock) : clock_(&clock) {}

void FrameLoop::run(const engine::Application& application) {
  previousFrameTime_ = clock_->now();
  while (!stopped_) {
    application.runStages(measureFrame());
  }
}

void FrameLoop::stop() { stopped_ = true; }

engine::Duration FrameLoop::measureFrame() {
  const engine::Duration now = clock_->now();
  const engine::Duration elapsed = now - previousFrameTime_;
  previousFrameTime_ = now;
  return elapsed;
}

}  // namespace rtype::client
