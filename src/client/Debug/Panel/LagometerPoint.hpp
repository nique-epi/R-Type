#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client {

/** @brief One point of the curve: a round trip as measured, and the datagrams
 * of the server accepted and missed since the point before it. */
struct LagometerPoint {
  std::chrono::microseconds roundTrip{};
  std::uint64_t accepted{0};
  std::uint64_t missed{0};

  bool operator==(const LagometerPoint&) const = default;
};

}  // namespace rtype::client
