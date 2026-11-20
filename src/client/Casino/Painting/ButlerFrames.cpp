#include "ButlerFrames.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include "ButlerPose.hpp"
#include "CasinoSpriteConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

sf::IntRect butlerFrameOf(ButlerPose pose, int frame) {
  return {{frame * BUTLER_FRAME_SIZE.x,
           static_cast<int>(pose) * BUTLER_FRAME_SIZE.y},
          BUTLER_FRAME_SIZE};
}

sf::IntRect idleButlerFrameAt(engine::Duration since) {
  return butlerFrameOf(ButlerPose::Idle,
                       static_cast<int>((since / BUTLER_IDLE_FRAME_TIME) %
                                        BUTLER_FRAMES_PER_POSE));
}

sf::IntRect gestureButlerFrameAt(ButlerPose pose, engine::Duration since) {
  const auto frame = since / BUTLER_GESTURE_FRAME_TIME;
  return butlerFrameOf(pose, static_cast<int>(std::min<engine::Duration::rep>(
                                 frame, BUTLER_FRAMES_PER_POSE - 1)));
}

}  // namespace rtype::client
