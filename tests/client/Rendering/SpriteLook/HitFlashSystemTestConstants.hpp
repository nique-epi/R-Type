#pragma once

#include <chrono>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief How long the entities of the tests are flashed: one flash, and one
 * twice as long. */
constexpr rtype::engine::Duration FLASH_TIME = std::chrono::milliseconds(60);
constexpr rtype::engine::Duration LONGER_FLASH_TIME =
    std::chrono::milliseconds(120);

/** @brief The frame the flashed entities of the scene show. */
constexpr rtype::game::SpriteFrame FRAME{
    .left = 0, .top = 0, .width = 8, .height = 8};
