#pragma once

#include <cstddef>

namespace rtype::game::testing {

/** @brief A body summoned for three ticks has two left after one tick; a
 * body on its last tick of summoning takes part after it. */
constexpr std::size_t THREE_TICKS_OF_SUMMONING = 3;
constexpr std::size_t TWO_TICKS_OF_SUMMONING = 2;
constexpr std::size_t LAST_TICK_OF_SUMMONING = 1;

}  // namespace rtype::game::testing
