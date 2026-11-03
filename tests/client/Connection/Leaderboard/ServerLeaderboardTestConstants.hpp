#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

constexpr std::uint8_t ASKED_PILOTS = 2;
constexpr std::string_view ASKED_DIFFICULTY = "normal";
constexpr std::string_view OTHER_DIFFICULTY = "hard";
constexpr std::string_view TOP_NICKNAME = "KIRA";
constexpr std::uint32_t TOP_SCORE = 1000;

/** @brief As the protocol page writes them: half a second between two requests,
 * and five seconds before the client gives up. */
constexpr engine::Duration ASK_INTERVAL = std::chrono::milliseconds(500);
constexpr engine::Duration GIVE_UP_TIME = std::chrono::seconds(5);
constexpr engine::Duration SMALLEST_TIME = std::chrono::nanoseconds(1);

/** @brief Frames short enough to walk the timeline: 50 of them make the five
 * seconds. A request goes out at once, then at 0.5 s and every 0.5 s up to 4.5
 * s: 10 in all, and the 5 s itself sends none. */
constexpr engine::Duration SHORT_FRAME = std::chrono::milliseconds(100);
constexpr int FRAMES_UNTIL_GIVING_UP = 50;
constexpr std::size_t REQUESTS_BEFORE_GIVING_UP = 10;

}  // namespace rtype::client::testing
