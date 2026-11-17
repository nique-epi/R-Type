#pragma once

#include <cstddef>
#include <string_view>

/** @brief The two content folders of the tests, below their temporary
 * folder: the assets, then a resource laid over them. */
constexpr std::string_view ASSET_FOLDER = "assets";
constexpr std::string_view RESOURCE_FOLDER = "resource";

/** @brief The files the tests write, below their temporary folder. */
constexpr std::string_view SPACE_OF_THE_ASSETS =
    "assets/backgrounds/space.toml";
constexpr std::string_view SPACE_OF_THE_RESOURCE =
    "resource/backgrounds/space.toml";
constexpr std::string_view NEBULA_OF_THE_RESOURCE =
    "resource/backgrounds/nebula.toml";
constexpr std::string_view PLANET_OF_THE_ASSETS =
    "assets/backgrounds/space/planet.png";
constexpr std::string_view PLANET_OF_THE_RESOURCE =
    "resource/backgrounds/space/planet.png";
constexpr std::string_view MOON_OF_THE_RESOURCE =
    "resource/backgrounds/space/moon.png";

/** @brief What the tests write in an image: the catalog only looks for the
 * file, it never opens it. */
constexpr std::string_view ANY_IMAGE = "image";

/** @brief The names of the backgrounds those files hold, and one no file
 * holds. */
constexpr std::string_view SPACE_NAME = "space";
constexpr std::string_view NEBULA_NAME = "nebula";
constexpr std::string_view UNWRITTEN_NAME = "ocean";

/** @brief A sky of one planet and a sky of one moon, each named after its
 * only layer, and the asset ids of their images. */
constexpr std::string_view A_SKY_OF_ONE_PLANET =
    "[[layer]]\nname = \"planet\"\n"
    "image = { file = \"backgrounds/space/planet.png\" }\n";
constexpr std::string_view A_SKY_OF_ONE_MOON =
    "[[layer]]\nname = \"moon\"\n"
    "image = { file = \"backgrounds/space/moon.png\" }\n";
constexpr std::string_view PLANET_LAYER_NAME = "planet";
constexpr std::string_view MOON_LAYER_NAME = "moon";
constexpr std::string_view PLANET_ASSET_ID = "backgrounds/space/planet.png";
constexpr std::string_view MOON_ASSET_ID = "backgrounds/space/moon.png";

/** @brief The six images of the sky the repository ships and the three
 * tilesets of its Pit: nine images, the planet and the near walls among
 * them. */
constexpr std::size_t IMAGES_OF_THE_SHIPPED_BACKGROUNDS = 9;
constexpr std::string_view NEAR_WALL_TILESET = "biomes/fosse/fosse_near.png";

/** @brief The backgrounds the repository ships: the sky, whose events never
 * show more than four at once, and the Pit, three layers of walls entered
 * through the last. */
constexpr std::string_view SHIPPED_SKY_NAME = "space";
constexpr std::size_t SKY_EVENTS_AT_ONCE = 4;
constexpr std::string_view SHIPPED_PIT_NAME = "fosse";
constexpr std::size_t LAYERS_OF_THE_PIT = 3;
