#pragma once

#include <cstdint>
#include "RoulettePhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Where a round of the table stands, as the wheel shows it: its
 * phase, the number drawn, meaningful from RoulettePhase::Spinning on, and
 * the time left in that phase. */
struct WheelMoment {
  network::RoulettePhase phase{network::RoulettePhase::Betting};
  std::uint8_t winningNumber{};
  engine::Duration timeLeft{};
};

}  // namespace rtype::client
