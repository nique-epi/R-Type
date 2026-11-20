#include "ButlerMotion.hpp"
#include <SFML/Graphics/Rect.hpp>
#include "ButlerFrames.hpp"
#include "ButlerPose.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "EntranceStage.hpp"
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The frame of the greeting then of the bow shown @p since the
 * doors began to open, the last one held once they ended. */
sf::IntRect openingFrameAt(engine::Duration since) {
  return since < DOORS_OPEN_AFTER
             ? gestureButlerFrameAt(ButlerPose::Greeting, since)
             : gestureButlerFrameAt(ButlerPose::Bowing,
                                    since - DOORS_OPEN_AFTER);
}

}  // namespace

sf::IntRect butlerFrameAt(EntranceStage stage, engine::Duration since,
                          MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return butlerFrameOf(ButlerPose::Idle, 0);
  }
  return stage == EntranceStage::Opening ? openingFrameAt(since)
                                         : idleButlerFrameAt(since);
}

const char* doorsPictureAt(EntranceStage stage, engine::Duration since) {
  return stage == EntranceStage::Opening && since >= DOORS_OPEN_AFTER
             ? CASINO_DOORS_OPEN_ID
             : CASINO_DOORS_CLOSED_ID;
}

}  // namespace rtype::client
