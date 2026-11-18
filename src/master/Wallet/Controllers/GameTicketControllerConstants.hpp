#pragma once

#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the game tickets say when they
 * refuse. */
constexpr std::string_view TICKET_QUOTA_DETAIL =
    "this account was given too many game tickets in the last hour";
constexpr std::string_view UNKNOWN_TICKET_DETAIL =
    "this account holds no game ticket of that fingerprint";

}  // namespace rtype::master
