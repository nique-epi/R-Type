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
#include "MusicTrack.hpp"
#include "SettingId.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns @p duration as SFML keeps it, in microseconds. */
sf::Time sfmlTimeOf(engine::Duration duration) {
  return {std::chrono::duration_cast<std::chrono::microseconds>(duration)};
}

/** @returns The part of the file of @p track that loops. */
sf::Music::TimeSpan loopOf(const MusicTrack& track) {
  return sf::Music::TimeSpan{.offset = sfmlTimeOf(track.loopStart),
                             .length = sfmlTimeOf(track.loopLength)};
}

}  // namespace

MusicPlayer::MusicPlayer(AssetLibrary& library, const SettingsService& settings,
                         MusicTrack track)
    : settings_(&settings),
      track_(track),
      logger_(std::string{AUDIO_LOG_MODULE_NAME}),
      opened_(open(library)) {}

void MusicPlayer::start() {
  if (!opened_ || music_.getStatus() == sf::SoundSource::Status::Playing) {
    return;
  }
  music_.setLoopPoints(loopOf(track_));
  music_.setLooping(true);
  applyVolume();
  music_.play();
}

void MusicPlayer::stop() { music_.stop(); }

void MusicPlayer::settingChanged(SettingId changed) {
  if (changesMusicVolume(changed)) {
    applyVolume();
  }
}

std::optional<engine::Duration> MusicPlayer::playingOffset() const {
  if (music_.getStatus() != sf::SoundSource::Status::Playing) {
    return std::nullopt;
  }
  return std::chrono::duration_cast<engine::Duration>(
             music_.getPlayingOffset().toDuration()) -
         track_.firstBeat;
}

bool MusicPlayer::open(AssetLibrary& library) {
  if (library.load(AssetKind::Music, track_.assetId) !=
      AssetLoadResult::Loaded) {
    logger_.warn("no music file ", track_.assetId,
                 ": the client plays without it");
    return false;
  }
  if (!music_.openFromFile(library.musicFile(track_.assetId))) {
    logger_.warn("the music file ", track_.assetId,
                 " cannot be read: the client plays without it");
    return false;
  }
  return true;
}

void MusicPlayer::applyVolume() {
  music_.setVolume(musicVolumeOf(settings_->settings()));
}

}  // namespace rtype::client
