#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "LdtkPlacement.hpp"
#include "LevelCheckpoint.hpp"
#include "PlayBand.hpp"

namespace rtype::game {

/**
 * @brief What a script registers under the name of a level: its stages, as
 * references in the registry of the machine, the LDtk file that places its
 * enemies, relative to a content folder, nothing when the level places none,
 * and those placements once readLevelPlacements() read the file, sorted by
 * the x of their center, and where the stages it declares a start for begin,
 * by stage counted from 0, so a game can start at one of them, and the band
 * of rows a ship flies between in each biome that closes the play area, by
 * biome name: a biome it does not name leaves the whole play area.
 */
struct LuauLevelEntry {
  std::vector<int> stages;
  std::optional<std::string> ldtkFile = std::nullopt;
  std::vector<LdtkPlacement> placements;
  std::map<std::size_t, LevelCheckpoint> starts;
  std::map<std::string, PlayBand, std::less<>> bands;
};

}  // namespace rtype::game
