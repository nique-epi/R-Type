#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief Three content folders inside a temporary folder: the assets, a
 * resource laid over them, and a resource laid over both. */
constexpr std::string_view ASSETS_BELOW_THE_RESOURCES = "assets";
constexpr std::string_view MOON_RESOURCE = "resources/moon";
constexpr std::string_view LATER_RESOURCE = "resources/later";

/** @brief A background that shows one image, the path of both in a content
 * folder, and the bytes of the image. */
constexpr std::string_view MOON_BACKGROUND_FILE = "backgrounds/moon.toml";
constexpr std::string_view MOON_IMAGE = "backgrounds/moon/moon.png";
constexpr std::string_view BACKGROUND_SHOWING_THE_MOON =
    "[[layer]]\nname = \"moon\"\n"
    "image = { file = \"backgrounds/moon/moon.png\" }\n";
constexpr std::string_view MOON_PIXELS = "\x89PNG\r\n\x1a\nmoon";

/** @brief A `.toml` file of a folder below the one of the backgrounds,
 * which is no background. */
constexpr std::string_view NOTES_BELOW_THE_BACKGROUNDS =
    "backgrounds/moon/notes.toml";

/** @brief What a later resource holds under the same names. */
constexpr std::string_view BACKGROUND_OF_THE_LATER_RESOURCE =
    "[[layer]]\nname = \"later moon\"\n"
    "image = { file = \"backgrounds/moon/moon.png\" }\n";
constexpr std::string_view LATER_MOON_PIXELS = "\x89PNG\r\n\x1a\nlater";

/** @brief The files of a background alone, and of a background and the one
 * image it names. */
constexpr std::size_t A_BACKGROUND_ALONE = 1;
constexpr std::size_t A_BACKGROUND_AND_ITS_IMAGE = 2;

/** @brief A background of tiles and of particles drawn with a sprite, and
 * the three files it names. */
constexpr std::string_view CAVE_BACKGROUND_FILE = "backgrounds/cave.toml";
constexpr std::string_view CAVE_DUST = "backgrounds/cave/dust.png";
constexpr std::string_view CAVE_PROJECT = "biomes/cave/cave.ldtk";
constexpr std::string_view CAVE_TILESET = "biomes/cave/walls.png";
constexpr std::string_view BACKGROUND_OF_WALLS_AND_DUST =
    "[[layer]]\nname = \"walls\"\n"
    "tiles = { project = \"biomes/cave/cave.ldtk\", layer = \"Walls\", "
    "tileset = \"biomes/cave/walls.png\" }\n\n"
    "[[layer]]\nname = \"dust\"\n\n[layer.particles]\npopulation = 4\n"
    "sprites = [{ file = \"backgrounds/cave/dust.png\" }]\n";
constexpr std::string_view CAVE_BYTES = "not read, only carried";

/** @brief The files of the cave: its background, its sprite, its project
 * and its tileset. */
constexpr std::size_t THE_CAVE_AND_ITS_THREE_FILES = 4;

}  // namespace rtype::game::testing
