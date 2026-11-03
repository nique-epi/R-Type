#pragma once

#include <functional>
#include "IMusicPlayer.hpp"

namespace rtype::client {

/** @brief What a soundtrack switches between, named so the two cannot be
 * swapped: the music outside a game, which the intro starts, and the music
 * of a game. Both must outlive the soundtrack. */
struct SoundtrackParts {
  std::reference_wrapper<IMusicPlayer> theme;
  std::reference_wrapper<IMusicPlayer> gameMusic;
};

}  // namespace rtype::client
