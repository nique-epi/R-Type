#pragma once

#include <cstdint>
#include <optional>

namespace rtype::engine {

/**
 * @returns The simulation ticks a script waits when it asks for
 * @p milliseconds: the nearest whole number of ticks, one at least, and
 * nothing for a duration that is negative or not finite.
 */
[[nodiscard]] std::optional<std::uint32_t> scriptWaitTicks(double milliseconds);

}  // namespace rtype::engine
