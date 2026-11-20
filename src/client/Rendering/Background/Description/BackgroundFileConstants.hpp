#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "BackgroundFileKeyConstants.hpp"
#include "DecimalRange.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::client {

/** @brief The folder of a content folder that holds its backgrounds, one
 * `.toml` file each, named after the background. */
constexpr std::string_view BACKGROUND_FOLDER_IN_ASSETS =
    game::BACKGROUND_FOLDER_IN_ASSETS;

/** @brief The keys of the root of a background file: its layers, from the
 * farthest to the nearest, its emission groups, and the music a game plays
 * while it shows, which it may leave out. */
constexpr std::string_view LAYER_TABLES_KEY = game::BACKGROUND_LAYER_TABLES_KEY;
constexpr std::string_view GROUP_TABLES_KEY = "group";
constexpr std::string_view BACKGROUND_MUSIC_KEY = "music";
constexpr std::array BACKGROUND_ROOT_KEYS{LAYER_TABLES_KEY, GROUP_TABLES_KEY,
                                          BACKGROUND_MUSIC_KEY};

/** @brief The most layers and groups a background holds, sprites a layer of
 * particles draws from, and colors a trail goes through. */
constexpr std::size_t MOST_BACKGROUND_LAYERS = 32;
constexpr std::size_t MOST_EMISSION_GROUPS = 8;
constexpr std::size_t MOST_PARTICLE_SPRITES = 16;
constexpr std::size_t MOST_TRAIL_STOPS = 16;

/** @brief The keys of a layer: its name, its motion and whether it is the
 * entrance, both of which it may leave out, and its content, exactly one of
 * LAYER_CONTENT_KEYS. */
constexpr std::string_view LAYER_NAME_KEY = "name";
constexpr std::string_view LAYER_MOTION_KEY = "motion";
constexpr std::string_view LAYER_ENTRANCE_KEY = "entrance";
constexpr std::string_view IMAGE_CONTENT_KEY = game::BACKGROUND_IMAGE_KEY;
constexpr std::string_view TILE_CONTENT_KEY = game::BACKGROUND_TILES_KEY;
constexpr std::string_view PARTICLE_CONTENT_KEY =
    game::BACKGROUND_PARTICLES_KEY;
constexpr std::string_view PROP_CONTENT_KEY = game::BACKGROUND_PROPS_KEY;
constexpr std::array LAYER_CONTENT_KEYS{IMAGE_CONTENT_KEY, TILE_CONTENT_KEY,
                                        PARTICLE_CONTENT_KEY, PROP_CONTENT_KEY};
constexpr std::array LAYER_KEYS{LAYER_NAME_KEY,     LAYER_MOTION_KEY,
                                LAYER_ENTRANCE_KEY, IMAGE_CONTENT_KEY,
                                TILE_CONTENT_KEY,   PARTICLE_CONTENT_KEY,
                                PROP_CONTENT_KEY};

/** @brief The keys of `motion`, each of which it may leave out, and what
 * they accept: a share of the scroll, background pixels per second (a
 * negative one moves to the right), and a width in background pixels. */
constexpr std::string_view PARALLAX_KEY = "parallax";
constexpr std::string_view AUTOSCROLL_KEY = "autoscroll";
constexpr std::string_view REPEAT_KEY = "repeat";
constexpr std::string_view WARPS_KEY = "warps";
constexpr std::array MOTION_KEYS{PARALLAX_KEY, AUTOSCROLL_KEY, REPEAT_KEY,
                                 WARPS_KEY};
constexpr engine::DecimalRange PARALLAX_RANGE{.lowest = 0.0, .highest = 4.0};
constexpr engine::DecimalRange AUTOSCROLL_RANGE{.lowest = -4096.0,
                                                .highest = 4096.0};
constexpr engine::DecimalRange REPEAT_RANGE{.lowest = 1.0, .highest = 65536.0};

/** @brief A place and a size of a background file, in background pixels. */
constexpr engine::DecimalRange PLACE_RANGE{.lowest = -65536.0,
                                           .highest = 65536.0};
constexpr engine::DecimalRange SIZE_RANGE{.lowest = 0.0, .highest = 65536.0};

/** @brief The keys of `image`: the asset id of its file, and where its
 * top-left corner lies, 0 on an axis it leaves out. */
constexpr std::string_view FILE_KEY = game::BACKGROUND_FILE_KEY;
constexpr std::string_view IMAGE_X_KEY = "x";
constexpr std::string_view IMAGE_Y_KEY = "y";
constexpr std::array IMAGE_KEYS{FILE_KEY, IMAGE_X_KEY, IMAGE_Y_KEY};

/** @brief The keys of `tiles` and of `props`: the asset ids of the LDtk
 * project and of the tileset, and the name of the layer of the project. */
constexpr std::string_view PROJECT_KEY = game::BACKGROUND_PROJECT_KEY;
constexpr std::string_view PROJECT_LAYER_KEY = "layer";
constexpr std::string_view TILESET_KEY = game::BACKGROUND_TILESET_KEY;
constexpr std::array PROJECT_LAYER_KEYS{PROJECT_KEY, PROJECT_LAYER_KEY,
                                        TILESET_KEY};

/** @brief The keys every `particles` holds: where they are born, and their
 * look, a pixel or sprites. */
constexpr std::string_view AREA_KEY = "area";
constexpr std::string_view PIXEL_KEY = "pixel";
constexpr std::string_view SPRITES_KEY = game::BACKGROUND_SPRITES_KEY;

/** @brief The keys of `area`, all four needed. */
constexpr std::string_view AREA_LEFT_KEY = "left";
constexpr std::string_view AREA_TOP_KEY = "top";
constexpr std::string_view AREA_WIDTH_KEY = "width";
constexpr std::string_view AREA_HEIGHT_KEY = "height";
constexpr std::array AREA_KEYS{AREA_LEFT_KEY, AREA_TOP_KEY, AREA_WIDTH_KEY,
                               AREA_HEIGHT_KEY};

/** @brief The keys of the particles of a population: how many, the color a
 * pixel twinkles in, how often, and the color of their streak in a warp; all
 * but the first may be left out. */
constexpr std::string_view POPULATION_KEY = "population";
constexpr std::string_view TWINKLE_KEY = "twinkle";
constexpr std::string_view TWINKLE_STEPS_KEY = "twinkle_steps";
constexpr std::string_view WARP_TRAIL_KEY = "warp_trail";
constexpr std::array POPULATION_KEYS{
    POPULATION_KEY, AREA_KEY,          PIXEL_KEY,     SPRITES_KEY,
    TWINKLE_KEY,    TWINKLE_STEPS_KEY, WARP_TRAIL_KEY};
constexpr engine::WholeNumberRange POPULATION_RANGE{.lowest = 1,
                                                    .highest = 4096};
constexpr engine::WholeNumberRange TWINKLE_STEPS_RANGE{.lowest = 1,
                                                       .highest = 1024};

/** @brief The keys of the particles of a group: the name of the group, and,
 * each of which may be left out, their chance in it, their speed, their
 * angle, their drift, their life, how far out of the background they end, and
 * their trail. */
constexpr std::string_view PARTICLE_GROUP_KEY = "group";
constexpr std::string_view WEIGHT_KEY = "weight";
constexpr std::string_view SPEED_KEY = "speed";
constexpr std::string_view ANGLE_KEY = "angle";
constexpr std::string_view DRIFT_KEY = "drift";
constexpr std::string_view LIFE_KEY = "life_ms";
constexpr std::string_view MARGIN_KEY = "margin";
constexpr std::string_view TRAIL_KEY = "trail";
constexpr std::array EMITTED_KEYS{PARTICLE_GROUP_KEY, WEIGHT_KEY,  AREA_KEY,
                                  PIXEL_KEY,          SPRITES_KEY, SPEED_KEY,
                                  ANGLE_KEY,          DRIFT_KEY,   LIFE_KEY,
                                  MARGIN_KEY,         TRAIL_KEY};
constexpr double DEFAULT_WEIGHT = 1.0;
constexpr engine::DecimalRange WEIGHT_RANGE{.lowest = 0.001, .highest = 1000.0};
constexpr engine::DecimalRange SPEED_RANGE{.lowest = 0.0, .highest = 8192.0};
constexpr engine::DecimalRange ANGLE_RANGE{.lowest = 0.0, .highest = 360.0};
constexpr engine::DecimalRange LIFE_RANGE{.lowest = 1.0, .highest = 600000.0};

/** @brief The angle of the particles of a layer that gives none: they fly to
 * the left, as everything the level scrolls past. */
constexpr double LEFTWARD_ANGLE = 180.0;

/** @brief The keys of a sprite: the asset id of its image, and, each of which
 * may be left out, how many frames it holds side by side, its chance among
 * the sprites of its layer, the pixel of a frame that lies on the particle,
 * and the frames per second it loops at. */
constexpr std::string_view FRAMES_KEY = "frames";
constexpr std::string_view ANCHOR_X_KEY = "anchor_x";
constexpr std::string_view ANCHOR_Y_KEY = "anchor_y";
constexpr std::string_view FRAMES_PER_SECOND_KEY = "fps";
constexpr std::array SPRITE_KEYS{FILE_KEY,     FRAMES_KEY,
                                 WEIGHT_KEY,   ANCHOR_X_KEY,
                                 ANCHOR_Y_KEY, FRAMES_PER_SECOND_KEY};
constexpr std::size_t DEFAULT_FRAMES = 1;
constexpr engine::WholeNumberRange FRAMES_RANGE{.lowest = 1, .highest = 1024};
constexpr engine::DecimalRange FRAMES_PER_SECOND_RANGE{.lowest = 0.0,
                                                       .highest = 240.0};

/** @brief The keys of `trail`, all needed: its longest, in background pixels,
 * how fast it grows and shrinks, in background pixels per second, and its
 * colors, each a table of TRAIL_STOP_KEYS. */
constexpr std::string_view TRAIL_LONGEST_KEY = "longest";
constexpr std::string_view TRAIL_GROWTH_KEY = "grow";
constexpr std::string_view TRAIL_SHRINK_KEY = "shrink";
constexpr std::string_view TRAIL_STOPS_KEY = "stops";
constexpr std::array TRAIL_KEYS{TRAIL_LONGEST_KEY, TRAIL_GROWTH_KEY,
                                TRAIL_SHRINK_KEY, TRAIL_STOPS_KEY};
constexpr std::string_view TRAIL_STOP_UNTIL_KEY = "until";
constexpr std::string_view TRAIL_STOP_COLOR_KEY = "color";
constexpr std::array TRAIL_STOP_KEYS{TRAIL_STOP_UNTIL_KEY,
                                     TRAIL_STOP_COLOR_KEY};
constexpr engine::DecimalRange TRAIL_LENGTH_RANGE{.lowest = 1.0,
                                                  .highest = 1024.0};
constexpr engine::DecimalRange TRAIL_PACE_RANGE{.lowest = 0.001,
                                                .highest = 65536.0};
constexpr engine::DecimalRange TRAIL_SHARE_RANGE{.lowest = 0.0, .highest = 1.0};

/** @brief The keys of a group, all needed: its name, the most particles of
 * its layers alive at once, the milliseconds before its first birth, and the
 * milliseconds drawn between a birth and the next. */
constexpr std::string_view GROUP_NAME_KEY = "name";
constexpr std::string_view MOST_ALIVE_KEY = "most_alive";
constexpr std::string_view FIRST_BIRTH_KEY = "first_ms";
constexpr std::string_view BIRTH_INTERVAL_KEY = "every_ms";
constexpr std::array GROUP_KEYS{GROUP_NAME_KEY, MOST_ALIVE_KEY, FIRST_BIRTH_KEY,
                                BIRTH_INTERVAL_KEY};
constexpr engine::WholeNumberRange MOST_ALIVE_RANGE{.lowest = 1, .highest = 64};
constexpr engine::WholeNumberRange FIRST_BIRTH_RANGE{.lowest = 0,
                                                     .highest = 600000};
constexpr engine::DecimalRange BIRTH_INTERVAL_RANGE{.lowest = 1.0,
                                                    .highest = 600000.0};

/** @brief The names a color key accepts, and the color of each, in the same
 * order: the palette of the sky. */
constexpr std::array PALETTE_NAMES{
    std::string_view{"deep_blue"}, std::string_view{"blue"},
    std::string_view{"pale_blue"}, std::string_view{"star_white"},
    std::string_view{"star_gold"}};
constexpr std::array PALETTE_COLORS{DEEP_BLUE, BLUE, PALE_BLUE, STAR_WHITE,
                                    STAR_GOLD};
static_assert(PALETTE_NAMES.size() == PALETTE_COLORS.size(),
              "one name per color");

/** @brief What a background file breaks when it is refused for more than one
 * key, as the error says it. */
constexpr std::string_view LAYER_CONTENT_RULE =
    "a layer holds exactly one of image, tiles, particles and props";
constexpr std::string_view ONE_ENTRANCE_RULE =
    "one layer at most is the entrance of a background";
constexpr std::string_view EMISSION_RULE =
    "particles hold either population or group";
constexpr std::string_view UNKNOWN_GROUP_RULE =
    "no [[group]] of the file has this name";
constexpr std::string_view SHARED_GROUP_NAME_RULE =
    "another [[group]] of the file has this name";
constexpr std::string_view PARTICLE_LOOK_RULE =
    "particles hold either pixel or at least one sprite";
constexpr std::string_view TWINKLE_RULE =
    "twinkle, the color a pixel twinkles in, goes with pixel and "
    "twinkle_steps, and a pixel with twinkle_steps needs it";
constexpr std::string_view ENDLESS_PARTICLE_RULE =
    "the particles of a group end with life_ms, or leave with a speed above 0";
constexpr std::string_view ASSET_ID_RULE =
    "a file is named by its asset id: lowercase letters, digits and _-., "
    "its folders separated by /";

}  // namespace rtype::client
