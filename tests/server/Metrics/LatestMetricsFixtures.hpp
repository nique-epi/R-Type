#pragma once

#include <cstddef>
#include "MetricsReport.hpp"

namespace rtype::server::testing {

/** @returns A report whose players and entities are both @p count, and that
 * holds @p count % MOST_SYSTEM_TIMES system times, so a reader can tell a whole
 * report from a torn one. */
[[nodiscard]] MetricsReport reportCounting(std::size_t count);

}  // namespace rtype::server::testing
