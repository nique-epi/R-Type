#pragma once

#include <string_view>
#include "DirectionConstants.hpp"

namespace rtype::game {

/** @brief The names a body is given its behaviour by. */
constexpr std::string_view STRAIGHT_BEHAVIOUR_NAME = "straight";
constexpr std::string_view WEAVE_BEHAVIOUR_NAME = "weave";
constexpr std::string_view SCATTER_BEHAVIOUR_NAME = "scatter";
constexpr std::string_view REAPER_BEHAVIOUR_NAME = "reaper";

/** @brief The horizontal share a behaviour writes to cross the playfield
 * towards the ships, at the full speed of its body. */
constexpr float LEFTWARD_STEERING = -FULL_STEERING;

}  // namespace rtype::game
