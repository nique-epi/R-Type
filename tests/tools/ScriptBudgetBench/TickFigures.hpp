#pragma once

#include <cstddef>
#include <span>
#include "DurationSpread.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What the bench reports of a run. */
struct TickFigures {
  DurationSpread tick;
  DurationSpread scripts;
  /** @brief The share of the time of all the ticks spent in the scripts, from
   * 0 to 1. */
  double scriptShare{0.0};
  /** @brief The ticks that took longer than the budget. */
  std::size_t ticksOverBudget{0};
};

/**
 * @returns The figures of @p samples: each rank is the nearest rank, so the
 * median of an even count is the lower of the two middle values. A tick that
 * takes exactly @p budget is within it. No sample gives figures of zero.
 */
[[nodiscard]] TickFigures figuresOf(std::span<const TickSample> samples,
                                    engine::Duration budget);

}  // namespace rtype::tools
