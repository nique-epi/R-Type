#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The least time between two lines the server relays for one player:
 * Quake III takes one command a second from each client. A line sooner is
 * ignored, and its client sends it again (see the protocol page), so it is
 * delayed, not lost. Provisional, see the protocol page.
 */
constexpr engine::Duration CHAT_LINE_INTERVAL = std::chrono::seconds(1);

/**
 * @brief CHAT_LINE_INTERVAL, in whole simulation ticks.
 */
constexpr std::size_t TICKS_BETWEEN_CHAT_LINES =
    engine::wholeTicksIn(CHAT_LINE_INTERVAL);

}  // namespace rtype::server
