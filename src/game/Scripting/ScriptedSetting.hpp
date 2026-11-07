#pragma once

#include <string>
#include "WholeNumberRange.hpp"

namespace rtype::game {

/** @brief A setting a scripted behaviour declares: its key in the table
 * `[gameplay.behaviour]` of a sheet, and the whole numbers it accepts. */
struct ScriptedSetting {
  std::string name;
  engine::WholeNumberRange range{};
};

}  // namespace rtype::game
