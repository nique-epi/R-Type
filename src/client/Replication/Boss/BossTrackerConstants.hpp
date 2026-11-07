#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The health of the boss splits into BOSS_PHASE_COUNT equal gauges;
 * its first gauge is empty at fullHealth / BOSS_PHASE_COUNT, the server
 * enraging it below that. */
constexpr std::uint16_t BOSS_PHASE_COUNT = 2;

/** @brief How long the points a hit took show as a lag: hits within it add
 * to the same lag. */
constexpr engine::Duration BOSS_LAG_TIME = std::chrono::milliseconds(500);

}  // namespace rtype::client
