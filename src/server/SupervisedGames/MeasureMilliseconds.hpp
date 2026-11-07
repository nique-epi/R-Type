#pragma once

#include <cstdint>
#include <optional>

namespace rtype::server {

/** @returns @p measure, a measure a game sent in whole microseconds, in
 * milliseconds with their fraction; nothing when the game did not take it. */
[[nodiscard]] std::optional<double> millisecondsOf(
    std::optional<std::uint32_t> measure);

}  // namespace rtype::server
