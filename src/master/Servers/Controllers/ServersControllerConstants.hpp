#pragma once

#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the servers say when they refuse.
 */
constexpr std::string_view UNKNOWN_KEY_DETAIL =
    "no valid server key in an Authorization: Bearer header";
constexpr std::string_view UNREGISTERED_SERVER_DETAIL =
    "this server has not registered: PUT /api/servers/me first";
constexpr std::string_view UNKNOWN_SERVER_DETAIL = "no game server has this id";
constexpr std::string_view INVALID_PAGE_DETAIL =
    "limit is a whole number from 1 to 100, and offset one from 0";
constexpr std::string_view CREATION_QUOTA_DETAIL =
    "this address created too many servers in the last hour";

}  // namespace rtype::master
