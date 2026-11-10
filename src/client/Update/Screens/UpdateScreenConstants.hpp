#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "InterfaceColorConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

/** @brief Texts of the update panel. */
constexpr std::string_view UPDATE_HEADING = "UPDATE AVAILABLE";
constexpr std::string_view VERSION_PREFIX = "VER ";
constexpr std::string_view VERSION_ARROW = " > VER ";
constexpr std::string_view DOWNLOADING_TEXT = "DOWNLOADING ";
constexpr std::string_view PERCENT_SIGN = "%";
constexpr std::string_view INSTALLING_TEXT = "INSTALLING";
constexpr std::string_view RESTARTING_TEXT = "RESTARTING";
constexpr std::string_view UPDATE_FAILED_TEXT = "UPDATE FAILED";

/** @brief The bar of the panel, from its top-left corner: as wide as the
 * panel less its margins of 16 pixels, a border of PANEL_BORDER_COLOR around a
 * fill of HUD_CYAN; then the status line under it. */
constexpr sf::Vector2i UPDATE_BAR_OFFSET(16, 52);
constexpr sf::Vector2i UPDATE_BAR_SIZE(240, 8);
constexpr int UPDATE_BAR_BORDER = 1;
constexpr sf::Color UPDATE_BAR_BORDER_COLOR = PANEL_BORDER_COLOR;
constexpr sf::Color UPDATE_BAR_FILL_COLOR = HUD_CYAN;
constexpr sf::Vector2i UPDATE_STATUS_OFFSET(16, 72);

/** @brief How long RESTARTING shows before the client closes, and UPDATE
 * FAILED before the panel lets the player go. */
constexpr engine::Duration RESTART_MESSAGE_DURATION =
    std::chrono::milliseconds(1000);
constexpr engine::Duration FAILED_MESSAGE_DURATION =
    std::chrono::milliseconds(3000);

/** @brief The panel of WHAT'S NEW, centered: a heading and its rule, up to
 * MAXIMUM_NOTE_LINES lines each after a bullet, from y 36 to 170 of the panel,
 * then PRESS ENTER centered at y 196, inside its 222 pixels. */
constexpr sf::Vector2i WHATS_NEW_PANEL_SIZE(400, 222);
constexpr std::string_view WHATS_NEW_HEADING = "WHAT'S NEW IN VER ";
constexpr sf::Vector2i WHATS_NEW_RULE_OFFSET(16, 22);
constexpr sf::Vector2i WHATS_NEW_RULE_SIZE(368, 1);
constexpr sf::Vector2i WHATS_NEW_BULLET_OFFSET(16, 36);
constexpr sf::Vector2i WHATS_NEW_LINE_OFFSET(28, 36);
constexpr int WHATS_NEW_LINE_PITCH = 18;
constexpr std::string_view NOTE_BULLET = "-";
constexpr sf::Vector2i WHATS_NEW_HINT_OFFSET(200, 196);
constexpr std::string_view WHATS_NEW_HINT = "PRESS ENTER";
constexpr TextStyle NOTE_BULLET_STYLE{.font = TextFont::Small,
                                      .color = STATUS_TEXT_COLOR};
constexpr TextStyle NOTE_LINE_STYLE{.font = TextFont::Small,
                                    .color = DIALOG_MESSAGE_COLOR};
constexpr TextStyle WHATS_NEW_HINT_STYLE{.font = TextFont::Regular,
                                         .color = STATUS_TEXT_COLOR,
                                         .alignment = TextAlignment::Center};

}  // namespace rtype::client
