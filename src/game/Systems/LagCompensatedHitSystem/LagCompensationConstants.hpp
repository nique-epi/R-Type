#pragma once

#include <cstddef>

namespace rtype::game {

/** @brief The most ticks a missile is tested in the past: 12 ticks, 200 ms at
 * 60 ticks per second, the cap the protocol page names. */
constexpr std::size_t MAXIMUM_REWIND_TICKS = 12;

/** @brief The ticks of enemies kept: the present and the MAXIMUM_REWIND_TICKS
 * before it. */
constexpr std::size_t KEPT_ENEMY_TICKS = MAXIMUM_REWIND_TICKS + 1;

}  // namespace rtype::game
