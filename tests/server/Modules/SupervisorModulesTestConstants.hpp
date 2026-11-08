#pragma once

#include <array>
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
constexpr std::string_view PUBLIC_ADDRESS_LINE = "set public-address \"\"";

/** @brief A line for each setting the master link declares. */
constexpr std::array MASTER_SETTING_LINES{MASTER_URL_LINE, MASTER_KEY_LINE,
                                          PUBLIC_ADDRESS_LINE};

/** @brief A public-address that names a host, which only an IPv4 address may
 * be. */
constexpr std::string_view HOST_NAME_PUBLIC_ADDRESS_LINE =
    "set public-address paris.example.org";

/** @brief Public addresses no player reaches a server at: a loopback address,
 * the address of every interface and the broadcast address. */
constexpr std::string_view LOOPBACK_PUBLIC_ADDRESS_LINE =
    "set public-address 127.0.0.2";
constexpr std::string_view ANY_INTERFACE_PUBLIC_ADDRESS_LINE =
    "set public-address 0.0.0.0";
constexpr std::string_view BROADCAST_PUBLIC_ADDRESS_LINE =
    "set public-address 255.255.255.255";

/** @brief A server created on the master, and the address it declares there
 * with public-address, which is not the address its requests come from. */
constexpr std::string_view LISTED_SERVER_NAME = "Paris 1";

/** @brief A pilot of a vanilla game saved in the file of results a test
 * starts the supervisor with, when it ended, and the one row the master keeps
 * of it. */
constexpr std::string_view SAVED_PILOT = "ACE";
constexpr std::chrono::sys_seconds SAVED_GAME_END{
    std::chrono::seconds{1'800'000'000}};
constexpr int ROW_OF_THE_SAVED_PILOT = 1;
constexpr std::string_view DECLARED_PUBLIC_ADDRESS = "203.0.113.20";

/** @brief How long a test waits for the supervisor to register with the
 * master, and how often it asks the master meanwhile. */
constexpr std::chrono::seconds REGISTRATION_DEADLINE{10};
constexpr std::chrono::milliseconds LISTING_WAIT_STEP{20};
constexpr std::string_view DATABASE_LINE = "set database \"\"";
