#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "SettingsConstants.hpp"
#include "Slider.hpp"
#include "SliderLook.hpp"
#include "TimeConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

/** @brief Heading of the options, and the notice written over a game. */
constexpr const char* SETTINGS_HEADING = "SETTINGS";
constexpr const char* GAME_GOES_ON_NOTICE = "THE GAME GOES ON";

/** @brief The tabs, left to right, and the order of each. */
constexpr std::size_t OPTIONS_TAB_COUNT = 5;
constexpr std::array<std::string_view, OPTIONS_TAB_COUNT> OPTIONS_TAB_LABELS{
    "SOUND", "CONTROLS", "DISPLAY", "ACCESS", "CHAT"};
constexpr std::size_t SOUND_TAB = 0;
constexpr std::size_t CONTROLS_TAB = 1;
constexpr std::size_t DISPLAY_TAB = 2;
constexpr std::size_t ACCESS_TAB = 3;
constexpr std::size_t CHAT_TAB = 4;

/** @brief Order of the rows whose description changes, in their tab. */
constexpr std::size_t MUSIC_ROW = 0;
constexpr std::size_t EFFECTS_ROW = 1;
constexpr std::size_t FIRST_KEY_ROW = 0;
constexpr std::size_t DISPLAY_MODE_ROW = 0;
constexpr std::size_t WINDOW_SIZE_ROW = 1;
constexpr std::size_t CHAT_KEY_ROW = 1;

/** @brief CONTROLS lists the key of every bound action but the last, OPEN
 * CHAT, which CHAT lists: CONTROLS already holds the eight rows of a panel. */
constexpr std::size_t OPEN_CHAT_ACTION =
    static_cast<std::size_t>(BoundAction::OpenChat);
static_assert(OPEN_CHAT_ACTION == BOUND_ACTION_COUNT - 1,
              "OPEN CHAT is the last bound action");

/** @brief How a description starts. */
constexpr std::string_view DESCRIPTION_START = "> ";

/**
 * @brief SOUND: the volumes, a percent of their range, moved by
 * VOLUME_STEP, their description built around their default; muting all;
 * putting the volumes back.
 */
constexpr Slider::Range VOLUME_RANGE{.minimum = MINIMUM_VOLUME,
                                     .maximum = MAXIMUM_VOLUME,
                                     .step = VOLUME_STEP,
                                     .pageSteps = VOLUME_PAGE_STEPS};
constexpr const char* PERCENT_UNIT = "%";
constexpr const char* MUSIC_LABEL = "MUSIC";
constexpr const char* EFFECTS_LABEL = "EFFECTS";
constexpr std::string_view VOLUME_DESCRIPTION_STEP = "> BY ";
constexpr std::string_view VOLUME_DESCRIPTION_DEFAULT = " %, DEFAULT ";
constexpr std::string_view VOLUME_DESCRIPTION_END = " %";
constexpr const char* MUTED_VOLUME_DESCRIPTION = "> SOUND MUTED: SEE MUTE ALL";
constexpr const char* MUTE_ALL_LABEL = "MUTE ALL";
constexpr const char* MUTE_ALL_DESCRIPTION = "> SILENCES ALL";
constexpr const char* DEFAULT_SOUND_LABEL = "DEFAULT SOUND";
constexpr const char* DEFAULT_SOUND_DESCRIPTION =
    "> PUTS BACK THE VOLUMES OF A FIRST LAUNCH";
constexpr const char* DEFAULT_SOUND_QUESTION = "PUT BACK THE DEFAULT SOUND?";
constexpr const char* RESET_TEXT = "RESET";

/** @brief CONTROLS: the label of each bound action, in the order of
 * BOUND_ACTIONS; what a key row says; putting the keys back. */
constexpr std::array<std::string_view, BOUND_ACTION_COUNT> BOUND_ACTION_LABELS{
    "MOVE UP", "MOVE DOWN", "MOVE LEFT", "MOVE RIGHT",
    "SHOOT",   "READY",     "OPEN CHAT"};
constexpr const char* KEY_DESCRIPTION = "> ENTER, THEN THE NEW KEY";
constexpr const char* LISTENING_DESCRIPTION =
    "> PRESS THE NEW KEY. ESC CANCELS";
constexpr const char* REFUSED_KEY_DESCRIPTION = "> THIS KEY CANNOT BE BOUND";
constexpr std::string_view CONFLICT_DESCRIPTION_START = "> SAME KEY AS ";
constexpr std::string_view CONFLICT_DESCRIPTION_END = ": CHANGE ONE";
constexpr const char* DEFAULT_KEYS_LABEL = "DEFAULT KEYS";
constexpr const char* DEFAULT_KEYS_DESCRIPTION =
    "> PUTS BACK THE KEYS OF A FIRST LAUNCH";
constexpr const char* DEFAULT_KEYS_QUESTION = "PUT BACK THE DEFAULT KEYS?";

/** @brief DISPLAY: the display mode, its choices in the order of DISPLAY_MODES,
 * and the size of the window, and what their rows say. */
constexpr const char* DISPLAY_MODE_LABEL = "DISPLAY MODE";
constexpr std::array<std::string_view, DISPLAY_MODE_COUNT> DISPLAY_MODE_LABELS{
    "WINDOWED", "BORDERLESS"};
constexpr const char* DISPLAY_MODE_DESCRIPTION =
    "> ENTER OPENS THE LIST OF MODES";
constexpr const char* BORDERLESS_DESCRIPTION =
    "> BORDERLESS COVERS THE WHOLE SCREEN";
constexpr const char* WINDOW_SIZE_LABEL = "WINDOW SIZE";
constexpr const char* WINDOW_SIZE_DESCRIPTION =
    "> ENTER OPENS THE LIST OF SIZES";
constexpr const char* OPEN_LIST_DESCRIPTION =
    "> UP / DOWN, THEN ENTER. ESC CLOSES";
constexpr std::string_view POINTED_TOO_LARGE_END =
    ": TOO LARGE FOR THIS SCREEN";
constexpr std::string_view LARGER_TOO_LARGE_END = " IS TOO LARGE HERE";

/** @brief ACCESS: the text sizes, in the order of TEXT_SIZES, the color
 * modes, in the order of COLOR_MODES, and the other rows. */
constexpr const char* TEXT_SIZE_LABEL = "TEXT SIZE";
constexpr const char* TEXT_SIZE_DESCRIPTION = "> LARGER TEXT IN MENUS AND HUD";
constexpr std::array<std::string_view, TEXT_SIZE_COUNT> TEXT_SIZE_LABELS{
    "NORMAL", "LARGE", "LARGEST"};
constexpr const char* HIGH_CONTRAST_LABEL = "HIGH CONTRAST";
constexpr const char* HIGH_CONTRAST_DESCRIPTION =
    "> STRONGER COLORS, PLATES UNDER TEXT";
constexpr const char* COLOR_MODE_LABEL = "COLOR MODE";
constexpr const char* COLOR_MODE_DESCRIPTION = "> PILOT COLORS TOLD APART";
constexpr std::array<std::string_view, COLOR_MODE_COUNT> COLOR_MODE_LABELS{
    "OFF", "PROTANOPIA", "DEUTERANOPIA", "TRITANOPIA"};
constexpr const char* REDUCE_FLASHES_LABEL = "REDUCE FLASHES";
constexpr const char* REDUCE_FLASHES_DESCRIPTION = "> NO FLASH OR SHAKE";
constexpr const char* SOUND_CUES_LABEL = "SOUND CUES";
constexpr const char* SOUND_CUES_DESCRIPTION = "> SOUNDS SHOWN ON SCREEN";
constexpr const char* AUTO_FIRE_LABEL = "AUTO-FIRE";
constexpr const char* AUTO_FIRE_DESCRIPTION = "> SHOOTS WITHOUT HOLDING";
constexpr const char* PILOT_NAMES_LABEL = "PILOT NAMES";
constexpr const char* PILOT_NAMES_DESCRIPTION = "> NICKNAMES OVER THE SHIPS";
constexpr const char* LAGOMETER_SCALE_LABEL = "LAGOMETER SIZE";
constexpr const char* LAGOMETER_SCALE_DESCRIPTION = "> F3 SHOWS IT IN GAME";
constexpr const char* LAGOMETER_BACKGROUND_LABEL = "LAGOMETER FILL";
constexpr const char* LAGOMETER_BACKGROUND_DESCRIPTION =
    "> DARKENS THE GAME BEHIND IT";
constexpr const char* TIMES_UNIT = "X";
constexpr SliderLook LAGOMETER_SCALE_LOOK{.unit = TIMES_UNIT};
constexpr SliderLook LAGOMETER_BACKGROUND_LOOK{
    .step = LAGOMETER_BACKGROUND_STEP, .unit = PERCENT_UNIT};

/** @brief CHAT: its rows, what they say, and how each slider reads. */
constexpr const char* CHAT_SHOWN_LABEL = "CHAT";
constexpr const char* CHAT_SHOWN_DESCRIPTION = "> LINES OF THE OTHER PILOTS";
constexpr const char* CHAT_LETTERS_LABEL = "LINE WIDTH";
constexpr const char* CHAT_LETTERS_DESCRIPTION = "> LETTERS IN ONE LINE";
constexpr const char* CHAT_CLOSED_LINES_LABEL = "LINES CLOSED";
constexpr const char* CHAT_CLOSED_LINES_DESCRIPTION =
    "> LINES SHOWN WHILE YOU PLAY";
constexpr const char* CHAT_OPEN_LINES_LABEL = "LINES OPEN";
constexpr const char* CHAT_OPEN_LINES_DESCRIPTION =
    "> LINES SHOWN WHILE YOU TYPE";
constexpr const char* CHAT_SECONDS_LABEL = "SHOWN FOR";
constexpr const char* CHAT_SECONDS_DESCRIPTION =
    "> SECONDS A LINE STAYS WHILE YOU PLAY";
constexpr const char* CHAT_BACKGROUND_LABEL = "BACKGROUND";
constexpr const char* CHAT_BACKGROUND_DESCRIPTION =
    "> DARKENS THE GAME WHILE YOU TYPE";
constexpr const char* SECONDS_UNIT = "S";
constexpr SliderLook CHAT_LETTERS_LOOK{.step = CHAT_LETTERS_STEP};
constexpr SliderLook CHAT_LINES_LOOK{};
constexpr SliderLook CHAT_SECONDS_LOOK{.unit = SECONDS_UNIT};
constexpr SliderLook CHAT_BACKGROUND_LOOK{.step = CHAT_BACKGROUND_STEP,
                                          .unit = PERCENT_UNIT};

/** @brief BACK, and the stamp written after a save, SAVED for
 * SAVED_STAMP_DURATION, NOT SAVED until the next save. */
constexpr const char* BACK_DESCRIPTION = "> CLOSES THE OPTIONS. ESC DOES TOO";

/** @brief Description of QUIT, on the options over a game. */
constexpr const char* QUIT_GAME_DESCRIPTION = "> LEAVES THE GAME";
constexpr const char* SAVED_STAMP = "SAVED";
constexpr const char* NOT_SAVED_STAMP = "NOT SAVED";
constexpr engine::Duration SAVED_STAMP_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client
