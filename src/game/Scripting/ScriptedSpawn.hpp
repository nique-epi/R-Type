#pragma once

#include <string_view>
#include "Position.hpp"

namespace rtype::game {

/** @brief What `behaviours.spawn` asks: the name of a sheet, and where its
 * body appears. */
struct ScriptedSpawn {
  std::string_view type;
  Position position{};
};

}  // namespace rtype::game
