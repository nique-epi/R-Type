#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/** @brief The ticks the game needs after the last Bydo is gone to find it. */
constexpr std::size_t TICKS_TO_FIND_THE_SKY_CLEAR = 1;

/** @brief A second of ticks: the Bydos of the wave are still far from the
 * ships. */
constexpr std::size_t A_SECOND_OF_TICKS =
    engine::wholeTicksIn(std::chrono::seconds(1));

/** @brief The ticks from the sky clear of the last wave to the boss: the
 * tick that finds the last wave over orders it, and it appears at the end of
 * the next tick. One more tick, and the boss system warned the players. */
constexpr std::size_t TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS =
    TICKS_TO_FIND_THE_SKY_CLEAR + 1;
constexpr std::size_t TICKS_FROM_THE_CLEAR_SKY_TO_THE_WARNING =
    TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS + 1;

}  // namespace rtype::server::testing
