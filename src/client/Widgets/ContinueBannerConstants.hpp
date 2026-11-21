#pragma once

#include <SFML/Graphics/Color.hpp>
#include <chrono>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The banner of a continue, shown CONTINUE_BANNER_TIME from the
 * LevelStatus that tells the team took one: CONTINUE_BANNER_TITLE, then under
 * it the continues left and CONTINUES_LEFT_SUFFIX, or NO_CONTINUE_LEFT_TEXT
 * once the team has none, so its next loss ends the game.
 */
constexpr std::string_view CONTINUE_BANNER_TITLE = "CONTINUE";
constexpr std::string_view CONTINUES_LEFT_SUFFIX = " LEFT";
constexpr std::string_view NO_CONTINUE_LEFT_TEXT = "NO CONTINUE LEFT";
constexpr engine::Duration CONTINUE_BANNER_TIME = std::chrono::seconds(2);

/** @brief The banner is centered where the banner that opens a game is,
 * MISSION_BANNER_AREA, between rules of that banner, whole: its
 * CONTINUE_BANNER_LINES lines, the second CONTINUE_BANNER_LINE_GAP pixels
 * under the first. */
constexpr int CONTINUE_BANNER_LINES = 2;
constexpr int CONTINUE_BANNER_LINE_GAP = 3;

/** @brief Colors of the banner; the line of a team without a continue is in
 * the color of the warnings of the status band. */
constexpr sf::Color CONTINUE_BANNER_TITLE_COLOR = STAR_WHITE;
constexpr sf::Color CONTINUES_LEFT_COLOR = HUD_CYAN;
constexpr sf::Color NO_CONTINUE_LEFT_COLOR = WARNING_ORANGE;
constexpr sf::Color CONTINUE_BANNER_RULE_COLOR = HUD_CYAN;

}  // namespace rtype::client
