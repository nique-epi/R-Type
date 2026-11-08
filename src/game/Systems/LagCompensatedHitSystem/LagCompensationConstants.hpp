#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief The furthest in the past anything is tested, the longest rewind a
 * mode of play allows (see IHitArbitration): 400 ms, measured enough for a
 * player at a round trip of 200 ms, whose view is 23 ticks behind. */
constexpr engine::Duration MAXIMUM_REWIND = std::chrono::milliseconds{400};

/** @brief The whole ticks of MAXIMUM_REWIND: 24 at 60 ticks per second. */
constexpr std::size_t MAXIMUM_REWIND_TICKS =
    engine::wholeTicksIn(MAXIMUM_REWIND);

/** @brief The ticks of enemies kept: the present and the MAXIMUM_REWIND_TICKS
 * before it. */
constexpr std::size_t KEPT_ENEMY_TICKS = MAXIMUM_REWIND_TICKS + 1;

}  // namespace rtype::game
