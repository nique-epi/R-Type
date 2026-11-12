#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client {

/** @brief One round trip the client measured with its server, as it was
 * measured, before any smoothing. */
struct RoundTripSample {
  /** @brief How many round trips the client measured up to this one, from 1:
   * a reader that kept the number of the previous sample knows a new one. */
  std::uint64_t number{0};
  std::chrono::microseconds roundTrip{};

  bool operator==(const RoundTripSample&) const = default;
};

}  // namespace rtype::client
