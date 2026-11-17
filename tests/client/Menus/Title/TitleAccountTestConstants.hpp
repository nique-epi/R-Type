#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The account a player signs the title in as, its token, and what
 * the account bar then says. */
constexpr std::uint64_t TITLE_ACCOUNT_ID = 7;
constexpr std::string_view TITLE_ACCOUNT_NICKNAME = "ACE";
constexpr std::string_view TITLE_ACCOUNT_TOKEN =
    "rts_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
constexpr std::string_view SIGN_OUT_ACE_TEXT = "SIGN OUT ACE";

}  // namespace rtype::client::testing
