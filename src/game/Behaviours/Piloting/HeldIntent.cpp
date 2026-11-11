#include "HeldIntent.hpp"
#include <cstdint>
#include <optional>
#include "Intent.hpp"

namespace rtype::game {

namespace {

/** @returns @p intent without what applies once: an action started or ended,
 * a jump, and an effect played. */
Intent withoutOneTimeRequests(Intent intent) {
  intent.startsAction = std::nullopt;
  intent.endsAction = false;
  intent.jumpsTo = std::nullopt;
  intent.playsEffect = std::nullopt;
  return intent;
}

}  // namespace

std::optional<Intent> HeldIntent::heldThisTick() {
  if (stopped_) {
    return Intent{.steered = true};
  }
  if (ticksLeft_ > 1) {
    --ticksLeft_;
    return held_;
  }
  return std::nullopt;
}

Intent HeldIntent::held() const { return held_; }

Intent HeldIntent::startHolding(const Intent& intent, std::uint32_t waitTicks) {
  ticksLeft_ = waitTicks;
  held_ = withoutOneTimeRequests(intent);
  return intent;
}

void HeldIntent::stop() {
  stopped_ = true;
  ticksLeft_ = 0;
  held_ = Intent{.steered = true};
}

bool HeldIntent::stopped() const { return stopped_; }

}  // namespace rtype::game
