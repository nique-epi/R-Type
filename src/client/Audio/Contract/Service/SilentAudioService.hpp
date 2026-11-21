#pragma once

#include "Ambience.hpp"
#include "IAudioService.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "LevelTracks.hpp"
#include "NullMusicPlayer.hpp"
#include "NullSoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The audio service of a client without sound: the sounds and the
 * music play nothing, and every call is accepted. */
class SilentAudioService final : public IAudioService {
 public:
  [[nodiscard]] ISoundPlayer& sounds() override;
  [[nodiscard]] IMusicPlayer& theme() override;
  void playTheme() override;
  void playGameMusic(const LevelTracks& tracks) override;
  [[nodiscard]] bool silenceMusic() override;
  void playBossMusic() override;
  void setAmbience(Ambience ambience) override;
  [[nodiscard]] Ambience ambience() const override;
  void open() override;
  void update(engine::Duration elapsed) override;

 private:
  NullSoundPlayer sounds_;
  NullMusicPlayer theme_;
};

}  // namespace rtype::client
