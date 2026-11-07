#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief What the systems of an R-Type game are called in the metrics of the
 * server.
 */
constexpr std::string_view PILOT_SYSTEM_NAME = "PilotSystem";
constexpr std::string_view MOVEMENT_SYSTEM_NAME = "MovementSystem";
constexpr std::string_view PLAYFIELD_EXIT_SYSTEM_NAME = "PlayfieldExitSystem";
constexpr std::string_view COLLISION_SYSTEM_NAME = "CollisionSystem";
constexpr std::string_view LAG_COMPENSATED_HIT_SYSTEM_NAME =
    "LagCompensatedHitSystem";
constexpr std::string_view HURT_ZONE_SYSTEM_NAME = "HurtZoneSystem";

}  // namespace rtype::server
