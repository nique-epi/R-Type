#pragma once

#include "BeamLine.hpp"
#include "Bounds.hpp"
#include "Direction.hpp"

namespace rtype::game::testing {

/** @brief A beam from the origin heading right, 100 units long and 10 thick:
 * it covers x from 0 to 100 and y from -5 to 5. A box 4 units wide at
 * (50, 0) is inside it; a box 10 units wide at (120, 0) is past its end. */
constexpr BeamLine BEAM_HEADING_RIGHT{
    .startX = 0.0F,
    .startY = 0.0F,
    .heading = Direction{.x = 1.0F, .y = 0.0F},
    .length = 100.0F,
    .thickness = 10.0F};
constexpr engine::Bounds BOX_ACROSS_THE_MIDDLE{
    .centerX = 50.0F, .centerY = 0.0F, .width = 4.0F, .height = 4.0F};
constexpr engine::Bounds BOX_OFF_THE_END{
    .centerX = 120.0F, .centerY = 0.0F, .width = 10.0F, .height = 10.0F};

/**
 * @brief A beam from the origin heading 45 degrees down and right, 100 units
 * long and 10 thick, and a box 10 units wide at (60, 10): the box is inside
 * the rectangle that bounds the beam on the two axes of the box, but 35 units
 * across the beam from its line, where the box and the beam reach 7 and 5:
 * only the axis across the beam separates them.
 */
constexpr float HALF_OF_THE_SQUARE_ROOT_OF_TWO = 0.70710677F;
constexpr BeamLine BEAM_TILTED_DOWN{
    .startX = 0.0F,
    .startY = 0.0F,
    .heading = Direction{.x = HALF_OF_THE_SQUARE_ROOT_OF_TWO,
                         .y = HALF_OF_THE_SQUARE_ROOT_OF_TWO},
    .length = 100.0F,
    .thickness = 10.0F};
constexpr engine::Bounds BOX_BESIDE_THE_TILTED_BEAM{
    .centerX = 60.0F, .centerY = 10.0F, .width = 10.0F, .height = 10.0F};

}  // namespace rtype::game::testing
