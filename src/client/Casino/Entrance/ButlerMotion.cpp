#include "ButlerMotion.hpp"
#include <algorithm>
#include "ButlerPose.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "EntranceStage.hpp"
#include "MotionPreference.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The idle frame shown @p since the stage began, round and round. */
game::SpriteFrame idleFrameAt(engine::Duration since) {
  return butlerFrameOf(ButlerPose::Idle,
                       static_cast<int>((since / BUTLER_IDLE_FRAME_TIME) %
                                        BUTLER_FRAMES_PER_POSE));
}

/** @returns The frame of the greeting then of the bow shown @p since the
 * doors began to open, the last one held once they ended. */
game::SpriteFrame openingFrameAt(engine::Duration since) {
  const int lastStep = (BUTLER_FRAMES_PER_POSE * OPENING_POSE_COUNT) - 1;
  const int step =
      std::min(static_cast<int>(since / OPENING_FRAME_TIME), lastStep);
  return butlerFrameOf(
      step < BUTLER_FRAMES_PER_POSE ? ButlerPose::Greeting : ButlerPose::Bowing,
      step % BUTLER_FRAMES_PER_POSE);
}

}  // namespace

game::SpriteFrame butlerFrameOf(ButlerPose pose, int frame) {
  return game::SpriteFrame{.left = frame * BUTLER_FRAME_SIZE.x,
                           .top = static_cast<int>(pose) * BUTLER_FRAME_SIZE.y,
                           .width = BUTLER_FRAME_SIZE.x,
                           .height = BUTLER_FRAME_SIZE.y};
}

game::SpriteFrame butlerFrameAt(EntranceStage stage, engine::Duration since,
                                MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return butlerFrameOf(ButlerPose::Idle, 0);
  }
  return stage == EntranceStage::Opening ? openingFrameAt(since)
                                         : idleFrameAt(since);
}

const char* doorsPictureAt(EntranceStage stage, engine::Duration since) {
  switch (stage) {
    case EntranceStage::Doors:
      return CASINO_DOORS_CLOSED_ID;
    case EntranceStage::Opening:
      return since < DOORS_OPEN_AFTER ? CASINO_DOORS_CLOSED_ID
                                      : CASINO_DOORS_OPEN_ID;
    case EntranceStage::Hall:
      break;
  }
  return CASINO_DOORS_OPEN_ID;
}

int butlerLeftAt(EntranceStage stage) {
  return stage == EntranceStage::Hall ? BUTLER_IN_THE_HALL_LEFT
                                      : BUTLER_AT_THE_DOORS_LEFT;
}

}  // namespace rtype::client
