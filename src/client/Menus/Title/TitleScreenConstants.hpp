#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

/** @brief The order of the account bar among the bars of the title: after
 * PLAY, LEADERBOARD and SETTINGS, before QUIT. */
constexpr std::size_t ACCOUNT_BUTTON = 3;

/** @brief What the account bar says: SIGN IN for a guest, SIGN OUT and the
 * nickname for a player signed in, with the line under each. */
constexpr std::string_view SIGN_OUT_START = "SIGN OUT ";
constexpr const char* SIGN_IN_DESCRIPTION =
    "CONNECT THIS GAME TO YOUR ACCOUNT ON THE SITE";
constexpr const char* SIGN_OUT_DESCRIPTION =
    "CLOSE THE SESSION OF THIS ACCOUNT ON THIS GAME";

}  // namespace rtype::client
