#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A small limit of voices, so a test lists every voice. */
constexpr std::size_t THREE_VOICES = 3;

/** @brief The voices of the client: 16 sounds at most at once. */
constexpr std::size_t SIXTEEN_VOICES = 16;

/** @brief More sounds than the client has voices: the cascade of the intro
 * starts 32 in less than half a second. */
constexpr std::size_t MANY_SOUNDS = 40;

/** @brief Volumes of a player, both away from the volumes of a first launch
 * (70 and 80), so a field read for the other is seen. */
constexpr int A_MUSIC_VOLUME = 35;
constexpr int AN_EFFECTS_VOLUME = 55;

/** @brief A position of the music far ahead of the time of the frames, past
 * the snap of 100 ms. */
constexpr engine::Duration MUSIC_FAR_AHEAD = std::chrono::milliseconds(200);

/** @brief A gap of 40 ms to the music and the tenth of it ONE_FRAME closes, a
 * frame of 16 ms closing 16 / 160 of a gap; the tenth of the snap of 100 ms;
 * a gap of the music behind the time, within the snap. */
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

/** @brief Where the fade out of the music of a game starts, in seconds of the
 * file: measured on assets/music/oh_the_city.mp3, where its level by tenth of
 * a second leaves -12 dBFS for good (-15 dBFS at 152.1 s, -22 at 153 s). */
constexpr float GAME_MUSIC_FADE_START = 152.1F;

/** @brief The silence between the theme and the music of a game, one second
 * as asked, and moments inside it and past it. */
constexpr engine::Duration THE_WHOLE_SILENCE = std::chrono::seconds(1);
constexpr engine::Duration MOST_OF_THE_SILENCE = std::chrono::milliseconds(900);
constexpr engine::Duration PART_OF_THE_SILENCE = std::chrono::milliseconds(600);
constexpr engine::Duration REST_OF_THE_SILENCE = std::chrono::milliseconds(400);
constexpr engine::Duration LONGER_THAN_THE_SILENCE = std::chrono::seconds(2);

}  // namespace rtype::client::testing
