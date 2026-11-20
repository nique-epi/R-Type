#pragma once

#include <string_view>
#include "LdtkConstants.hpp"
#include "PlayfieldConstants.hpp"

namespace rtype::game {

/** @brief A column of the ground without floor or ceiling. */
constexpr int GROUND_NONE = -1;

/** @brief Where the first map of a biome begins as the biome begins, in
 * pixels of its maps from the left edge of the playfield: the right edge, as
 * the client draws the near plane of a background that arrives through its
 * entrance. */
constexpr int GROUND_ENTRANCE_PIXEL =
    static_cast<int>(PLAYFIELD_WIDTH / LDTK_PIXEL);

/** @brief Where the LDtk project of the maps of a biome lies in a content
 * folder: biomes/<biome>/<biome>.ldtk. */
constexpr std::string_view BIOMES_FOLDER = "biomes";
constexpr std::string_view BIOME_PROJECT_EXTENSION = ".ldtk";

/** @brief The level fields of an LDtk project that hold the ground of each of
 * its levels, as generator/procedural/ground_fields.py writes them. */
constexpr std::string_view FLOOR_FIELD = "Floor";
constexpr std::string_view CEILING_FIELD = "Ceiling";

/** @brief Why the ground of a project is refused; each is followed by the
 * identifier of the level. */
constexpr std::string_view GROUND_FIELD_MISSING_REASON =
    "the field Floor or Ceiling is missing in the level ";
constexpr std::string_view GROUND_FIELD_SIZE_REASON =
    "the field Floor or Ceiling is not a whole number for each pixel column "
    "of the level ";

}  // namespace rtype::game
