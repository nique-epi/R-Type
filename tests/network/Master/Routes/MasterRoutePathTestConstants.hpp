#pragma once

#include <cstdint>
#include <string_view>

/** @brief A server of the tests, and where its games are listed. */
constexpr std::int64_t SEVENTH_SERVER_ID = 7;
constexpr std::string_view SEVENTH_SERVER_GAMES_PATH = "/api/servers/7/games";

/** @brief A game of the tests, and where its result is kept. */
constexpr std::string_view SOME_GAME_ID = "0123456789abcdef0123456789abcdef";
constexpr std::string_view SOME_GAME_RESULT_PATH =
    "/api/servers/me/results/0123456789abcdef0123456789abcdef";
