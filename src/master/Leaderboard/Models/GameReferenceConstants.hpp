#pragma once

#include <string_view>

namespace rtype::master {

/** @brief How the journal of a wallet and credited_results name the result of
 * a game: `game:<server>:<game>:<nickname>`. The leaderboard of the accounts
 * finds the account a game was credited to by that name. */
constexpr std::string_view GAME_REFERENCE_PREFIX = "game:";
constexpr std::string_view GAME_REFERENCE_SEPARATOR = ":";

}  // namespace rtype::master
