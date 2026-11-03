#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "LeaderboardFilterChoice.hpp"

namespace rtype::client::testing {

/** @brief The score of an entry, which no test here reads. */
constexpr std::uint32_t ENTRY_SCORE = 1000;

/** @brief 18 October 2026 at 12:00 UTC, and the first second of 1970. */
constexpr std::int64_t OCTOBER_18_2026_SECONDS = 1792324800;
constexpr std::string_view OCTOBER_18_2026_TEXT = "18 OCT 26";
constexpr std::int64_t FIRST_SECOND = 0;
constexpr std::string_view FIRST_SECOND_TEXT = "01 JAN 70";

/** @brief The last second a year of two digits tells: 2099-12-31 23:59:59 UTC.
 */
constexpr std::int64_t LAST_SECOND = 4102444799;
constexpr std::string_view LAST_SECOND_TEXT = "31 DEC 99";
constexpr std::string_view UNKNOWN_TEXT = "-- --- --";

constexpr LeaderboardFilterChoice NORMAL_TWO_PILOTS{.difficulty = 1,
                                                    .pilots = 1};
constexpr LeaderboardFilterChoice HARD_ONE_PILOT{.difficulty = 2, .pilots = 0};

/** @brief The widest row: 16 characters of 8 pixels, YOU as silkscreen.ttf
 * draws it at 8 pixels (875, 750 and 750 units of 1000), and 7 digits of 8
 * pixels. */
constexpr int LONGEST_NICKNAME_WIDTH = 128;
constexpr int YOU_WIDTH_IN_SMALL_FONT = 19;
constexpr int SEVEN_DIGITS_WIDTH = 56;
constexpr std::size_t NAME_COLUMN = 1;
constexpr std::size_t SCORE_COLUMN = 2;

}  // namespace rtype::client::testing
