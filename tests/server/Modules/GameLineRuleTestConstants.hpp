#pragma once

#include <string_view>

/** @brief Lines of a configuration the tests cut into words, as the reader
 * does before it asks the rule: lines of the games, and lines of the
 * supervisor. */
constexpr std::string_view SET_SEED_LINE = "set seed 7";
constexpr std::string_view SET_STATISTICS_FILE_LINE =
    "set game-statistics-file stats.csv";
constexpr std::string_view PLUGIN_LINE = "plugin game-statistics";
constexpr std::string_view PLUGIN_FOLDER_LINE = "plugin-folder plugins";
constexpr std::string_view LOBBY_OFF_RULE_LINE = "module lobby off";
constexpr std::string_view NETWORK_OFF_LINE = "module network off";
constexpr std::string_view SET_PORT_LINE = "set port 4242";
constexpr std::string_view SET_UNKNOWN_LINE = "set no-such 1";
constexpr std::string_view BARE_MODULE_LINE = "module";
