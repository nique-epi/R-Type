#pragma once

#include <cstddef>
#include <string_view>

/** @brief What a terminal shows after a line ended: a new row, then the prompt
 * "> " the console promises. */
constexpr std::string_view NEW_ROW_AND_PROMPT = "\n> ";

/** @brief How a terminal erases the character left of the cursor. */
constexpr std::string_view ERASED_CHARACTER = "\b \b";

/** @brief Keys the tests type. */
constexpr std::string_view HALF_OF_GAMES = "gam";
constexpr std::string_view GAMES_TYPED = "games";
constexpr std::string_view GAMES_AND_CARRIAGE_RETURN = "games\r";
constexpr std::string_view GAMES_AND_NEW_LINE = "games\n";
constexpr std::string_view NEW_LINE_ALONE = "\n";
constexpr std::string_view GAMES_AND_A_WRONG_KEY = "gamx";
constexpr std::string_view DELETE_KEY = "\x7f";
constexpr std::string_view BACKSPACE_KEY = "\b";
constexpr std::string_view CAFE_WITH_AN_ACUTE_E = "caf\xc3\xa9";
constexpr std::string_view CAFE_WITHOUT_ITS_E = "caf";
constexpr std::string_view GAMES_WITH_AN_UP_ARROW = "ga\x1b[Ames";
constexpr std::string_view LONE_ESCAPE_THEN_G = "\x1bg";
constexpr std::string_view G_KEY = "g";
constexpr std::string_view BAN_TYPED = "ban";

/** @brief The longest line the console keeps, as its contract says: 1024
 * bytes. */
constexpr std::size_t LONGEST_TYPED_LINE = 1024;

/** @brief The byte the long lines of the tests are made of. */
constexpr char TYPED_FILLING = 'x';
