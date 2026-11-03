#pragma once

#include <optional>
#include "IMusicClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The time of the intro: the position of the music while it plays, so
 * each picture lands on its beat, and the time of the frames otherwise.
 *
 * advance() moves the time by the time of the frame, then, while the music
 * plays, closes part of the gap to the position of the music plus the shift
 * of the last jump: elapsed / MUSIC_CATCH_UP_TIME of it, so the steps of that
 * position, one audio block each, do not shake the pictures; a gap wider than
 * MUSIC_SYNC_SNAP is closed at once. jumpTo() moves the time and leaves the
 * music where it is: the gap becomes the shift.
 */
class SyncedClock {
 public:
  /** @param music Where the music is; it must outlive the clock. */
  explicit SyncedClock(const IMusicClock& music);

  /** @brief Moves the time by @p elapsed, then follows the music; see the
   * class. */
  void advance(engine::Duration elapsed);

  /** @brief Puts the time at @p time; the music goes on where it is. */
  void jumpTo(engine::Duration time);

  /** @returns The time since the clock was built, its jumps included. */
  [[nodiscard]] engine::Duration now() const;

  /** @returns How far the music, plus the shift of the last jump, is ahead of
   * the time, negative when it is behind; nothing while it does not play. */
  [[nodiscard]] std::optional<engine::Duration> gapToMusic() const;

 private:
  const IMusicClock* music_;
  engine::Duration time_{};
  engine::Duration shift_{};
};

}  // namespace rtype::client
