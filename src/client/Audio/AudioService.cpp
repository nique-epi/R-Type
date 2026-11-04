#include "AudioService.hpp"
#include "AssetLibrary.hpp"
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "SettingsService.hpp"
#include "SoundConstants.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

AudioService::AudioService(AssetLibrary& library, SettingsService& settings)
    : library_(&library),
      settings_(&settings),
      sounds_(bank_, settings),
      theme_(settings, THEME_TRACK),
      gameMusic_(settings, GAME_TRACK),
      soundtrack_(SoundtrackParts{.theme = theme_, .gameMusic = gameMusic_}) {
  settings_->subscribe(sounds_);
  settings_->subscribe(theme_);
  settings_->subscribe(gameMusic_);
}

AudioService::~AudioService() {
  settings_->unsubscribe(gameMusic_);
  settings_->unsubscribe(theme_);
  settings_->unsubscribe(sounds_);
}

ISoundPlayer& AudioService::sounds() {
  if (bank_.isEmpty()) {
    return silence_;
  }
  return sounds_;
}

IMusicPlayer& AudioService::theme() { return theme_; }

void AudioService::playTheme() { soundtrack_.playTheme(); }

void AudioService::playGameMusic() { soundtrack_.playGameMusic(); }

void AudioService::open() {
  bank_.load(*library_, ALL_SOUND_IDS);
  theme_.open(*library_);
  gameMusic_.open(*library_);
}

void AudioService::update(engine::Duration elapsed) {
  soundtrack_.advance(elapsed);
}

}  // namespace rtype::client
