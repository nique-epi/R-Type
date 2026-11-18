#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "ScreenId.hpp"

namespace rtype::client::testing {

/** @brief The second the tests read the clock at, and the expiry of a code
 * nine and a half minutes later, then 30 seconds later. */
constexpr std::chrono::sys_seconds SIGN_IN_NOW{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds NINE_AND_A_HALF_MINUTES_LATER =
    SIGN_IN_NOW + std::chrono::seconds{570};
constexpr std::chrono::sys_seconds HALF_A_MINUTE_LATER =
    SIGN_IN_NOW + std::chrono::seconds{30};

/** @brief The code a link shows, its pages, the token it gives, and what
 * the dialog writes of them: nine whole minutes left, or under a minute. */
constexpr std::string_view SHOWN_CODE = "KQFB-WZPM";
constexpr std::string_view SHOWN_PAGE = "https://rtype.app/link";
constexpr std::string_view SHOWN_PAGE_WITH_CODE =
    "https://rtype.app/link?code=KQFB-WZPM";
constexpr std::string_view SIGN_IN_TOKEN =
    "rts_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
constexpr std::string_view NINE_MINUTES_LINE =
    "ENTER IT AT RTYPE.APP/LINK - 9 MIN";
constexpr std::string_view LAST_MINUTE_LINE =
    "ENTER IT AT RTYPE.APP/LINK - UNDER A MINUTE";
constexpr std::string_view PAGE_AS_SHOWN = "RTYPE.APP/LINK";

/** @brief The account the token opens, and the screen that asked to sign
 * in. */
constexpr std::uint64_t SIGNED_IN_ACCOUNT = 7;
constexpr ScreenId ASKING_SCREEN{"casino of the tests"};

}  // namespace rtype::client::testing
