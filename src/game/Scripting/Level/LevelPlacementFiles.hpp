#pragma once

#include <filesystem>
#include <span>
#include "EntitySheets.hpp"
#include "LuauLevels.hpp"

namespace rtype::game {

/**
 * @brief Gives each level of @p levels that names an LDtk file the enemies
 * that file places (see ldtkPlacementsOf()): the file is looked up in
 * @p contentFolders as engine::contentFilesOf() does, the resource laid last
 * winning, and each entity must name a sheet of @p types that a behaviour
 * pilots, never a ship.
 * @throws engine::UnreadableDataFileException, naming the file, when no
 * content folder holds it; what ldtkPlacementsOf() throws; and
 * InvalidLdtkFileException, naming the entity and where it is placed, when
 * an entity names no sheet, or a ship.
 */
void readLevelPlacements(LuauLevels& levels,
                         std::span<const std::filesystem::path> contentFolders,
                         const EntitySheets& types);

}  // namespace rtype::game
