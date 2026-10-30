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
#include "PlayerSettings.hpp"
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

MusicPlayer::MusicPlayer(const PlayerSettings& settings, MusicTrack track)
    : settings_(&settings),
      track_(track),
      logger_(std::string{AUDIO_LOG_MODULE_NAME}) {}

void MusicPlayer::start() {
  if (!opened_ || music_.getStatus() == sf::SoundSource::Status::Playing) {
    return;
  }
  music_.setLoopPoints(loopOf(track_));
  music_.setLooping(true);
  update();
  music_.play();
}

void MusicPlayer::stop() { music_.stop(); }

void MusicPlayer::update() { music_.setVolume(musicVolumeOf(*settings_)); }

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
  if (library.load(AssetKind::Music, track_.assetId) !=
      AssetLoadResult::Loaded) {
    logger_.warn("no music file ", track_.assetId,
                 ": the client plays without it");
    return;
  }
  if (!music_.openFromFile(library.musicFile(track_.assetId))) {
    logger_.warn("the music file ", track_.assetId,
                 " cannot be read: the client plays without it");
    return;
  }
  opened_ = true;
}

}  // namespace rtype::client
