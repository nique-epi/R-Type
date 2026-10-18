#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief Players of the game tests, as indexes into FIVE_PLAYER_PORTS: the
 * player at index i joins as player number i + 1.
 */
constexpr std::size_t FIRST_PLAYER = 0;
constexpr std::size_t SECOND_PLAYER = 1;
constexpr std::size_t FOURTH_PLAYER = 3;

/**
 * @brief Sequences of the datagrams a test player sends, in the order it sends
 * them.
 */
constexpr std::uint16_t FIRST_SEQUENCE = 1;
constexpr std::uint16_t SECOND_SEQUENCE = 2;
constexpr std::uint16_t THIRD_SEQUENCE = 3;
constexpr std::uint16_t FOURTH_SEQUENCE = 4;

/**
 * @brief A few ticks, for the tests that check nothing changes while they
 * run.
 */
constexpr std::size_t SOME_TICKS = 3;

}  // namespace rtype::server::testing
