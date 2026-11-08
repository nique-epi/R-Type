#pragma once

#include "Ambience.hpp"
#include "AssetLibrary.hpp"
#include "IAudioService.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "MusicPlayer.hpp"
#include "SettingsService.hpp"
#include "SoundBank.hpp"
#include "SoundPlayer.hpp"
#include "Soundtrack.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The audio service of a client with sound: the bank of the sounds,
 * played on SOUND_VOICE_LIMIT voices for the whole client, the theme, the music
 * of a game and the music of the boss, the Soundtrack that switches between
 * them, and the ambiences that play under the theme. Their volumes follow the
 * settings service, which tells them when one changes.
 *
 * Building it reads no file: open() reads the sounds and opens the music
 * files, as the window opens in open().
 */
class AudioService final : public IAudioService {
 public:
  /**
   * @param library Locates the files of the sounds and of the music; it must
   * outlive the service.
   * @param settings Tells the sounds and the music when a volume changes;
   * it must outlive the service.
   */
  AudioService(AssetLibrary& library, SettingsService& settings);

  AudioService(const AudioService&) = delete;
  AudioService& operator=(const AudioService&) = delete;
  AudioService(AudioService&&) = delete;
  AudioService& operator=(AudioService&&) = delete;
  ~AudioService() override;

  [[nodiscard]] ISoundPlayer& sounds() override;
  [[nodiscard]] IMusicPlayer& theme() override;
  void playTheme() override;
  void playGameMusic() override;
  void silenceMusic() override;
  void playBossMusic() override;
  void setAmbience(Ambience ambience) override;
  [[nodiscard]] Ambience ambience() const override;
  /** @throws AssetLoadingException naming every sound of ALL_SOUND_IDS that
   * could not be read. */
  void open() override;
  void update(engine::Duration elapsed) override;

 private:
  AssetLibrary* library_;
  SettingsService* settings_;
  SoundBank bank_;
  SoundPlayer sounds_;
  MusicPlayer theme_;
  MusicPlayer gameMusic_;
  MusicPlayer bossMusic_;
  MusicPlayer spaceAmbience_;
  MusicPlayer hangarAmbience_;
  MusicPlayer crowdAmbience_;
  Ambience ambience_{Ambience::None};
  Soundtrack soundtrack_;
};

}  // namespace rtype::client
