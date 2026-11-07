#pragma once

#include <cstddef>
#include <string_view>

/** @brief Two lines, the first ended the Windows way. */
constexpr std::string_view GAMES_THEN_PLAYERS_FROM_WINDOWS =
    "games\r\nplayers\n";

/** @brief close-game 1 read in two parts. */
constexpr std::string_view FIRST_PART_OF_CLOSE = "clo";
constexpr std::string_view SECOND_PART_OF_CLOSE = "se-game 1\n";

/** @brief The longest line the console keeps: a command line of the
 * supervisor is never near it, a pasted block may be. */
constexpr std::size_t LONGEST_LINE_LENGTH = 1024;
constexpr char FILLER = 'a';
