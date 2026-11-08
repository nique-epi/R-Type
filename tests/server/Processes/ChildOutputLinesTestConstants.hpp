#pragma once

#include <cstddef>
#include <string_view>

/** @brief A line a game writes, read in two parts, and the line it makes. */
constexpr std::string_view FIRST_PART = "game 3 st";
constexpr std::string_view SECOND_PART = "arted\n";
constexpr std::string_view JOINED_LINE = "game 3 started";

/** @brief A line a Windows program ends with "\r\n", and the line kept. */
constexpr std::string_view WINDOWS_LINE = "ready\r\n";
constexpr std::string_view READY_LINE = "ready";

/** @brief The longest line relayed whole, as the relay promises it: 4096
 * bytes; and a line three bytes longer, ended by nothing. */
constexpr std::size_t LONGEST_RELAYED_LINE = 4096;
constexpr std::size_t OVERLONG_LINE = LONGEST_RELAYED_LINE + 3;
constexpr std::string_view OVERLONG_LINE_REST = "xxx";

/** @brief The byte the long lines of the tests are made of. */
constexpr char FILLING_BYTE = 'x';

/** @brief A line ended, after which nothing is left. */
constexpr std::string_view ENDED_LINE = "a\n";
