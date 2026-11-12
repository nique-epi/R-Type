#include "PositionHistory.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iterator>
#include <optional>
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
  if (!samples_.empty() && sample.tick <= samples_.back().tick) {
    return;
  }
  samples_.push_back(sample);
  while (samples_.size() > 2 &&
         samples_[1].tick + HISTORY_SPAN_TICKS <= sample.tick) {
    samples_.pop_front();
  }
}

std::optional<game::Position> PositionHistory::at(double tick) const {
  if (samples_.empty()) {
    return std::nullopt;
  }
  const std::deque<PositionSample>& kept = samples_;
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
  if (kept.size() == 1) {
    return newest.position;
  }
  const PositionSample& previous = kept[kept.size() - 2];
  const double ahead = std::min(tick - static_cast<double>(newest.tick),
                                MAXIMUM_EXTRAPOLATION_TICKS);
  return moved(
      previous, newest,
      1.0 + (ahead / static_cast<double>(newest.tick - previous.tick)));
}

std::optional<std::uint32_t> PositionHistory::oldestTick() const {
  if (samples_.empty()) {
    return std::nullopt;
  }
  return samples_.front().tick;
}

}  // namespace rtype::client
