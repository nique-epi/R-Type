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

/** @brief Every setting of the master, in the order its usage lists them. */
constexpr std::array MASTER_SETTING_NAMES{
    PORT_SETTING_NAME, DATABASE_SETTING_NAME, SITE_KEY_SETTING_NAME};

/**
 * @brief The database of the master while DATABASE_SETTING_NAME is empty, in
 * the folder of the executable.
 */
constexpr std::string_view DATABASE_FILE_NAME = "r-type_master.db";

}  // namespace rtype::master
