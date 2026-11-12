#pragma once

#include <optional>
#include <string>
#include <vector>
#include "LdtkPlacement.hpp"

namespace rtype::game {

/**
 * @brief What a script registers under the name of a level: its stages, as
 * references in the registry of the machine, the LDtk file that places its
 * enemies, relative to a content folder, nothing when the level places none,
 * and those placements once readLevelPlacements() read the file, sorted by
 * the x of their center.
 */
struct LuauLevelEntry {
  std::vector<int> stages;
  std::optional<std::string> ldtkFile = std::nullopt;
  std::vector<LdtkPlacement> placements;
};

}  // namespace rtype::game
