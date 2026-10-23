#pragma once

#include "Animation.hpp"

namespace rtype::client {

/**
 * @brief The two versions of an effect: the standard one, and the one played
 * when the player turned REDUCE FLASHES on, which shows no white burst. Both
 * have as many frames, each as long.
 */
struct EffectAnimations {
  Animation standard;
  Animation withoutFlash;
};

}  // namespace rtype::client
