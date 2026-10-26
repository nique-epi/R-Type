#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

float musicVolumeOf(const PlayerSettings& settings) {
  return settings.muted ? 0.0F : static_cast<float>(settings.musicVolume);
}

float effectsVolumeOf(const PlayerSettings& settings) {
  return settings.muted ? 0.0F : static_cast<float>(settings.effectsVolume);
}

}  // namespace rtype::client
