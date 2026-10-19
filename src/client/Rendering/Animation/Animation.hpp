#pragma once

#include <cstdint>
#include <optional>
#include <span>
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
  /** @brief What happens after the last frame. */
  enum class Playback : std::uint8_t {
    /** @brief Starts again from the first frame. */
    Loop,
    /** @brief Stays on the last frame, then ends once it has shown for
     * frameDuration; AnimationSystem then destroys the entity showing it, so
     * only effects play once. */
    Once,
  };

  std::span<const game::SpriteFrame> frames;
  engine::Duration frameDuration{};
  Playback playback{};

  /**
   * @param elapsed Time since the animation started, zero or more.
   * @returns Frame floor(elapsed / frameDuration), taken modulo the number of
   * frames for a loop and capped at the last frame for an animation played
   * once; nothing when there is no frame or frameDuration is not positive.
   */
  [[nodiscard]] std::optional<game::SpriteFrame> frameAt(
      engine::Duration elapsed) const;

  /**
   * @returns true for an animation played once whose last frame has shown for
   * frameDuration, that is once elapsed reaches cycle(); a loop never ends.
   */
  [[nodiscard]] bool hasEnded(engine::Duration elapsed) const;

  /** @returns The time one pass over every frame takes: the number of frames
   * times frameDuration. */
  [[nodiscard]] engine::Duration cycle() const;
};

}  // namespace rtype::client
