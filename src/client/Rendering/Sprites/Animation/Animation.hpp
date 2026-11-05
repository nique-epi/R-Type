#pragma once

#include <optional>
#include <span>
#include "FrameAnchor.hpp"
#include "Playback.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief An animation described as data: the rectangles of a sheet shown in
 * turn, how long each one stays, and whether it loops or plays once.
 *
 * The frame shown comes from the time elapsed since the animation started,
 * never from the number of pictures drawn: at 60 or at 10 pictures per second
 * the same frame shows at the same moment, and one long step lands on the
 * frame due at its end. The rectangles belong to the sheet the Sprite of the
 * entity names. The frames are only viewed: the array they come from must
 * outlive the animation, which a constant array does.
 */
struct Animation {
  std::span<const game::SpriteFrame> frames;
  engine::Duration frameDuration{};
  Playback playback{};
  /** @brief How many pixels of the canvas each frame is drawn away from the
   * center of the entity: a frame wider than the body hangs from the side it
   * grows on. */
  FrameAnchor anchor{};

  /**
   * @param elapsed Time since the animation started. A negative time shows
   * the first frame: an effect started at minus a hold keeps its first frame
   * that much longer.
   * @returns Frame floor(elapsed / frameDuration), taken modulo the number of
   * frames for a loop and capped between the first and the last frame for an
   * animation played once or held; nothing when there is no frame or
   * frameDuration is not positive.
   */
  [[nodiscard]] std::optional<game::SpriteFrame> frameAt(
      engine::Duration elapsed) const;

  /**
   * @returns true for an animation played once or held whose last frame has
   * shown for frameDuration, that is once elapsed reaches cycle(); a loop never
   * ends.
   */
  [[nodiscard]] bool hasEnded(engine::Duration elapsed) const;

  /** @returns The time one pass over every frame takes: the number of frames
   * times frameDuration. */
  [[nodiscard]] engine::Duration cycle() const;
};

}  // namespace rtype::client
