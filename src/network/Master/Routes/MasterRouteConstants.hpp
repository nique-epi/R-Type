#pragma once

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

/** @brief What joins the path of the servers and the id of one of them. */
constexpr std::string_view PATH_SEPARATOR = "/";

}  // namespace rtype::network
