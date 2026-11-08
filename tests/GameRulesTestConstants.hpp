#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::testing {

/** @brief The rules a player chose for a new game in the tests of the client:
 * the difficulty "hard" and five lives, both other than the default rules;
 * the tests set friendly fire on as well. */
constexpr std::string_view CHOSEN_DIFFICULTY = "hard";
constexpr std::uint8_t CHOSEN_LIVES = 5;

/** @brief Other tables of a waves file: two every server ships, and one of a
 * server's own. */
constexpr std::string_view EASY_DIFFICULTY = "easy";
constexpr std::string_view NORMAL_DIFFICULTY = "normal";
constexpr std::string_view VETERAN_DIFFICULTY = "veteran";

/** @brief The rules of a game played hard, with five lives and friendly fire
 * on, as the panel of the games and the waiting room write them. */
constexpr std::string_view HARD_RULES_LINE = "HARD, 5 LIVES, FRIENDLY FIRE ON";

}  // namespace rtype::testing
