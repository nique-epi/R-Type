#pragma once

#include <string_view>

/** @brief A traffic file in a folder, the text a game adds, and what the game
 * writes. */
constexpr std::string_view TRAFFIC_FILE = "logs/traffic.csv";
constexpr std::string_view GAME_THREE_TEXT = "_game_3";
constexpr std::string_view TRAFFIC_FILE_OF_GAME_THREE =
    "logs/traffic_game_3.csv";

/** @brief A file with no extension, and what it becomes. */
constexpr std::string_view FILE_WITHOUT_EXTENSION = "logs/traffic";
constexpr std::string_view FILE_WITHOUT_EXTENSION_OF_GAME_THREE =
    "logs/traffic_game_3";
