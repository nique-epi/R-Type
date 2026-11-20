#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "ButlerPose.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns Frame @p frame, 0 to BUTLER_FRAMES_PER_POSE - 1, of @p pose on the
 * sheet of the butler, who greets at the doors and spins the roulette. */
[[nodiscard]] sf::IntRect butlerFrameOf(ButlerPose pose, int frame);

/** @returns The idle frame of the butler @p since a moment, round and round,
 * one every BUTLER_IDLE_FRAME_TIME. */
[[nodiscard]] sf::IntRect idleButlerFrameAt(engine::Duration since);

/** @returns The frame of @p pose @p since it began, one every
 * BUTLER_GESTURE_FRAME_TIME, its last frame held once the pose ended. */
[[nodiscard]] sf::IntRect gestureButlerFrameAt(ButlerPose pose,
                                               engine::Duration since);

}  // namespace rtype::client
