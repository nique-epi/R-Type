#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A small limit of voices, so a test lists every voice. */
constexpr std::size_t THREE_VOICES = 3;

/** @brief More sounds than the client has voices: the cascade of the intro
 * starts 32 in less than half a second. */
constexpr std::size_t MANY_SOUNDS = 40;

/** @brief Volumes of a player, both away from the volumes of a first launch
 * (70 and 80), so a field read for the other is seen. */
constexpr int A_MUSIC_VOLUME = 35;
constexpr int AN_EFFECTS_VOLUME = 55;

/** @brief One frame of the tests of the clock: a frame of 16 ms closes a
 * tenth of a gap to the music (16 / 160). */
constexpr engine::Duration A_FRAME = std::chrono::milliseconds(16);

/** @brief A position of the music far ahead of the time of the frames, past
 * the snap of 100 ms. */
constexpr engine::Duration MUSIC_FAR_AHEAD = std::chrono::milliseconds(200);

/** @brief A gap of 40 ms to the music and the tenth of it a frame closes; the
 * tenth of the snap of 100 ms; a gap of the music behind the time, within
 * the snap. */
constexpr engine::Duration SMALL_GAP = std::chrono::milliseconds(40);
constexpr engine::Duration TENTH_OF_THE_SMALL_GAP =
    std::chrono::milliseconds(4);
constexpr engine::Duration TENTH_OF_THE_SNAP = std::chrono::milliseconds(10);
constexpr engine::Duration MUSIC_BEHIND = std::chrono::milliseconds(90);

/** @brief How far a part of a gap may be from its exact value: the clock
 * computes it in float. */
constexpr engine::Duration DURATION_ROUNDING = std::chrono::microseconds(1);

/** @brief Where the music is when the intro is skipped, and where the intro
 * jumps to: the slam of the logo, nine beats in. */
constexpr engine::Duration MUSIC_AT_SKIP = std::chrono::seconds(1);
constexpr engine::Duration SKIP_TARGET = std::chrono::microseconds(4954128);

/** @brief Where the fade out of the theme starts, in seconds of the file:
 * measured on assets/music/spacesuit_suite.mp3, where its level by half
 * second starts falling for good (-29 dBFS at 197 s, -40 at 197.5 s). */
constexpr float THEME_FADE_START = 197.0F;

}  // namespace rtype::client::testing
