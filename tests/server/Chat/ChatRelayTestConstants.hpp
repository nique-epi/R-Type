#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The seats of the tests, from 0. */
constexpr std::size_t FIRST_SEAT = 0;
constexpr std::size_t SECOND_SEAT = 1;

/** @brief 1 s at 60 ticks a second is 60 ticks: 59 is one short of the interval
 * of the protocol page. */
constexpr std::size_t TICKS_OF_THE_CHAT_INTERVAL = 60;
constexpr std::size_t TICKS_ONE_SHORT_OF_THE_CHAT_INTERVAL =
    TICKS_OF_THE_CHAT_INTERVAL - 1;

/** @brief Line numbers: one, the one after it, one before it, and the last
 * before the wrap with the first after it. */
constexpr std::uint16_t SOME_LINE_NUMBER = 7;
constexpr std::uint16_t NEXT_LINE_NUMBER = 8;
constexpr std::uint16_t EARLIER_LINE_NUMBER = 6;
constexpr std::uint16_t LAST_LINE_NUMBER_BEFORE_THE_WRAP = 65535;
constexpr std::uint16_t FIRST_LINE_NUMBER_AFTER_THE_WRAP = 0;
constexpr std::uint16_t FIRST_LINE_NUMBER = 1;

constexpr std::string_view SOME_TEXT = "gg";

}  // namespace rtype::server::testing
