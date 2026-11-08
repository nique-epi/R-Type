#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A music of the client: its asset id, the part of its file that
 * loops, from @c loopStart for @c loopLength, and where its first beat falls
 * in the file, which its position counts from (MusicPlayer). */
struct MusicTrack {
  const char* assetId{};
  engine::Duration loopStart{};
  engine::Duration loopLength{};
  engine::Duration firstBeat{};
  /** @brief How loud the track plays against the MUSIC volume of the player,
   * from 0 to 1: 1 for a music that carries the screen, less for one that
   * plays under another. */
  float gain{1.0F};
};

}  // namespace rtype::client
