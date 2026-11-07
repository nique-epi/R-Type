#pragma once

#include <string_view>

/** @brief games, then players cut across two reads. */
constexpr std::string_view GAMES_AND_HALF_OF_PLAYERS = "games\npla";
constexpr std::string_view REST_OF_PLAYERS = "yers\n";

/** @brief games, ended. */
constexpr std::string_view GAMES_ENDED = "games\n";
