#pragma once

#include "PixelSize.hpp"

namespace rtype::client::testing {

/** @brief The file of a first launch, as the format of the options writes
 * it: a comment, the version, one line per setting. */
constexpr const char* FIRST_LAUNCH_FILE =
    "# R-Type settings, written by the game\n"
    "version=1\n"
    "music_volume=70\n"
    "effects_volume=80\n"
    "mute_all=off\n"
    "display_mode=windowed\n"
    "key_move_up=UP\n"
    "key_move_down=DOWN\n"
    "key_move_left=LEFT\n"
    "key_move_right=RIGHT\n"
    "key_shoot=SPACE\n"
    "key_ready=R\n"
    "color_mode=off\n"
    "reduce_flashes=off\n"
    "sound_cues=off\n"
    "auto_fire=off\n";

constexpr int ANY_VOLUME = 40;
constexpr int CHANGED_MUSIC_VOLUME = 35;
constexpr int CHANGED_EFFECTS_VOLUME = 5;
constexpr PixelSize SMALLER_WINDOW{.width = 1280, .height = 720};

/** @brief The music volume of a first launch, and the lowest and the highest
 * volumes a file may hold. */
constexpr int FIRST_LAUNCH_MUSIC_VOLUME = 70;
constexpr int LOWEST_VOLUME = 0;
constexpr int HIGHEST_VOLUME = 100;

/** @brief What a reading lists for a first line it leaves out: an invalid
 * value, an unknown setting, a line without an equals sign. */
constexpr const char* INVALID_FIRST_LINE = "line 1: invalid value";
constexpr const char* UNKNOWN_FIRST_LINE = "line 1: unknown setting";
constexpr const char* FIRST_LINE_WITHOUT_SEPARATOR = "line 1: not NAME=VALUE";

}  // namespace rtype::client::testing
