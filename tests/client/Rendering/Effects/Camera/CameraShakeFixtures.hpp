#pragma once

/** @brief The largest offset on either axis over a second, the trauma kept
 * where it was, and whether the camera moved at all. */
struct ShakeOverASecond {
  int largest{};
  bool moved = false;
};

/** @returns How a camera shakes over FRAMES_IN_A_SECOND frames of ONE_FRAME,
 * its trauma brought back to @p trauma before each frame. */
ShakeOverASecond shakeKeeping(float trauma);
