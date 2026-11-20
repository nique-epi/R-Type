#pragma once

#include <filesystem>
#include <span>
#include "LuauLevels.hpp"

namespace rtype::game {

/**
 * @brief Gives each level of @p levels the ground of each biome it gives a
 * band (see ldtkGroundOf()): the LDtk project of a biome is
 * BIOMES_FOLDER/<biome>/<biome>BIOME_PROJECT_EXTENSION, looked up in
 * @p contentFolders as engine::contentFilesOf() does, the resource laid last
 * winning. A biome no content folder holds a project for, or whose project
 * holds no ground, gets none: `level.ground` then answers nil there.
 * @throws What ldtkGroundOf() throws.
 */
void readLevelGrounds(LuauLevels& levels,
                      std::span<const std::filesystem::path> contentFolders);

}  // namespace rtype::game
