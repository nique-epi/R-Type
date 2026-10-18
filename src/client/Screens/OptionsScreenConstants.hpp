#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "SettingsConstants.hpp"
#include "Slider.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Heading of the options, and the notice written over a game. */
constexpr const char* SETTINGS_HEADING = "SETTINGS";
constexpr const char* GAME_GOES_ON_NOTICE = "THE GAME GOES ON";

/** @brief The tabs, left to right, and the order of each. */
constexpr std::size_t OPTIONS_TAB_COUNT = 4;
constexpr std::array<std::string_view, OPTIONS_TAB_COUNT> OPTIONS_TAB_LABELS{
    "SOUND", "CONTROLS", "DISPLAY", "ACCESS"};
constexpr std::size_t SOUND_TAB = 0;
constexpr std::size_t CONTROLS_TAB = 1;
constexpr std::size_t DISPLAY_TAB = 2;
constexpr std::size_t ACCESS_TAB = 3;

/** @brief Order of the rows whose description changes, in their tab. */
constexpr std::size_t MUSIC_ROW = 0;
constexpr std::size_t EFFECTS_ROW = 1;
constexpr std::size_t FIRST_KEY_ROW = 0;
constexpr std::size_t WINDOW_SIZE_ROW = 0;

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
constexpr std::string_view VOLUME_DESCRIPTION_END = " % (COMING SOON)";
constexpr const char* MUTED_VOLUME_DESCRIPTION = "> SOUND MUTED: SEE MUTE ALL";
constexpr const char* MUTE_ALL_LABEL = "MUTE ALL";
constexpr const char* MUTE_ALL_DESCRIPTION = "> SILENCES ALL (COMING SOON)";
constexpr const char* DEFAULT_SOUND_LABEL = "DEFAULT SOUND";
constexpr const char* DEFAULT_SOUND_DESCRIPTION =
    "> PUTS BACK THE VOLUMES OF A FIRST LAUNCH";
constexpr const char* DEFAULT_SOUND_QUESTION = "PUT BACK THE DEFAULT SOUND?";
constexpr const char* RESET_TEXT = "RESET";

/** @brief CONTROLS: the label of each bound action, in the order of
 * BOUND_ACTIONS; what a key row says; putting the keys back. */
constexpr std::array<std::string_view, BOUND_ACTION_COUNT> BOUND_ACTION_LABELS{
    "MOVE UP", "MOVE DOWN", "MOVE LEFT", "MOVE RIGHT", "SHOOT", "READY"};
constexpr const char* KEY_DESCRIPTION = "> ENTER, THEN THE NEW KEY";
constexpr const char* READY_KEY_DESCRIPTION =
    "> ENTER, THEN THE NEW KEY (COMING SOON)";
constexpr const char* LISTENING_DESCRIPTION =
    "> PRESS THE NEW KEY. ESC CANCELS";
constexpr const char* REFUSED_KEY_DESCRIPTION = "> THIS KEY CANNOT BE BOUND";
constexpr std::string_view CONFLICT_DESCRIPTION_START = "> SAME KEY AS ";
constexpr std::string_view CONFLICT_DESCRIPTION_END = ": CHANGE ONE";
constexpr const char* DEFAULT_KEYS_LABEL = "DEFAULT KEYS";
constexpr const char* DEFAULT_KEYS_DESCRIPTION =
    "> PUTS BACK THE KEYS OF A FIRST LAUNCH";
constexpr const char* DEFAULT_KEYS_QUESTION = "PUT BACK THE DEFAULT KEYS?";

/** @brief DISPLAY: the size of the window, and what its row says. */
constexpr const char* WINDOW_SIZE_LABEL = "WINDOW SIZE";
constexpr const char* WINDOW_SIZE_DESCRIPTION =
    "> ENTER OPENS THE LIST OF SIZES";
constexpr const char* OPEN_LIST_DESCRIPTION =
    "> UP / DOWN, THEN ENTER. ESC CLOSES";
constexpr std::string_view POINTED_TOO_LARGE_END =
    ": TOO LARGE FOR THIS SCREEN";
constexpr std::string_view LARGER_TOO_LARGE_END = " IS TOO LARGE HERE";

/** @brief ACCESS: the color modes, in the order of COLOR_MODES, and the
 * other rows. */
constexpr const char* COLOR_MODE_LABEL = "COLOR MODE";
constexpr const char* COLOR_MODE_DESCRIPTION =
    "> SHIP AND MISSILE COLORS (COMING SOON)";
constexpr std::array<std::string_view, COLOR_MODE_COUNT> COLOR_MODE_LABELS{
    "OFF", "PROTANOPIA", "DEUTERANOPIA", "TRITANOPIA"};
constexpr const char* REDUCE_FLASHES_LABEL = "REDUCE FLASHES";
constexpr const char* REDUCE_FLASHES_DESCRIPTION = "> NO FLASH OR SHAKE";
constexpr const char* SOUND_CUES_LABEL = "SOUND CUES";
constexpr const char* SOUND_CUES_DESCRIPTION =
    "> SOUNDS SHOWN ON SCREEN (COMING SOON)";
constexpr const char* AUTO_FIRE_LABEL = "AUTO-FIRE";
constexpr const char* AUTO_FIRE_DESCRIPTION =
    "> SHOOTS WITHOUT HOLDING (COMING SOON)";

/** @brief BACK, and the stamp written after a save, SAVED for
 * SAVED_STAMP_DURATION, NOT SAVED until the next save. */
constexpr const char* BACK_DESCRIPTION = "> CLOSES THE OPTIONS. ESC DOES TOO";
constexpr const char* SAVED_STAMP = "SAVED";
constexpr const char* NOT_SAVED_STAMP = "NOT SAVED";
constexpr engine::Duration SAVED_STAMP_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client
