#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief A new ship listed at START (400, 300) once the server applied
 * input 1: the command of input 2 fires from its muzzle, 96 units ahead at
 * 496, and flies one tick, 16 units, by input 3. */
constexpr std::uint32_t APPLIED_BEFORE_THE_NEW_SHIP = 1;
constexpr float MISSILE_OF_THE_NEW_SHIP_AT_INPUT_THREE = 512.0F;

}  // namespace rtype::client::testing
