#pragma once

#include <cstddef>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief Where the enemy that drops the crystal died, and the two players
 * told about it. */
constexpr game::Position WHERE_THE_CARRIER_DIED{.x = 900.0F, .y = 420.0F};
constexpr std::size_t FIRST_DROP_PLAYER = 0;
constexpr std::size_t SECOND_DROP_PLAYER = 1;

}  // namespace rtype::server::testing
