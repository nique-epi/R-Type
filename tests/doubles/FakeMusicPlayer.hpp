#pragma once

#include <optional>
#include "AudioConstants.hpp"
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

  /** @brief Stops the music, as MusicPlayer does. */
  void stop() override;
  /** @brief Pauses a music that plays, as MusicPlayer does: start() then
   * plays it on from the position it had. */
  void pause() override;
  /** @brief Keeps @p level for fade() to read. */
  void setFade(float level) override;
  /** @returns The fade level set last, MUSIC_FADE_FULL before any. */
  [[nodiscard]] float fade() const;

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
  std::optional<rtype::engine::Duration> pausedAt_;
  float fade_{rtype::client::MUSIC_FADE_FULL};
};
