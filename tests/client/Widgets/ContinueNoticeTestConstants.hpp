#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

/** @brief Continues a team holds: all of them, 2, 1 and none. */
constexpr std::uint8_t EVERY_CONTINUE = 3;
constexpr std::uint8_t TWO_CONTINUES = 2;
constexpr std::uint8_t ONE_CONTINUE = 1;
constexpr std::uint8_t NO_CONTINUE = 0;

/** @brief When the first status came and when a continue was spent, since the
 * start of the game, and how long the notice shows: 2 s. */
constexpr std::chrono::nanoseconds FIRST_STATUS_AT{0};
constexpr std::chrono::nanoseconds CONTINUE_SPENT_AT = std::chrono::seconds(30);
constexpr std::chrono::nanoseconds CONTINUE_NOTICE_SHOWN =
    std::chrono::seconds(2);

/** @brief The line of the notice for 2, 1 and no continue left. */
constexpr std::string_view TWO_CONTINUES_LINE = "CHECKPOINT - 2 CONTINUES LEFT";
constexpr std::string_view ONE_CONTINUE_LINE = "CHECKPOINT - 1 CONTINUE LEFT";
constexpr std::string_view NO_CONTINUE_LINE = "CHECKPOINT - NO CONTINUE LEFT";
