#pragma once

#include <array>
#include <string_view>

namespace rtype::server::testing {

/** @brief The systems of an R-Type game, in the order they run. */
constexpr std::array<std::string_view, 5> NAMES_OF_THE_SYSTEMS{
    "BydoControlSystem", "PatternSystem", "MovementSystem",
    "PlayfieldExitSystem", "CollisionSystem"};

}  // namespace rtype::server::testing
