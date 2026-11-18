#pragma once

#include <chrono>
#include <functional>
#include "MillisecondDate.hpp"

namespace rtype::master {

/** @brief The clock the CasinoRoundJob reads, and how long it sleeps between
 * two passes: the system clock and CASINO_ROUND_INTERVAL, a driven clock in
 * the tests. */
struct RoundClock {
  std::function<network::MillisecondDate()> now;
  std::chrono::milliseconds interval{};
};

}  // namespace rtype::master
