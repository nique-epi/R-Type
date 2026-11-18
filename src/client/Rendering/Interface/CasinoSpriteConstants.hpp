#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>

namespace rtype::client {

/**
 * @brief Asset ids of the pictures of the casino, the Bydo Palace, each as
 * large as the interface picture: its front, the doors closed, ajar, then
 * open.
 */
constexpr const char* CASINO_DOORS_CLOSED_ID =
    "sprites/casino/entrance_doors_closed.png";
constexpr const char* CASINO_DOORS_AJAR_ID =
    "sprites/casino/entrance_doors_ajar.png";
constexpr const char* CASINO_DOORS_OPEN_ID =
    "sprites/casino/entrance_doors_open.png";

/**
 * @brief The sheet of Balthazar, the Bydo butler of the casino: frames of
 * BUTLER_FRAME_SIZE, BUTLER_FRAMES_PER_POSE in a row for each pose, the rows
 * from the top: idle, greeting, bowing. His feet stand BUTLER_FLOOR_ROW pixels
 * under the top of a frame.
 */
constexpr const char* BUTLER_SHEET_ID = "sprites/casino/majordome.png";
constexpr sf::Vector2i BUTLER_FRAME_SIZE(80, 112);
constexpr int BUTLER_FRAMES_PER_POSE = 4;
constexpr int BUTLER_FLOOR_ROW = 108;

/**
 * @brief The sheet of the chips of the selector, as the cashier and the
 * table show them: the chips of 1, 5, 25, 100 and 500 in a row, each
 * CHIP_SELECTOR_FRAME_SIZE large, CHIP_SELECTOR_STRIDE apart from the left of
 * one to the left of the next.
 */
constexpr const char* CHIP_SELECTOR_SHEET_ID =
    "sprites/casino/chips_selector.png";
constexpr sf::Vector2i CHIP_SELECTOR_FRAME_SIZE(23, 24);
constexpr int CHIP_SELECTOR_STRIDE = 24;

/** @brief The pictures of the roulette room: the room, as large as the
 * interface picture, and the table, as wide. */
constexpr const char* ROULETTE_ROOM_ID = "sprites/casino/roulette_room.png";
constexpr const char* ROULETTE_TABLE_ID = "sprites/casino/roulette_table.png";

/** @brief The wheel of the roulette: its bowl, its rotor at rest, the whole
 * wheel, each WHEEL_FRAME_SIZE large, and its ball, WHEEL_BALL_SIZE. */
constexpr const char* WHEEL_BOWL_ID = "sprites/casino/wheel_bowl.png";
constexpr const char* WHEEL_ROTOR_ID = "sprites/casino/wheel_rotor.png";
constexpr const char* WHEEL_WHOLE_ID = "sprites/casino/wheel_whole.png";
constexpr const char* WHEEL_BALL_ID = "sprites/casino/wheel_ball.png";
constexpr sf::Vector2i WHEEL_FRAME_SIZE(125, 125);
constexpr sf::Vector2i WHEEL_BALL_SIZE(5, 6);

/**
 * @brief The turns of the rotor: WHEEL_ROTOR_TURN_FRAMES frames of
 * WHEEL_FRAME_SIZE, each half a pocket further clockwise than the one before,
 * frame 0 the rotor at rest, laid WHEEL_ROTOR_TURN_COLUMNS to a row from the
 * top left, so the sheet, 1250 x 1000, stays under the 8192 pixels some
 * graphics cards take for a side of one texture.
 */
constexpr const char* WHEEL_ROTOR_TURN_ID =
    "sprites/casino/wheel_rotor_turn.png";
constexpr int WHEEL_ROTOR_TURN_FRAMES = 74;
constexpr int WHEEL_ROTOR_TURN_COLUMNS = 10;
constexpr sf::Vector2i WHEEL_ROTOR_TURN_SHEET_SIZE(1250, 1000);

/** @brief The small chips laid on the cloth, those of 1, 5, 25, 100 and 500
 * in a row, each CLOTH_CHIP_FRAME_SIZE large, CLOTH_CHIP_STRIDE apart from
 * the left of one to the left of the next; and the props of the room. */
constexpr const char* CASINO_CHIPS_SHEET_ID = "sprites/casino/chips.png";
constexpr sf::Vector2i CLOTH_CHIP_FRAME_SIZE(13, 14);
constexpr int CLOTH_CHIP_STRIDE = 14;
constexpr const char* CASINO_PROPS_SHEET_ID = "sprites/casino/props.png";

/** @brief Every picture of the casino a screen draws, which the title loads,
 * so the casino opens over the waiting room without loading. */
constexpr std::array CASINO_TEXTURES{
    CASINO_DOORS_CLOSED_ID, CASINO_DOORS_AJAR_ID,   CASINO_DOORS_OPEN_ID,
    BUTLER_SHEET_ID,        CHIP_SELECTOR_SHEET_ID, ROULETTE_ROOM_ID,
    ROULETTE_TABLE_ID,      WHEEL_BOWL_ID,          WHEEL_ROTOR_ID,
    WHEEL_ROTOR_TURN_ID,    WHEEL_WHOLE_ID,         WHEEL_BALL_ID,
    CASINO_CHIPS_SHEET_ID,  CASINO_PROPS_SHEET_ID};

}  // namespace rtype::client
