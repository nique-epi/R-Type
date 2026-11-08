#pragma once

#include <cstdint>
#include <string_view>
#include "MasterMessageConstants.hpp"
#include "ProtocolConstants.hpp"
#include "QueryBounds.hpp"

namespace rtype::master {

/**
 * @brief The parts of what the routes of the leaderboard say when they
 * refuse, around GAME_ID_SIZE, the sizes of a nickname, the bounds of a number
 * of players and FIRST_SERVER_ID: "the id of a game is 32 lowercase
 * hexadecimal digits", and "difficulty, a name of 1 to 16 bytes, and
 * playerCount, a whole number from 1 to 4, are required; server, when given,
 * is a whole number from 1, and player a nickname of 1 to 16 bytes".
 */
constexpr std::string_view GAME_ID_DETAIL_START = "the id of a game is ";
constexpr std::string_view GAME_ID_DETAIL_END = " lowercase hexadecimal digits";
constexpr std::string_view DIFFICULTY_DETAIL_START = "difficulty, a name of ";
constexpr std::string_view PLAYER_COUNT_DETAIL_START =
    " bytes, and playerCount, a whole number from ";
constexpr std::string_view SERVER_DETAIL_START =
    ", are required; server, when given, is a whole number from ";
constexpr std::string_view PLAYER_DETAIL_START = ", and player a nickname of ";
constexpr std::string_view BYTES_DETAIL_END = " bytes";
constexpr std::string_view NOT_VANILLA_DETAIL =
    "a game that is not vanilla stays in the leaderboard of its server";

/** @brief The values the number of players and the server of the query of the
 * leaderboard may take; neither has a default, since the first is required
 * and the second filters only when it is given. */
constexpr QueryBounds PLAYER_COUNT_BOUNDS{
    .lowest = network::MINIMUM_PLAYERS_PER_GAME,
    .highest = static_cast<std::int64_t>(network::MAXIMUM_PLAYERS)};
constexpr QueryBounds SERVER_BOUNDS{
    .lowest = static_cast<std::int64_t>(network::FIRST_SERVER_ID),
    .highest = static_cast<std::int64_t>(network::HIGHEST_SERVER_ID)};

}  // namespace rtype::master
