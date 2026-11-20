#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the plaque under the croupier shows while the bets are open:
 * the seconds begun before the wheel spins, the share of the bar still full,
 * from 0 to 1, and whether the last seconds have come, painted red. */
struct SpinCountdown {
  std::int64_t seconds{};
  float share{};
  bool hurry{};
};

}  // namespace rtype::client
