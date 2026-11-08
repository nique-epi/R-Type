#pragma once

#include <cstdint>

/** @brief What a RecordingAudioService was asked to do with the music: play
 * the theme, play the music of a game, silence the music of a game, or play
 * the music of the boss. */
enum class MusicCall : std::uint8_t {
  Theme,
  GameMusic,
  Silence,
  BossMusic,
};
