#pragma once

#include <string>
#include <vector>
#include "LdtkField.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief An enemy a level places: the identifier of its entity in LDtk, the
 * sheet that identifier names, the center of its box in logical units of the
 * level (x grows with the scroll, 0 at the start of the level), its width in
 * logical units, and its custom fields in the order of the file.
 */
struct LdtkPlacement {
  std::string entity;
  std::string sheet;
  Position position{};
  float width{};
  std::vector<LdtkField> fields;
};

}  // namespace rtype::game
