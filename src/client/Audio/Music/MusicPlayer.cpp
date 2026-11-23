#include "MusicPlayer.hpp"
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
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

MusicPlayer::MusicPlayer(const SettingsService& settings, MusicTrack track)
    : settings_(&settings),
      track_(track),
      assetId_(track.assetId),
      logger_(std::string{AUDIO_LOGGER_NAME}) {}

void MusicPlayer::start() {
  if (!opened_ || music_.getStatus() == sf::SoundSource::Status::Playing) {
    return;
  }
  if (assetId_ == track_.assetId &&
      track_.loopLength > engine::Duration::zero()) {
    music_.setLoopPoints(loopOf(track_));
  }
  music_.setLooping(true);
  applyVolume();
  music_.play();
}

void MusicPlayer::stop() { music_.stop(); }

void MusicPlayer::pause() {
  if (music_.getStatus() == sf::SoundSource::Status::Playing) {
    music_.pause();
  }
}

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

void MusicPlayer::open(AssetLibrary& library) {
  opened_ = false;
  if (library.load(AssetKind::Music, assetId_) != AssetLoadResult::Loaded) {
    logger_.warn("no music file ", assetId_, ": the client plays without it");
    return;
  }
  if (!music_.openFromFile(library.musicFile(assetId_))) {
    logger_.warn("the music file ", assetId_,
                 " cannot be read: the client plays without it");
    return;
  }
  opened_ = true;
}

void MusicPlayer::switchTo(AssetLibrary& library, std::string_view assetId) {
  if (assetId == assetId_) {
    return;
  }
  const bool played = playingOffset().has_value();
  music_.stop();
  assetId_ = assetId;
  open(library);
  if (played) {
    start();
  }
}

void MusicPlayer::setFade(float level) {
  fade_ = level;
  applyVolume();
}

void MusicPlayer::applyVolume() {
  music_.setVolume(musicVolumeOf(settings_->settings()) * track_.gain * fade_);
}

}  // namespace rtype::client
