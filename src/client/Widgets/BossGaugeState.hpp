#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How a gauge of the boss bar shows: Waiting (phase II during phase
 * I, border of the marks), Active (border EDGE_BLUE, pink fill), Enraged
 * (border and fill RAGE_RED, hot pixels), Spent (dim and hatched). */
enum class BossGaugeState : std::uint8_t {
  Waiting,
  Active,
  Enraged,
  Spent,
};

}  // namespace rtype::client
