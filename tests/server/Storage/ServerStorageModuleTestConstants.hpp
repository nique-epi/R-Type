#pragma once

#include <cstdint>
#include <string_view>

/** @brief The file of the database when no module gives a port: the file of
 * the default port, 4242. */
constexpr std::string_view DEFAULT_PORT_DATABASE_FILE = "r-type_server_4242.db";

/** @brief A port the double of the network gives, and the file it names. */
constexpr std::uint16_t LISTENING_PORT = 4243;
constexpr std::string_view LISTENING_PORT_DATABASE_FILE =
    "r-type_server_4243.db";

/** @brief A folder no test creates, so a file in it cannot be opened. */
constexpr std::string_view MISSING_FOLDER = "no-such-folder";
