#pragma once

#include <functional>
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
  void playGameMusic() override;
  void silenceMusic() override;
  void playBossMusic() override;
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
  rtype::client::Ambience ambience_{rtype::client::Ambience::None};
};
