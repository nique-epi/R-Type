#pragma once

#include "TimeConstants.hpp"

namespace rtype::engine::testing {

/**
 * @brief A path whose two ways last the same, to a server whose tick 0 stood
 * at a given instant of the clock of the client.
 */
struct SymmetricPath {
  /** @brief The instant of tick 0 of the server, on the clock of the client. */
  Duration tickZero{};
  /** @brief The round trip, half of it for each way. */
  Duration roundTrip{};
};

}  // namespace rtype::engine::testing
