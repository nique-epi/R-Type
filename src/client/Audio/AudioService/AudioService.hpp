#pragma once

#include "AssetLibrary.hpp"
#include "IAudioService.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "MusicPlayer.hpp"
#include "PlayerSettings.hpp"
#include "SoundPlayer.hpp"
#include "Soundtrack.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The audio service of a client with sound: the sounds of every screen
 * on SOUND_VOICE_LIMIT voices for the whole client, the theme and the music
 * of a game, and the Soundtrack that switches between them. Their volumes
 * follow the settings at each update().
 *
 * Building it opens no music file: open() does, as the window opens in
 * open().
 */
class AudioService final : public IAudioService {
 public:
  /**
   * @param library Holds the sounds and locates the music files; it must
   * outlive the service.
   * @param settings The volumes of the player; they must outlive the service.
   */
  AudioService(AssetLibrary& library, const PlayerSettings& settings);

  AudioService(const AudioService&) = delete;
  AudioService& operator=(const AudioService&) = delete;
  AudioService(AudioService&&) = delete;
  AudioService& operator=(AudioService&&) = delete;
  ~AudioService() override = default;

  [[nodiscard]] ISoundPlayer& sounds() override;
  [[nodiscard]] IMusicPlayer& theme() override;
  void playTheme() override;
  void playGameMusic() override;
  void open() override;
  void update(engine::Duration elapsed) override;

 private:
  AssetLibrary* library_;
  SoundPlayer sounds_;
  MusicPlayer theme_;
  MusicPlayer gameMusic_;
  Soundtrack soundtrack_;
};

}  // namespace rtype::client
