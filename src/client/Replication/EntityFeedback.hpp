#pragma once

#include <functional>

namespace rtype::client {

class EffectSpawner;
class ISoundPlayer;

/** @brief What the player sees and hears of what the server announces: the
 * effects of the game screen and the sounds of the client. */
struct EntityFeedback {
  std::reference_wrapper<EffectSpawner> effects;
  std::reference_wrapper<ISoundPlayer> sounds;
};

}  // namespace rtype::client
