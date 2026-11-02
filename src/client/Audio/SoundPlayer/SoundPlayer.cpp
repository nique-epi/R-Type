#include "SoundPlayer.hpp"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <array>
#include <cstddef>
#include <span>
#include <string_view>
#include "AssetLibrary.hpp"
#include "AudioConstants.hpp"
#include "AudioVolume.hpp"
#include "SettingId.hpp"
#include "SettingsService.hpp"
#include "VoiceChoice.hpp"
#include "VoiceState.hpp"

namespace rtype::client {

SoundPlayer::SoundPlayer(const AssetLibrary& library,
                         const SettingsService& settings)
    : library_(&library),
      settings_(&settings),
      volume_(effectsVolumeOf(settings.settings())) {
  voices_.reserve(SOUND_VOICE_LIMIT);
  startRanks_.reserve(SOUND_VOICE_LIMIT);
}

void SoundPlayer::play(std::string_view soundId) {
  if (volume_ <= 0.0F) {
    return;
  }
  sf::Sound& voice = voiceFor(library_->sound(soundId));
  voice.setVolume(volume_);
  voice.play();
}

void SoundPlayer::settingChanged(SettingId changed) {
  if (changesEffectsVolume(changed)) {
    volume_ = effectsVolumeOf(settings_->settings());
  }
}

std::array<VoiceState, SOUND_VOICE_LIMIT> SoundPlayer::voiceStates() const {
  std::array<VoiceState, SOUND_VOICE_LIMIT> states{};
  for (std::size_t voice = 0; voice < voices_.size(); ++voice) {
    states.at(voice) = VoiceState{.playing = voices_[voice].getStatus() ==
                                             sf::SoundSource::Status::Playing,
                                  .startRank = startRanks_[voice]};
  }
  return states;
}

sf::Sound& SoundPlayer::voiceFor(const sf::SoundBuffer& buffer) {
  const std::array<VoiceState, SOUND_VOICE_LIMIT> states = voiceStates();
  const std::size_t chosen = voiceForNextSound(
      std::span(states).first(voices_.size()), SOUND_VOICE_LIMIT);
  if (chosen == voices_.size()) {
    voices_.emplace_back(buffer);
    startRanks_.push_back(startedSounds_);
  } else {
    voices_[chosen].stop();
    voices_[chosen].setBuffer(buffer);
    startRanks_[chosen] = startedSounds_;
  }
  ++startedSounds_;
  return voices_[chosen];
}

}  // namespace rtype::client
