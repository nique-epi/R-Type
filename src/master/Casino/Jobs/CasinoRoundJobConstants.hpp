#pragma once

#include <chrono>

namespace rtype::master {

/** @brief How often the CasinoRoundJob looks for rounds to draw: a round is
 * drawn at most that long after its bets closed. */
constexpr std::chrono::milliseconds CASINO_ROUND_INTERVAL{250};

}  // namespace rtype::master
