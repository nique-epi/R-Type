#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Ambience.hpp"
#include "FakeMusicPlayer.hpp"
#include "IAudioService.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "MusicCall.hpp"
#include "TimeConstants.hpp"

/**
 * @brief An audio service that plays nothing: it records what it was asked
 * to do with the music, in order, hands out the sound player it was given,
 * and a theme that only records whether it plays.
 */
class RecordingAudioService final : public rtype::client::IAudioService {
 public:
  /** @param sounds What sounds() returns; it must outlive the service. */
  explicit RecordingAudioService(rtype::client::ISoundPlayer& sounds);

  [[nodiscard]] rtype::client::ISoundPlayer& sounds() override;
  [[nodiscard]] rtype::client::IMusicPlayer& theme() override;
  void playTheme() override;
  /** @brief Writes down GameMusic, or LevelMusic when @p levelMusic names
   * a music. */
  void playGameMusic(const std::optional<std::string>& levelMusic) override;
  /** @brief Writes down Silence; false once a level named its music, as the
   * real service. */
  [[nodiscard]] bool silenceMusic() override;
  void playBossMusic(std::string_view music) override;
  void setAmbience(rtype::client::Ambience ambience) override;
  [[nodiscard]] rtype::client::Ambience ambience() const override;
  void open() override;
  void update(rtype::engine::Duration elapsed) override;

  /** @returns What it was asked to do with the music, oldest first. */
  [[nodiscard]] const std::vector<MusicCall>& musicCalls() const;

 private:
  std::reference_wrapper<rtype::client::ISoundPlayer> sounds_;
  FakeMusicPlayer theme_;
  std::vector<MusicCall> musicCalls_;
  bool levelMusicNamed_{false};
  rtype::client::Ambience ambience_{rtype::client::Ambience::None};
};
