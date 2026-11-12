#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How often a state received moved the tick the other entities are
 * shown at, and how far in all: each move is a jump of the entities along
 * their path, on top of the time of the frame. */
struct ClockRecalibrations {
  /** @brief The states that moved the tick shown. */
  std::uint64_t count{0};
  /** @brief The size of those moves added up, in server ticks, each counted
   * without its sign. */
  double ticks{0.0};

  bool operator==(const ClockRecalibrations&) const = default;
};

}  // namespace rtype::client
