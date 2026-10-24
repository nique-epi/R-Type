#pragma once

#include <cstddef>

namespace rtype::server::testing {

/**
 * @brief The fire rate chosen for the game: a ship whose player holds shoot
 * fires three missiles per second, one every 20 ticks at 60 ticks per second.
 */
constexpr std::size_t MISSILES_PER_SECOND_OF_FIRE = 3;
constexpr std::size_t TICKS_BETWEEN_TWO_SHOTS = 20;

}  // namespace rtype::server::testing
