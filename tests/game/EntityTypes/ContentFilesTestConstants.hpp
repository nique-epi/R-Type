#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief Two sheets of a folder of assets, given out of the order of their
 * names, both naming one sprite sheet, and the paths they get in a pack. */
constexpr std::string_view WEAVER_SHEET_FILE = "entities/weaver.toml";
constexpr std::string_view DRIFTER_SHEET_FILE = "entities/drifter.toml";
constexpr std::string_view SHARED_SPRITE_SHEET = "sprites/bydos.png";
constexpr std::string_view SHEET_NAMING_THE_BYDOS =
    "[gameplay]\n[appearance]\nsheet = \"sprites/bydos.png\"\n";
constexpr std::string_view BYDO_PIXELS = "\x89PNG\r\n\x1a\nbydos";

/** @brief A sprite sheet no sheet names, which a pack leaves out. */
constexpr std::string_view UNNAMED_SPRITE_SHEET = "sprites/hud.png";
constexpr std::string_view HUD_PIXELS = "\x89PNG\r\n\x1a\nhud";

/** @brief A sprite sheet whose path comes before `entities/`, and a sheet
 * naming it. */
constexpr std::string_view ART_SPRITE_SHEET = "art/bydos.png";
constexpr std::string_view SHEET_NAMING_THE_ART =
    "[gameplay]\n[appearance]\nsheet = \"art/bydos.png\"\n";

/** @brief A sheet whose sprite sheet is missing from the folder. */
constexpr std::string_view SHEET_NAMING_A_MISSING_IMAGE =
    "[gameplay]\n[appearance]\nsheet = \"sprites/missing.png\"\n";

/** @brief A sheet that names no sprite sheet. */
constexpr std::string_view SHEET_WITHOUT_SPRITE_SHEET =
    "[gameplay]\n[appearance]\n";

/** @brief A sheet and its sprite sheet, or two sheets and their one sprite
 * sheet. */
constexpr std::size_t FILES_OF_ONE_SHEET_AND_ITS_IMAGE = 2;
constexpr std::size_t FILES_OF_TWO_SHEETS_AND_ONE_IMAGE = 3;

/** @brief Where the sheet of a type lies in the assets folder:
 * `entities/<name>.toml`. */
constexpr std::string_view SHEET_FOLDER_PREFIX = "entities/";
constexpr std::string_view SHEET_FILE_EXTENSION = ".toml";

/** @brief How many times a file is listed: once. */
constexpr std::ptrdiff_t LISTED_ONCE = 1;

/** @brief Sprite sheets the shipped sheets name: the sheet of the first
 * Bydos and the sheet of the Bydomancer, which three sheets name. */
constexpr std::string_view SHIPPED_ENTITY_SPRITES = "sprites/entities.png";
constexpr std::string_view SHIPPED_BYDOMANCER_SPRITES =
    "sprites/mobs/bydomancer.png";

}  // namespace rtype::game::testing
