#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief Lines of server.cfg that belong to the games: a module the
 * supervisor does not have turned off, and a seed. */
constexpr std::string_view LOBBY_OFF_LINE = "module lobby off";
constexpr std::string_view SEED_SEVEN_LINE = "set seed 7";

/** @brief The state rate a launch option of the supervisor gives, and the
 * line of `set` it becomes. */
constexpr std::string_view STATE_RATE_THIRTY = "30";
constexpr std::string_view STATE_RATE_THIRTY_LINE = "set state-rate 30";

/** @brief A line no program of the server knows. */
constexpr std::string_view UNKNOWN_SETTING_LINE = "set no-such-setting 1";

/** @brief The file of commands of the games of the supervisor on port p is
 * this prefix, p and this suffix, next to the executable. */
constexpr std::string_view GAMES_FILE_PREFIX = "r-type_games_";
constexpr std::string_view GAMES_FILE_SUFFIX = ".cfg";

/** @brief How many ticks askInformation() runs at most, five seconds, and how
 * long it waits for the answer after each. */
constexpr std::size_t INFORMATION_WAIT_TICKS = 300;
constexpr std::chrono::milliseconds ONE_TICK_WAIT{17};

/** @brief What an idle supervisor answers: no player, the eight games it may
 * run, and their 32 seats free. */
constexpr std::uint16_t GAMES_OF_A_SUPERVISOR = 8;
constexpr std::uint16_t SEATS_OF_A_SUPERVISOR = 32;

/** @brief Lines of server.cfg for the settings the supervisor owns. */
constexpr std::string_view SERVER_NAME_LINE = "set server-name Paris";
constexpr std::string_view MASTER_URL_LINE = "set master-url \"\"";
constexpr std::string_view MASTER_KEY_LINE = "set master-key \"\"";
constexpr std::string_view DATABASE_LINE = "set database \"\"";
