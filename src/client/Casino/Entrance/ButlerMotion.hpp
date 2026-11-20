#pragma once

#include "ButlerPose.hpp"
#include "EntranceStage.hpp"
#include "MotionPreference.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns Frame @p frame, 0 to BUTLER_FRAMES_PER_POSE - 1, of @p pose on the
 * sheet of the butler. */
[[nodiscard]] game::SpriteFrame butlerFrameOf(ButlerPose pose, int frame);

/**
 * @returns The frame of the butler @p since the casino reached @p stage:
 * before the doors and in the hall, his idle frames round and round; while
 * the doors open, his greeting then his bow, the last frame of the bow held
 * past OPENING_DURATION. With MotionPreference::Reduced, his first idle frame
 * whatever the time.
 */
[[nodiscard]] game::SpriteFrame butlerFrameAt(EntranceStage stage,
                                              engine::Duration since,
                                              MotionPreference motion);

/** @returns The picture of the front of the casino @p since it reached
 * @p stage: the doors closed before them and during the greeting, open from
 * DOORS_OPEN_AFTER on and in the hall. */
[[nodiscard]] const char* doorsPictureAt(EntranceStage stage,
                                         engine::Duration since);

/** @returns The column of the left of the frame of the butler at @p stage:
 * beside the doors until the hall shows, aside in the hall. */
[[nodiscard]] int butlerLeftAt(EntranceStage stage);

}  // namespace rtype::client
