#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/**
 * @brief The stages of the loop of the master: none, since the threads of its
 * HTTP server answer every request and nothing runs at a tick.
 */
constexpr std::array<std::string_view, 0> MASTER_LOOP_STAGES{};

/**
 * @brief How the master names itself at the start of its usage and of the
 * errors it prints about its arguments: the server program, in its master
 * mode.
 */
constexpr std::string_view MASTER_PROGRAM_NAME = "r-type_server --mode master";

/** @brief The logger every line of the master is written with. */
constexpr std::string_view MASTER_LOGGER_NAME = "Master";

/**
 * @brief The file of commands the master reads when it starts, in the folder
 * it is started from; a missing file is not an error.
 */
constexpr std::string_view MASTER_CONFIGURATION_FILE_NAME = "master.cfg";

/**
 * @brief The names the settings of the master go by in `set` and in `--set`.
 */
constexpr std::string_view PORT_SETTING_NAME = "port";
constexpr std::string_view DATABASE_SETTING_NAME = "database";
constexpr std::string_view SITE_KEY_SETTING_NAME = "site-key";

/** @brief The settings of the module wallet: what a wallet holds when it
 * opens, at the first read of an account. */
constexpr std::string_view WALLET_STARTING_KOPECKS_SETTING_NAME =
    "wallet-starting-kopecks";
constexpr std::string_view WALLET_STARTING_CHIPS_SETTING_NAME =
    "wallet-starting-chips";

/** @brief The settings of the module casino: how many roulette rooms it runs,
 * the players each seats, and how long each phase of a round lasts. */
constexpr std::string_view CASINO_ROOMS_SETTING_NAME = "casino-rooms";
constexpr std::string_view CASINO_SEATS_SETTING_NAME = "casino-seats";
constexpr std::string_view CASINO_BETTING_SECONDS_SETTING_NAME =
    "casino-betting-seconds";
constexpr std::string_view CASINO_SPINNING_SECONDS_SETTING_NAME =
    "casino-spinning-seconds";
constexpr std::string_view CASINO_RESULT_SECONDS_SETTING_NAME =
    "casino-result-seconds";

/** @brief Every setting of the master, in the order its usage lists them. */
constexpr std::array MASTER_SETTING_NAMES{
    PORT_SETTING_NAME, DATABASE_SETTING_NAME, SITE_KEY_SETTING_NAME};

/**
 * @brief The database of the master while DATABASE_SETTING_NAME is empty, in
 * the folder of the executable.
 */
constexpr std::string_view DATABASE_FILE_NAME = "r-type_master.db";

/**
 * @brief The environment variable the key of the passwords is read from at
 * launch: network::PASSWORD_KEY_BYTES in hexadecimal. Unset or empty, the
 * master runs without accounts.
 */
constexpr std::string_view PASSWORD_KEY_VARIABLE = "RT_PASSWORD_KEY";

}  // namespace rtype::master
