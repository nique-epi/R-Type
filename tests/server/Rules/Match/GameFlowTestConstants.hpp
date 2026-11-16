#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "BossStageConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/** @brief The ticks the game needs after the last Bydo is gone to find it. */
constexpr std::size_t TICKS_TO_FIND_THE_SKY_CLEAR = 1;

/** @brief A look item a player won, the first of the catalog. */
constexpr std::uint8_t A_WON_LOOK_NUMBER = 1;

/** @brief A second of ticks: the Bydos of the wave are still far from the
 * ships. */
constexpr std::size_t A_SECOND_OF_TICKS =
    engine::wholeTicksIn(std::chrono::seconds(1));

/** @brief The ticks from the sky clear of the last wave to the boss: the
 * tick that finds the last wave over announces it, the warning runs
 * BOSS_ARRIVAL_WARNING_TICKS ticks, and the next tick brings it in. */
constexpr std::size_t TICKS_FROM_THE_CLEAR_SKY_TO_THE_BOSS =
    TICKS_TO_FIND_THE_SKY_CLEAR + BOSS_ARRIVAL_WARNING_TICKS + 1;

}  // namespace rtype::server::testing
