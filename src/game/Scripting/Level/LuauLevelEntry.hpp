#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "LdtkPlacement.hpp"
#include "LevelCheckpoint.hpp"

namespace rtype::game {

/**
 * @brief What a script registers under the name of a level: its stages, as
 * references in the registry of the machine, the LDtk file that places its
 * enemies, relative to a content folder, nothing when the level places none,
 * and those placements once readLevelPlacements() read the file, sorted by
 * the x of their center, and where the stages it declares a start for begin,
 * by stage counted from 0, so a game can start at one of them.
 */
struct LuauLevelEntry {
  std::vector<int> stages;
  std::optional<std::string> ldtkFile = std::nullopt;
  std::vector<LdtkPlacement> placements;
  std::map<std::size_t, LevelCheckpoint> starts;
};

}  // namespace rtype::game
