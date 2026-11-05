#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What happens to an Animation after its last frame. */
enum class Playback : std::uint8_t {
  /** @brief Starts again from the first frame. */
  Loop,
  /** @brief Stays on the last frame, then ends once it has shown for
   * frameDuration; AnimationSystem then destroys the entity showing it, so
   * only effects play once. */
  Once,
  /** @brief Stays on the last frame, and ends like Once; AnimationSystem then
   * replaces it by the animation that follows it, or keeps the last frame when
   * none does. An attack of an enemy holds its last frame until the server
   * announces the next phase. */
  Hold,
};

}  // namespace rtype::client
