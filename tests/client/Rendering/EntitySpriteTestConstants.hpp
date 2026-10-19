#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief Frame times of the entities, as listed in assets/README.md. */
constexpr rtype::engine::Duration SHIP_FRAME_TIME_LISTED =
    std::chrono::milliseconds(80);
constexpr rtype::engine::Duration BYDO_FRAME_TIME_LISTED =
    std::chrono::milliseconds(250);
constexpr rtype::engine::Duration PLAYER_MISSILE_FRAME_TIME_LISTED =
    std::chrono::milliseconds(60);
constexpr rtype::engine::Duration ENEMY_MISSILE_FRAME_TIME_LISTED =
    std::chrono::milliseconds(120);
