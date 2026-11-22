#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "TextStyle.hpp"
#include "Translation.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

/** @brief The panel of WHAT'S NEW, centered: a heading and its rule, up to
 * MAXIMUM_NOTE_LINES lines each after a bullet, from y 36 to 170 of the panel,
 * then PRESS ENTER centered at y 196, inside its 222 pixels. */
constexpr sf::Vector2i WHATS_NEW_PANEL_SIZE(400, 222);
constexpr Translation WHATS_NEW_HEADING{
    .english = "WHAT'S NEW IN VER {VERSION}",
    .french = "NOUVEAUTÉS DE LA VER {VERSION}"};
constexpr std::string_view NOTES_VERSION_MARK = "{VERSION}";
constexpr sf::Vector2i WHATS_NEW_RULE_OFFSET(16, 22);
constexpr sf::Vector2i WHATS_NEW_RULE_SIZE(368, 1);
constexpr sf::Vector2i WHATS_NEW_BULLET_OFFSET(16, 36);
constexpr sf::Vector2i WHATS_NEW_LINE_OFFSET(28, 36);
constexpr int WHATS_NEW_LINE_PITCH = 18;
constexpr std::string_view NOTE_BULLET = "-";
constexpr sf::Vector2i WHATS_NEW_HINT_OFFSET(200, 196);
constexpr Translation WHATS_NEW_HINT{.english = "PRESS ENTER",
                                     .french = "APPUYEZ SUR ENTRÉE"};
constexpr TextStyle NOTE_BULLET_STYLE{.font = TextFont::Small,
                                      .color = STATUS_TEXT_COLOR};
constexpr TextStyle NOTE_LINE_STYLE{.font = TextFont::Small,
                                    .color = DIALOG_MESSAGE_COLOR};
constexpr TextStyle WHATS_NEW_HINT_STYLE{.font = TextFont::Regular,
                                         .color = STATUS_TEXT_COLOR,
                                         .alignment = TextAlignment::Center};

}  // namespace rtype::client
