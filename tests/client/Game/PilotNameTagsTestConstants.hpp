#pragma once

#include <cstdint>
#include "Position.hpp"
#include "SpriteFrame.hpp"

/** @brief The frame of a ship, 16 pixels tall, as the frames of the sheet. */
constexpr rtype::game::SpriteFrame SHIP_FRAME{
    .left = 0, .top = 0, .width = 32, .height = 16};

/** @brief How far above a ship of SHIP_FRAME its tag is centered, in logical
 * units: half the ship and half a line, 12 pixels, plus a gap of 2, times the
 * 4 units of a pixel. */
constexpr float RISE_OVER_THE_SHIP = 56.0F;

/** @brief A ship in the middle of the playfield, far from every edge. */
constexpr rtype::game::Position MIDDLE_OF_THE_PLAYFIELD{.x = 960.0F,
                                                        .y = 540.0F};

/** @brief The lowest ships whose tag leaves the top of the playfield: at
 * 76, the line, its outline included, starts at y 0 (76 - 56 - 20); at 75, it
 * would start at y -1. */
constexpr float LOWEST_Y_WITH_ROOM_ABOVE = 76.0F;
constexpr float HIGHEST_Y_WITHOUT_ROOM_ABOVE = 75.0F;

/** @brief A name of 16 characters, the most the protocol allows: 128 pixels,
 * 65 pixels or 260 units each side of its center with the outline. */
constexpr const char* LONGEST_NAME = "SIXTEEN_LETTERS!";

/** @brief A name of 8 characters of 2 bytes each, 16 bytes as LONGEST_NAME:
 * a character outside ASCII may be as wide as 10 pixels in the small font, so
 * the tag keeps the room of 8 pixels per byte and moves in as far. */
constexpr const char* LONGEST_NAME_OUTSIDE_ASCII = "ÉÉÉÉÉÉÉÉ";

/** @brief Ships near the sides, and how far the tag of LONGEST_NAME moves in
 * for each: 260 - 64, 260 - 259, none at 260, 1660 - 1856. */
constexpr float SHIP_AT_THE_LEFT_EDGE = 64.0F;
constexpr float SHIP_ONE_UNIT_TOO_FAR_LEFT = 259.0F;
constexpr float SHIP_WITH_JUST_ENOUGH_ROOM = 260.0F;
constexpr float SHIP_AT_THE_RIGHT_EDGE = 1856.0F;
constexpr float MOVE_IN_FROM_THE_LEFT_EDGE = 196.0F;
constexpr float MOVE_IN_BY_ONE_UNIT = 1.0F;
constexpr float MOVE_IN_FROM_THE_RIGHT_EDGE = -196.0F;

/** @brief Pilot 2 flies the ship of color 1; pilot 3 the ship of color 2;
 * color 3 is the ship of pilot 4. */
constexpr std::uint8_t SECOND_PILOT = 2;
constexpr int SECOND_PILOT_COLOR = 1;
constexpr std::uint8_t THIRD_PILOT = 3;
constexpr int THIRD_PILOT_COLOR = 2;
constexpr int FOURTH_PILOT_COLOR = 3;

/** @brief The nickname of pilot 2, and how pilot 3 is shown, whom the waiting
 * room never named. */
constexpr const char* SECOND_PILOT_NAME = "ACE";
constexpr const char* THIRD_PILOT_SHOWN_NAME = "P3";
