#pragma once

#include <string_view>

/** @brief The folder a location falls back to, and a file the setting names
 * instead, written as the line of server.cfg sets it. */
constexpr std::string_view LOCATION_FOLDER = "server-files";
constexpr std::string_view SETTING_DATABASE_FILE = "elsewhere.db";
constexpr std::string_view SET_DATABASE_LINE = "set database elsewhere.db";
