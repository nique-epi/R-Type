#include "InterpolationTally.hpp"
#include <cstdint>
#include <optional>
#include "ClockRecalibrations.hpp"

namespace rtype::client {

void InterpolationTally::startGame() {
  newestTick_.reset();
  delayTicks_.reset();
  ended_ = false;
}

void InterpolationTally::endGame() {
  newestTick_.reset();
  delayTicks_.reset();
  ended_ = true;
}

void InterpolationTally::observe(std::uint32_t tick) {
  if (ended_) {
    return;
  }
  if (!newestTick_.has_value() || tick > *newestTick_) {
    newestTick_ = tick;
  }
}

void InterpolationTally::countFrame(std::optional<double> renderTick,
                                    std::optional<double> serverTick) {
  if (!renderTick.has_value() || !newestTick_.has_value()) {
    return;
  }
  if (*renderTick > static_cast<double>(*newestTick_)) {
    ++pastTheNewest_;
  }
  if (serverTick.has_value()) {
    delayTicks_ = *serverTick - *renderTick;
  }
}

void InterpolationTally::countRecalibration(double jumpTicks) {
  if (ended_) {
    return;
  }
  ++recalibrations_.count;
  recalibrations_.ticks += jumpTicks;
}

ClockRecalibrations InterpolationTally::recalibrations() const {
  return recalibrations_;
}

std::uint64_t InterpolationTally::pastTheNewest() const {
  return pastTheNewest_;
}

std::optional<double> InterpolationTally::delayTicks() const {
  return delayTicks_;
}

}  // namespace rtype::client
