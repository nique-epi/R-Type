#pragma once

#include <chrono>
#include <string_view>

/** @brief The file the tests of the ban commands open in their folder. */
constexpr std::string_view BAN_COMMANDS_FILE_NAME = "ban-commands.db";

/** @brief Lines typed in the console, and the lines the commands answer. */
constexpr std::string_view BAN_ACE_FOR_TWO_HOURS_LINE = "ban ACE 2h";
constexpr std::string_view BAN_ACE_FOR_THIRTY_MINUTES_LINE = "ban ACE 30m";
constexpr std::string_view BAN_ADDRESS_FOR_GOOD_LINE = "ban 203.0.113.7";
constexpr std::string_view BAN_A_WORD_TOO_LONG_LINE = "ban ABCDEFGHIJKLMNOPQ";
constexpr std::string_view BAN_FOR_AN_UNKNOWN_LENGTH_LINE = "ban ACE 2x";
constexpr std::string_view BAN_WITH_THREE_WORDS_LINE = "ban ACE 2h 3h";
constexpr std::string_view BAN_WITHOUT_TARGET_LINE = "ban";
constexpr std::string_view UNBAN_ACE_IN_SMALL_LETTERS_LINE = "unban ace";
constexpr std::string_view BANS_LINE = "bans";
constexpr std::string_view ACE_BANNED_FOR_TWO_HOURS =
    "nickname ACE banned for 2 h";
constexpr std::string_view ADDRESS_BANNED_FOR_GOOD =
    "address 203.0.113.7 banned for good";
constexpr std::string_view ACE_UNBANNED = "nickname ace unbanned";
constexpr std::string_view ACE_WITH_ONE_HOUR_FIFTY_NINE_LEFT =
    "nickname ACE, 1 h 59 min left";
constexpr std::string_view ADDRESS_FOR_GOOD = "address 203.0.113.7, for good";
constexpr std::string_view NO_BAN_LINE = "no ban";

/** @brief A ban of two hours, and the time that passes before bans is typed:
 * 1 h 58 min 30 s are left, which round up to 1 h 59 min. */
constexpr std::chrono::seconds TWO_HOURS_OF_BAN = std::chrono::hours{2};
constexpr std::chrono::seconds A_MINUTE_AND_A_HALF{90};
constexpr std::chrono::seconds THIRTY_MINUTES_OF_BAN = std::chrono::minutes{30};
