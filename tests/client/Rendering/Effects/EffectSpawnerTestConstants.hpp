#pragma once

#include <SFML/Graphics/Color.hpp>
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

/** @brief A Bydo that fires, the missile it fires, which appears 64 logical
 * units at its left as the server places it ((24 + 8) / 2 pixels of 4 units),
 * and where the middle of its cue stands: 2 pixels above the top of the
 * 24-pixel Bydo, then half an 8-pixel line, (12 + 2 + 4) x 4 = 72 units above
 * its center. */
constexpr rtype::game::Position FIRING_BYDO{.x = 1200.0F, .y = 500.0F};
constexpr rtype::game::Position MISSILE_OF_THE_FIRING_BYDO{.x = 1136.0F,
                                                           .y = 500.0F};
constexpr rtype::game::Position ABOVE_THE_FIRING_BYDO{.x = 1200.0F,
                                                      .y = 428.0F};

/** @brief Where the middle of the cue of a ship hit at SHIP_CENTER stands:
 * (8 + 2 + 4) x 4 = 56 units above the center of the 16-pixel ship. */
constexpr rtype::game::Position ABOVE_THE_SHIP{.x = 400.0F, .y = 304.0F};

/** @brief The light orange of the enemy missile, the color of the cue of a
 * shot. */
constexpr sf::Color LIGHT_ORANGE_OF_THE_ENEMY_MISSILE{255, 179, 107};

/** @brief How many speed lines a ship arriving at the start draws behind it. */
constexpr std::size_t SPEED_LINE_COUNT = 4;
