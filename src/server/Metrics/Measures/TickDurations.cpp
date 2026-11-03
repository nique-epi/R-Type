#include "TickDurations.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include "DurationConversion.hpp"
#include "MetricsConstants.hpp"
#include "ServerConstants.hpp"
#include "TickDurationSummary.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The duration at the nearest rank for @p percent: the one of rank @p
 * percent / 100 * size, rounded up, counting from 1.
 * @param sorted In ascending order, never empty.
 */
engine::Duration durationAtPercent(std::span<const engine::Duration> sorted,
                                   std::size_t percent) {
  const std::size_t rank =
      ((percent * sorted.size()) + WHOLE_PERCENT - 1) / WHOLE_PERCENT;
  return sorted[rank - 1];
}

/** @param sorted In ascending order, never empty. */
TickDurationSummary summaryOf(std::span<const engine::Duration> sorted) {
  engine::Duration total = engine::Duration::zero();
  for (const engine::Duration duration : sorted) {
    total += duration;
  }
  return TickDurationSummary{
      .count = sorted.size(),
      .average = engine::averageOf(total, sorted.size()),
      .median = durationAtPercent(sorted, MEDIAN_PERCENT),
      .ninetyFifthPercentile = durationAtPercent(sorted, NINETY_FIFTH_PERCENT),
      .worst = sorted.back()};
}

}  // namespace

TickDurations::TickDurations() { samples_.reserve(TICKS_PER_REPORT); }

void TickDurations::record(engine::Duration duration) {
  if (!isFull()) {
    samples_.push_back(duration);
  }
}

bool TickDurations::isFull() const {
  return samples_.size() >= TICKS_PER_REPORT;
}

TickDurationSummary TickDurations::takeSummary() {
  if (samples_.empty()) {
    return TickDurationSummary{};
  }
  std::ranges::sort(samples_);
  const TickDurationSummary summary = summaryOf(samples_);
  samples_.clear();
  return summary;
}

}  // namespace rtype::server
