#include "AudioService.hpp"
#include "Ambience.hpp"
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
      bossMusic_(settings, BOSS_TRACK),
      spaceAmbience_(settings, SPACE_AMBIENCE_TRACK),
      hangarAmbience_(settings, HANGAR_AMBIENCE_TRACK),
      crowdAmbience_(settings, CROWD_AMBIENCE_TRACK),
      shopMusic_(settings, SHOP_MUSIC_TRACK),
      shopRadio_(settings, SHOP_RADIO_TRACK),
      soundtrack_(SoundtrackParts{
          .theme = theme_, .gameMusic = gameMusic_, .bossMusic = bossMusic_}) {
  settings_->subscribe(sounds_);
  settings_->subscribe(theme_);
  settings_->subscribe(gameMusic_);
  settings_->subscribe(bossMusic_);
  settings_->subscribe(spaceAmbience_);
  settings_->subscribe(hangarAmbience_);
  settings_->subscribe(crowdAmbience_);
  settings_->subscribe(shopMusic_);
  settings_->subscribe(shopRadio_);
}

AudioService::~AudioService() {
  settings_->unsubscribe(shopRadio_);
  settings_->unsubscribe(shopMusic_);
  settings_->unsubscribe(crowdAmbience_);
  settings_->unsubscribe(hangarAmbience_);
  settings_->unsubscribe(spaceAmbience_);
  settings_->unsubscribe(bossMusic_);
  settings_->unsubscribe(gameMusic_);
  settings_->unsubscribe(theme_);
  settings_->unsubscribe(sounds_);
}

ISoundPlayer& AudioService::sounds() { return sounds_; }

IMusicPlayer& AudioService::theme() { return theme_; }

void AudioService::playTheme() { soundtrack_.playTheme(); }

void AudioService::playGameMusic() { soundtrack_.playGameMusic(); }

void AudioService::silenceMusic() { soundtrack_.silenceMusic(); }

void AudioService::playBossMusic() { soundtrack_.playBossMusic(); }

void AudioService::setAmbience(Ambience ambience) {
  if (ambience == ambience_) {
    return;
  }
  spaceAmbience_.stop();
  hangarAmbience_.stop();
  crowdAmbience_.stop();
  shopMusic_.stop();
  shopRadio_.stop();
  ambience_ = ambience;
  if (ambience == Ambience::Shop || ambience == Ambience::ShopRadio) {
    soundtrack_.pauseTheme();
  } else {
    soundtrack_.resumeTheme();
  }
  switch (ambience) {
    case Ambience::Space:
      spaceAmbience_.start();
      break;
    case Ambience::Hangar:
      hangarAmbience_.start();
      crowdAmbience_.start();
      break;
    case Ambience::Shop:
      shopMusic_.start();
      break;
    case Ambience::ShopRadio:
      shopRadio_.start();
      break;
    case Ambience::None:
      break;
  }
}

Ambience AudioService::ambience() const { return ambience_; }

void AudioService::open() {
  bank_.load(*library_, ALL_SOUND_IDS);
  theme_.open(*library_);
  gameMusic_.open(*library_);
  bossMusic_.open(*library_);
  spaceAmbience_.open(*library_);
  hangarAmbience_.open(*library_);
  crowdAmbience_.open(*library_);
  shopMusic_.open(*library_);
  shopRadio_.open(*library_);
}

void AudioService::update(engine::Duration elapsed) {
  soundtrack_.advance(elapsed);
}

}  // namespace rtype::client
