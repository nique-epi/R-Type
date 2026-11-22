#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

constexpr std::array<std::string_view, 4> NICKNAMES{"ACE", "BOB", "CID", "DEE"};
constexpr std::string_view KEEPER_DIFFICULTY = "normal";
constexpr std::uint32_t KEEPER_SCORE = 1200;
constexpr std::uint32_t KEEPER_BOUNTY = 650;
constexpr std::uint8_t NO_LIFE = 0;
constexpr std::uint32_t EVERY_LIFE_LOST = 3;
constexpr std::uint8_t THREE_AT_THE_START = 3;

/** @brief A game whose rules give five lives, a player who has two left at
 * the end, and the three it lost. */
constexpr std::uint8_t FIVE_LIVES_OF_THE_RULES = 5;
constexpr std::uint8_t TWO_LIVES_LEFT = 2;
constexpr std::uint32_t THREE_LIVES_LOST = 3;

/**
 * @brief Two look items a player won, as LookItemWon numbers them (their place
 * in the catalog of the ship looks, plus one), and their names in that
 * catalog: the crown, third, and the halo, fourteenth.
 */
constexpr std::uint8_t CROWN_LOOK_NUMBER = 3;
constexpr std::uint8_t HALO_LOOK_NUMBER = 14;
constexpr std::string_view CROWN_LOOK_NAME = "crown";
constexpr std::string_view HALO_LOOK_NAME = "halo";

/** @brief One tick short of the 500 ms of the protocol page, at 60 ticks per
 * second. */
constexpr std::size_t TICKS_ONE_SHORT_OF_THE_INTERVAL = 29;

}  // namespace rtype::server::testing
