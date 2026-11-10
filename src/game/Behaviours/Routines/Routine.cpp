#include "Routine.hpp"
#include <cstdint>
#include <optional>
#include "Intent.hpp"

namespace rtype::game {

namespace {

/** @returns @p intent without what applies once: an action started or ended,
 * and a jump. */
Intent withoutOneTimeRequests(Intent intent) {
  intent.startsAction = std::nullopt;
  intent.endsAction = false;
  intent.jumpsTo = std::nullopt;
  return intent;
}

}  // namespace

std::optional<Intent> Routine::heldThisTick() {
  if (stopped_) {
    return Intent{.steered = true};
  }
  if (ticksLeft_ > 1) {
    --ticksLeft_;
    return held_;
  }
  return std::nullopt;
}

Intent Routine::held() const { return held_; }

Intent Routine::stepped(const Intent& intent, std::uint32_t waitTicks) {
  ticksLeft_ = waitTicks;
  held_ = withoutOneTimeRequests(intent);
  return intent;
}

void Routine::stop() {
  stopped_ = true;
  ticksLeft_ = 0;
  held_ = Intent{.steered = true};
}

bool Routine::stopped() const { return stopped_; }

}  // namespace rtype::game
