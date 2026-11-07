#pragma once

#include <array>
#include <string_view>

namespace rtype::server::testing {

/** @brief The systems of an R-Type game, in the order they run. */
constexpr std::array<std::string_view, 7> NAMES_OF_THE_SYSTEMS{
    "BossPhaseSystem",     "PilotSystem",     "MovementSystem",
    "PlayfieldExitSystem", "CollisionSystem", "LagCompensatedHitSystem",
    "HurtZoneSystem"};

}  // namespace rtype::server::testing
