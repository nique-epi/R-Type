#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How long the number of a GameEvent is remembered: its three copies
 * come 50 ms apart, and the protocol page says a second is enough. */
constexpr engine::Duration GAME_EVENT_MEMORY = std::chrono::seconds(1);

}  // namespace rtype::client
