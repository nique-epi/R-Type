#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief What the systems of an R-Type game are called in the metrics of the
 * server.
 */
constexpr std::string_view BYDO_CONTROL_SYSTEM_NAME = "BydoControlSystem";
constexpr std::string_view PATTERN_SYSTEM_NAME = "PatternSystem";
constexpr std::string_view MOVEMENT_SYSTEM_NAME = "MovementSystem";
constexpr std::string_view PLAYFIELD_EXIT_SYSTEM_NAME = "PlayfieldExitSystem";
constexpr std::string_view HURT_ZONE_SYSTEM_NAME = "HurtZoneSystem";
constexpr std::string_view COLLISION_SYSTEM_NAME = "CollisionSystem";

}  // namespace rtype::server
