#pragma once

#include <cstddef>
#include <cstdint>
#include "ServerConstants.hpp"

namespace rtype::server::testing {

constexpr std::size_t SOME_DELAY_TICKS = 3;
constexpr std::size_t NO_DELAY_TICKS = 0;
constexpr std::size_t TOO_MANY_DELAY_TICKS = HIGHEST_INPUT_DELAY_TICKS + 1;
constexpr std::size_t SECOND_PLAYER_INDEX = 1;
constexpr std::uint32_t FIRST_HELD_INPUT_NUMBER = 1;
constexpr std::uint32_t SECOND_HELD_INPUT_NUMBER = 2;
constexpr std::uint8_t FIRST_HELD_ACTIONS = 0x01;
constexpr std::uint8_t SECOND_HELD_ACTIONS = 0x02;

/** @brief Ten inputs of one player read in one tick, with no delay: one per
 * tick for the ticks 0 to 6, while the oldest waited at most
 * INPUT_CATCH_UP_TICKS; in tick 7 it waited 7, so two come out. After 8
 * ticks, 9 came out. */
constexpr std::size_t INPUTS_READ_TOGETHER = INPUT_CATCH_UP_TICKS + 4;
constexpr std::size_t TICKS_UP_TO_THE_FIRST_CATCH_UP = INPUT_CATCH_UP_TICKS + 2;
constexpr std::size_t RELEASED_BY_THE_FIRST_CATCH_UP = INPUT_CATCH_UP_TICKS + 3;

}  // namespace rtype::server::testing
