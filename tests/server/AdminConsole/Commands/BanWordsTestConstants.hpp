#pragma once

#include <chrono>
#include <string_view>

/** @brief Words of a ban line: an address, a text that only reads as a
 * nickname, and a word one byte too long to be a nickname. */
constexpr std::string_view WORD_OF_AN_ADDRESS = "1.2.3.4";
constexpr std::string_view ADDRESS_WITH_A_LEADING_ZERO = "01.2.3.4";
constexpr std::string_view ADDRESS_PAST_255 = "999.1.1.1";
constexpr std::string_view SEVENTEEN_BYTE_WORD = "ABCDEFGHIJKLMNOPQ";

/** @brief Lengths of a ban, and what each one gives. */
constexpr std::string_view THIRTY_MINUTES_WORD = "30m";
constexpr std::string_view TWO_HOURS_WORD = "2h";
constexpr std::string_view SEVEN_DAYS_WORD = "7d";
constexpr std::string_view LARGEST_LENGTH_WORD = "9999d";
constexpr std::chrono::seconds THIRTY_MINUTES = std::chrono::minutes{30};
constexpr std::chrono::seconds TWO_HOURS = std::chrono::hours{2};
constexpr std::chrono::seconds SEVEN_DAYS = std::chrono::days{7};
constexpr std::chrono::seconds LARGEST_LENGTH = std::chrono::days{9999};

/** @brief Words that are no length. */
constexpr std::string_view ZERO_LENGTH_WORD = "0m";
constexpr std::string_view LENGTH_PAST_THE_LARGEST_WORD = "10000d";
constexpr std::string_view COUNT_WITHOUT_UNIT_WORD = "5";
constexpr std::string_view UNIT_WITHOUT_COUNT_WORD = "m";
constexpr std::string_view UNKNOWN_UNIT_WORD = "5x";
constexpr std::string_view NEGATIVE_LENGTH_WORD = "-5m";
constexpr std::string_view SIGNED_LENGTH_WORD = "+5m";
constexpr std::string_view LENGTH_WITH_A_SPACE_WORD = "5 m";
