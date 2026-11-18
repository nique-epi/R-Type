#pragma once

#include <array>
#include <string_view>

namespace rtype::game {

/** @brief The folder of a content folder that holds its backgrounds, one
 * `.toml` file each. */
constexpr std::string_view BACKGROUND_FOLDER_IN_ASSETS = "backgrounds";

/** @brief The key of the layers of a background file, an array of tables. */
constexpr std::string_view BACKGROUND_LAYER_TABLES_KEY = "layer";

/** @brief The keys of what a layer shows, one table each: an image, the
 * tiles or the props of an LDtk project, particles. */
constexpr std::string_view BACKGROUND_IMAGE_KEY = "image";
constexpr std::string_view BACKGROUND_TILES_KEY = "tiles";
constexpr std::string_view BACKGROUND_PROPS_KEY = "props";
constexpr std::string_view BACKGROUND_PARTICLES_KEY = "particles";

/** @brief The keys under which a background names a file: the image of a
 * layer or of a sprite, the LDtk project of tiles or props, and the image
 * their pictures are cut from. */
constexpr std::string_view BACKGROUND_FILE_KEY = "file";
constexpr std::string_view BACKGROUND_PROJECT_KEY = "project";
constexpr std::string_view BACKGROUND_TILESET_KEY = "tileset";

/** @brief The key of the sprites of particles, an array of tables that each
 * name an image under BACKGROUND_FILE_KEY. */
constexpr std::string_view BACKGROUND_SPRITES_KEY = "sprites";

/** @brief The keys that name a file in an image, and in tiles or props. */
constexpr std::array BACKGROUND_IMAGE_FILE_KEYS{BACKGROUND_FILE_KEY};
constexpr std::array BACKGROUND_MAP_FILE_KEYS{BACKGROUND_PROJECT_KEY,
                                              BACKGROUND_TILESET_KEY};

}  // namespace rtype::game
