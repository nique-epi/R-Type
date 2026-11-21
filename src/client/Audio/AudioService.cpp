#include "AudioService.hpp"
#include <optional>
#include "Ambience.hpp"
#include "AmbienceMusic.hpp"
#include "AssetLibrary.hpp"
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "LevelTracks.hpp"
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
      bossMusic_(settings, BOSS_TRACK),
      ambiences_(settings),
      levelMusic_(settings),
      soundtrack_(SoundtrackParts{
          .theme = theme_, .gameMusic = gameMusic_, .bossMusic = bossMusic_}) {
  settings_->subscribe(sounds_);
  settings_->subscribe(theme_);
  settings_->subscribe(gameMusic_);
  settings_->subscribe(bossMusic_);
}

AudioService::~AudioService() {
  settings_->unsubscribe(bossMusic_);
  settings_->unsubscribe(gameMusic_);
  settings_->unsubscribe(theme_);
  settings_->unsubscribe(sounds_);
}

ISoundPlayer& AudioService::sounds() { return sounds_; }

IMusicPlayer& AudioService::theme() { return theme_; }

void AudioService::playTheme() { soundtrack_.playTheme(); }

void AudioService::playGameMusic(const LevelTracks& tracks) {
  if (tracks.music.has_value()) {
    IMusicPlayer* const boss =
        tracks.bossMusic.has_value()
            ? &levelMusic_.playerOf(*tracks.bossMusic, *library_)
            : nullptr;
    soundtrack_.useLevelMusic(levelMusic_.playerOf(*tracks.music, *library_),
                              boss);
  }
  soundtrack_.playGameMusic();
}

bool AudioService::silenceMusic() { return soundtrack_.silenceMusic(); }

void AudioService::playBossMusic() { soundtrack_.playBossMusic(); }

void AudioService::setAmbience(Ambience ambience) {
  if (ambience == ambience_) {
    return;
  }
  ambience_ = ambience;
  if (replacesTheTheme(ambience)) {
    soundtrack_.pauseTheme();
  } else {
    soundtrack_.resumeTheme();
  }
  ambiences_.play(ambience);
}

Ambience AudioService::ambience() const { return ambience_; }

void AudioService::open() {
  bank_.load(*library_, ALL_SOUND_IDS);
  theme_.open(*library_);
  gameMusic_.open(*library_);
  bossMusic_.open(*library_);
  ambiences_.open(*library_);
}

void AudioService::update(engine::Duration elapsed) {
  soundtrack_.advance(elapsed);
}

}  // namespace rtype::client
