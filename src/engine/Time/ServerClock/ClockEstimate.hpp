#pragma once

#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief What one ClockSample gives: the instant, on the clock of the client,
 * of tick 0 of the server, and the round trip it was read with, which says
 * how far to trust it.
 */
struct ClockEstimate {
  Duration roundTrip{};
  Duration tickZero{};
};

}  // namespace rtype::engine
