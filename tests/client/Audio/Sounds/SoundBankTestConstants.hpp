#pragma once

#include <array>
#include <cstdint>

namespace rtype::client::testing {

/** @brief Ids of sounds the tests write in a temporary assets folder, the
 * first in a nested folder. */
constexpr const char* SHOT_SOUND_ID = "sounds/weapons/shot.wav";
constexpr const char* EXPLOSION_SOUND_ID = "sounds/explosion.wav";

/** @brief The ids of a load that cannot read them: a file that holds no
 * sound, a file never written, and an id that breaks the asset id rule. */
constexpr const char* CORRUPT_SOUND_ID = "sounds/corrupt.wav";
constexpr const char* MISSING_SOUND_ID = "sounds/missing.wav";
constexpr const char* BADLY_NAMED_SOUND_ID = "sounds/Shot.wav";
constexpr const char* CORRUPT_CONTENT = "not a sound";

/** @brief Two lengths of silence, so a test tells which file was read. */
constexpr std::uint64_t SHORT_SOUND_SAMPLES = 100;
constexpr std::uint64_t LONG_SOUND_SAMPLES = 200;

/** @brief The lists of sounds the tests load. */
constexpr std::array ONLY_THE_SHOT{SHOT_SOUND_ID};
constexpr std::array THE_SHOT_AND_THE_EXPLOSION{SHOT_SOUND_ID,
                                                EXPLOSION_SOUND_ID};
constexpr std::array THREE_UNREADABLE_SOUNDS_AND_THE_SHOT{
    CORRUPT_SOUND_ID, MISSING_SOUND_ID, BADLY_NAMED_SOUND_ID, SHOT_SOUND_ID};

}  // namespace rtype::client::testing
