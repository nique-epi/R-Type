#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string_view>
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Module name the game screen writes its log lines under. */
constexpr std::string_view GAME_SCREEN_LOGGER_NAME = "GameScreen";

/** @brief Key whose release opens the options during a game: the key that
 * closes every menu, so the same key opens and closes them. */
constexpr sf::Keyboard::Key OPTIONS_KEY = MENU_CANCEL_KEY;

/** @brief Asset id of the logo of the title screen, and its size in
 * pixels. */
constexpr const char* LOGO_ID = "sprites/logo.png";
constexpr sf::Vector2i LOGO_SIZE(210, 42);

/** @brief Module name the intro writes its log line under. */
constexpr std::string_view INTRO_LOG_MODULE_NAME = "Intro";

/**
 * @brief Texts of the title screen. The copyright sign is written as its two
 * UTF-8 bytes, so every compiler stores the same text.
 */
constexpr const char* GAME_VERSION_TEXT = "VER 0.1";
constexpr const char* TITLE_SUBTITLE = "4-PLAYER ONLINE CO-OP";
constexpr const char* PLAY_LABEL = "PLAY";
constexpr const char* PLAY_DESCRIPTION = "> JOIN A SERVER, FIGHT";
constexpr const char* SETTINGS_LABEL = "SETTINGS";
constexpr const char* SETTINGS_DESCRIPTION = "> SOUND, KEYS, ACCESS";
constexpr const char* QUIT_LABEL = "QUIT";
constexpr const char* QUIT_DESCRIPTION = "> LEAVE THE GAME";
constexpr const char* COPYRIGHT_TEXT = "\xC2\xA9 2026 R-TYPE TEAM";

/** @brief Texts of the quit confirmation. */
constexpr const char* CONFIRM_HEADING = "CONFIRM";
constexpr const char* QUIT_QUESTION = "ARE YOU SURE YOU WANT TO QUIT?";
constexpr const char* NO_LABEL = "NO";
constexpr const char* YES_LABEL = "YES";

/**
 * @brief Size of the progress bar of the loading screen, centered in the
 * playfield, and thickness of its border, in playfield units.
 */
constexpr float LOADING_BAR_WIDTH = 960.0F;
constexpr float LOADING_BAR_HEIGHT = 24.0F;
constexpr float LOADING_BAR_OUTLINE_THICKNESS = 3.0F;

}  // namespace rtype::client
