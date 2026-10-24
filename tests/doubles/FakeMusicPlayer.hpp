#pragma once

#include <optional>
#include "FakeMusicClock.hpp"
#include "IMusicPlayer.hpp"
#include "TimeConstants.hpp"

/**
 * @brief Music player double: start() makes it play from the start of its
 * file, THEME_FIRST_BEAT before the first beat, as MusicPlayer counts its
 * position, unless it plays already or its file was removed. Its position
 * then stands still unless the test moves it; it needs no audio device.
 */
class FakeMusicPlayer : public rtype::client::IMusicPlayer {
 public:
  void start() override;

  [[nodiscard]] std::optional<rtype::engine::Duration> playingOffset()
      const override;

  /** @brief Makes the music play at @p offset, or stop with nothing. */
  void setPlayingOffset(std::optional<rtype::engine::Duration> offset);

  /** @brief Makes start() leave the music silent, as MusicPlayer does
   * without its file. */
  void removeTheFile();

 private:
  FakeMusicClock clock_;
  bool fileRemoved_{};
};
