#include "TickFigures.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>
#include "DurationSpread.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

namespace {

constexpr std::size_t WHOLE_PERCENT = 100;

/** @returns The value at the nearest rank @p percent of @p sorted, which is
 * not empty. */
engine::Duration atRank(std::span<const engine::Duration> sorted,
                        std::size_t percent) {
  const std::size_t rank =
      ((percent * sorted.size()) + WHOLE_PERCENT - 1) / WHOLE_PERCENT;
  return sorted[rank - 1];
}

/** @returns The spread of what @p member reads of each of @p samples. */
DurationSpread spreadOf(std::span<const TickSample> samples,
                        engine::Duration TickSample::* member) {
  std::vector<engine::Duration> sorted;
  sorted.reserve(samples.size());
  for (const TickSample& sample : samples) {
    sorted.push_back(sample.*member);
  }
  std::ranges::sort(sorted);
  return DurationSpread{.median = atRank(sorted, MEDIAN_PERCENT),
                        .high = atRank(sorted, HIGH_PERCENT),
                        .worst = atRank(sorted, WORST_PERCENT)};
}

/** @returns The share of the time of @p samples spent in the scripts. */
double scriptShareOf(std::span<const TickSample> samples) {
  engine::Duration whole{};
  engine::Duration scripts{};
  for (const TickSample& sample : samples) {
    whole += sample.whole;
    scripts += sample.scripts;
  }
  if (whole == engine::Duration::zero()) {
    return 0.0;
  }
  return static_cast<double>(scripts.count()) /
         static_cast<double>(whole.count());
}

}  // namespace

TickFigures figuresOf(std::span<const TickSample> samples,
                      engine::Duration budget) {
  if (samples.empty()) {
    return TickFigures{};
  }
  return TickFigures{
      .tick = spreadOf(samples, &TickSample::whole),
      .scripts = spreadOf(samples, &TickSample::scripts),
      .scriptShare = scriptShareOf(samples),
      .ticksOverBudget = static_cast<std::size_t>(
          std::ranges::count_if(samples, [budget](const TickSample& sample) {
            return sample.whole > budget;
          }))};
}

}  // namespace rtype::tools
