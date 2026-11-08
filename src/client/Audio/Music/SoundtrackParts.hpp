#pragma once

#include <functional>
#include "IMusicPlayer.hpp"

namespace rtype::client {

/** @brief What a soundtrack switches between, named so they cannot be
 * swapped: the music outside a game, which the intro starts, the music of a
 * game and the music of the boss. All must outlive the soundtrack. */
struct SoundtrackParts {
  std::reference_wrapper<IMusicPlayer> theme;
  std::reference_wrapper<IMusicPlayer> gameMusic;
  std::reference_wrapper<IMusicPlayer> bossMusic;
};

}  // namespace rtype::client
