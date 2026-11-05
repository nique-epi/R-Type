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
