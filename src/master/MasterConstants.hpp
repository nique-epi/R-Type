#pragma once

#include <string_view>

namespace rtype::master {

/**
 * @brief How the master names itself at the start of its usage and of the
 * errors it prints about its arguments: the server program, in its master
 * mode.
 */
constexpr std::string_view MASTER_PROGRAM_NAME = "r-type_server --mode master";

/** @brief The version of the master, which GET /api/health answers with. */
constexpr std::string_view MASTER_VERSION = "0.1.0";

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

/**
 * @brief Launch option followed by the path of the database of the master: it
 * forces DATABASE_SETTING_NAME to that path.
 */
constexpr std::string_view DATABASE_OPTION = "--database";

/**
 * @brief The database of the master while DATABASE_SETTING_NAME is empty, in
 * the folder of the executable.
 */
constexpr std::string_view DATABASE_FILE_NAME = "r-type_master.db";

}  // namespace rtype::master
