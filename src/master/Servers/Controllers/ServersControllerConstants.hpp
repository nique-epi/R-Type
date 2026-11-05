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
/**
 * @brief The parts of the detail of a page out of its bounds, around
 * MINIMUM_PAGE_SIZE, MAXIMUM_PAGE_SIZE and FIRST_PAGE_OFFSET: "limit is a
 * whole number from 1 to 100, and offset one from 0".
 */
constexpr std::string_view INVALID_LIMIT_DETAIL_START =
    "limit is a whole number from ";
constexpr std::string_view DETAIL_RANGE_JOIN = " to ";
constexpr std::string_view INVALID_OFFSET_DETAIL_START =
    ", and offset one from ";
constexpr std::string_view CREATION_QUOTA_DETAIL =
    "this address created too many servers in the last hour";

}  // namespace rtype::master
