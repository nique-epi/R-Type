#include "InterpolationTally.hpp"
#include <cstdint>
#include <optional>

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
  if (*renderTick > static_cast<double>(*newestTick_)) {
    ++pastTheNewest_;
  }
  if (serverTick.has_value()) {
    delayTicks_ = *serverTick - *renderTick;
  }
}

std::uint64_t InterpolationTally::pastTheNewest() const {
  return pastTheNewest_;
}

std::optional<double> InterpolationTally::delayTicks() const {
  return delayTicks_;
}

}  // namespace rtype::client
