#pragma once

#include <string_view>
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @brief A frame of a ship and the sheet it lies on: the entity sheet for
 * the standard hull, the hull sheet for any other. The sheet is a constant
 * asset id, so the view never dangles. */
struct ShipSheetFrame {
  std::string_view sheet;
  game::SpriteFrame frame;
};

}  // namespace rtype::client
