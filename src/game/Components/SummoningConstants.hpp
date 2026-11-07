#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief How long an enemy a volley summons stays harmless before it takes
 * part: the server counts it in ticks (see Summoning), the client shows the
 * warning of its arrival over the same time. */
constexpr engine::Duration SUMMONING_TIME = std::chrono::milliseconds(1000);

}  // namespace rtype::game
