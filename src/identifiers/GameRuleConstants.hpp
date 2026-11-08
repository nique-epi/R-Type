#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::identifiers {

/**
 * @brief The difficulty a game is played at unless its creator chose another
 * one: a table of the waves file every server ships.
 */
constexpr std::string_view DEFAULT_DIFFICULTY = "normal";

/**
 * @brief The lives each player starts a game with unless its creator chose
 * another count.
 */
constexpr std::uint8_t DEFAULT_LIVES = 3;

}  // namespace rtype::identifiers
