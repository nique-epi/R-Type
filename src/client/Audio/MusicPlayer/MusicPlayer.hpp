#pragma once

#include <SFML/Audio/Music.hpp>
#include <optional>
#include "IMusicPlayer.hpp"
#include "Logger.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class AssetLibrary;
struct PlayerSettings;

/**
 * @brief The music of the client: THEME_MUSIC_ID, read from disk while it
 * plays, started once, by the intro after its black second or by the first
 * title screen, and played at the MUSIC volume of the player, in a loop over
 * its first THEME_LOOP_BEATS beats, which end before its fade out.
 *
 * Without the file, or with a file SFML cannot read, it writes one warning
 * and stays silent: the client runs without music. start() never restarts a
 * music that plays, so the title screen and the options leave it where it
 * is.
 */
class MusicPlayer : public IMusicPlayer {
 public:
  /**
   * @param library Locates the music file.
   * @param settings The volumes of the player; they must outlive the player.
   */
  MusicPlayer(AssetLibrary& library, const PlayerSettings& settings);

  /** @brief Starts the music, looping over THEME_LOOP_BEATS beats, at the
   * volume of the settings, unless it plays already or has no file. */
  void start() override;

  /** @brief Sets the volume the settings ask for: called each frame, so the
   * options change it at once. */
  void update();

  [[nodiscard]] std::optional<engine::Duration> playingOffset() const override;

 private:
  /** @returns Whether the music file was found and opened; writes why not
   * otherwise. */
  [[nodiscard]] bool open(AssetLibrary& library);

  const PlayerSettings* settings_;
  logging::Logger logger_;
  sf::Music music_;
  bool opened_;
};

}  // namespace rtype::client
