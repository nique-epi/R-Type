#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

/**
 * @brief A file given after `--database`, and the line of the command `set`
 * it becomes.
 */
constexpr std::string_view DATABASE_ARGUMENT = "results.db";
constexpr std::string_view DATABASE_LINE = "set database results.db";

/**
 * @brief A file whose name holds a space, and its line: the value is quoted,
 * so `set` reads it as one word.
 */
constexpr std::string_view DATABASE_ARGUMENT_WITH_A_SPACE = "my results.db";
constexpr std::string_view DATABASE_LINE_WITH_A_SPACE =
    "set database \"my results.db\"";

/**
 * @brief A port given after `-p`, written in digits.
 */
constexpr std::string_view PORT_ARGUMENT = "4243";

/**
 * @brief The port PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_PORT_ARGUMENT = 4243;

/**
 * @brief A second port given after `-p`, and the port it names.
 */
constexpr std::string_view OTHER_PORT_ARGUMENT = "4244";

/**
 * @brief The port OTHER_PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_OTHER_PORT_ARGUMENT = 4244;

/**
 * @brief A value after `-p` that is no port.
 */
constexpr std::string_view NOT_A_PORT_ARGUMENT = "abc";

/**
 * @brief An option no program of the repository defines.
 */
constexpr std::string_view UNKNOWN_OPTION = "-x";

/**
 * @brief The client's option for the server address, which the server does
 * not take.
 */
constexpr std::string_view ADDRESS_OPTION = "-h";

/**
 * @brief The line of the usage that gives the default port.
 */
constexpr std::string_view DEFAULT_PORT_LINE =
    "UDP port to listen on, from 1 to 65535. Default: 4242.";

/**
 * @brief Seeds given after `--seed`, and the numbers they name.
 */
constexpr std::string_view SEED_ARGUMENT = "1234";
constexpr std::uint32_t SEED_OF_SEED_ARGUMENT = 1234;
constexpr std::string_view OTHER_SEED_ARGUMENT = "5678";
constexpr std::uint32_t SEED_OF_OTHER_SEED_ARGUMENT = 5678;
constexpr std::string_view LOWEST_SEED_ARGUMENT = "0";
constexpr std::string_view HIGHEST_SEED_ARGUMENT = "4294967295";
constexpr std::uint32_t HIGHEST_SEED = 4294967295U;

/**
 * @brief Values after `--seed` that are not seeds: one above the highest, a
 * negative number, nothing.
 */
constexpr std::string_view TOO_HIGH_SEED_ARGUMENT = "4294967296";
constexpr std::string_view NEGATIVE_SEED_ARGUMENT = "-1";
constexpr std::string_view EMPTY_SEED_ARGUMENT{};

/**
 * @brief The values of the state rate option the tests give: ones the server
 * accepts, with the number each reads as, and ones it refuses.
 */
constexpr std::string_view STATE_RATE_ARGUMENT = "30";
constexpr std::size_t RATE_OF_STATE_RATE_ARGUMENT = 30;
constexpr std::string_view OTHER_STATE_RATE_ARGUMENT = "45";
constexpr std::size_t RATE_OF_OTHER_STATE_RATE_ARGUMENT = 45;
constexpr std::string_view LOWEST_STATE_RATE_ARGUMENT = "2";
constexpr std::size_t LOWEST_STATE_RATE = 2;
constexpr std::string_view HIGHEST_STATE_RATE_ARGUMENT = "60";
constexpr std::size_t HIGHEST_STATE_RATE = 60;
constexpr std::string_view TOO_LOW_STATE_RATE_ARGUMENT = "1";
constexpr std::string_view TOO_HIGH_STATE_RATE_ARGUMENT = "61";
constexpr std::string_view NEGATIVE_STATE_RATE_ARGUMENT = "-30";
constexpr std::string_view NOT_A_NUMBER_STATE_RATE_ARGUMENT = "fast";
constexpr std::string_view EMPTY_STATE_RATE_ARGUMENT{};

/**
 * @brief The lines the launch options force.
 */
constexpr std::string_view PORT_LINE = "set port 4243";
constexpr std::string_view OTHER_PORT_LINE = "set port 4244";
constexpr std::string_view SEED_LINE = "set seed 1234";
constexpr std::string_view OTHER_SEED_LINE = "set seed 5678";
constexpr std::string_view LOWEST_SEED_LINE = "set seed 0";
constexpr std::string_view HIGHEST_SEED_LINE = "set seed 4294967295";
constexpr std::string_view STATE_RATE_LINE = "set state-rate 30";
constexpr std::string_view OTHER_STATE_RATE_LINE = "set state-rate 45";
constexpr std::string_view LOWEST_STATE_RATE_LINE = "set state-rate 2";
constexpr std::string_view HIGHEST_STATE_RATE_LINE = "set state-rate 60";

/**
 * @brief A value of the set option, and the line it forces.
 */
constexpr std::string_view ASSIGNMENT_ARGUMENT = "nickname=ace";
constexpr std::string_view ASSIGNMENT_LINE = "set nickname ace";

/**
 * @brief Values of the set option that are refused.
 */
constexpr std::string_view ASSIGNMENT_WITHOUT_MARK = "nickname";
constexpr std::string_view ASSIGNMENT_WITHOUT_NAME = "=ace";
constexpr std::string_view INPUT_DELAY_ARGUMENT = "5";
constexpr std::string_view INPUT_DELAY_LINE = "set input-delay 5";
constexpr std::string_view OTHER_INPUT_DELAY_ARGUMENT = "8";
constexpr std::string_view OTHER_INPUT_DELAY_LINE = "set input-delay 8";
constexpr std::string_view LOWEST_INPUT_DELAY_ARGUMENT = "0";
constexpr std::string_view LOWEST_INPUT_DELAY_LINE = "set input-delay 0";
constexpr std::string_view HIGHEST_INPUT_DELAY_ARGUMENT = "30";
constexpr std::string_view HIGHEST_INPUT_DELAY_LINE = "set input-delay 30";
constexpr std::string_view TOO_HIGH_INPUT_DELAY_ARGUMENT = "31";
constexpr std::string_view NEGATIVE_INPUT_DELAY_ARGUMENT = "-1";
constexpr std::string_view NOT_A_NUMBER_INPUT_DELAY_ARGUMENT = "slow";
constexpr std::string_view EMPTY_INPUT_DELAY_ARGUMENT{};

/**
 * @brief The options a supervisor launched with `--log-level debug --network
 * bad` hands its games: the level, then the five values of the "bad" network
 * of the table of the scenarios (300 ms, 50 ms, 10 %, 2 %, 5 %).
 */
constexpr std::array<std::string_view, 12>
    GAME_ARGUMENTS_OF_DEBUG_ON_A_BAD_NETWORK{
        "--log-level", "debug", "--lag",        "300", "--jitter",  "50",
        "--loss",      "10",    "--duplicates", "2",   "--reorder", "5"};
