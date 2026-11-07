#pragma once

#include <chrono>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "HurtBeam.hpp"
#include "HurtWindow.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief The beams of the tests start on the enemy, at the origin, and are
 * 400 units long and 20 thick. The straight one heads left, along y = 0 from
 * x = -400 to 0, in the strike of attack 2. The ship is the square of
 * CollisionScene, 10 units wide: centered at x = -200 it is on the beam; at
 * y = -30 it is above it (its bottom edge at -25, the top of the beam at
 * -10); at x = -420 it is past the end of the beam; at x = -405 its right
 * edge only touches that end.
 */
constexpr float BEAM_LENGTH = 400.0F;
constexpr float BEAM_THICKNESS = 20.0F;
constexpr float SHIP_ON_THE_BEAM_X = -200.0F;
constexpr float SHIP_ABOVE_THE_BEAM_Y = -30.0F;
constexpr float SHIP_PAST_THE_BEAM_X = -420.0F;
constexpr float SHIP_TOUCHING_THE_END_X = -405.0F;

constexpr ActionKey BEAM_STRIKE{.attack = 2, .phase = EnemyPhase::Strike};
constexpr ActionKey BEAM_SIGNAL{.attack = 2, .phase = EnemyPhase::Signal};
constexpr HurtBeam STRAIGHT_BEAM{.offsetX = 0.0F,
                                 .offsetY = 0.0F,
                                 .length = BEAM_LENGTH,
                                 .thickness = BEAM_THICKNESS,
                                 .fromDegrees = 0.0F,
                                 .toDegrees = 0.0F,
                                 .activeIn = BEAM_STRIKE,
                                 .window = HurtWindow{}};

/**
 * @brief A beam that sweeps from the left (0 degrees) down to straight down
 * (90 degrees) over the first second of the strike of attack 15: half a
 * second in, it heads 45 degrees down and left, and a ship centered 200 units
 * along that line, at (-141.42, 141.42), is on it; 100 ms in it heads 9
 * degrees down, 36 degrees away from that ship, about 117 units from it.
 */
constexpr ActionKey SWEEP_STRIKE{.attack = 15, .phase = EnemyPhase::Strike};
constexpr engine::Duration SWEEP_LENGTH = std::chrono::seconds(1);
constexpr float SWEEP_TO_DEGREES = 90.0F;
constexpr HurtBeam SWEEPING_BEAM{
    .offsetX = 0.0F,
    .offsetY = 0.0F,
    .length = BEAM_LENGTH,
    .thickness = BEAM_THICKNESS,
    .fromDegrees = 0.0F,
    .toDegrees = SWEEP_TO_DEGREES,
    .activeIn = SWEEP_STRIKE,
    .window =
        HurtWindow{.from = engine::Duration::zero(), .until = SWEEP_LENGTH}};
constexpr engine::Duration HALFWAY_THROUGH_THE_SWEEP =
    std::chrono::milliseconds(500);
constexpr engine::Duration EARLY_IN_THE_SWEEP = std::chrono::milliseconds(100);
constexpr float SHIP_HALFWAY_DOWN_THE_SWEEP_X = -141.42F;
constexpr float SHIP_HALFWAY_DOWN_THE_SWEEP_Y = 141.42F;

}  // namespace rtype::game::testing
