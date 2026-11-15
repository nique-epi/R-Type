#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"
#include "SettingId.hpp"

namespace rtype::client {

float musicVolumeOf(const PlayerSettings& settings) {
  return settings.muted ? 0.0F : static_cast<float>(settings.musicVolume);
}

float effectsVolumeOf(const PlayerSettings& settings) {
  return settings.muted ? 0.0F : static_cast<float>(settings.effectsVolume);
}

float voicesVolumeOf(const PlayerSettings& settings) {
  return settings.muted ? 0.0F : static_cast<float>(settings.voicesVolume);
}

bool changesMusicVolume(SettingId changed) {
  return changed == SettingId::MusicVolume || changed == SettingId::Muted;
}

bool changesEffectsVolume(SettingId changed) {
  return changed == SettingId::EffectsVolume || changed == SettingId::Muted;
}

bool changesVoicesVolume(SettingId changed) {
  return changed == SettingId::VoicesVolume || changed == SettingId::Muted;
}

}  // namespace rtype::client
