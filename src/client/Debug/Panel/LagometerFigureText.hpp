#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

namespace rtype::client {

/** @returns @p figure in decimal, or NO_FIGURE_TEXT without one. */
[[nodiscard]] std::string figureText(std::optional<std::uint64_t> figure);

/** @returns @p duration in whole milliseconds, rounded to the nearest. */
[[nodiscard]] std::uint64_t wholeMilliseconds(
    std::chrono::microseconds duration);

}  // namespace rtype::client
