#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The frames a game drew of the other entities, and those among them
 * that showed a tick past the newest state received, in totals that only
 * grow. */
struct InterpolationCounts {
  std::uint64_t frames{0};
  /** @brief Frames whose tick lay past the newest state: the entities were
   * moved on from their last two states, or held there once
   * MAXIMUM_EXTRAPOLATION ran out. */
  std::uint64_t pastTheNewest{0};

  bool operator==(const InterpolationCounts&) const = default;
};

}  // namespace rtype::client
