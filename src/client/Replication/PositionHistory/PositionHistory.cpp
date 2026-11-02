#include "PositionHistory.hpp"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <optional>
#include <span>
#include "Position.hpp"
#include "PositionHistoryConstants.hpp"
#include "PositionSample.hpp"

namespace rtype::client {

namespace {

/** @returns @p before moved towards @p after by @p share: 0 stays at
 * @p before, 1 reaches @p after, more goes on past it. */
game::Position moved(const PositionSample& before, const PositionSample& after,
                     double share) {
  const auto along = static_cast<float>(share);
  return game::Position{
      .x = std::lerp(before.position.x, after.position.x, along),
      .y = std::lerp(before.position.y, after.position.y, along)};
}

/** @returns How far @p tick lies from @p before towards @p after, in whole
 * spans between them: 0 at @p before, 1 at @p after. */
double shareOf(const PositionSample& before, const PositionSample& after,
               double tick) {
  return (tick - static_cast<double>(before.tick)) /
         static_cast<double>(after.tick - before.tick);
}

}  // namespace

void PositionHistory::record(PositionSample sample) {
  if (count_ > 0 && sample.tick <= samples_[count_ - 1].tick) {
    return;
  }
  if (count_ == samples_.size()) {
    std::ranges::rotate(samples_, std::next(samples_.begin()));
    --count_;
  }
  samples_[count_] = sample;
  ++count_;
}

std::optional<game::Position> PositionHistory::at(double tick) const {
  if (count_ == 0) {
    return std::nullopt;
  }
  const std::span<const PositionSample> kept{samples_.data(), count_};
  const auto after =
      std::ranges::find_if(kept, [tick](const PositionSample& sample) {
        return static_cast<double>(sample.tick) > tick;
      });
  if (after == kept.begin()) {
    return kept.front().position;
  }
  if (after != kept.end()) {
    const PositionSample& before = *std::prev(after);
    return moved(before, *after, shareOf(before, *after, tick));
  }
  const PositionSample& newest = kept.back();
  if (count_ == 1) {
    return newest.position;
  }
  const PositionSample& previous = kept[count_ - 2];
  const double ahead = std::min(tick - static_cast<double>(newest.tick),
                                MAXIMUM_EXTRAPOLATION_TICKS);
  return moved(
      previous, newest,
      1.0 + (ahead / static_cast<double>(newest.tick - previous.tick)));
}

}  // namespace rtype::client
