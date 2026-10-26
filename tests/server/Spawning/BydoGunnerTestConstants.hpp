#pragma once

#include <cstddef>
#include "Position.hpp"

namespace rtype::server::testing {

/**
 * @brief The fire rate chosen for the Bydos: one and a half seconds at 60 ticks
 * per second, how long a Bydo waits after it appears before its first missile,
 * and between two missiles.
 */
constexpr std::size_t ONE_AND_A_HALF_SECONDS_IN_TICKS = 90;

/** @brief Where the gunner tests make a Bydo appear: on screen, above the
 * status band. */
constexpr game::Position SOME_BYDO{.x = 1000.0F, .y = 300.0F};

/** @brief Where the gunner tests make a second Bydo appear, on another row. */
constexpr game::Position OTHER_BYDO{.x = 1400.0F, .y = 600.0F};

/**
 * @brief Where the missile of SOME_BYDO starts: the left side of the Bydo is
 * half of its 96 units before its center, at 952, and the center of the missile
 * is half of its 32 units further, at 936.
 */
constexpr float SOME_BYDO_MISSILE_X = 936.0F;

/** @brief Half a second at 60 ticks per second: how long after SOME_BYDO the
 * test of two Bydos makes OTHER_BYDO appear. */
constexpr std::size_t HALF_A_SECOND_IN_TICKS = 30;

}  // namespace rtype::server::testing
