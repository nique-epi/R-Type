#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "WindowConstants.hpp"

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
 * @brief The chat: the letters of one of its lines, the lines it shows while
 * closed and while open, the seconds a line stays while it is closed, and the
 * opacity of the layer laid under the open chat, in percent. For each, its
 * range, the step one press moves its slider by, and its value at a first
 * launch. A line starts at SAFE_MARGIN and a letter is 8 pixels wide, so 33
 * letters end 12 pixels before the HELP chip of the waiting room; a fourth
 * closed line would reach the status line of the waiting room.
 */
constexpr int MINIMUM_CHAT_LETTERS = 12;
constexpr int MAXIMUM_CHAT_LETTERS = 33;
constexpr int CHAT_LETTERS_STEP = 3;
constexpr int DEFAULT_CHAT_LETTERS = 33;
constexpr int MINIMUM_CHAT_CLOSED_LINES = 1;
constexpr int MAXIMUM_CHAT_CLOSED_LINES = 3;
constexpr int DEFAULT_CHAT_CLOSED_LINES = 3;
constexpr int MINIMUM_CHAT_OPEN_LINES = 4;
constexpr int MAXIMUM_CHAT_OPEN_LINES = 16;
constexpr int DEFAULT_CHAT_OPEN_LINES = 8;
constexpr int MINIMUM_CHAT_SECONDS = 2;
constexpr int MAXIMUM_CHAT_SECONDS = 20;
constexpr int DEFAULT_CHAT_SECONDS = 8;
constexpr int MINIMUM_CHAT_BACKGROUND = 0;
constexpr int MAXIMUM_CHAT_BACKGROUND = 100;
constexpr int CHAT_BACKGROUND_STEP = 5;
constexpr int DEFAULT_CHAT_BACKGROUND = 70;
constexpr int CHAT_SETTING_STEP = 1;
constexpr int CHAT_PAGE_STEPS = 4;

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
constexpr std::string_view DISPLAY_MODE_NAME = "display_mode";
constexpr std::string_view COLOR_MODE_NAME = "color_mode";
constexpr std::string_view REDUCE_FLASHES_NAME = "reduce_flashes";
constexpr std::string_view SOUND_CUES_NAME = "sound_cues";
constexpr std::string_view AUTO_FIRE_NAME = "auto_fire";
constexpr std::string_view CLIENT_PREDICTION_NAME = "client_prediction";
constexpr std::string_view PILOT_NAMES_NAME = "pilot_names";
constexpr std::string_view CHAT_SHOWN_NAME = "chat_shown";
constexpr std::string_view CHAT_LETTERS_NAME = "chat_letters";
constexpr std::string_view CHAT_CLOSED_LINES_NAME = "chat_closed_lines";
constexpr std::string_view CHAT_OPEN_LINES_NAME = "chat_open_lines";
constexpr std::string_view CHAT_SECONDS_NAME = "chat_seconds";
constexpr std::string_view CHAT_BACKGROUND_NAME = "chat_background";
constexpr std::string_view MASTER_NAME = "master";

/** @brief Names of the keys of the bound actions, in the order of
 * BOUND_ACTIONS. */
constexpr std::array<std::string_view, BOUND_ACTION_COUNT>
    BOUND_ACTION_KEY_NAMES{"key_move_up",    "key_move_down", "key_move_left",
                           "key_move_right", "key_shoot",     "key_ready",
                           "key_open_chat"};

/** @brief Values of a switch; of the display modes, in the order of
 * DISPLAY_MODES; of the color modes, in the order of COLOR_MODES; and what
 * joins the width and height of a window size. */
constexpr std::string_view SWITCH_ON_VALUE = "on";
constexpr std::string_view SWITCH_OFF_VALUE = "off";
constexpr std::array<std::string_view, DISPLAY_MODE_COUNT> DISPLAY_MODE_VALUES{
    "windowed", "borderless"};
constexpr std::array<std::string_view, COLOR_MODE_COUNT> COLOR_MODE_VALUES{
    "off", "protanopia", "deuteranopia", "tritanopia"};
constexpr char WINDOW_SIZE_VALUE_SEPARATOR = 'x';

/** @brief Why a line of the settings file was left out, after its number. */
constexpr std::string_view LINE_WITHOUT_SEPARATOR = "not NAME=VALUE";
constexpr std::string_view UNKNOWN_SETTING = "unknown setting";
constexpr std::string_view INVALID_SETTING_VALUE = "invalid value";
constexpr std::string_view LINE_PREFIX = "line ";
constexpr std::string_view LINE_REASON_SEPARATOR = ": ";

/** @brief Logger name the settings file writes its log lines under. */
constexpr std::string_view SETTINGS_LOGGER_NAME = "Settings";

}  // namespace rtype::client
