#pragma once

#include <array>
#include <string_view>
#include "Biome.hpp"
#include "BiomeLayer.hpp"

namespace rtype::client {

/** @brief The share of the scroll of the level each layer of a decor moves
 * by: the near layer moves with the level, the others lag behind it. */
constexpr float FAR_LAYER_SCROLL_FACTOR = 0.25F;
constexpr float MIDDLE_LAYER_SCROLL_FACTOR = 0.5F;
constexpr float NEAR_LAYER_SCROLL_FACTOR = 1.0F;

/** @brief The LDtk project of the Pit, written by generator/procedural/, and
 * the tilesets of its three planes. */
constexpr std::string_view FOSSE_PROJECT_ID = "biomes/fosse/fosse.ldtk";
constexpr std::string_view FOSSE_FAR_TILESET_ID = "biomes/fosse/fosse_far.png";
constexpr std::string_view FOSSE_MIDDLE_TILESET_ID =
    "biomes/fosse/fosse_mid.png";
constexpr std::string_view FOSSE_NEAR_TILESET_ID =
    "biomes/fosse/fosse_near.png";

/** @brief The tile layers of the project of the Pit, one per plane. */
constexpr std::string_view FOSSE_FAR_LAYER_NAME = "Far";
constexpr std::string_view FOSSE_MIDDLE_LAYER_NAME = "Mid";
constexpr std::string_view FOSSE_NEAR_LAYER_NAME = "Near";

/** @brief The layers of the Pit, from the farthest to the nearest. */
constexpr std::array FOSSE_LAYERS{
    BiomeLayer{.layer = FOSSE_FAR_LAYER_NAME,
               .tileset = FOSSE_FAR_TILESET_ID,
               .scrollFactor = FAR_LAYER_SCROLL_FACTOR},
    BiomeLayer{.layer = FOSSE_MIDDLE_LAYER_NAME,
               .tileset = FOSSE_MIDDLE_TILESET_ID,
               .scrollFactor = MIDDLE_LAYER_SCROLL_FACTOR},
    BiomeLayer{.layer = FOSSE_NEAR_LAYER_NAME,
               .tileset = FOSSE_NEAR_TILESET_ID,
               .scrollFactor = NEAR_LAYER_SCROLL_FACTOR}};

/** @brief The names the server gives the biomes, as a level script writes
 * them in `level.biome`. */
constexpr std::string_view FOSSE_BIOME_NAME = "fosse";

/** @brief Every biome the client has a decor for; a name it lacks shows the
 * stars without any decor. */
constexpr std::array BIOMES{Biome{.name = FOSSE_BIOME_NAME,
                                  .project = FOSSE_PROJECT_ID,
                                  .layers = FOSSE_LAYERS}};

}  // namespace rtype::client
