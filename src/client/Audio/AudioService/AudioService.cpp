#include "AudioService.hpp"
#include "AssetLibrary.hpp"
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "PlayerSettings.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

AudioService::AudioService(AssetLibrary& library,
                           const PlayerSettings& settings)
    : library_(&library),
      sounds_(library, settings),
      theme_(settings, THEME_TRACK),
      gameMusic_(settings, GAME_TRACK),
      soundtrack_(SoundtrackParts{.theme = theme_, .gameMusic = gameMusic_}) {}

ISoundPlayer& AudioService::sounds() { return sounds_; }

IMusicPlayer& AudioService::theme() { return theme_; }

void AudioService::playTheme() { soundtrack_.playTheme(); }

void AudioService::playGameMusic() { soundtrack_.playGameMusic(); }

void AudioService::open() {
  theme_.open(*library_);
  gameMusic_.open(*library_);
}

void AudioService::update(engine::Duration elapsed) {
  theme_.update();
  gameMusic_.update();
  soundtrack_.advance(elapsed);
}

}  // namespace rtype::client
