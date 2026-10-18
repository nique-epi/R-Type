#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "BoundAction.hpp"
#include "ColorMode.hpp"

namespace rtype::client {

/**
 * @brief Volumes, in percent: their range, the step one press moves them by,
 * the steps of a page, and the volumes of a first launch.
 */
constexpr int MINIMUM_VOLUME = 0;
constexpr int MAXIMUM_VOLUME = 100;
constexpr int VOLUME_STEP = 5;
constexpr int VOLUME_PAGE_STEPS = 2;
constexpr int DEFAULT_MUSIC_VOLUME = 70;
constexpr int DEFAULT_EFFECTS_VOLUME = 80;

/**
 * @brief The settings file, in the settings folder; the file written next to
 * it, then renamed over it, so a save cut short never leaves half a file; and
 * the largest file read, in bytes (a full file holds about 300).
 */
constexpr const char* SETTINGS_FILE_NAME = "settings.ini";
constexpr const char* TEMPORARY_SETTINGS_SUFFIX = ".tmp";
constexpr std::uintmax_t MAXIMUM_SETTINGS_FILE_SIZE = 65536;

/**
 * @brief Lines of the settings file: NAME=VALUE, spaces around both ignored;
 * an empty line, or one starting with COMMENT_START, is skipped. The first
 * line is SETTINGS_FILE_COMMENT, the second the version of the format.
 */
constexpr char SETTING_SEPARATOR = '=';
constexpr char COMMENT_START = '#';
constexpr char LINE_END = '\n';
constexpr std::string_view LINE_SPACES = " \t\r";
constexpr std::string_view SETTINGS_FILE_COMMENT =
    "# R-Type settings, written by the game";
constexpr std::string_view SETTINGS_VERSION_NAME = "version";
constexpr int SETTINGS_VERSION = 1;

/** @brief Names of the settings in the file. */
constexpr std::string_view MUSIC_VOLUME_NAME = "music_volume";
constexpr std::string_view EFFECTS_VOLUME_NAME = "effects_volume";
constexpr std::string_view MUTED_NAME = "mute_all";
constexpr std::string_view WINDOW_SIZE_NAME = "window_size";
constexpr std::string_view COLOR_MODE_NAME = "color_mode";
constexpr std::string_view REDUCE_FLASHES_NAME = "reduce_flashes";
constexpr std::string_view SOUND_CUES_NAME = "sound_cues";
constexpr std::string_view AUTO_FIRE_NAME = "auto_fire";

/** @brief Names of the keys of the bound actions, in the order of
 * BOUND_ACTIONS. */
constexpr std::array<std::string_view, BOUND_ACTION_COUNT>
    BOUND_ACTION_KEY_NAMES{"key_move_up",    "key_move_down", "key_move_left",
                           "key_move_right", "key_shoot",     "key_ready"};

/** @brief Values of a switch; of the color modes, in the order of
 * COLOR_MODES; and what joins the width and height of a window size. */
constexpr std::string_view SWITCH_ON_VALUE = "on";
constexpr std::string_view SWITCH_OFF_VALUE = "off";
constexpr std::array<std::string_view, COLOR_MODE_COUNT> COLOR_MODE_VALUES{
    "off", "protanopia", "deuteranopia", "tritanopia"};
constexpr char WINDOW_SIZE_VALUE_SEPARATOR = 'x';

/** @brief Why a line of the settings file was left out, after its number. */
constexpr std::string_view LINE_WITHOUT_SEPARATOR = "not NAME=VALUE";
constexpr std::string_view UNKNOWN_SETTING = "unknown setting";
constexpr std::string_view INVALID_SETTING_VALUE = "invalid value";
constexpr std::string_view LINE_PREFIX = "line ";
constexpr std::string_view LINE_REASON_SEPARATOR = ": ";

/** @brief Module name of the log lines about the settings. */
constexpr std::string_view SETTINGS_LOG_MODULE_NAME = "Settings";

}  // namespace rtype::client
