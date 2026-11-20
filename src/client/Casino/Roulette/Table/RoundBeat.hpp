#pragma once

#include <cstdint>
#include "RoulettePhase.hpp"

namespace rtype::client {

/** @brief Where a round stood on a frame, as its sounds follow it: the
 * round, its phase, and whether the ball already lies in its pocket. */
struct RoundBeat {
  std::uint64_t round{};
  network::RoulettePhase phase{network::RoulettePhase::Betting};
  bool ballInPocket{};
};

}  // namespace rtype::client
