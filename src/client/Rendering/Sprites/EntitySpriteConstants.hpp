#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <numbers>
#include "Animation.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sprite sheet that holds every entity of a match.
 *
 * Its sprites are at their native size: one pixel of the sheet is one pixel of
 * a PixelCanvas, which covers BACKGROUND_PIXEL_SIZE logical units, like one
 * pixel of the scrolling background.
 */
constexpr const char* ENTITY_SHEET_ID = "sprites/entities.png";

/**
 * @brief Rectangles of the entity sheet, in pixels of the sheet.
 *
 * Each is the first frame of its entity. The player ship is the first of the
 * four player colors.
 */
constexpr game::SpriteFrame PLAYER_SHIP_FRAME{
    .left = 0, .top = 0, .width = 32, .height = 16};
constexpr game::SpriteFrame BYDO_FRAME{
    .left = 0, .top = 72, .width = 24, .height = 24};
constexpr game::SpriteFrame PLAYER_MISSILE_FRAME{
    .left = 0, .top = 124, .width = 16, .height = 6};
constexpr game::SpriteFrame ENEMY_MISSILE_FRAME{
    .left = 0, .top = 132, .width = 8, .height = 8};

/** @brief Pixels between two frames of a row of a sprite sheet, the entity
 * sheet or the effect sheet, and between two rows. */
constexpr int SPRITE_SPACING = 2;

/** @brief Colors of ship the entity sheet holds, one row each from
 * PLAYER_SHIP_FRAME down. */
constexpr int PLAYER_COLOR_COUNT = 4;

/** @brief Each entity of the sheet has this many frames, side by side in its
 * row. */
constexpr std::size_t SPRITE_FRAME_COUNT = 2;

/** @brief How long each frame of an entity shows, as listed in
 * assets/README.md. */
constexpr engine::Duration PLAYER_SHIP_FRAME_TIME =
    std::chrono::milliseconds(80);
constexpr engine::Duration BYDO_FRAME_TIME = std::chrono::milliseconds(250);
constexpr engine::Duration PLAYER_MISSILE_FRAME_TIME =
    std::chrono::milliseconds(60);
constexpr engine::Duration ENEMY_MISSILE_FRAME_TIME =
    std::chrono::milliseconds(120);

/** @brief The frames of each entity, along its row of the sheet. */
constexpr std::array PLAYER_SHIP_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(PLAYER_SHIP_FRAME, SPRITE_SPACING);
constexpr std::array BYDO_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(BYDO_FRAME, SPRITE_SPACING);
constexpr std::array PLAYER_MISSILE_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(PLAYER_MISSILE_FRAME, SPRITE_SPACING);
constexpr std::array ENEMY_MISSILE_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(ENEMY_MISSILE_FRAME, SPRITE_SPACING);

/** @brief The animation of each entity type: its frames, in a loop. */
constexpr Animation PLAYER_SHIP_ANIMATION{
    .frames = PLAYER_SHIP_FRAMES,
    .frameDuration = PLAYER_SHIP_FRAME_TIME,
    .playback = Animation::Playback::Loop};
constexpr Animation BYDO_ANIMATION{.frames = BYDO_FRAMES,
                                   .frameDuration = BYDO_FRAME_TIME,
                                   .playback = Animation::Playback::Loop};
constexpr Animation PLAYER_MISSILE_ANIMATION{
    .frames = PLAYER_MISSILE_FRAMES,
    .frameDuration = PLAYER_MISSILE_FRAME_TIME,
    .playback = Animation::Playback::Loop};
constexpr Animation ENEMY_MISSILE_ANIMATION{
    .frames = ENEMY_MISSILE_FRAMES,
    .frameDuration = ENEMY_MISSILE_FRAME_TIME,
    .playback = Animation::Playback::Loop};

/**
 * @brief The bonus Bydo, right under the Bydo on the entity sheet, its two
 * frames BONUS_BYDO_FRAME_TIME each, as listed in assets/README.md: no entity
 * of a match is one yet, the intro draws it.
 */
constexpr game::SpriteFrame BONUS_BYDO_FRAME =
    frameUnder(BYDO_FRAME, SPRITE_SPACING);
constexpr engine::Duration BONUS_BYDO_FRAME_TIME =
    std::chrono::milliseconds(200);
constexpr std::array BONUS_BYDO_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(BONUS_BYDO_FRAME, SPRITE_SPACING);
constexpr Animation BONUS_BYDO_ANIMATION{.frames = BONUS_BYDO_FRAMES,
                                         .frameDuration = BONUS_BYDO_FRAME_TIME,
                                         .playback = Animation::Playback::Loop};

/**
 * @brief Share of a cycle between the loop starts of two entities whose
 * network ids follow each other: the fractional part of the golden ratio,
 * which spreads any run of ids over the whole cycle, so Bydos created in the
 * same tick still pulse out of step.
 */
constexpr double LOOP_START_SPREAD = std::numbers::phi - 1.0;

}  // namespace rtype::client
