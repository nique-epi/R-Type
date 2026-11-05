#pragma once

#include <string_view>

/** @brief The folder of the shipped entity sheets, in the assets folder. */
constexpr std::string_view SHIPPED_ENTITY_SHEET_FOLDER = "entities";

/** @brief A type the game does not ship, and the sprite sheet it is drawn
 * from, which no list of the game names. */
constexpr std::string_view NEW_ENTITY_TYPE_NAME = "voltjelly";
constexpr std::string_view NEW_ENTITY_SHEET_ID = "sprites/voltjelly.png";

/** @brief The text of its sheet, around the asset id of its sprite sheet. */
constexpr std::string_view NEW_APPEARANCE_BEFORE_SHEET =
    "[gameplay]\n[appearance]\nsheet = \"";
constexpr std::string_view NEW_APPEARANCE_AFTER_SHEET =
    "\"\n[appearance.idle]\nleft = 0\ntop = 0\nwidth = 8\nheight = 8\n"
    "frames = 1\nspacing = 0\nframe_ms = 100\n";
