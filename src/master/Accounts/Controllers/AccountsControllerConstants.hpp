#pragma once

#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the accounts say when they
 * refuse. */
constexpr std::string_view ACCOUNTS_CLOSED_DETAIL =
    "this master runs without accounts";
constexpr std::string_view NICKNAME_TAKEN_DETAIL =
    "another account has this nickname";
constexpr std::string_view ACCOUNT_QUOTA_DETAIL =
    "this address created too many accounts in the last hour";
constexpr std::string_view WRONG_CREDENTIALS_DETAIL =
    "wrong nickname or password";
constexpr std::string_view SIGN_IN_QUOTA_DETAIL =
    "too many failed sign-ins from this address: wait before the next";
constexpr std::string_view NO_SESSION_DETAIL =
    "no valid session token in an Authorization: Bearer header";

}  // namespace rtype::master
