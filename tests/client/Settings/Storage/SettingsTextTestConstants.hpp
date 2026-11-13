#pragma once

#include <cstddef>
#include <cstdint>
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
    "key_open_chat=T\n"
    "color_mode=off\n"
    "reduce_flashes=off\n"
    "sound_cues=off\n"
    "auto_fire=off\n"
    "pilot_names=on\n"
    "chat_shown=on\n"
    "chat_letters=33\n"
    "chat_closed_lines=3\n"
    "chat_open_lines=8\n"
    "chat_seconds=8\n"
    "chat_background=70\n"
    "lagometer_scale=1\n"
    "lagometer_background=70\n"
    "ship_head=none\n"
    "ship_eyes=none\n"
    "ship_face=none\n"
    "ship_hull=none\n"
    "ship_locker=top_hat,cool_shades,mustache\n"
    "kopecks=0\n";

/** @brief The ship wearing the crown, the third item of the locker, and the
 * shades, the ninth: what a slot holds is the place of its item plus one. */
constexpr std::uint8_t WEARS_CROWN = 3;
constexpr std::uint8_t WEARS_SHADES = 9;
constexpr std::uint8_t WEARS_NOTHING = 0;
constexpr const char* CROWN_LINE = "ship_head=crown\n";
constexpr const char* CROWN_AND_SHADES_FILE =
    "ship_head=crown\n"
    "ship_eyes=cool_shades\n";
/** @brief The shades named on the head, a slot they never take. */
constexpr const char* SHADES_ON_THE_HEAD_FILE = "ship_head=cool_shades\n";
constexpr const char* BARE_HEAD_FILE = "ship_head=none\n";

/** @brief Chat settings changed from a first launch, all on a step of their
 * slider; and a file whose chat values each lie one past their range. */
constexpr int CHANGED_CHAT_LETTERS = 21;
constexpr int CHANGED_CHAT_CLOSED_LINES = 1;
constexpr int CHANGED_CHAT_OPEN_LINES = 12;
constexpr int CHANGED_CHAT_SECONDS = 15;
constexpr int CHANGED_CHAT_BACKGROUND = 40;
constexpr const char* CHAT_PAST_ITS_RANGES_FILE =
    "chat_letters=34\n"
    "chat_closed_lines=0\n"
    "chat_open_lines=17\n"
    "chat_seconds=1\n"
    "chat_background=101\n";
constexpr std::size_t CHAT_LINES_PAST_THEIR_RANGES = 5;

/** @brief Lagometer settings changed from a first launch, on a step of their
 * slider; and a file whose lagometer values each lie one past their range. */
constexpr int CHANGED_LAGOMETER_SCALE = 2;
constexpr int CHANGED_LAGOMETER_BACKGROUND = 35;
constexpr const char* LAGOMETER_PAST_ITS_RANGES_FILE =
    "lagometer_scale=4\n"
    "lagometer_background=101\n";
constexpr std::size_t LAGOMETER_LINES_PAST_THEIR_RANGES = 2;

constexpr int ANY_VOLUME = 40;
constexpr int CHANGED_MUSIC_VOLUME = 35;
constexpr int CHANGED_EFFECTS_VOLUME = 5;
constexpr PixelSize SMALLER_WINDOW{.width = 1280, .height = 720};

/** @brief The line an earlier version kept the master the player chose on. */
constexpr const char* EARLIER_MASTER_LINE = "master=10.0.0.2:8080";

/** @brief The line an earlier version kept the switch of the prediction on,
 * which the player can no longer turn off. */
constexpr const char* EARLIER_PREDICTION_LINE = "client_prediction=off";

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

/** @brief The golden ace, the twenty-ninth item of the catalog and a hull;
 * the line that names it, and a file that names the crown as a hull. */
constexpr std::uint8_t FLIES_GOLDEN_ACE = 29;
constexpr const char* GOLDEN_ACE_LINE = "ship_hull=golden_ace\n";
constexpr const char* CROWN_AS_A_HULL_FILE = "ship_hull=crown\n";
/** @brief A file written before the hulls: it names no hull. */
constexpr const char* FILE_WITHOUT_A_HULL =
    "ship_head=crown\n"
    "ship_face=none\n";
/** @brief A locker line naming a new accessory and a hull, at places 10 and
 * 26 of the catalog. */
constexpr const char* VOSTOK_AND_CHAPKA_LOCKER = "ship_locker=chapka,vostok\n";
constexpr std::size_t CHAPKA_ITEM = 10;
constexpr std::size_t VOSTOK_ITEM = 26;

/** @brief A balance, the line that writes it, and files whose balance is no
 * whole number, is negative, or passes MAXIMUM_KOPECKS (999999). */
constexpr std::uint32_t A_BALANCE = 1250;
constexpr const char* A_BALANCE_LINE = "kopecks=1250\n";
constexpr const char* WORDS_AS_A_BALANCE_FILE = "kopecks=abc\n";
constexpr const char* NEGATIVE_BALANCE_FILE = "kopecks=-5\n";
constexpr const char* BALANCE_PAST_THE_LARGEST_FILE = "kopecks=1000000\n";

}  // namespace rtype::client::testing
