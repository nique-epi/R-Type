#pragma once

#include <SFML/Audio/Music.hpp>
#include <optional>
#include "IMusicPlayer.hpp"
#include "ISettingsListener.hpp"
#include "Logger.hpp"
#include "MusicTrack.hpp"
#include "SettingId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class AssetLibrary;
class SettingsService;

/**
 * @brief A music of the client, THEME_TRACK or GAME_TRACK: its file, read
 * from disk while it plays, at the MUSIC volume of the player, in a loop over
 * the part of the track that loops, which ends before its fade out; its
 * position counts from the first beat of the track.
 *
 * Building it opens nothing: open() does, once the assets folder is known.
 * Without the file, or with a file SFML cannot read, open() writes one
 * warning and the music stays silent: the client runs without that music.
 * start() never restarts a music that plays, so the title screen and the
 * options leave it where it is.
 */
class MusicPlayer : public IMusicPlayer, public ISettingsListener {
 public:
  /**
   * @param settings Holds the volumes of the player; it must outlive the
   * player, and tell it through subscribe().
   * @param track The music to play and the part of it that loops.
   */
  MusicPlayer(const SettingsService& settings, MusicTrack track);

  /** @brief Opens the music file that @p library locates, for start() to
   * play; writes why not when the file is missing or cannot be read. */
  void open(AssetLibrary& library);

  /** @brief Starts the music from the start of its file, looping over the
   * part of its track that loops, at the volume of the settings, unless it
   * plays already or has no file. */
  void start() override;

  void stop() override;

  /** @brief Sets the volume the settings now hold when @p changed moves it, so
   * the options change the music at once; ignores the other settings. */
  void settingChanged(SettingId changed) override;

  [[nodiscard]] std::optional<engine::Duration> playingOffset() const override;

 private:
  void applyVolume();

  const SettingsService* settings_;
  MusicTrack track_;
  logging::Logger logger_;
  sf::Music music_;
  bool opened_{false};
};

}  // namespace rtype::client
