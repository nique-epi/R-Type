#pragma once

#include "ConnectionConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Tells, frame after frame, whether the game sends a PlayerInput in that
 * frame: every PLAYER_INPUT_INTERVAL, and never twice in one frame.
 *
 * The first frame sends at once. The time a frame lasts beyond the interval
 * counts toward the next input, so frames a little shorter or longer than the
 * interval still send PLAYER_INPUTS_PER_SECOND inputs per second. When two
 * intervals or more have built up, one input is sent and the rest of that time
 * is dropped: the actions held do not change within a frame, so a burst of
 * inputs would carry nothing new. Below PLAYER_INPUTS_PER_SECOND frames per
 * second, the game therefore sends one input per frame.
 */
class PlayerInputPacer {
 public:
  /**
   * @param elapsed Time the frame lasted, never negative.
   * @returns Whether the game sends a PlayerInput in this frame.
   */
  [[nodiscard]] bool advance(engine::Duration elapsed);

 private:
  engine::Duration sinceLastInput_{PLAYER_INPUT_INTERVAL};
};

}  // namespace rtype::client
