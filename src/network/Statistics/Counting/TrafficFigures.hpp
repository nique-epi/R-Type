#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief How many things crossed a socket since the program started, and the
 * bytes they held. These totals only grow: at a gigabyte per second, 64 bits
 * last over 500 years.
 */
struct TrafficFigures {
  std::uint64_t count{0};
  std::uint64_t bytes{0};

  bool operator==(const TrafficFigures&) const = default;
};

/** @brief Adds @p part to @p total. */
constexpr void addFigures(TrafficFigures& total, const TrafficFigures& part) {
  total.count += part.count;
  total.bytes += part.bytes;
}

}  // namespace rtype::network
