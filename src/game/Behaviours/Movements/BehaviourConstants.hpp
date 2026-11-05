#pragma once

#include <numbers>
#include <string_view>
#include "DirectionConstants.hpp"

namespace rtype::game {

/** @brief The names a body is given its behaviour by. */
constexpr std::string_view STRAIGHT_BEHAVIOUR_NAME = "straight";
constexpr std::string_view WEAVE_BEHAVIOUR_NAME = "weave";
constexpr std::string_view SCATTER_BEHAVIOUR_NAME = "scatter";
constexpr std::string_view REAPER_BEHAVIOUR_NAME = "reaper";
constexpr std::string_view DRILLBACK_BEHAVIOUR_NAME = "drillback";
constexpr std::string_view BYDOMANCER_BEHAVIOUR_NAME = "bydomancer";

/** @brief The horizontal share a behaviour writes to cross the playfield
 * towards the ships, at the full speed of its body. */
constexpr float LEFTWARD_STEERING = -FULL_STEERING;

/** @brief One whole turn of the sine a body follows when it weaves or bobs,
 * in radians. */
constexpr float FULL_TURN_RADIANS = 2.0F * std::numbers::pi_v<float>;

}  // namespace rtype::game
