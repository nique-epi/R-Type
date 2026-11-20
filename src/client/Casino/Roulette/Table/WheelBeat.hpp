#pragma once

#include <cstdint>
#include "RoulettePhase.hpp"

namespace rtype::client {

/** @brief Where the wheel of a round stood on a frame, as its sounds follow
 * it: the round, its phase, and whether the ball already lies in its
 * pocket. */
struct WheelBeat {
  std::uint64_t round{};
  network::RoulettePhase phase{network::RoulettePhase::Betting};
  bool ballInPocket{};
};

}  // namespace rtype::client
