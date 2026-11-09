#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace rtype::identifiers {

/**
 * @brief The difficulty a game is played at unless its creator chose another
 * one: a table of the waves file every server ships.
 */
constexpr std::string_view DEFAULT_DIFFICULTY = "normal";

/**
 * @brief The difficulties every server ships, from the easiest: a list of
 * difficulties shows these in this order, then any other in alphabetical
 * order.
 */
constexpr std::array<std::string_view, 3> DIFFICULTY_ORDER{"easy", "normal",
                                                           "hard"};

/**
 * @brief The lives each player starts a game with unless its creator chose
 * another count.
 */
constexpr std::uint8_t DEFAULT_LIVES = 3;

/**
 * @brief The fewest and the most lives the creator of a game may give each
 * player: a server refuses any other count (LivesOutOfRange), and the new game
 * dialog of the client offers these only. Provisional, until gameplay tunes
 * them.
 */
constexpr std::uint8_t LOWEST_LIVES = 1;
constexpr std::uint8_t HIGHEST_LIVES = 9;

static_assert(DEFAULT_LIVES >= LOWEST_LIVES && DEFAULT_LIVES <= HIGHEST_LIVES,
              "the default lives must be a count the rules accept");

}  // namespace rtype::identifiers
