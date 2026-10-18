#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief Lengths of the effects, as the animation page shows them. */
constexpr rtype::engine::Duration BYDO_EXPLOSION_LENGTH =
    std::chrono::milliseconds(420);
constexpr rtype::engine::Duration SHIP_EXPLOSION_LENGTH =
    std::chrono::milliseconds(600);
constexpr rtype::engine::Duration IMPACT_LENGTH =
    std::chrono::milliseconds(150);
constexpr rtype::engine::Duration MUZZLE_FLASH_LENGTH =
    std::chrono::milliseconds(100);
constexpr rtype::engine::Duration HIT_FLASH_LENGTH =
    std::chrono::milliseconds(60);
