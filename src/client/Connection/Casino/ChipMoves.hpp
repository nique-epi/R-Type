#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The chips the roulette table moved for the player since they were
 * last taken: the stakes of the rounds the wheel turned on, to take off the
 * chips, and what those rounds paid back, to add.
 */
struct ChipMoves {
  std::uint64_t staked{0};
  std::uint64_t won{0};

  bool operator==(const ChipMoves&) const = default;
};

}  // namespace rtype::client
