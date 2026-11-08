#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include "MasterRoute.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

/** @returns The path a client calls @p route by: MASTER_API_PREFIX, then its.
 */
[[nodiscard]] std::string apiPathOf(MasterRoute route);

/**
 * @returns Where the member @p memberId of the collection @p route answers
 * lives: `/api/servers/7` for CREATE_SERVER_ROUTE and 7.
 */
[[nodiscard]] std::string memberPathOf(MasterRoute route,
                                       std::int64_t memberId);

/**
 * @returns The path a client calls @p route by when its path names the server
 * @p serverId: `/api/servers/7/games` for LIST_SERVER_GAMES_ROUTE and 7.
 */
[[nodiscard]] std::string serverPathOf(MasterRoute route,
                                       std::int64_t serverId);

/**
 * @returns The path a client calls @p route by when its path names the game
 * @p gameId: `/api/servers/me/results/<id>` for KEEP_OWN_RESULT_ROUTE. The id
 * is written as it comes; the master refuses one that is not GAME_ID_SIZE
 * lowercase hexadecimal digits.
 */
[[nodiscard]] std::string gamePathOf(MasterRoute route,
                                     std::string_view gameId);

/** @returns How HTTP names @p method: GET_METHOD, POST_METHOD or PUT_METHOD. */
[[nodiscard]] std::string_view methodNameOf(RouteMethod method);

}  // namespace rtype::network
