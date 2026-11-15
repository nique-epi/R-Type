#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The seats of the players of an announcer test. */
constexpr std::size_t SEATED_PLAYER = 0;
constexpr std::size_t PLAYER_IN_THE_ROOM = 1;

/** @brief The clock of a round, then the same clock a tick later. */
constexpr std::uint16_t SOME_TICKS_LEFT = 600;
constexpr std::uint16_t ONE_TICK_LATER = 599;

/** @brief The round after the first. */
constexpr std::uint16_t SECOND_ROUND = 1;

}  // namespace rtype::server::testing
