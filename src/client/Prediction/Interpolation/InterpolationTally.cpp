#include "InterpolationTally.hpp"
#include <cstdint>
#include <optional>
#include "InterpolationCounts.hpp"

namespace rtype::client {

void InterpolationTally::startGame() {
  newestTick_.reset();
  delayTicks_.reset();
}

void InterpolationTally::observe(std::uint32_t tick) {
  if (!newestTick_.has_value() || tick > *newestTick_) {
    newestTick_ = tick;
  }
}

void InterpolationTally::countFrame(std::optional<double> renderTick,
                                    std::optional<double> serverTick) {
  if (!renderTick.has_value() || !newestTick_.has_value()) {
    return;
  }
  ++counts_.frames;
  if (*renderTick > static_cast<double>(*newestTick_)) {
    ++counts_.pastTheNewest;
  }
  if (serverTick.has_value()) {
    delayTicks_ = *serverTick - *renderTick;
  }
}

InterpolationCounts InterpolationTally::counts() const { return counts_; }

std::optional<double> InterpolationTally::delayTicks() const {
  return delayTicks_;
}

}  // namespace rtype::client
