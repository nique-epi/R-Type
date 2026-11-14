#include "LevelPlacementFiles.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <span>
#include <string>
#include "ContentFolders.hpp"
#include "DataFileReading.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "GameException.hpp"
#include "LdtkPlacement.hpp"
#include "LdtkPlacementReading.hpp"
#include "LdtkRefusal.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevels.hpp"

namespace rtype::game {

namespace {

/** @returns Where @p placement is, for an error: its entity and the center
 * of its box, rounded to the unit. */
std::string placeText(const LdtkPlacement& placement) {
  return std::string{PLACED_ENTITY_TEXT} + placement.entity +
         std::string{PLACED_AT_TEXT} +
         std::to_string(std::lround(placement.position.x)) +
         std::string{POINT_SEPARATOR} +
         std::to_string(std::lround(placement.position.y));
}

/** @throws InvalidLdtkFileException about @p source when a placement names
 * no sheet of @p types, or a sheet of a ship. */
void checkSheets(std::span<const LdtkPlacement> placements,
                 const EntitySheets& types, const std::string& source) {
  for (const LdtkPlacement& placement : placements) {
    const auto sheet =
        std::ranges::find(types.all(), placement.sheet, &EntitySheet::name);
    if (sheet == types.all().end()) {
      throw InvalidLdtkFileException(LdtkRefusal{
          .file = source,
          .reason = placeText(placement) + std::string{NO_SHEET_REASON} +
                    placement.sheet});
    }
    if (!sheet->pilot.has_value()) {
      throw InvalidLdtkFileException(LdtkRefusal{
          .file = source,
          .reason = placeText(placement) + std::string{SHIP_SHEET_REASON}});
    }
  }
}

}  // namespace

void readLevelPlacements(LuauLevels& levels,
                         std::span<const std::filesystem::path> contentFolders,
                         const EntitySheets& types) {
  for (const std::string& name : levels.names()) {
    LuauLevelEntry& entry = *levels.entryOf(name);
    if (!entry.ldtkFile.has_value()) {
      continue;
    }
    const std::filesystem::path file =
        engine::contentFilesOf(contentFolders, *entry.ldtkFile).back();
    entry.placements = ldtkPlacementsOf(file);
    checkSheets(entry.placements, types, engine::shownPath(file));
  }
}

}  // namespace rtype::game
