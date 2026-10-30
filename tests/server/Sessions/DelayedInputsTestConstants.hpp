#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

constexpr std::size_t SOME_DELAY_TICKS = 3;
constexpr std::size_t NO_DELAY_TICKS = 0;
constexpr std::size_t TOO_MANY_DELAY_TICKS = 31;
constexpr std::size_t SECOND_PLAYER_INDEX = 1;
constexpr std::uint32_t FIRST_HELD_INPUT_NUMBER = 1;
constexpr std::uint32_t SECOND_HELD_INPUT_NUMBER = 2;
constexpr std::uint8_t FIRST_HELD_ACTIONS = 0x01;
constexpr std::uint8_t SECOND_HELD_ACTIONS = 0x02;

}  // namespace rtype::server::testing
