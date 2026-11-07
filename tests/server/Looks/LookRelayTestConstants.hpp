#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The seats of the tests, from 0. */
constexpr std::size_t FIRST_SEAT = 0;
constexpr std::size_t SECOND_SEAT = 1;

/** @brief 100 ms at 60 ticks a second are 6 ticks: 5 is one short of the wait
 * between two choices. */
constexpr std::size_t TICKS_OF_THE_CHOICE_WAIT = 6;
constexpr std::size_t TICKS_ONE_SHORT_OF_THE_CHOICE_WAIT =
    TICKS_OF_THE_CHOICE_WAIT - 1;

/** @brief The number of the first and the second player. */
constexpr std::uint8_t FIRST_PLAYER = 1;
constexpr std::uint8_t SECOND_PLAYER = 2;

/** @brief The party hat: the item 2, which a head slot holds. */
constexpr std::uint8_t PARTY_HAT_NUMBER = 2;

}  // namespace rtype::server::testing
