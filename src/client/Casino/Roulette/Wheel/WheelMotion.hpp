#pragma once

#include <cstdint>
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"
#include "WheelMoment.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

/** @returns The frame of the turns of the rotor on which @p number, 0 to
 * HIGHEST_ROULETTE_NUMBER, faces the top of the wheel, where the ball
 * stops. */
[[nodiscard]] int stoppingFrameOf(std::uint8_t number);

/** @returns How many frames the rotor still turns when @p timeLeft is left of
 * the spin: one per ROTOR_FASTEST_FRAME_TIME, then the steps of
 * ROTOR_SLOWDOWN, a frame begun counted whole; none once the spin ended. */
[[nodiscard]] int rotorFramesLeft(engine::Duration timeLeft);

/** @returns Whether the ball already lies in its pocket when @p timeLeft is
 * left of the spin: once the rotor has BALL_DROP_FRAMES frames or fewer left
 * to turn. */
[[nodiscard]] bool ballDropped(engine::Duration timeLeft);

/**
 * @returns What the wheel shows at @p moment: during the bets, the rotor at
 * rest and no ball; while it spins, the rotor turning and slowing so that it
 * stops on stoppingFrameOf() the number drawn as the spin ends, the ball
 * running round the track then dropping in the winning pocket; at the
 * result, the rotor stopped and the ball at the top, in its pocket. With
 * MotionPreference::Reduced, the rotor stays at rest and the ball out of play
 * until the result.
 */
[[nodiscard]] WheelPicture wheelPictureAt(const WheelMoment& moment,
                                          MotionPreference motion);

}  // namespace rtype::client
