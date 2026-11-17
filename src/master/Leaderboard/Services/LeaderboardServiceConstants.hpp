#pragma once

#include <string_view>

namespace rtype::master {

/** @brief How the journal of a wallet and credited_results name the result of
 * a game: `game:<server>:<game>:<nickname>`. */
constexpr std::string_view GAME_REFERENCE_PREFIX = "game:";
constexpr std::string_view GAME_REFERENCE_SEPARATOR = ":";

}  // namespace rtype::master
