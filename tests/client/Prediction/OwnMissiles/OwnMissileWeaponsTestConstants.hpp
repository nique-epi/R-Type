#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::client::testing {

/** @brief The fan of the double flies its outer missiles 12 degrees off the
 * heading: three ticks after the input that fired them they are 16 + 48 = 64
 * units from the muzzle along their heading, 64 sin 12 degrees, 13.306 units,
 * above and below it. */
constexpr float FAN_OFFSET_AFTER_THREE_TICKS = 13.306348F;
constexpr float MUZZLE_ROW = 300.0F;
constexpr float POSITION_TOLERANCE = 0.01F;

/** @brief Where the server put the upper missile of the fan, the third of
 * the shot (rank 2): 4 units ahead of where the client laid it, at 296 + 16
 * cos 12 degrees and 300 - 16 sin 12 degrees. */
constexpr std::uint8_t UPPER_MISSILE_RANK = 2;
constexpr game::Position SERVER_START_OF_THE_UPPER_MISSILE{.x = 315.650362F,
                                                           .y = 296.673413F};

/** @brief Commands holding shoot up to 20 fire on 1 and 11 with the rows of
 * the double, two missiles a shot: four missiles. */
constexpr std::uint32_t TWENTIETH_COMMAND = 20;
constexpr std::size_t MISSILES_OF_TWO_SHOTS_OF_ROWS = 4;

}  // namespace rtype::client::testing
