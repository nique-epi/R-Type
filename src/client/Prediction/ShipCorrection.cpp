#include "ShipCorrection.hpp"
#include <cmath>
#include "Position.hpp"
#include "PredictionConstants.hpp"

namespace rtype::client {

namespace {

float lengthOf(game::Position gap) { return std::hypot(gap.x, gap.y); }

}  // namespace

void ShipCorrection::absorb(game::Position shown, game::Position predicted) {
  const game::Position gap{.x = shown.x - predicted.x,
                           .y = shown.y - predicted.y};
  gap_ = lengthOf(gap) > SNAP_DISTANCE ? game::Position{} : gap;
}

void ShipCorrection::decay() {
  gap_ = game::Position{.x = gap_.x * CORRECTION_KEPT_PER_TICK,
                        .y = gap_.y * CORRECTION_KEPT_PER_TICK};
  if (lengthOf(gap_) < CORRECTION_REST) {
    clear();
  }
}

void ShipCorrection::clear() { gap_ = game::Position{}; }

game::Position ShipCorrection::shownFor(game::Position predicted) const {
  return game::Position{.x = predicted.x + gap_.x, .y = predicted.y + gap_.y};
}

}  // namespace rtype::client
