#pragma once

#include <string_view>
#include "WholeNumberRange.hpp"

namespace rtype::game {

/**
 * @brief A setting a behaviour reads from the sheet of its body: its key in
 * the table `[gameplay.behaviour]` and the whole numbers it accepts.
 *
 * A behaviour declares its settings (IBehaviour::settings()), so a sheet that
 * misspells one or gives a value out of its range is refused when the sheets
 * are read, never run with a default.
 */
struct BehaviourSetting {
  std::string_view name;
  engine::WholeNumberRange range{};
};

}  // namespace rtype::game
