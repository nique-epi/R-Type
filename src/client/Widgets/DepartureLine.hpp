#pragma once

#include <cstdint>
#include <string>
#include "PilotStatus.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A line of the feed of departures. */
struct DepartureLine {
  std::uint8_t playerNumber{};
  /** @brief The name the pilot is shown by (see pilotNameOf()). */
  std::string name;
  /** @brief Left or SignalLost. */
  PilotState state{};
  /** @brief How long the line has been shown. */
  engine::Duration age{};
};

}  // namespace rtype::client
