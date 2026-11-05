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

/** @returns How HTTP names @p method: GET_METHOD, POST_METHOD or PUT_METHOD. */
[[nodiscard]] std::string_view methodNameOf(RouteMethod method);

}  // namespace rtype::network
