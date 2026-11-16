#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::master {

/** @brief How the journal of a wallet and credited_results name the result of
 * a game: `game:<server>:<game>:<nickname>`. */
constexpr std::string_view GAME_REFERENCE_PREFIX = "game:";
constexpr std::string_view GAME_REFERENCE_SEPARATOR = ":";

/** @brief The lowest id of an account: SQLite numbers rows from 1. */
constexpr std::int64_t FIRST_ACCOUNT_ID = 1;

}  // namespace rtype::master
