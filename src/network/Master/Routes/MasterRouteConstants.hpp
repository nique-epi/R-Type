#pragma once

#include <cstdint>
#include <limits>
#include <string_view>
#include "MasterRoute.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

/** @brief The path every route of the master starts with. */
constexpr std::string_view MASTER_API_PREFIX = "/api";

/** @brief Whether the master answers, and in which version. */
constexpr MasterRoute HEALTH_ROUTE{.method = RouteMethod::Get,
                                   .path = "/health"};

/**
 * @brief Creates a game server, named by its host on the web site; the
 * answer holds its key, the only time it is shown.
 */
constexpr MasterRoute CREATE_SERVER_ROUTE{.method = RouteMethod::Post,
                                          .path = "/servers"};

/** @brief Replaces the representation of the server of the key. */
constexpr MasterRoute REPLACE_OWN_SERVER_ROUTE{.method = RouteMethod::Put,
                                               .path = "/servers/me"};

/** @brief Replaces the last state of the server of the key. */
constexpr MasterRoute REPLACE_OWN_HEARTBEAT_ROUTE{
    .method = RouteMethod::Put, .path = "/servers/me/heartbeat"};

/** @brief The parameter of a path that names a server, after the prefix. */
constexpr std::string_view PATH_PARAMETER_PREFIX = ":";
constexpr std::string_view SERVER_ID_PATH_PARAMETER = "id";

/** @brief Lists the games of the server named in the path; open to everyone. */
constexpr MasterRoute LIST_SERVER_GAMES_ROUTE{.method = RouteMethod::Get,
                                              .path = "/servers/:id/games"};

/** @brief Lists the registered servers, a page at a time, by id; open to
 * everyone. */
constexpr MasterRoute LIST_SERVERS_ROUTE{.method = RouteMethod::Get,
                                         .path = "/servers"};

/** @brief Lists the samples of the server named in the path, a page at a
 * time, newest first; open to everyone. */
constexpr MasterRoute LIST_SERVER_SAMPLES_ROUTE{.method = RouteMethod::Get,
                                                .path = "/servers/:id/samples"};

/** @brief Lists the statuses the server named in the path held, each with the
 * second it took it, a page at a time, newest first; open to everyone. */
constexpr MasterRoute LIST_SERVER_STATUSES_ROUTE{
    .method = RouteMethod::Get, .path = "/servers/:id/statuses"};

/** @brief The parameter of a path that names a game of a server. */
constexpr std::string_view GAME_ID_PATH_PARAMETER = "gameId";

/**
 * @brief Keeps the result of a vanilla game of the server of the key, named in
 * the path by the id the server drew; a game the master already keeps stays
 * as it was, so a server may send a result again safely.
 */
constexpr MasterRoute KEEP_OWN_RESULT_ROUTE{
    .method = RouteMethod::Put, .path = "/servers/me/results/:gameId"};

/**
 * @brief The best score of each player for one difficulty and one number of
 * players, on every server or on one, a page at a time, with the row of a
 * player the query names; open to everyone.
 */
constexpr MasterRoute READ_LEADERBOARD_ROUTE{.method = RouteMethod::Get,
                                             .path = "/leaderboard"};

/** @brief The parameters of the query of the leaderboard:
 * `?difficulty=normal&playerCount=2`, then `server=7` and `player=ACE`, both
 * optional. */
constexpr std::string_view DIFFICULTY_QUERY_PARAMETER = "difficulty";
constexpr std::string_view PLAYER_COUNT_QUERY_PARAMETER = "playerCount";
constexpr std::string_view SERVER_QUERY_PARAMETER = "server";
constexpr std::string_view PLAYER_QUERY_PARAMETER = "player";

/**
 * @brief The parameters of the query of a list, `?limit=50&offset=0`, and
 * their bounds: a page holds MINIMUM_PAGE_SIZE to MAXIMUM_PAGE_SIZE items,
 * DEFAULT_PAGE_SIZE when the query leaves limit out, and starts after the
 * first offset items, FIRST_PAGE_OFFSET when it leaves offset out.
 */
constexpr std::string_view LIMIT_QUERY_PARAMETER = "limit";
constexpr std::string_view OFFSET_QUERY_PARAMETER = "offset";
constexpr std::int64_t MINIMUM_PAGE_SIZE = 1;
constexpr std::int64_t DEFAULT_PAGE_SIZE = 50;
constexpr std::int64_t MAXIMUM_PAGE_SIZE = 100;
constexpr std::int64_t FIRST_PAGE_OFFSET = 0;
constexpr std::int64_t HIGHEST_PAGE_OFFSET =
    std::numeric_limits<std::int64_t>::max();

/** @brief What joins the path of the servers and the id of one of them. */
constexpr std::string_view PATH_SEPARATOR = "/";

}  // namespace rtype::network
