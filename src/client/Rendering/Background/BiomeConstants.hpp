#pragma once

#include <array>
#include <string_view>
#include "Biome.hpp"
#include "BiomeLayer.hpp"

namespace rtype::client {

/** @brief The share of the scroll of the level each layer of a decor moves
 * by: the near layer moves with the level, the haze eight times slower. */
constexpr float HAZE_LAYER_SCROLL_FACTOR = 0.125F;
constexpr float FAR_LAYER_SCROLL_FACTOR = 0.25F;
constexpr float MIDDLE_LAYER_SCROLL_FACTOR = 0.5F;
constexpr float NEAR_LAYER_SCROLL_FACTOR = 1.0F;

/** @brief The asset ids of the layers of each biome, pictures written by
 * tests/tools/biome-layers/make_biome_layers.py. */
constexpr std::string_view DRIFT_FIELDS_HAZE_LAYER_ID =
    "backgrounds/drift_fields_haze.png";
constexpr std::string_view DRIFT_FIELDS_FAR_LAYER_ID =
    "backgrounds/drift_fields_far.png";
constexpr std::string_view DRIFT_FIELDS_MIDDLE_LAYER_ID =
    "backgrounds/drift_fields_middle.png";
constexpr std::string_view DRIFT_FIELDS_NEAR_LAYER_ID =
    "backgrounds/drift_fields_near.png";
constexpr std::string_view EMBER_REACH_HAZE_LAYER_ID =
    "backgrounds/ember_reach_haze.png";
constexpr std::string_view EMBER_REACH_FAR_LAYER_ID =
    "backgrounds/ember_reach_far.png";
constexpr std::string_view EMBER_REACH_MIDDLE_LAYER_ID =
    "backgrounds/ember_reach_middle.png";
constexpr std::string_view EMBER_REACH_NEAR_LAYER_ID =
    "backgrounds/ember_reach_near.png";
constexpr std::string_view HIVE_DEPTHS_HAZE_LAYER_ID =
    "backgrounds/hive_depths_haze.png";
constexpr std::string_view HIVE_DEPTHS_FAR_LAYER_ID =
    "backgrounds/hive_depths_far.png";
constexpr std::string_view HIVE_DEPTHS_MIDDLE_LAYER_ID =
    "backgrounds/hive_depths_middle.png";
constexpr std::string_view HIVE_DEPTHS_NEAR_LAYER_ID =
    "backgrounds/hive_depths_near.png";

/** @brief The layers of each biome, from the farthest to the nearest. */
constexpr std::array DRIFT_FIELDS_LAYERS{
    BiomeLayer{.image = DRIFT_FIELDS_HAZE_LAYER_ID,
               .scrollFactor = HAZE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = DRIFT_FIELDS_FAR_LAYER_ID,
               .scrollFactor = FAR_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = DRIFT_FIELDS_MIDDLE_LAYER_ID,
               .scrollFactor = MIDDLE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = DRIFT_FIELDS_NEAR_LAYER_ID,
               .scrollFactor = NEAR_LAYER_SCROLL_FACTOR}};
constexpr std::array EMBER_REACH_LAYERS{
    BiomeLayer{.image = EMBER_REACH_HAZE_LAYER_ID,
               .scrollFactor = HAZE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = EMBER_REACH_FAR_LAYER_ID,
               .scrollFactor = FAR_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = EMBER_REACH_MIDDLE_LAYER_ID,
               .scrollFactor = MIDDLE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = EMBER_REACH_NEAR_LAYER_ID,
               .scrollFactor = NEAR_LAYER_SCROLL_FACTOR}};
constexpr std::array HIVE_DEPTHS_LAYERS{
    BiomeLayer{.image = HIVE_DEPTHS_HAZE_LAYER_ID,
               .scrollFactor = HAZE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = HIVE_DEPTHS_FAR_LAYER_ID,
               .scrollFactor = FAR_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = HIVE_DEPTHS_MIDDLE_LAYER_ID,
               .scrollFactor = MIDDLE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.image = HIVE_DEPTHS_NEAR_LAYER_ID,
               .scrollFactor = NEAR_LAYER_SCROLL_FACTOR}};

/** @brief The names the server gives the biomes, as a level script writes
 * them in `level.biome`. */
constexpr std::string_view DRIFT_FIELDS_BIOME_NAME = "drift_fields";
constexpr std::string_view EMBER_REACH_BIOME_NAME = "ember_reach";
constexpr std::string_view HIVE_DEPTHS_BIOME_NAME = "hive_depths";

/** @brief Every biome the client has a decor for; a name it lacks shows the
 * sky without any decor. */
constexpr std::array BIOMES{
    Biome{.name = DRIFT_FIELDS_BIOME_NAME, .layers = DRIFT_FIELDS_LAYERS},
    Biome{.name = EMBER_REACH_BIOME_NAME, .layers = EMBER_REACH_LAYERS},
    Biome{.name = HIVE_DEPTHS_BIOME_NAME, .layers = HIVE_DEPTHS_LAYERS}};

}  // namespace rtype::client
