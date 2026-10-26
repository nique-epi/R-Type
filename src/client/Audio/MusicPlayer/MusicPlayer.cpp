#include "MusicPlayer.hpp"
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>
#include <optional>
#include <string>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetLoadResult.hpp"
#include "AudioConstants.hpp"
#include "AudioVolume.hpp"
#include "PlayerSettings.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The part of the theme that loops: THEME_LOOP_BEATS beats from
 * its first beat. */
sf::Music::TimeSpan themeLoop() {
  return sf::Music::TimeSpan{
      .offset = sf::Time(std::chrono::duration_cast<std::chrono::microseconds>(
          THEME_FIRST_BEAT)),
      .length = sf::seconds(THEME_BEAT * static_cast<float>(THEME_LOOP_BEATS))};
}

}  // namespace

MusicPlayer::MusicPlayer(AssetLibrary& library, const PlayerSettings& settings)
    : settings_(&settings),
      logger_(std::string{AUDIO_LOG_MODULE_NAME}),
      opened_(open(library)) {}

void MusicPlayer::start() {
  if (!opened_ || music_.getStatus() == sf::SoundSource::Status::Playing) {
    return;
  }
  music_.setLoopPoints(themeLoop());
  music_.setLooping(true);
  update();
  music_.play();
}

void MusicPlayer::update() { music_.setVolume(musicVolumeOf(*settings_)); }

std::optional<engine::Duration> MusicPlayer::playingOffset() const {
  if (music_.getStatus() != sf::SoundSource::Status::Playing) {
    return std::nullopt;
  }
  return std::chrono::duration_cast<engine::Duration>(
             music_.getPlayingOffset().toDuration()) -
         THEME_FIRST_BEAT;
}

bool MusicPlayer::open(AssetLibrary& library) {
  if (library.load(AssetKind::Music, THEME_MUSIC_ID) !=
      AssetLoadResult::Loaded) {
    logger_.warn("no music file ", THEME_MUSIC_ID,
                 ": the client plays without music");
    return false;
  }
  if (!music_.openFromFile(library.musicFile(THEME_MUSIC_ID))) {
    logger_.warn("the music file ", THEME_MUSIC_ID,
                 " cannot be read: the client plays without music");
    return false;
  }
  return true;
}

}  // namespace rtype::client
