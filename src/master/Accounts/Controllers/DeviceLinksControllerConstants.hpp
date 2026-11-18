#pragma once

#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the device links say when they
 * refuse. */
constexpr std::string_view DEVICE_LINK_QUOTA_DETAIL =
    "this address asked for too many device links in the last hour";
constexpr std::string_view NO_DEVICE_LINK_DETAIL =
    "no device link has this secret in an Authorization: Bearer header";
constexpr std::string_view UNKNOWN_USER_CODE_DETAIL =
    "no device link has this code";
constexpr std::string_view READ_TOO_SOON_DETAIL =
    "read the device link again after the interval it gives";
constexpr std::string_view ALREADY_DECIDED_DETAIL =
    "this device link was already decided otherwise, or it expired";
constexpr std::string_view LINK_PENDING_DETAIL =
    "no player approved this device link yet";
constexpr std::string_view LINK_DENIED_DETAIL =
    "the player refused to connect this game";
constexpr std::string_view LINK_EXPIRED_DETAIL =
    "this device link expired: ask for another";

}  // namespace rtype::master
