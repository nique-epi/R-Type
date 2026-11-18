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

/** @brief A room and a round of the casino, and the paths of the seat and of
 * the bets that name them. */
constexpr std::uint8_t SECOND_ROOM = 2;
constexpr std::uint64_t SOME_ROUND = 815;
constexpr std::string_view SECOND_ROOM_SEAT_PATH =
    "/api/casino/rooms/2/seats/me";
constexpr std::string_view SOME_ROUND_BETS_PATH =
    "/api/casino/rooms/2/rounds/815/bets/me";

/** @brief A look of the shop, and where it is bought. */
constexpr std::string_view CROWN_LOOK = "crown";
constexpr std::string_view CROWN_PURCHASE_PATH = "/api/users/me/looks/crown";
