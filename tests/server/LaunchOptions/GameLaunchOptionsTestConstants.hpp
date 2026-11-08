#pragma once

#include <cstdint>
#include <string_view>

/** @brief The values of a supervisor link in the tests: the port and the token
 * as the command line writes them, and as the options read them. */
constexpr std::string_view SUPERVISOR_PORT_TEXT = "4242";
constexpr std::uint16_t SUPERVISOR_PORT_VALUE = 4242;
constexpr std::string_view LINK_TOKEN_TEXT = "987654321";
constexpr std::uint64_t LINK_TOKEN_VALUE = 987654321;

/** @brief A link token that is not one, and a supervisor port no one can reach.
 */
constexpr std::string_view ZERO_TEXT = "0";

/** @brief The rules a supervisor gives a game in the tests, as the command
 * line writes them, and the lines that force them. */
constexpr std::string_view HARD_DIFFICULTY_ARGUMENT = "hard";
constexpr std::string_view FIVE_LIVES_ARGUMENT = "5";
constexpr std::string_view FRIENDLY_FIRE_ON_ARGUMENT = "1";
constexpr std::string_view HARD_DIFFICULTY_LINE = "set difficulty hard";
constexpr std::string_view FIVE_LIVES_LINE = "set lives 5";
constexpr std::string_view FRIENDLY_FIRE_ON_LINE = "set friendly-fire 1";

/** @brief A seed, which a game process is not given at launch. */
constexpr std::string_view SOME_SEED_TEXT = "7";

/** @brief The file of commands a supervisor gives a game in the tests. */
constexpr std::string_view GAME_CONFIGURATION_ARGUMENT =
    "games/r-type_games.cfg";
