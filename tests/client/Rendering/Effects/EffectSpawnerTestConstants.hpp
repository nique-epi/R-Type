#pragma once

#include <cstddef>
#include "Position.hpp"
#include "SpriteFrame.hpp"

/** @brief Where the entities of the tests die, and where the scene puts the
 * targets it makes. */
constexpr rtype::game::Position WHERE_IT_DIED{.x = 640.0F, .y = 360.0F};

/** @brief The frame of the Bydos and targets the scene makes. */
constexpr rtype::game::SpriteFrame TARGET_FRAME{
    .left = 0, .top = 0, .width = 24, .height = 24};

/** @brief The asset ids of the Bydos and targets the scene makes. */
constexpr const char* BYDO_SHEET_ID = "bydo";
constexpr const char* TARGET_SHEET_ID = "target";

/** @brief Where the scene puts a ship, and a point ahead of it, where a
 * missile it fires appears. */
constexpr rtype::game::Position SHIP_CENTER{.x = 400.0F, .y = 360.0F};
constexpr rtype::game::Position AHEAD_OF_THE_SHIP{.x = 460.0F, .y = 360.0F};

/** @brief How many Bydos killed in the same frame flash within the flash
 * budget: 13 x 275 = 3575 pixels of white, under 3600. */
constexpr int BYDO_FLASHES_WITHIN_BUDGET = 13;

/** @brief How many speed lines a ship arriving at the start draws behind it. */
constexpr std::size_t SPEED_LINE_COUNT = 4;
