#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "EntranceStage.hpp"
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns The frame of the butler @p since the casino reached @p stage:
 * before the doors, his idle frames round and round; while the doors open,
 * his greeting then his bow, the last frame of the bow held past
 * OPENING_DURATION. With MotionPreference::Reduced, his first idle frame
 * whatever the time.
 */
[[nodiscard]] sf::IntRect butlerFrameAt(EntranceStage stage,
                                        engine::Duration since,
                                        MotionPreference motion);

/** @returns The picture of the front of the casino @p since it reached
 * @p stage: the doors closed before them and during the greeting, open from
 * DOORS_OPEN_AFTER on. */
[[nodiscard]] const char* doorsPictureAt(EntranceStage stage,
                                         engine::Duration since);

}  // namespace rtype::client
