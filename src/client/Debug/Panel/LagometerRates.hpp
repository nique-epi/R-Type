#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns @p count over @p period, per second, rounded to the nearest;
 * @p period is never zero. */
[[nodiscard]] std::uint64_t perSecond(std::uint64_t count,
                                      engine::Duration period);

/** @returns @p delayTicks of the server as a duration, to the microsecond;
 * nothing without it. */
[[nodiscard]] std::optional<std::chrono::microseconds> displayDelayOf(
    std::optional<double> delayTicks);

}  // namespace rtype::client
