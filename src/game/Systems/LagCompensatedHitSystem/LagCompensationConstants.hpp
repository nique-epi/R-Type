#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief The furthest in the past a missile is tested, the cap the protocol
 * page names. */
constexpr engine::Duration MAXIMUM_REWIND = std::chrono::milliseconds{200};

/** @brief The whole ticks of MAXIMUM_REWIND: 12 at 60 ticks per second. */
constexpr std::size_t MAXIMUM_REWIND_TICKS =
    engine::wholeTicksIn(MAXIMUM_REWIND);

/** @brief The ticks of enemies kept: the present and the MAXIMUM_REWIND_TICKS
 * before it. */
constexpr std::size_t KEPT_ENEMY_TICKS = MAXIMUM_REWIND_TICKS + 1;

}  // namespace rtype::game
