#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Most sounds the client plays at once, for the whole client: the
 * cascade of the intro starts 32 sounds in less than half a second, a game
 * a shot and an explosion per kill. A sound started while every voice plays
 * cuts the one that started first.
 */
constexpr std::size_t SOUND_VOICE_LIMIT = 16;

/**
 * @brief Asset id of the music, played in a loop from launch: Spacesuit
 * Suite, by Stationary Sign (see assets/README.md).
 */
constexpr const char* THEME_MUSIC_ID = "music/spacesuit_suite.mp3";

/**
 * @brief Where the first beat of the theme falls in its file: 85 ms in,
 * measured on its sixteenth notes (aubio onsets, within 82 to 88 ms in 17
 * windows of 10 s out of 20). The music counts its position from there, and
 * loops from there.
 */
constexpr engine::Duration THEME_FIRST_BEAT = std::chrono::milliseconds(85);

/**
 * @brief One beat of the theme, which plays 109 beats a minute, in seconds.
 * The theme loops over THEME_LOOP_BEATS beats from its first beat, 22
 * phrases of 16 beats that end 193.85 s into the file, before its fade out,
 * which starts at 197 s.
 */
constexpr float THEME_BEAT = 60.0F / 109.0F;
constexpr int THEME_LOOP_BEATS = 352;

/**
 * @brief How the time of the intro follows the position of the music, which
 * moves by one audio block at a time (about 10 ms in SFML 3.0.2): each frame
 * closes elapsed / MUSIC_CATCH_UP_TIME of the gap, a tenth of it for a frame
 * of 16 ms, so the steps of that position barely move the pictures and a gap
 * of 40 ms falls under 4 ms in about 0.37 s. A gap wider than MUSIC_SYNC_SNAP,
 * either way, is closed at once. MUSIC_SYNC_SNAP is shorter than
 * MUSIC_CATCH_UP_TIME, so a frame that closes part of a gap never moves the
 * time back.
 */
constexpr engine::Duration MUSIC_CATCH_UP_TIME = std::chrono::milliseconds(160);
constexpr engine::Duration MUSIC_SYNC_SNAP = std::chrono::milliseconds(100);

/** @brief Module name the music and the sounds write their log lines
 * under. */
constexpr std::string_view AUDIO_LOG_MODULE_NAME = "Audio";

}  // namespace rtype::client
